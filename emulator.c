//Using libs SDL, glibc
#include <SDL.h>	//SDL version 2.0
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SCREEN_WIDTH 640    // window height
#define SCREEN_HEIGHT 480   // windoe width
#define AMOUNT_OF_DATA 14 
#define AMOUNT_OF_INSTRUTIONS 716


// Global variables

int endGame = 0;

int PC = 0; // instruction of the code that is running

int BR[32] = {0, 0, 0, 0,   // registers values
              0, 0, 0, 0, 
              0, 0, 0, 0, 
              0, 0, 0, 0, 
              0, 0, 0, 0,
              0, 0, 0, 0,
              0, 0, 0, 0,
              268468224, 2147479548, 0, 0};

int mem_data[AMOUNT_OF_DATA];   // data saved in the program

char instruction_mem[AMOUNT_OF_INSTRUTIONS][32];

/*
This method adds the numbers saved in the data part of 
the assembler into the mem_data array.
*/
int add_data(int amount_words) {

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    // open the text
    fp = fopen("data.txt", "r");

    if (fp == NULL) {
        printf("Error en la lectura del data.txt");
        return 0;
    }

    int i = 0;
    // Save the binaries as integers in the mem_data array
    while (((read = getline(&line, &len, fp)) != -1) & i<amount_words) {
        mem_data[i] = strtol(line, NULL, 2);
        i ++;
    }

    // close the file
    fclose(fp);
    if (line)
        free(line);

    return 1;
    
}

/*
This method adds the numbers saved in the data part of 
the assembler into the mem_data array.
*/
int add_instructions() {

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    // open the text
    fp = fopen("text.txt", "r");
    if (fp == NULL) {
        printf("Error en la lectura del text.txt");
        return 0;
    }

    int i = 0;
    // Save the binaries as integers in the mem_data array
    while (((read = getline(&line, &len, fp)) != -1)) {
         //= line;
        strcpy(instruction_mem[i], line);

        printf("%s", instruction_mem[i]);
        i ++;
    }

    // close the file
    fclose(fp);
    if (line)
        free(line);

    return 1;
    
}

void execute_ins(){
    
    char instruction[32];
    strncpy(instruction, instruction_mem[PC], 32);
    char opcode[6];
    strncpy(opcode, &instruction[0], 6);
    
    printf("The original string is: %s\n", instruction_mem[PC]);
    printf("The opcode is %s\n", opcode);

    endGame = 1;
}

int main(void)
{
    int data = add_data(AMOUNT_OF_DATA);
    int text = add_instructions();
    //printf("The original string is: %s\n", instruction_mem[PC]);
    if (!(data & text)) return 0;
    
    while (!endGame)
    {
        execute_ins();
    }


    return 0;
}