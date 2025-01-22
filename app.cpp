#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <windows.h>

using namespace std;
using namespace std::chrono;

steady_clock::time_point gameStartTime, gameEndTime;

const int boardSize = 10;
const int maxBomb = 10;
int board[boardSize][boardSize];
int playerX = 0, playerY = 0;
int score = 0;
int moves = 0;
int bombsUsed = 0;
bool isRunning = true;
int currentSkill = 0;
int explosionRadius = 1;
string playerName;
int bombs[maxBomb][3];
int bombCount = 0;

void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void initializeBoard()
{
    for (int y = 0; y < boardSize; y++)
    {
        for (int x = 0; x < boardSize; x++)
        {
            board[y][x] = 0;
        }
    }
}

void printBoard()
{
    cout << "**********************" << endl;

    for (int y = 0; y < boardSize; y++)
    {
        cout << "*";

        for (int x = 0; x < boardSize; x++)
        {
            switch (board[y][x])
            {
            case 0:
                setColor(7);
                cout << "--";
                break;
            case 1:
                setColor(8);
                cout << "|##|";
                break;
            case 2:
                setColor(14);
                cout << "|_-|";
                break;
            case 3:
                setColor(10);
                cout << " SS| ";
                break;
            case 4:
                setColor(12);
                cout << " |EE| ";
                break;
            case 6:
                setColor(11);
                cout << " <> ";
                break;
            case 5:
                setColor(13);
                cout << " Bo ";
                break;
            }
        }

        cout << "*" << endl;
    }

    cout << "**********************" << endl;

    cout << "Score: " << score << " | Moves: " << moves << " | Bombs Used: " << bombsUsed << endl;
}

void generateGameElements()
{
    srand(time(0));

    for (int i = 0; i < boardSize / 2; i++)
    {
        int x = rand() % boardSize;
        int y = rand() % boardSize;

        if (board[y][x] == 0)
        {
            int randomValue = rand() % 3;
            if (randomValue == 0)
                board[y][x] = 2;
            else if (randomValue == 1)
                board[y][x] = 4;
            else
                board[y][x] = 1;
        }
    }

    board[boardSize - 1][boardSize - 1] = 6;

    board[playerY][playerX] = 3;
}

void setDifficulty()
{
    initializeBoard();

    int enemyCount = 0;
    int brickCount = 0;
    int concreteCount = 0;

    if (currentSkill == 1)
    {
        enemyCount = boardSize * 2;
        brickCount = boardSize * 3;
        concreteCount = 6;
    }
    else if (currentSkill == 2)
    {
        enemyCount = boardSize * 3;
        brickCount = boardSize * 4;
        concreteCount = 9;
    }
    else
    {
        enemyCount = boardSize * 4;
        brickCount = boardSize * 5;
        concreteCount = 12;
    }

    srand(time(0));

    for (int i = 0; i < brickCount; i++)
    {
        int x = rand() % boardSize;
        int y = rand() % boardSize;
        if (board[y][x] == 0)
        {
            board[y][x] = 2;
        }
    }

    for (int i = 0; i < enemyCount; i++)
    {
        int x = rand() % boardSize;
        int y = rand() % boardSize;
        if (board[y][x] == 0)
        {
            board[y][x] = 4;
        }
    }

    int placedConcreteCount = 0;
    while (placedConcreteCount < concreteCount)
    {
        int x = rand() % boardSize;
        int y = rand() % boardSize;
        if (board[y][x] == 0)
        {
            board[y][x] = 1;
            placedConcreteCount++;
        }
    }

    board[boardSize - 1][boardSize - 1] = 6;

    board[playerY][playerX] = 3;
}

void placeBomb()
{
    if (bombCount >= maxBomb)
    {
        cout << "Maximum number of bombs" << endl;
        return;
    }

    int x = playerX, y = playerY;
    bool validPosition = false;

    for (int attempts = 0; attempts < 10; attempts++)
    {
        x = rand() % boardSize;
        y = rand() % boardSize;

        if (board[y][x] == 0)
        {
            validPosition = true;
            break;
        }
    }

    if (validPosition)
    {
        bombs[bombCount][0] = x;
        bombs[bombCount][1] = y;
        bombs[bombCount][2] = 2;
        bombCount++;
        bombsUsed++;
        board[y][x] = 5;
        cout << "bomb placed (" << x << " : " << y << ")" << endl;
    }
    else
    {
        cout << "Cannot place bomb, try again" << endl;
    }
}

void saveGame(const string &filename)
{
    ofstream outFile(filename);
    if (outFile.is_open())
    {
        outFile << playerX << " " << playerY << endl;
        outFile << score << " " << moves << " " << bombsUsed << endl;
        for (int y = 0; y < boardSize; y++)
        {
            for (int x = 0; x < boardSize; x++)
            {
                outFile << board[y][x] << " ";
            }
            outFile << endl;
        }

        outFile << bombCount << endl;
        for (int i = 0; i < bombCount; i++)
        {
            outFile << bombs[i][0] << " " << bombs[i][1] << " " << bombs[i][2] << endl;
        }

        outFile.close();
        cout << "game saved successfully!" << endl;
    }
}

