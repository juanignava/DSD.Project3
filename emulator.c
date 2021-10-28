//Using libs SDL, glibc
#include <SDL.h>//SDL version 2.0
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define SCREEN_WIDTH 640    // window height
#define SCREEN_HEIGHT 320   // windoe width
#define AMOUNT_OF_DATA 100  // 100 for PONG
#define AMOUNT_OF_INSTRUTIONS 2000 // 716 for PONG
#define SPACES_IN_STACK 100

/*
// Archivos de PONG
#define TEXT_FILE "text.txt"
#define DATA_FILE "data.txt"
*/


// Archivos de WIZARDS OF WOR
#define TEXT_FILE "text_p2.txt"
#define DATA_FILE "data_p2.txt"



//function prototypes
//initilise SDL
int init(int w, int h, int argc, char *args[]);

int sleep = 0;
Uint32 next_game_tick;

int width, height;		//used if fullscreen

SDL_Window* window = NULL;	//The window we'll be rendering to
SDL_Renderer *renderer;		//The renderer SDL will use to draw to the screen


//surfaces
static SDL_Surface *screen;
static SDL_Surface *title;
static SDL_Surface *numbermap;
static SDL_Surface *end;

//textures
SDL_Texture *screen_texture;


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

int stack[SPACES_IN_STACK];

