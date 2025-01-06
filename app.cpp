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

// موقعیت بازیکن
int playerX = 0, playerY = 0;

// امتیاز بازی و تعداد حرکت‌ها
int score = 0;
int moves = 0;
int bombsUsed = 0;

// وضعیت اجرای بازی
bool isRunning = true;

SkillType currentSkill = SkillType::None;

// شعاع انفجار بمب‌ها
int explosionRadius = 1;

std::string playerName;

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
int main()
{
    cout << "wellcome to this game...";
    return 0;
}
