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

int main()
{
    cout << "wellcome to this game...";
    return 0;
}