void loadGame(const string &filename)
{
    ifstream inFile(filename);
    if (inFile.is_open())
    {
        inFile >> playerX >> playerY;
        inFile >> score >> moves >> bombsUsed;
        for (int y = 0; y < boardSize; y++)
        {
            for (int x = 0; x < boardSize; x++)
            {
                inFile >> board[y][x];
            }
        }

        inFile >> bombCount;
        for (int i = 0; i < bombCount; i++)
        {
            inFile >> bombs[i][0] >> bombs[i][1] >> bombs[i][2];
        }

        inFile.close();
        cout << "Game loaded successfully!" << endl;
    }
    else
    {
        cout << "Invalid save file or file not found. Starting a new game." << endl;
        initializeBoard();
        generateGameElements();
    }
}

void movePlayer(char direction)
{
    board[playerY][playerX] = 0;

    int oldX = playerX, oldY = playerY;

    if ((direction == 'W' || direction == 'w') && playerY > 0)
        playerY--;
    if ((direction == 'S' || direction == 's') && playerY < boardSize - 1)
        playerY++;
    if ((direction == 'A' || direction == 'a') && playerX > 0)
        playerX--;
    if ((direction == 'D' || direction == 'd') && playerX < boardSize - 1)
        playerX++;

    if (board[playerY][playerX] == 4)
    {
        cout << "You encountered an enemy!" << endl;
        score += 100;
        board[playerY][playerX] = 0;
    }

    if (board[playerY][playerX] == 1)
    {
        cout << "You hit a wall!" << endl;
        playerX = oldX;
        playerY = oldY;
    }

    board[playerY][playerX] = 3;
    moves++;
}

void showMenu()
{
    cout << "1. Start Game" << endl;
    cout << "2. Load Game" << endl;
    cout << "3. Set Difficulty" << endl;
    cout << "4. Guide" << endl;
    cout << "5. Scoreboard" << endl;
    cout << "6. Exit" << endl;
    cout << "Enter your choice: ";
}

void showGuide()
{
    cout << "Welcome to the game!" << endl;
    cout << "Move with W, A, S, D." << endl;
    cout << "Place bombs with B." << endl;
    cout << "Reach the exit to win." << endl;
}



void calculateScore()
{
    double elapsedTime = duration_cast<seconds>(gameEndTime - gameStartTime).count();
    const double WT = 0.5, WM = 0.3, WB = 0.2;
    score = static_cast<int>(1000 / (1 + WT * elapsedTime + WM * moves + WB * bombsUsed));
    cout << "Game Over!" << endl;
    cout << "Your score: " << score << endl;

    ofstream outFile("scoreboard.txt", ios::app);
    if (outFile.is_open())
    {
        outFile << playerName << ": " << score << endl;
        outFile.close();
    }
}

void showScoreboard()
{
    ifstream inFile("scoreboard.txt");
    if (inFile.is_open())
    {
        string line;
        while (getline(inFile, line))
        {
            cout << line << endl;
        }
        inFile.close();
    }
    else
    {
        cout << "No scores available." << endl;
    }
}

int main()
{
    cout << "Enter player name: ";
    cin >> playerName;

    int choice;
    do
    {
        showMenu();
        cin >> choice;

        switch (choice)
        {
        case 1:
            gameStartTime = steady_clock::now();
            initializeBoard();
            generateGameElements();
            isRunning = true;
            break;
        case 2:
            cout << "Enter save file name: ";
            {
                string filename;
                cin >> filename;
                loadGame(filename);
                isRunning = true;
            }
            break;
        case 3:
            cout << "Select difficulty:" << endl;
            cout << "1. Easy" << endl;
            cout << "2. Medium" << endl;
            cout << "3. Hard" << endl;
            int difficultyChoice;
            cin >> difficultyChoice;
            if (difficultyChoice == 1)
                currentSkill = 1;
            else if (difficultyChoice == 2)
                currentSkill = 2;
            else
                currentSkill = 3;
            break;
        case 4:
            showGuide();
            break;
        case 5:
            showScoreboard();
            break;
        case 6:
            isRunning = false;
            break;
        default:
            cout << "Invalid choice. Try again." << endl;
        }

        if (isRunning)
        {
            while (isRunning)
            {
                printBoard();
                char move;
                cout << "Enter move (W/A/S/D): ";
                cin >> move;

                if (move == 'Q' || move == 'q')
                {
                    cout << "Saving game..." << endl;
                    cout << "Enter save file name: ";
                    string filename;
                    cin >> filename;
                    saveGame(filename);
                    break;
                }

                movePlayer(move);

                // Move bombs
                for (int i = 0; i < bombCount; i++)
                {
                    bombs[i][2]--;
                    if (bombs[i][2] == 0)
                    {
                        cout << "Bomb exploded at (" << bombs[i][0] << ", " << bombs[i][1] << ")" << endl;
                        for (int j = -explosionRadius; j <= explosionRadius; j++)
                        {
                            for (int k = -explosionRadius; k <= explosionRadius; k++)
                            {
                                int nx = bombs[i][0] + j, ny = bombs[i][1] + k;
                                if (nx >= 0 && nx < boardSize && ny >= 0 && ny < boardSize)
                                {
                                    if (board[ny][nx] == 2 || board[ny][nx] == 4)
                                    {
                                        board[ny][nx] = 0;
                                    }
                                }
                            }
                        }
                    }
                }

                // Check if the game has ended
                if (board[playerY][playerX] == 6)
                {
                    gameEndTime = steady_clock::now();
                    calculateScore();
                    break;
                }
            }
        }

    } while (choice != 6);

    return 0;
}
