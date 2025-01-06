#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

// عناصر مورد نیاز بازی

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

const int BOARD_SIZE = 10;

TileType board[BOARD_SIZE][BOARD_SIZE]; // ساخت ارایه برای تخته بازی

int playerX = 0, playerY = 0;

int score = 0;
int moves = 0;
int bombsUsed = 0;

bool isRunning = true;
SkillType currentSkill = SkillType::None;
int explosionRadius = 1;
std::string playerName;

// تابع برای تعریف ایجاد تخته بازی
void initializeBoard()
{
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            board[i][j] = TileType::Empty;
        }
    }
}

// تابع برای نام بازیکن
void getPlayerName()
{
    cout << "Enter your name: ";
    cin >> playerName;
    cout << "Welcome, " << playerName << "!" << endl;
}

// تابع قرار دادن بازیکن روی تخته
void placePlayer()
{
    board[playerX][playerY] = TileType::Player;
}

// تابع برای چاپ تخته بازی
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
// تابع تولید عناصر بازی
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

// تابع برای حرکت
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
// تابع برای ذخیره بازی
void saveGame()
{
    ofstream outFile("savegame.txt");
    if (outFile.is_open())
    {
        outFile << playerX << " " << playerY << " " << moves << " " << score << endl;
        for (int i = 0; i < BOARD_SIZE; ++i)
        {
            for (int j = 0; j < BOARD_SIZE; ++j)
            {
                outFile << static_cast<int>(board[i][j]) << " ";
            }
            outFile << endl;
        }
        outFile.close();
        cout << "Game saved successfully!" << endl;
    }
}

// تابع برای بارگذاری بازی
void loadGame()
{
    ifstream inFile("savegame.txt");
    if (inFile.is_open())
    {
        inFile >> playerX >> playerY >> moves >> score;
        for (int i = 0; i < BOARD_SIZE; ++i)
        {
            for (int j = 0; j < BOARD_SIZE; ++j)
            {
                int tile;
                inFile >> tile;
                board[i][j] = static_cast<TileType>(tile);
            }
        }
        inFile.close();
        cout << "Game loaded successfully!" << endl;
    }
    else
    {
        cout << "No saved game found." << endl;
    }
}
// چاپ وضعیت بازیکن بعد از هر حرکت
void printPlayerStats()
{
    cout << "Player: " << playerName << " | Score: " << score << " | Moves: " << moves << endl;
}

// تابع منوی بازی
void showMenu()
{
    const char *menuItems[] = {
        "Start Game",
        "Load Game",
        "Set Difficulty",
        "Guide",
        "Scoreboard",
        "Exit"};
    const int menuSize = sizeof(menuItems) / sizeof(menuItems[0]);

    cout << "\nMenu Options:\n";
    for (int i = 0; i < menuSize; ++i)
    {
        cout << i + 1 << ". " << menuItems[i] << "\n";
    }
    cout << "Enter your choice: ";
}

int main()
{
    // درخواست نام بازیکن
    getPlayerName();

    // مقداردهی اولیه تخته
    initializeBoard();

    // قرار دادن بازیکن در ابتدا
    placePlayer();

    // چاپ وضعیت تخته
    printBoard();

    // تولید عناصر بازی
    generateGameElements();
    // تابع برای ذخیره بازی
    saveGame();

    // تابع برای بارگذاری بازی
    loadGame();
    printPlayerStats();
    showMenu();

    return 0;
}
