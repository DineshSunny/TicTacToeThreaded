/*
CSCI310 Project 1: Multithreaded Tic Tac Toe game
Authors: Zak Groenewold, Dinesh Seveti, Samuel Arne
Date: 9/20/2024
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

char gameBoard[3][3];
const char PLAYER_A = 'X';
const char PLAYER_B = 'O';

//Prototype of all functions required
void resetBoard();
void printBoard();
void* playerAMove(void *ptr);
void* playerBMove(void *ptr);
char checkWinner();
void printWinner(char);
int checkFreeSpaces();

pthread_mutex_t lock; //Global lock for player move functions
pthread_cond_t cond;
int turn = 0; //0 for Player A, 1 for Player B

int main(){
    //initialize thread objects
    pthread_t playerA, playerB;
    srand(time(0)); //Seed for random number generation

    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    char winner = ' ';
    char response = ' ';

    do{
        winner = ' ';
        response = ' ';
        resetBoard();
        while(checkFreeSpaces() > 0 && winner == ' ')
        {
            //Create player threads
            pthread_create(&playerA, NULL, playerAMove, NULL);
            pthread_create(&playerB, NULL, playerBMove, NULL);

            pthread_join(playerA, NULL);
            pthread_join(playerB, NULL);
            winner = checkWinner();
        }

        printWinner(winner);

        printf("Run game again? (Y/N)\n");
        scanf("%c", &response);
        response = tolower(response);
    
    }while (response == 'y');

    printf("Thanks for playing!");
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
    return 0;
}

///////////////////////////////////////
//      Game Function Definitions    //
//////////////////////////////////////


void resetBoard()
{
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            gameBoard[i][j] = ' ';
        }
    }
}

void printBoard()
{
    printf("\n");
    printf(" %c | %c | %c ", gameBoard[0][0], gameBoard[0][1], gameBoard[0][2]);
    printf("\n---|---|---\n");
    printf(" %c | %c | %c ", gameBoard[1][0], gameBoard[1][1], gameBoard[1][2]);
    printf("\n---|---|---\n");
    printf(" %c | %c | %c ", gameBoard[2][0], gameBoard[2][1], gameBoard[2][2]);
    printf("\n");
}


void* playerAMove(void *ptr)  
{
    
    while(checkFreeSpaces() > 0 && checkWinner() == ' ')
    {
        pthread_mutex_lock(&lock);
        while (turn != 0) {
            pthread_cond_wait(&cond, &lock);
        }

        if (checkFreeSpaces() > 0) {
            int x, y;
            do {
                x = rand() % 3;
                y = rand() % 3;
            } while (gameBoard[x][y] != ' ');

            gameBoard[x][y] = PLAYER_A;
            printBoard();
            turn = 1;
            pthread_cond_signal(&cond);
        }
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}


void* playerBMove(void *ptr)
{
    while (checkFreeSpaces() > 0 && checkWinner() == ' ') {
        pthread_mutex_lock(&lock);

        while (turn != 1) {
            pthread_cond_wait(&cond, &lock);
        }

        if (checkFreeSpaces() > 0) {
            int x, y;
            do {
                x = rand() % 3;
                y = rand() % 3;
            } while (gameBoard[x][y] != ' ');

            gameBoard[x][y] = PLAYER_B;
            printBoard();
            turn = 0;
            pthread_cond_signal(&cond);
        }

        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

char checkWinner()
{
    //Check for row win
    for(int i = 0; i < 3; i++)
    {
        if(gameBoard[i][0] == gameBoard[i][1] && gameBoard[i][0] == gameBoard[i][2] && gameBoard[i][0] != ' ')
        {
            return gameBoard[i][0];
        }
    }
    //Check for column win
    for(int i = 0; i < 3; i++)
    {
        if(gameBoard[0][i] == gameBoard[1][i] && gameBoard[0][i] == gameBoard[2][i] && gameBoard[0][i] != ' ')
        {
            return gameBoard[0][i];
        }
    }
    //Check for diagonal win
    if(gameBoard[0][0] == gameBoard[1][1] && gameBoard[0][0] == gameBoard[2][2] && gameBoard[0][0] != ' ')
    {
        return gameBoard[0][0];
    }
    if(gameBoard[0][2] == gameBoard[1][1] && gameBoard[0][2] == gameBoard[2][0] && gameBoard[0][2] != ' ')
    {
        return gameBoard[0][2];
    }
    return ' ';
}

void printWinner(char winner)
{
    if(winner == PLAYER_A)
    {
        printf("Player A wins\n");
    }
    else if(winner == PLAYER_B)
    {
        printf("Player B wins\n");
    }
    else
    {
        printf("It's a Tie!\n");
    }
}

int checkFreeSpaces()
{
    int freeSpaces = 9;
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            if(gameBoard[i][j] != ' ')
            {
                freeSpaces--;
            }
        }
    }
    return freeSpaces;
}