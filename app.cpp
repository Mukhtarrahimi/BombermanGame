#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <windows.h> // برای استفاده از SetConsoleCursorPosition

using namespace std;
using namespace std::chrono;

steady_clock::time_point gameStartTime, gameEndTime;

enum class TileType
{
    Empty,
    Concrete,
    Brick,
    Player,
    Enemy,
    Exit,
    Bomb
};

enum class SkillType
{
    None,
    Remote,
    ExplosionRadius
};

enum class Difficulty
{
    Easy,
    Medium,
    Hard
};

Difficulty currentDifficulty = Difficulty::Medium;

const int boardSize = 10;
const int maxBomb = 10;

TileType board[boardSize][boardSize];
int playerX = 0, playerY = 0;
int score = 0;
int moves = 0;
int bombsUsed = 0;
bool isRunning = true;
SkillType currentSkill = SkillType::None;
int explosionRadius = 1;
string playerName;

struct Bomb
{
    int x, y;
    int remainingMoves;

    Bomb() : x(-1), y(-1), remainingMoves(0) {}

    Bomb(int x, int y) : x(x), y(y), remainingMoves(2) {}

    void move()
    {
        if (remainingMoves > 0)
        {
            remainingMoves--;
        }
        if (remainingMoves == 0)
        {
            explode();
        }
    }

    void explode()
    {
        cout << "Bomb exploded at (" << x << ", " << y << ")" << endl;
        for (int i = -explosionRadius; i <= explosionRadius; i++)
        {
            for (int j = -explosionRadius; j <= explosionRadius; j++)
            {
                int nx = x + i, ny = y + j;
                if (nx >= 0 && nx < boardSize && ny >= 0 && ny < boardSize)
                {
                    if (board[ny][nx] == TileType::Brick || board[ny][nx] == TileType::Enemy)
                    {
                        board[ny][nx] = TileType::Empty;
                    }
                }
            }
        }
    }
};

Bomb bombs[maxBomb];
int bombCount = 0;

// تابع gotoXY برای حرکت مکان نما به مختصات مشخص
void gotoXY(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void placeBomb()
{
    if (bombCount >= maxBomb)
    {
        cout << "Maximum number of bombs reached!\n";
        return;
    }

    int x = playerX, y = playerY;
    bool validPosition = false;

    for (int attempts = 0; attempts < 10; attempts++)
    {
        x = rand() % boardSize;
        y = rand() % boardSize;

        if (board[y][x] == TileType::Empty)
        {
            validPosition = true;
            break;
        }
    }

    if (validPosition)
    {
        bombs[bombCount] = Bomb(x, y);
        bombCount++;
        bombsUsed++;
        board[y][x] = TileType::Bomb;
        cout << "Bomb placed at (" << x << ", " << y << ")\n";
    }
    else
    {
        cout << "No suitable position for bomb found. Try again.\n";
    }
}

void initializeBoard()
{
    for (int y = 0; y < boardSize; y++)
    {
        for (int x = 0; x < boardSize; x++)
        {
            board[y][x] = TileType::Empty;
        }
    }
}

void printBoard()
{
    for (int y = 0; y < boardSize; y++)
    {
        for (int x = 0; x < boardSize; x++)
        {
            switch (board[y][x])
            {
            case TileType::Empty:
                cout << " - ";
                break;
            case TileType::Concrete:
                cout << " XX ";
                break;
            case TileType::Brick:
                cout << " -_ ";
                break;
            case TileType::Player:
                cout << " SS ";
                break;
            case TileType::Enemy:
                cout << " EE ";
                break;
            case TileType::Exit:
                cout << " <> ";
                break;
            case TileType::Bomb:
                cout << " Bo ";
                break;
            }
        }
        cout << endl;
    }

    cout << "Score: " << score << " | Moves: " << moves << " | Bombs Used: " << bombsUsed << endl;
}

void generateGameElements()
{
    srand(time(0));
    for (int i = 0; i < boardSize / 2; i++)
    {
        int x = rand() % boardSize;
        int y = rand() % boardSize;
        if (board[y][x] == TileType::Empty)
            board[y][x] = TileType::Brick;

        x = rand() % boardSize;
        y = rand() % boardSize;
        if (board[y][x] == TileType::Empty)
            board[y][x] = TileType::Enemy;

        x = rand() % boardSize;
        y = rand() % boardSize;
        if (board[y][x] == TileType::Empty)
            board[y][x] = TileType::Concrete;
    }
    board[boardSize - 1][boardSize - 1] = TileType::Exit;
    board[playerY][playerX] = TileType::Player;
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
                outFile << static_cast<int>(board[y][x]) << " ";
            }
            outFile << endl;
        }

        // Save bombs
        outFile << bombCount << endl;
        for (int i = 0; i < bombCount; i++)
        {
            outFile << bombs[i].x << " " << bombs[i].y << " " << bombs[i].remainingMoves << endl;
        }

        outFile.close();
        cout << "Game saved successfully!" << endl;
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
                int temp;
                inFile >> temp;
                board[y][x] = static_cast<TileType>(temp);
            }
        }

        // Load bombs
        inFile >> bombCount;
        for (int i = 0; i < bombCount; i++)
        {
            inFile >> bombs[i].x >> bombs[i].y >> bombs[i].remainingMoves;
        }

        inFile.close();
        cout << "Game loaded successfully!" << endl;
    }
    else
    {
        cout << "Invalid save file or file not found. Starting a new game.\n";
        initializeBoard();
        generateGameElements();
    }
}

