#include <iostream>
#include <vector>
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

int main()
{
    cout << "wellcome to this game...";
    return 0;
}
