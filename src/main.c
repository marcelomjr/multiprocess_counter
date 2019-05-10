#include <stdio.h>
#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // waitpid()
#include <stdlib.h>     // exit()
#include <unistd.h>     // fork() and sleep
#include <sys/mman.h>   // mmap
#include <stdint.h>     // uint64_t

void child_process(uint64_t number, int input_index, int *results);
int get_free_process_index(int pids[]);
int is_prime_number(uint64_t number);
void get_inputs(uint64_t inputs[], int* number_of_inputs);

#define MAX_CHILD_PROCESSES 3

int main() {

    uint64_t inputs[30];
    int number_of_inputs;
    int* results;
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANON;
    
    pid_t pids[] = {0, 0, 0}; // processes (parent and 3 children)
    int free_process_index;
    uint64_t current_input;
    
    // Get the numbers and save them in inputs array
    get_inputs(inputs, &number_of_inputs);
    
    // Create a shared memory to save which numbers are prime
    results = mmap(NULL, number_of_inputs, protection, visibility, 0, 0);
    
    // Repeat for each input number
    for (int input_index = 0; input_index < number_of_inputs; input_index++ ) {
        
        // Selects the input for the child process
        current_input = inputs[input_index];

        /* Selects a free position on pids array to create a new child process.
        If all positions are unavailable, wait until one of the processes end. */
        free_process_index = get_free_process_index(pids);
        
        // Create a new child process and save its pid in pids array
        pids[free_process_index] = fork();

        /* If is a child process running this 
        point call the child_process function */
        if (pids[free_process_index] == 0) {
            child_process(current_input, input_index, results);
        }
    }
    
    // Wait until the last process end
    for (int index = 0; index < MAX_CHILD_PROCESSES + 1; index++) {
    
        /* If pids[index] is zero this position has 
        never been used by a process.*/
        if (pids[index] != 0) {
            waitpid(pids[index], NULL, 0);
        }
    }
    
    // Count the number of prime numbers
    int number_of_primes = 0;
    for (int result = 0; result < number_of_inputs; result++) {
        number_of_primes += results[result];
    }
    
    // Final result
    printf("%d\n", number_of_primes);
        
    return 0;
}

/* Return a position of pids array that is free (value 0), or has a ended process
*/
int get_free_process_index(int pids[]) {

    int status;
    int index = 0;
    int free_process_index = -1;
    pid_t returned_pid;
    
    // Repeat until it find a free position or ended process
    while (free_process_index == -1) {

        // If there is no running process in this index
        if (pids[index] == 0) {
            // find a free position
            free_process_index = index;
        }
        else {
            /*Checks if the process is ended, without suspend the 
              parent process execution*/
            returned_pid = waitpid(pids[index], &status, WNOHANG);
            
            // If returned_pid is zero is because the process is still running
            if (returned_pid != 0) {
                free_process_index = index;
            }
            // If returned_pid is zero try the next position in the next iteration
        }
        
        // Update the index
        index++;
        
        // If it has already tried all the pids, start from the beginning
        if (index == MAX_CHILD_PROCESSES) { 
            index = 0;
        }
    }
    return free_process_index;
}

/* Check if the number received is prime and finish the process.
    The result is saved in a array (results), which each field is a input number,
    if the number is prime, is saved 1 otherwise is saved 0.
*/
void child_process(uint64_t number, int input_index, int *results) {

    if (is_prime_number(number) == 1) {
        results[input_index] = 1;
        
    } else {
        results[input_index] = 0;
    }
        
    exit(0);
}

/* Verify if a received number is prime, 
   if true returns 1, otherwise returns 0.
*/
int is_prime_number(uint64_t number) {
    int is_prime = 1;
    
    // If number is smaller than 2 it isn't a prime number
    if (number < 2) {
        is_prime = 0;
    }
    /* If the number is two, it is a prime number, but if it is bigger
     than two we need test if there is a division with remainder equals 
     zero, in this case the number isn't prime.*/
    else if (number > 2) {
        for (uint64_t div = 2; (div < number && is_prime == 1); div++) {
           
            if (number % div == 0) {
                is_prime = 0;
            }
        }
    }
    return is_prime;
}

/* It gets the numbers from the standard input, one by one, 
   until the end of file character is found */
void get_inputs(uint64_t inputs[], int* number_of_inputs) {
    
    uint64_t input;
    int index = 0;
    
    while (scanf("%lu", &input) != EOF) {
        inputs[index] = input;
        index++;
    }
    *number_of_inputs = index;
}
