/* Contador de palavras
 *
 * Este programa recebera uma serie de caracteres representando palavras em sua
 * entrada. Ao receber um caractere fim de linha ('\n'), deve imprimir na tela o
 * numero de palavras separadas que recebeu e, apos, encerrar.
 */

#include <stdio.h>

#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // waitpid()
#include <stdlib.h>     // exit()
#include <unistd.h>     // fork() and sleep

#include <stdint.h> // uint64_t

void child_process(uint64_t number);
int get_free_process_index(int pids[]);
int is_prime_number(uint64_t number);
void get_input(uint64_t inputs[], int* number_of_inputs);

int main() {

    uint64_t inputs[] = {11, 17, 31, 19, 29, 37};    
    int number_of_inputs = 6;
    
    pid_t pids[] = {0, 0, 0}; // processes (parent and 3 children)
    int free_process_index;
    uint64_t current_input;
    
    
    for (int input_index = 0; input_index < number_of_inputs; input_index++ ) {
        
        // Select the input for the child process
        current_input = inputs[input_index];

        /* When a process end, get its position to replace it for 
        a new child process */
        free_process_index = get_free_process_index(pids);
        
        // Create a new child process and save its pid in pids array
        pids[free_process_index] = fork();

        /* If is a child process running this 
        point call the child_process function */
        if (pids[free_process_index] == 0) {
            child_process(current_input);
        }
    }
    
    // Wait until the last process end
    for (int index = 0; index < 4; index++) {
    
        /* If pids[index] is zero this position has 
        never been used by a process.*/
        if (pids[index] != 0) {
            waitpid(pids[index], NULL, 0);
        }
    }
        
    return 0;
}

int get_free_process_index(int pids[]) {

    int status;
    int index = 0;
    int free_process_index = -1;
    pid_t returned_pid;
    
    // Repeat until it find a free process
    while (free_process_index == -1) {

        // If there is no running process in this index
        if (pids[index] == 0) {
            free_process_index = index;
        }
        else {

            returned_pid = waitpid(pids[index], &status, WNOHANG);
            
            // If returned_pid is zero is because the process is still running
            if (returned_pid != 0) {
                free_process_index = index;
            }
        }
        
        // Update the index
        index++;
        
        // If it has already tried all the pids, start from the beginning
        if (index == 3) { 
            index = 0;
        }
    }
    return free_process_index;
}

void child_process(uint64_t number) {

    printf("vai processar %lu\n", number);
    
    if (is_prime_number(number) == 1) {
        printf("É PRIMO>>>>>>>>>>>>>>>>>>>>>>>>>>>>>%lu\n", number);
    } else {
        printf("%lu it's not a prime number!\n", number);
    }
    
    
    printf("saindo do processo para achar %lu\n", number);
    
    exit(0);
}

int is_prime_number(uint64_t number) {
    int is_prime = 1;
    
    // If number is smaller than 2 it isn't a prime number
    if (number < 2) {
        is_prime = 0;
    }
    /* If the number is two, it is a prime number, but 
    if bigger than two we need test if there is a division with 
    remainder equals zero, in this case the number isn't a prime number.*/
    else if (number > 2) {
        for (int div = 2; (div < number && is_prime == 1); div++) {
            
            sleep(1);
            printf("processing the number %lu\n", number);
        
            if (number % div == 0) {
                is_prime = 0;
            }
        }
    }
    return is_prime;
}

void get_input(uint64_t inputs[], int* number_of_inputs) {
    
    uint64_t input; 
    while (scanf("%lu", &input) != EOF) {
        // malloc mais um numero
        // incrementa contador
    }
}