int stackPointer = 0;




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
    fp = fopen(DATA_FILE, "r");

    if (fp == NULL) {
        printf("Error en la lectura del data.txt");
        return 0;
    }

    int i = 0;
    int c;
    int lineLenght = 32;

    // Save the binaries as integers in the mem_data array
    while (((read = getline(&line, &len, fp)) != -1) & i<amount_words) {

        if (line[0] == '0')
        {
            mem_data[i] = strtol(line, NULL, 2);
        }
        else {
            c = 0;

            while (c < lineLenght)
            {
                if (line[c] == '0') line[c] = '1';
                else line[c] = '0';
                c++;
            }
            line[c] = '\0';
            
            mem_data[i]  = -1*(strtol(line, NULL, 2) + 1);
        }

        //mem_data[i] = strtol(line, NULL, 2);
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
    fp = fopen(TEXT_FILE, "r");
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
    List of instructions based in the funct (in decimal)

    syscall -> 12
    addu    -> 33
    slt     -> 42
    sub     -> 34
    sll     -> 0
    jr      -> 8
    add     -> 32
    xor     -> 38
    */
    if (opcodeNum == 0)
    {

        // Get the rs value
        char rs[5];
        int lenght_rs = 5;
        int initPosition = 6;
        int c_rs = 0;

        while (c_rs < lenght_rs) 
        {
            rs[c_rs] = instruction_mem[PC][initPosition+c_rs];
            c_rs++;
        }
        rs[c_rs] = '\0';
        int rsNum = strtol(rs, NULL, 2);

        // Get the rt value
        char rt[5];
        int lenght_rt = 5;
        initPosition = 11;
        int c_rt = 0;

        while (c_rt < lenght_rt) 
        {
            rt[c_rt] = instruction_mem[PC][initPosition+c_rt];
            c_rt++;
        }
        rt[c_rt] = '\0';
        int rtNum = strtol(rt, NULL, 2);

        // Get the rd value
        char rd[5];
        int lenght_rd = 5;
        initPosition = 16;
        int c_rd = 0;

        while (c_rd < lenght_rt) 
        {
            rd[c_rd] = instruction_mem[PC][initPosition+c_rd];
            c_rd++;
        }
        rd[c_rd] = '\0';
        int rdNum = strtol(rd, NULL, 2);

        // Get the shamt value
        char shamt[5];
        int lenght_sh = 5;
        initPosition = 21;
        int c_sh = 0;

        while (c_sh < lenght_sh) 
        {
            shamt[c_sh] = instruction_mem[PC][initPosition+c_sh];
            c_sh++;
        }
        shamt[c_sh] = '\0';
        int shamtNum = strtol(shamt, NULL, 2);

        // Get the funct value
        char funct[6];
        int lenght_fn = 6;
        initPosition = 26;
        int c_fn = 0;

        while (c_fn < lenght_fn) 
        {
            funct[c_fn] = instruction_mem[PC][initPosition+c_fn];
            c_fn++;
        }
        funct[c_fn] = '\0';
        int functNum = strtol(funct, NULL, 2);

        // Syscall (used to pause the execusion)
        if (functNum == 12)
        {
            printf("PC = %d, Instruction: syscall\n", PC);

            if (BR[2] == 32) SDL_Delay(BR[4]);

            PC = PC+1;
        }

        // addu (add unsigned)
        if (functNum == 33)
        {
            printf("PC = %d, Instruction: addu\n", PC);
            BR[rdNum] = BR[rsNum] + BR[rtNum];
            PC = PC+1;
        }
        
        // slt (set less than)
        if (functNum == 42)
        {
            printf("PC = %d, Instruction: slt\n", PC);

            if (BR[rsNum] < BR[rtNum]) BR[rdNum] = 1;

            else BR[rdNum] = 0;
            PC = PC+1;
            
        }
        
        // sub (substract)
        if (functNum == 34) {
            printf("PC = %d, Instruction: sub\n", PC);
            BR[rdNum] = BR[rsNum] - BR[rtNum];
            PC = PC+1;   
        }
        
        // sll (shift left logical)
        if (functNum == 0) {
            printf("PC = %d, Instruction: sll\n", PC);
            BR[rdNum] = BR[rtNum] * pow(2, shamtNum);
            PC = PC+1;

        }
        
        // jr (jump register)
        if (functNum == 8)
        {
            printf("PC = %d, Instruction: jr\n", PC);
            PC = BR[rsNum];
        }
        
        // add (add)
        if (functNum == 32) {
            printf("PC = %d, Instruction: add\n", PC);
            BR[rdNum] = BR[rsNum] + BR[rtNum];

            if (rdNum == 23) {
                //endGame = 1;
            }

            PC = PC + 1;
        }
        
        // xor (xor)
        if (functNum == 38) {
            printf("PC = %d, Instruction: xor\n", PC);

           if(BR[rsNum] == 1) BR[rdNum] = -2;

           else BR[rdNum] = 0;

            PC = PC + 1;

        }

    }
    
    /*
    All type I instructions
    List of instructions: (opcodes in decimal)
    
    addiu   -> opcode 9
    lui     -> opcode 15
    lw      -> opcode 35 
    sw      -> opcode 43
    beq     -> opcode 4
    addi    -> opcode 8
    bne     -> opcode 5
    ori     -> opcode 13
    blez    -> opcode 6
    slti    -> opcode 10
    */
    else if (opcodeNum == 35 | opcodeNum == 9 | opcodeNum == 15 | opcodeNum == 35 | opcodeNum == 43 | opcodeNum == 4 | opcodeNum == 8 | opcodeNum == 5 | opcodeNum == 13 | opcodeNum == 6 | opcodeNum == 10){

        // Get the rs value
        char rs[5];
        int lenght_rs = 5;
        int initPosition = 6;
        int c_rs = 0;

        while (c_rs < lenght_rs) 
        {
            rs[c_rs] = instruction_mem[PC][initPosition+c_rs];
            c_rs++;
        }
        rs[c_rs] = '\0';
        int rsNum = strtol(rs, NULL, 2);

        // Get the rt value
        char rt[5];
        int lenght_rt = 5;
        initPosition = 11;
        int c_rt = 0;

        while (c_rt < lenght_rt) 
        {
            rt[c_rt] = instruction_mem[PC][initPosition+c_rt];
            c_rt++;
        }
        rt[c_rt] = '\0';
        int rtNum = strtol(rt, NULL, 2);
        

        // Get the inmediate value
        char inmediate[16];
        int lenght_inm = 16;
        initPosition = 16;
        int c_inm = 0;

        while (c_inm < lenght_inm) 
        {
            inmediate[c_inm] = instruction_mem[PC][initPosition+c_inm];
            c_inm++;
        }
        inmediate[c_inm] = '\0';

        int inmNum;
        if (inmediate[0] == '0')
        {
            inmNum = strtol(inmediate, NULL, 2);
        }
        else {
            c_inm = 0;

            while (c_inm < lenght_inm)
            {
                if (inmediate[c_inm] == '0') inmediate[c_inm] = '1';
                else inmediate[c_inm] = '0';
                c_inm++;
            }
            inmediate[c_inm] = '\0';
            
            inmNum = -1*(strtol(inmediate, NULL, 2) + 1);
        }

        // Execute the respective instruction

        // addiu (add inmediate unsigned)
        if (opcodeNum == 9) {
            printf("PC = %d, Instruction: addiu\n", PC);
            BR[rtNum] = BR[rsNum] + inmNum;
            PC = PC+1;
        }
        
        // lui (load upper inmediate)
        if (opcodeNum == 15) {
            printf("PC = %d, Instruction: lui\n", PC);
            int inmNumExt = 0;
            int k = 16;
            while (inmNum!=0) {
                int lastDig = inmNum%2;
                inmNumExt += lastDig * pow(2, k);
                k++;
                inmNum = inmNum/2;
            }
            BR[rtNum] = inmNumExt;
            PC = PC+1;
        }

        // lw (load word)
        if (opcodeNum == 35) {
            printf("PC = %d, Instruction: lw\n", PC);

            // Para load word se presentan varios casos

            // Caso en el que se toma un valor en el stack
            if (rsNum == 29)
            {
                BR[rtNum] = stack[inmNum/4 + stackPointer];
            }

            // Caso en el que se toma un dato guardado en .data
            if (BR[rsNum] == 268500992) { // 268500992 es la dirección inicial de .data
                
                // se debe buscar en el arreglo mem_data
                BR[rtNum] = mem_data[inmNum/4];
            }

            // Caso en el que se debe esperar una entrada
            if (BR[rsNum] == -65536)// & inmNum == 4) *******************
            {
                // se debe esperar a una entrada del teclado
		        SDL_PumpEvents();

                const Uint8 *keystate = SDL_GetKeyboardState(NULL);

                if (keystate[SDL_SCANCODE_1]) {
                    BR[rtNum] = 49; // significa que se presiono un 1
                }
                
                if (keystate[SDL_SCANCODE_2]) {
                    BR[rtNum] = 50; // significa que se presiono un 2
                }

                if (keystate[SDL_SCANCODE_A]) {
                    BR[rtNum] = 97; // significa que se presiono una A
                }

                if (keystate[SDL_SCANCODE_Z]) {
                    BR[rtNum] = 122; // significa que se presiono una Z
                }

                if (keystate[SDL_SCANCODE_K]) {
                    BR[rtNum] = 107; // significa que se presiono una K
                }

                if (keystate[SDL_SCANCODE_M]) {
                    BR[rtNum] = 109; // significa que se presiono una M
                }

                if (keystate[SDL_SCANCODE_ESCAPE]) {
                    endGame =1; // significa que se presiono ESC entonces se debe salir del programa
                }

                if (keystate[SDL_SCANCODE_SPACE]) {
                    BR[rtNum] = 32; // significa que se presiono el space
                }

                if (keystate[SDL_SCANCODE_S]) {
                    BR[rtNum] = 115; // significa que se presiono una S
                }

                if (keystate[SDL_SCANCODE_W]) {
                    BR[rtNum] = 119; // significa que se presiono una W
                }

                if (keystate[SDL_SCANCODE_D]) {
                    BR[rtNum] = 100; // significa que se presiono una D
                }

                if (keystate[SDL_SCANCODE_I]) {
                    BR[rtNum] = 105; // significa que se presiono una I
                }

                if (keystate[SDL_SCANCODE_J]) {
                    BR[rtNum] = 106; // significa que se presiono una J
                }

                if (keystate[SDL_SCANCODE_L]) {
                    BR[rtNum] = 108; // significa que se presiono una L
                }






                
            }
            
            PC = PC+1;
            
        }

        // sw (store word)
        if (opcodeNum == 43) {
            printf("PC = %d, Instruction: sw\n", PC);


            // Caso en el que se guarda un valor en el stack
            if (rsNum == 29)
            {
                stack[inmNum/4 + stackPointer] = BR[rtNum];
            }
            

            // Caso en el que se guarda un dato en .data
            if (BR[rsNum] == 268500992) { // 268500992 es la dirección inicial de .data
                
                // se debe buscar en el arreglo mem_data
                mem_data[inmNum/4] = BR[rtNum];
            }

            // Este es el caso en el que se pinta uno de los cuadros de la pantalla
            if (rsNum == 2 | rsNum == 10)
            {
                /*
                La pantalla se divide en rectangulos, en general son 64 x 32 cuadros
                hay que pintar con el color guardado en el registro rt el cuadro con el
                numero guardado en rs - $gp ( rs - BN[28] )
                */

                int pointWidth = SCREEN_WIDTH / 64;
                int pointHeight = SCREEN_HEIGHT / 32;
                int boardPointNum = (BR[rsNum] - BR[28])/4;
                int boardPointy = (boardPointNum / 64) * pointHeight;
                int boardPointx = (boardPointNum - (boardPointy/10) * 64)*pointWidth;

                SDL_Rect src;
                src.x = boardPointx;
                src.y = boardPointy;
                src.w = pointWidth;
                src.h = pointHeight;
                
                int r;
                
                switch (BR[rtNum])
                {
                    /*
                    colores:
                    blanco-> 0xffffffff
                    rojo-> 0xff2222ff
                    azul->  0xffe28743
                    verde -> 0xff22ff11
                    amarillo -> 0xff22ffff
                    light green -> 0xff22aa00
                    orange -> 0xff2222aa
                    gris -> 0xff222222
                    purple -> 0x55aaaa00
                    */
                case 0:
                    r = SDL_FillRect(screen , &src, 0xff222222); // fill with gray
                    break;

                case 16744448:
                    r = SDL_FillRect(screen , &src, 0xff22ff11); // fill with green
                    break;

                case 12583040:
                    r = SDL_FillRect(screen , &src, 0xff2222ff); // fill with red
                    break;

                case 16777215:
                    r = SDL_FillRect(screen , &src, 0xffffffff); // fill with white
                    break;
                    
                
                case 1245175:
                    r = SDL_FillRect(screen , &src, 0xffe28743); // fill with blue
                    break;

                case 16776960:
                    r = SDL_FillRect(screen , &src, 0xff22ffff); // fill with yellow
                    break;

                case 65280:
                    r = SDL_FillRect(screen , &src, 0xff22ff11); // fill with yellow
                    break;

                case 16711680:
                    r = SDL_FillRect(screen , &src, 0xff2222ff); // fill with red
                    break;

                case 65471:
                    r = SDL_FillRect(screen , &src, 0xff22aa00); // fill with light green
                    break;

                case 16753152:
                    r = SDL_FillRect(screen , &src, 0xff2222aa); // fill with orange
                    break;

                case 9700348:
                    r = SDL_FillRect(screen , &src, 0x55aaaa00); // fill with purple
                    break;

                default:
                    break;
                }
                
                if (r !=0){
	
                    printf("fill rectangle faliled");
                }

                SDL_UpdateTexture(screen_texture, NULL, screen->pixels, screen->w * sizeof (Uint32));
                SDL_RenderCopy(renderer, screen_texture, NULL, NULL);
                SDL_RenderPresent(renderer);
            }

            PC = PC+1;
            
        }
        
        // beq (branch on equal)
        if (opcodeNum == 4) {
            printf("PC = %d, Instruction: beq\n", PC);
            if (BR[rsNum] == BR[rtNum]) PC = PC + 1 + inmNum;

            else PC = PC + 1;

        }

        // addi (add immediate)
        if (opcodeNum == 8) {
            printf("PC = %d, Instruction: addi\n", PC);

            if(rtNum == 29)  BR[rtNum] = BR[rsNum] + inmNum/4;
            else BR[rtNum] = BR[rsNum] + inmNum;

            PC = PC + 1;
        }
        
        // bne (branch not equal)
        if (opcodeNum == 5) {
            printf("PC = %d, Instruction: bne\n", PC);
            if (BR[rsNum] != BR[rtNum]) PC = PC + 1 + inmNum;

            else PC = PC + 1;
        }
        
        // ori (or immediate)
        if (opcodeNum == 13) {
            printf("PC = %d, Instruction: ori\n", PC);
            // solo se va a sumar pero es porque para eso se suele usar
            BR[rtNum] = BR[rsNum] + inmNum;
            PC = PC + 1;
        }

        // blez (branch on less or equal zero)
        if (opcodeNum == 6) {
            printf("PC = %d, Instruction: ori\n", PC);
            if (BR[rsNum] <= 0) PC = PC + 1 + inmNum;

            else PC = PC + 1;

        }
        
        // slti (set less than immediate
        if (opcodeNum == 10) {
            printf("PC = %d, Instruction: slti\n", PC);

            if (BR[rsNum] < inmNum) BR[rtNum] = 1;
            else BR[rtNum] = 0;

            PC = PC + 1;
        }
        

    }

    /*
    All type J instructions
    List of instructions:
    jal and j
    */
    else if (opcodeNum == 3 | opcodeNum == 2)
    {
        
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
        addressNum = (addressNum-4194304)/4;

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

    else {
        printf("Instruction not found at PC: %d", PC);
        endGame = 1;
    }
    //endGame = 0;
}

int main(int argc, char *args[])
{
    
    //SDL Window setup
	if (init(SCREEN_WIDTH, SCREEN_HEIGHT, argc, args) == 1) {
		
		return 0;
	}
	
	SDL_GetWindowSize(window, &width, &height);
    

    int data = add_data(AMOUNT_OF_DATA);
    
    for (int i = 0; i < AMOUNT_OF_DATA; i++)
    {
        printf("%d\n", mem_data[i]);
    }
    
    
    int text = add_instructions();
    
    if (!(data & text)) return 0;
    
    next_game_tick = SDL_GetTicks();
    //endGame=1;
    while (!endGame)
    {
        execute_ins();
    }
    
    //free renderer and all textures used with it
	SDL_DestroyRenderer(renderer);
	
	//Destroy window 
	SDL_DestroyWindow(window);

	//Quit SDL subsystems 
	SDL_Quit(); 
    

    return 0;
}

int init(int width, int height, int argc, char *args[]) {
    
    //Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {

		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		
		return 1;
	} 

    int i;
	
	for (i = 0; i < argc; i++) {
		
		//Create window	
		if(strcmp(args[i], "-f")) {
			
			SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer);
		
		} else {
		
			SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer);
		}
	}

    //create the screen sruface where all the elemnts will be drawn onto (ball, paddles, net etc)
	screen = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
	
	if (screen == NULL) {
		
		printf("Could not create the screen surfce! SDL_Error: %s\n", SDL_GetError());

		return 1;
	}

	//create the screen texture to render the screen surface to the actual display
	screen_texture = SDL_CreateTextureFromSurface(renderer, screen);

	if (screen_texture == NULL) {
		
		printf("Could not create the screen_texture! SDL_Error: %s\n", SDL_GetError());

		return 1;
	}

    	
	return 0;

}