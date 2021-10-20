//Using libs SDL, glibc
#include <SDL.h>	//SDL version 2.0
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "instructions.h"

#define SCREEN_WIDTH 640    // window height
#define SCREEN_HEIGHT 480   // windoe width
#define AMOUNT_OF_DATA 14 
#define AMOUNT_OF_INSTRUTIONS 716

// Global variables
void instructions(void);

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

char instruction_mem[AMOUNT_OF_INSTRUTIONS][34];




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
    //char * line = NULL;
    char line[34];
    size_t len = 0;
    ssize_t read;

    // open the text
    fp = fopen("text.txt", "r");
    if (fp == NULL) {
        printf("Error en la lectura del text.txt");
        return 0;
    }

    int i = 0;
    // Save the binaries as integers in the instruction_mem array
    while (fgets(line, sizeof(line), fp)){
        strcpy(instruction_mem[i], line);
        i ++;
    }

    // close the file
    fclose(fp);

    return 1;
    
}

void execute_ins(){

    // Get the opcode from the instruction
    char opcode[6];
    int lenght = 6;
    int c = 0;
    while (c < lenght)
    {
        opcode[c] = instruction_mem[PC][c];
        c++;
    }
    opcode[c] = '\0';
    
    int opcodeNum = strtol(opcode, NULL, 2);
    
    /*
    All type R instructions have opcode = 0
    List of instructions
    */
    if (opcodeNum == 0)
    {
        printf("instruction type R\n");
    }
    
    // Filter the type of the instruction
    else if (opcodeNum == 3 | opcodeNum == 2)
    {
        printf("Instruction type J\n");
        
        // Get and complete the address

        char address[32];

        // Emulator PC MSB are 0000
        address [0] = '0';
        address [1] = '0';
        address [2] = '0';
        address [3] = '0';
        int lenght_add = 30;
        int initPosition = 6;
        int c_add = 4;
        while (c_add < lenght_add)
        {
            address[c_add] = instruction_mem[PC][initPosition+c_add-4];
            c_add++;
        }

        // Add the 00 at the end of the instruction
        address[30] = '0';
        address[31] = '0';
        address[32] = '\0';

        int addressNum = strtol(address, NULL, 2);

        // Este calculo es para redimensionar la dirección para un PC
        // que va de 1 en 1 como el del emulador
        addressNum = (addressNum-4194304)/4+1;

        // Execute the corresponding operation

        // jump (j)
        if (opcodeNum == 2) {
            printf("PC = %d, Instruction: jump\n", PC);
            PC = addressNum;
        }

        // jump and link (jal)
        else if (opcodeNum == 3) {
            printf("PC = %d, Instruction: jump and link\n", PC);
            BR[31] = PC + 1;
            PC = addressNum;
        }

    }

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