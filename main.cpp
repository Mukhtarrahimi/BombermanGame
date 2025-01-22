#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>

using namespace std;

const int boardSize = 10;
const int maxBomb = 10;
char board[boardSize][boardSize];
int playerX = 0, playerY = 0;
int score = 0;
int moves = 0;
int bombsUsed = 0;
bool isRunning = true;
int currentDifficulty = 1; // Easy: 1, Medium: 2, Hard: 3

// Color function
void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// Initialize the game board
void initializeBoard()
{
    for (int y = 0; y < boardSize; y++)
    {
        for (int x = 0; x < boardSize; x++)
        {
            board[y][x] = ' '; // Empty spaces
        }
    }
}

// Print the game board
void printBoard()
{
    cout << "**********************" << endl;
    for (int y = 0; y < boardSize; y++)
    {
        cout << "*";
        for (int x = 0; x < boardSize; x++)
        {
            if (board[y][x] == ' ')
            {
                setColor(7);
                cout << "--";
            }
            else if (board[y][x] == '#')
            {
                setColor(8);
                cout << "##";
            }
            else if (board[y][x] == 'E')
            {
                setColor(12);
                cout << "EE";
            }
            else if (board[y][x] == 'P')
            {
                setColor(10);
                cout << "SS";
            }
            else if (board[y][x] == 'X')
            {
                setColor(14);
                cout << "XX";
            }
        }
        cout << "*" << endl;
    }
    cout << "**********************" << endl;
    cout << "Score: " << score << " | Moves: " << moves << " | Bombs Used: " << bombsUsed << endl;
}

// Place enemies, walls, and concrete based on difficulty
void generateGameElements()
{
    srand(time(0));

    // Place walls and enemies based on difficulty
    int enemyCount, wallCount;
    if (currentDifficulty == 1)
    { // Easy
        enemyCount = 2;
        wallCount = 5;
    }
    else if (currentDifficulty == 2)
    { // Medium
        enemyCount = 4;
        wallCount = 7;
    }
    else
    { // Hard
        enemyCount = 6;
        wallCount = 10;
    }

    // Place walls
    for (int i = 0; i < wallCount; i++)
    {
        int x = rand() % boardSize;
        int y = rand() % boardSize;
        if (board[y][x] == ' ')
        {
            board[y][x] = '#'; // Wall
        }
    }

    // Place enemies
    for (int i = 0; i < enemyCount; i++)
    {
        int x = rand() % boardSize;
        int y = rand() % boardSize;
        if (board[y][x] == ' ')
        {
            board[y][x] = 'E'; // Enemy
        }
    }

    // Set the exit
    board[boardSize - 1][boardSize - 1] = 'X';

    // Place player at the starting position
    board[playerY][playerX] = 'P';
}

// Move the player on the board
void movePlayer(char direction)
{
    board[playerY][playerX] = ' ';

    if (direction == 'W' || direction == 'w')
    {
        if (playerY > 0)
            playerY--;
    }
    else if (direction == 'S' || direction == 's')
    {
        if (playerY < boardSize - 1)
            playerY++;
    }
    else if (direction == 'A' || direction == 'a')
    {
        if (playerX > 0)
            playerX--;
    }
    else if (direction == 'D' || direction == 'd')
    {
        if (playerX < boardSize - 1)
            playerX++;
    }

    if (board[playerY][playerX] == 'E')
    {
        cout << "You encountered an enemy!\n";
        score += 100;
        board[playerY][playerX] = ' '; // Enemy defeated
    }

    if (board[playerY][playerX] == '#')
    {
        cout << "You hit a wall!\n";
        return;
    }

    board[playerY][playerX] = 'P'; // Player moves to new position
    moves++;
}

int main()
{
    cout << "Enter difficulty level (1: Easy, 2: Medium, 3: Hard): ";
    cin >> currentDifficulty;

    initializeBoard();
    generateGameElements();

    while (isRunning)
    {
        printBoard();
        char move;
        cout << "Enter move (W/A/S/D): ";
        cin >> move;

        if (move == 'Q' || move == 'q')
        {
            cout << "Quitting game...\n";
            break;
        }

        movePlayer(move);

        // Check if player has reached the exit
        if (board[playerY][playerX] == 'X')
        {
            cout << "You reached the exit! Game Over.\n";
            isRunning = false;
        }
    }

    return 0;
}
