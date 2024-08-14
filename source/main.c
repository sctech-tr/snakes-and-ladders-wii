#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;



// Function to roll a six-sided die 
int rollDie() { return rand() % 6 + 1; } 

// Global variables to store positions of player1 and player2 
int player1 = 0, player2 = 0; 

// Function to print the board 
void printBoard() 
{ 
    // Logic to print a Snake and Ladder Game board 
    int board[101]; 
    for (int i = 1; i <= 100; i++) { 
        board[i] = i; 
    } 

    int alt = 0; // to switch between the alternate nature of the board 
    int iterLR = 101; // iterator to print from left to right 
    int iterRL = 80; // iterator to print from right to left 
    int val = 100;	 
    while (val--) { 
        if (alt == 0) { 
            iterLR--; 
            if (iterLR == player1) { 
                printf("#P1 "); 
            } 
            else if (iterLR == player2) { 
                printf("#P2 "); 
            } 
            else if (iterLR == 6 || iterLR == 77) { // Ladders
                printf("\033[0;32m%d\033[0m ", board[iterLR]); // Green
            } 
            else if (iterLR == 23 || iterLR == 45 || iterLR == 61 || iterLR == 65 || iterLR == 98) { // Snakes
                printf("\033[0;31m%d\033[0m ", board[iterLR]); // Red
            }
            else {
                printf("%d ", board[iterLR]); 
            }

            if (iterLR % 10 == 1) { 
                printf("\n\n"); 
                alt = 1; 
                iterLR -= 10; 
            } 
        } 
        else { 
            iterRL++; 
            if (iterRL == player1) { 
                printf("#P1 "); 
            } 
            else if (iterRL == player2) { 
                printf("#P2 "); 
            } 
            else if (iterRL == 6 || iterRL == 77) { // Ladders
                printf("\033[0;32m%d\033[0m ", board[iterRL]); // Green
            } 
            else if (iterRL == 23 || iterRL == 45 || iterRL == 61 || iterRL == 65 || iterRL == 98) { // Snakes
                printf("\033[0;31m%d\033[0m ", board[iterRL]); // Red
            }
            else {
                printf("%d ", board[iterRL]); 
            }

            if (iterRL % 10 == 0) { 
                printf("\n\n"); 
                alt = 0; 
                iterRL -= 30; 
            } 
        } 
        if (iterRL == 10) 
            break; 
    } 
    printf("\n"); 
} 

// Function to move the player 
int movePlayer(int currentPlayer, int roll) 
{ 
    int newPosition = currentPlayer + roll; 
    // Define the positions of snakes and ladders on the board 
    int snakesAndLadders[101]; 

    for (int i = 0; i <= 100; i++) { 
        snakesAndLadders[i] = 0; 
    } 
	
    // here positive weights represent a ladder
    // and negative weights represent a snake. 
    snakesAndLadders[6] = 40; 
    snakesAndLadders[23] = -10; 
    snakesAndLadders[45] = -7; 
    snakesAndLadders[61] = -18; 
    snakesAndLadders[65] = -8; 
    snakesAndLadders[77] = 5; 
    snakesAndLadders[98] = -10; 

    int newSquare = newPosition + snakesAndLadders[newPosition]; 

    if (newSquare > 100) { 
        return currentPlayer; // Player cannot move beyond square 100 
    } 

    return newSquare; 
} 

int main(int argc, char **argv) {
    // Some initializing stuff
    // -------------------------------------------
    VIDEO_Init();
    WPAD_Init();
    rmode = VIDEO_GetPreferredMode(NULL);
    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(false);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
    printf("\x1b[2;0H");
    // -------------------------------------------
    srand(time(0)); // Initialize random seed 
    int currentPlayer = 1; 
    int won = 0; 
    int roll;

    printf("Hi! Welcome to the Snakes and Ladders game!\n"); 
    printf("Press A to start game...\n");
    while (1) {
        WPAD_ScanPads();
        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A) {
            printf("\033[2J"); // clear the screen
            printf("\x1b[2;0H");
            break;
        }
    }
    while (!won) {
        WPAD_ScanPads();

        printf("\nPlayer %d, please press A to roll the die...", currentPlayer);

        while (1) {
            WPAD_ScanPads();
            u32 pressed = WPAD_ButtonsDown(0);

            if (pressed & WPAD_BUTTON_A) {
                roll = rollDie();
                break;
            }

            VIDEO_WaitVSync();
        }

        printf("You rolled a %d.\n", roll);

        if (currentPlayer == 1) {
            player1 = movePlayer(player1, roll);
            printf("Player 1 is now at square %d.\n\n", player1);
            printBoard();
            if (player1 == 100) {
                printf("Player 1 wins!\n");
                won = 1;
            }
        }
        else {
            player2 = movePlayer(player2, roll);
            printf("Player 2 is now at square %d.\n\n", player2);
            printBoard();
            if (player2 == 100) {
                printf("Player 2 wins!\n");
                won = 1;
            }
        }

        // Switch to the other player
        currentPlayer = (currentPlayer == 1) ? 2 : 1;

        VIDEO_WaitVSync();
    }

    return 0; 
}