void movePlayer(char direction)
{
    board[playerY][playerX] = TileType::Empty;

    int oldX = playerX, oldY = playerY;

    if ((direction == 'W' || direction == 'w') && playerY > 0)
        playerY--;
    if ((direction == 'S' || direction == 's') && playerY < boardSize - 1)
        playerY++;
    if ((direction == 'A' || direction == 'a') && playerX > 0)
        playerX--;
    if ((direction == 'D' || direction == 'd') && playerX < boardSize - 1)
        playerX++;

    if (board[playerY][playerX] == TileType::Concrete)
    {
        cout << "You hit a wall!\n";
        playerX = oldX;
        playerY = oldY;
    }

    board[playerY][playerX] = TileType::Player;
    moves++;
}

void showMenu()
{
    cout << "\n1. Start Game\n2. Load Game\n3. Set Difficulty\n4. Guide\n5. Scoreboard\n6. Exit\n";
    cout << "Enter your choice: ";
}

void showGuide()
{
    cout << "Welcome to the game!\n";
    cout << "Move with W, A, S, D.\nPlace bombs with B.\n";
    cout << "Reach the exit to win.\n";
}

void adjustGameForDifficulty()
{
    switch (currentDifficulty)
    {
    case Difficulty::Easy:
        cout << "Setting up game for Easy difficulty.\n";
        break;
    case Difficulty::Medium:
        cout << "Setting up game for Medium difficulty.\n";
        break;
    case Difficulty::Hard:
        cout << "Setting up game for Hard difficulty.\n";
        break;
    }

    initializeBoard();

    int enemyCount, brickCount;
    if (currentDifficulty == Difficulty::Easy)
    {
        enemyCount = boardSize * 2;
        brickCount = boardSize * 3;
    }
    else if (currentDifficulty == Difficulty::Medium)
    {
        enemyCount = boardSize * 3;
        brickCount = boardSize * 4;
    }
    else
    {
        enemyCount = boardSize * 4;
        brickCount = boardSize * 5;
    }

    srand(time(0));
    for (int i = 0; i < brickCount; i++)
    {
        int x = rand() % boardSize;
        int y = rand() % boardSize;
        if (board[y][x] == TileType::Empty)
            board[y][x] = TileType::Brick;
    }
    for (int i = 0; i < enemyCount; i++)
    {
        int x = rand() % boardSize;
        int y = rand() % boardSize;
        if (board[y][x] == TileType::Empty)
            board[y][x] = TileType::Enemy;
    }

    board[boardSize - 1][boardSize - 1] = TileType::Exit;
    board[playerY][playerX] = TileType::Player;
}

void calculateScore()
{
    double elapsedTime = duration_cast<seconds>(gameEndTime - gameStartTime).count();
    const double WT = 0.5, WM = 0.3, WB = 0.2;
    score = static_cast<int>(1000 / (1 + WT * elapsedTime + WM * moves + WB * bombsUsed));
    cout << "Game Over!\n";
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
        cout << "No scores available.\n";
    }
}

void startGame()
{
    gameStartTime = steady_clock::now();

    initializeBoard();
    generateGameElements();
    while (isRunning)
    {
        printBoard();
        cout << "Enter command (WASD to move, B to place bomb, Q to quit, M for menu): ";
        char command;
        cin >> command;

        if (command == 'Q')
        {
            isRunning = false;
        }
        else if (command == 'M')
        {
            isRunning = false;
            cout << "Returning to main menu...\n";
            return;
        }
        else if (command == 'B')
        {
            placeBomb();
        }
        else
        {
            movePlayer(command);
        }

        for (int i = 0; i < bombCount; i++)
        {
            bombs[i].move();
        }

        if (board[playerY][playerX] == TileType::Exit)
        {
            cout << "Congratulations! You reached the exit.\n";
            isRunning = false;
        }
    }

    if (!isRunning && board[playerY][playerX] != TileType::Exit)
    {
        cout << "Game over. You failed to reach the exit.\n";
    }

    gameEndTime = steady_clock::now();

    calculateScore();
}

int main()
{
    while (true)
    {
        showMenu();
        int choice;
        cin >> choice;

        if (cin.fail() || choice < 1 || choice > 6)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice. Please enter a number between 1 and 6.\n";
            continue;
        }

        switch (choice)
        {
        case 1:
            startGame();
            break;
        case 2:
            loadGame("savegame.txt");
            break;
        case 3:
            adjustGameForDifficulty();
            break;
        case 4:
            showGuide();
            break;
        case 5:
            showScoreboard();
            break;
        case 6:
            return 0;
        }
    }
    return 0;
}
