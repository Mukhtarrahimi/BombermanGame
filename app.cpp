#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <chrono> // برای محاسبه زمان

using namespace std;
using namespace std::chrono; // استفاده از کلاس‌های زمان
// متغیرهای زمان‌بندی
steady_clock::time_point gameStartTime, gameEndTime;

// انواع عناصر بازی
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

// متغیرهای درجه سختی
enum class Difficulty
{
    Easy,
    Medium,
    Hard
};

Difficulty currentDifficulty = Difficulty::Medium;

// ثابت‌های بازی
const int BOARD_SIZE = 10;
const int MAX_BOMBS = 10;

// متغیرهای عمومی
TileType board[BOARD_SIZE][BOARD_SIZE];
int playerX = 0, playerY = 0;
int score = 0;
int moves = 0;
int bombsUsed = 0;
bool isRunning = true;
SkillType currentSkill = SkillType::None;
int explosionRadius = 1;
string playerName;

// ساختار بمب
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
                if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE)
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

Bomb bombs[MAX_BOMBS];
int bombCount = 0;

// مقداردهی اولیه صفحه بازی
void initializeBoard()
{
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            board[y][x] = TileType::Empty;
        }
    }
}

// نمایش صفحه بازی
void printBoard()
{
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            switch (board[y][x])
            {
            case TileType::Empty:
                cout << " . ";
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
}

// تولید عناصر بازی
void generateGameElements()
{
    srand(time(0));
    for (int i = 0; i < BOARD_SIZE / 2; i++)
    {
        int x = rand() % BOARD_SIZE;
        int y = rand() % BOARD_SIZE;
        if (board[y][x] == TileType::Empty)
            board[y][x] = TileType::Brick;

        x = rand() % BOARD_SIZE;
        y = rand() % BOARD_SIZE;
        if (board[y][x] == TileType::Empty)
            board[y][x] = TileType::Enemy;

        x = rand() % BOARD_SIZE;
        y = rand() % BOARD_SIZE;
        if (board[y][x] == TileType::Empty)
            board[y][x] = TileType::Concrete;
    }
    board[BOARD_SIZE - 1][BOARD_SIZE - 1] = TileType::Exit;
    board[playerY][playerX] = TileType::Player;
}

// ذخیره بازی
void saveGame(const string &filename)
{
    ofstream outFile(filename);
    if (outFile.is_open())
    {
        outFile << playerX << " " << playerY << endl;
        outFile << score << " " << moves << " " << bombsUsed << endl;
        for (int y = 0; y < BOARD_SIZE; y++)
        {
            for (int x = 0; x < BOARD_SIZE; x++)
            {
                outFile << static_cast<int>(board[y][x]) << " ";
            }
            outFile << endl;
        }
        outFile.close();
        cout << "Game saved successfully!" << endl;
    }
}

// بارگذاری بازی
void loadGame(const string &filename)
{
    ifstream inFile(filename);
    if (inFile.is_open())
    {
        inFile >> playerX >> playerY;
        inFile >> score >> moves >> bombsUsed;
        for (int y = 0; y < BOARD_SIZE; y++)
        {
            for (int x = 0; x < BOARD_SIZE; x++)
            {
                int temp;
                inFile >> temp;
                board[y][x] = static_cast<TileType>(temp);
            }
        }
        inFile.close();
        cout << "Game loaded successfully!" << endl;
    }
    else
    {
        cout << "Failed to load the game. File not found." << endl;
    }
}

// حرکت بازیکن
void movePlayer(char direction)
{
    board[playerY][playerX] = TileType::Empty;
    if (direction == 'W' && playerY > 0)
        playerY--;
    if (direction == 'S' && playerY < BOARD_SIZE - 1)
        playerY++;
    if (direction == 'A' && playerX > 0)
        playerX--;
    if (direction == 'D' && playerX < BOARD_SIZE - 1)
        playerX++;
    board[playerY][playerX] = TileType::Player;
    moves++;
}

// منوی اصلی
void showMenu()
{
    cout << "\n1. Start Game\n2. Load Game\n3. Set Difficulty\n4. Guide\n5. Scoreboard\n6. Exit\n";
    cout << "Enter your choice: ";
}

// راهنمای بازی
void showGuide()
{
    cout << "Welcome to the game!\n";
    cout << "Move with W, A, S, D.\nPlace bombs with B.\n";
    cout << "Reach the exit to win.\n";
    cout << "Team: Your Team Name\n";
}

// تنظیم درجه سختی
void setDifficulty()
{
    cout << "Choose Difficulty:\n1. Easy\n2. Medium\n3. Hard\n";
    int choice;
    cin >> choice;
    if (choice == 1)
    {
        cout << "Easy selected.\n";
    }
    else if (choice == 2)
    {
        cout << "Medium selected.\n";
    }
    else if (choice == 3)
    {
        cout << "Hard selected.\n";
    }
}

// جدول امتیازات
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

// محاسبه امتیاز
void calculateScore()
{
    double elapsedTime = duration_cast<seconds>(gameEndTime - gameStartTime).count(); // زمان سپری‌شده
    const double WT = 0.5, WM = 0.3, WB = 0.2;                                        // ضرایب وزن
    score = static_cast<int>(1000 / (1 + WT * elapsedTime + WM * moves + WB * bombsUsed));
    cout << "Game Over!\n";
    cout << "Your score: " << score << endl;

    // ذخیره امتیاز در فایل
    ofstream outFile("scoreboard.txt", ios::app);
    if (outFile.is_open())
    {
        outFile << playerName << ": " << score << endl;
        outFile.close();
    }
}

// شروع بازی: ثبت زمان شروع
void startGame()
{
    gameStartTime = steady_clock::now(); // ثبت زمان شروع بازی

    initializeBoard();
    generateGameElements();
    while (isRunning)
    {
        printBoard();
        cout << "Enter command (WASD to move, B to place bomb, Q to quit): ";
        char command;
        cin >> command;
        if (command == 'Q')
        {
            isRunning = false;
        }
        else if (command == 'B')
        {
            if (bombCount < MAX_BOMBS)
            {
                bombs[bombCount] = Bomb(playerX, playerY);
                bombCount++;
                bombsUsed++;
            }
            else
            {
                cout << "Maximum number of bombs reached!\n";
            }
        }
        else
        {
            movePlayer(command);
        }

        // به‌روزرسانی بمب‌ها
        for (int i = 0; i < bombCount; i++)
        {
            bombs[i].move();
        }

        // بررسی شرایط اتمام بازی (مثلاً رسیدن به درب خروج)
        if (board[playerY][playerX] == TileType::Exit)
        {
            cout << "Congratulations! You reached the exit.\n";
            isRunning = false;
        }
    }

    // ثبت زمان پایان بازی
    gameEndTime = steady_clock::now();

    // محاسبه امتیاز
    calculateScore();
}

// برنامه اصلی
int main()
{
    while (true)
    {
        showMenu();
        int choice;
        cin >> choice;
        switch (choice)
        {
        case 1:
            startGame();
            break;
        case 2:
            loadGame("savegame.txt");
            break;
        case 3:
            setDifficulty();
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
