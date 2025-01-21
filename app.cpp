#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <windows.h> // برای استفاده از SetConsoleTextAttribute

using namespace std;
using namespace std::chrono;

steady_clock::time_point gameStartTime, gameEndTime;

enum class Elements
{
    Empty,
    Concrete,
    Wall,
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

Elements board[boardSize][boardSize];
int playerX = 0, playerY = 0;
int score = 0;
int moves = 0;
int bombsUsed = 0;
bool isRunning = true;
SkillType currentSkill = SkillType::None;
int explosionRadius = 1;
string playerName;

void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
// حذف ساختار struct برای بمب‌ها
int bombsX[maxBomb];
int bombsY[maxBomb];
int bombRemainingMoves[maxBomb];
int bombCount = 0;

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

        if (board[y][x] == Elements::Empty)
        {
            validPosition = true;
            break;
        }
    }

    if (validPosition)
    {
        bombsX[bombCount] = x;
        bombsY[bombCount] = y;
        bombRemainingMoves[bombCount] = 2; // مدت زمان فعال بودن بمب
        bombCount++;
        bombsUsed++;
        board[y][x] = Elements::Bomb;
        cout << "Bomb placed at (" << x << ", " << y << ")\n";
    }
    else
    {
        cout << "No suitable position for bomb found. Try again.\n";
    }
}

void explodeBomb(int bombIndex)
{
    int x = bombsX[bombIndex], y = bombsY[bombIndex];
    cout << "Bomb exploded at (" << x << ", " << y << ")" << endl;
    for (int i = -explosionRadius; i <= explosionRadius; i++)
    {
        for (int j = -explosionRadius; j <= explosionRadius; j++)
        {
            int nx = x + i, ny = y + j;
            if (nx >= 0 && nx < boardSize && ny >= 0 && ny < boardSize)
            {
                if (board[ny][nx] == Elements::Wall || board[ny][nx] == Elements::Enemy)
                {
                    board[ny][nx] = Elements::Empty;
                }
            }
        }
    }
}

void initializeBoard()
{
    for (int y = 0; y < boardSize; y++)
    {
        for (int x = 0; x < boardSize; x++)
        {
            board[y][x] = Elements::Empty;
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
            case Elements::Empty:
                setColor(7); // White text for empty
                cout << " - ";
                break;
            case Elements::Concrete:
                setColor(8); // Dark gray text for concrete
                cout << " ## ";
                break;
            case Elements::Wall:
                setColor(14); // Yellow for brick
                cout << " -_ ";
                break;
            case Elements::Player:
                setColor(10); // Green for player
                cout << " SS ";
                break;
            case Elements::Enemy:
                setColor(12); // Red for enemy
                cout << " EE ";
                break;
            case Elements::Exit:
                setColor(11); // Cyan for exit
                cout << " <> ";
                break;
            case Elements::Bomb:
                setColor(13); // Magenta for bomb
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
        if (board[y][x] == Elements::Empty)
            board[y][x] = Elements::Wall;

        x = rand() % boardSize;
        y = rand() % boardSize;
        if (board[y][x] == Elements::Empty)
            board[y][x] = Elements::Enemy;

        x = rand() % boardSize;
        y = rand() % boardSize;
        if (board[y][x] == Elements::Empty)
            board[y][x] = Elements::Concrete;
    }
    board[boardSize - 1][boardSize - 1] = Elements::Exit;
    board[playerY][playerX] = Elements::Player;
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
                board[y][x] = static_cast<Elements>(temp);
            }
        }

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
    board[playerY][playerX] = Elements::Empty;

    int oldX = playerX, oldY = playerY;

    if ((direction == 'W' || direction == 'w') && playerY > 0)
        playerY--;
    if ((direction == 'S' || direction == 's') && playerY < boardSize - 1)
        playerY++;
    if ((direction == 'A' || direction == 'a') && playerX > 0)
        playerX--;
    if ((direction == 'D' || direction == 'd') && playerX < boardSize - 1)
        playerX++;

    if (board[playerY][playerX] == Elements::Enemy)
    {
        cout << "You encountered an enemy!\n";
        score += 100;
        board[playerY][playerX] = Elements::Empty;
    }

    if (board[playerY][playerX] == Elements::Concrete)
    {
        cout << "You hit a wall!\n";
        playerX = oldX;
        playerY = oldY;
    }

    board[playerY][playerX] = Elements::Player;
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

    int enemyCount, brickCount, concreteCount;
    if (currentDifficulty == Difficulty::Easy)
    {
        enemyCount = boardSize * 2;
        brickCount = boardSize * 3;
        concreteCount = 6;
    }
    else if (currentDifficulty == Difficulty::Medium)
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
        if (board[y][x] == Elements::Empty)
            board[y][x] = Elements::Wall;
    }

    for (int i = 0; i < enemyCount; i++)
    {
        int x = rand() % boardSize;
        int y = rand() % boardSize;
        if (board[y][x] == Elements::Empty)
            board[y][x] = Elements::Enemy;
    }

    int placedConcreteCount = 0;
    while (placedConcreteCount < concreteCount)
    {
        int x = rand() % boardSize;
        int y = rand() % boardSize;
        if (board[y][x] == Elements::Empty)
        {
            board[y][x] = Elements::Concrete;
            placedConcreteCount++;
        }
    }

    board[boardSize - 1][boardSize - 1] = Elements::Exit;

    board[playerY][playerX] = Elements::Player;
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

#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <windows.h> // برای استفاده از SetConsoleTextAttribute

using namespace std;
using namespace std::chrono;

steady_clock::time_point gameStartTime, gameEndTime;

enum class Elements
{
    Empty,
    Concrete,
    Wall,
    Player,
    Enemy,
    Exit,
    Bomb
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

Elements board[boardSize][boardSize];
int playerX = 0, playerY = 0;
int score = 0;
int moves = 0;
int bombsUsed = 0;
bool isRunning = true;
int bombCount = 0;

// تغییرات: استفاده از آرایه‌ها به جای ساختار Bomb
int bombsX[maxBomb];
int bombsY[maxBomb];
int bombRemainingMoves[maxBomb];

void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
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

        if (board[y][x] == Elements::Empty)
        {
            validPosition = true;
            break;
        }
    }

    if (validPosition)
    {
        bombsX[bombCount] = x;
        bombsY[bombCount] = y;
        bombRemainingMoves[bombCount] = 2; // مدت زمان فعال بودن بمب
        bombCount++;
        bombsUsed++;
        board[y][x] = Elements::Bomb;
        cout << "Bomb placed at (" << x << ", " << y << ")\n";
    }
    else
    {
        cout << "No suitable position for bomb found. Try again.\n";
    }
}

void moveBombs()
{
    for (int i = 0; i < bombCount; i++)
    {
        if (bombRemainingMoves[i] > 0)
        {
            bombRemainingMoves[i]--;
        }
    }
}

void explodeBomb(int index)
{
    int x = bombsX[index], y = bombsY[index];
    if (board[y][x] == Elements::Bomb)
    {
        board[y][x] = Elements::Empty;
        cout << "Bomb at (" << x << ", " << y << ") exploded!\n";
        score -= 100; // کاهش امتیاز به ازای انفجار بمب
        bombCount--;
    }
}

void handleBombs()
{
    for (int i = 0; i < bombCount; i++)
    {
        if (bombRemainingMoves[i] <= 0)
        {
            explodeBomb(i);
        }
    }
}

void initializeBoard()
{
    for (int y = 0; y < boardSize; y++)
    {
        for (int x = 0; x < boardSize; x++)
        {
            board[y][x] = Elements::Empty;
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
            case Elements::Empty:
                setColor(7); // White text for empty
                cout << " - ";
                break;
            case Elements::Concrete:
                setColor(8); // Dark gray text for concrete
                cout << " ## ";
                break;
            case Elements::Wall:
                setColor(14); // Yellow for brick
                cout << " -_ ";
                break;
            case Elements::Player:
                setColor(10); // Green for player
                cout << " SS ";
                break;
            case Elements::Enemy:
                setColor(12); // Red for enemy
                cout << " EE ";
                break;
            case Elements::Exit:
                setColor(11); // Cyan for exit
                cout << " <> ";
                break;
            case Elements::Bomb:
                setColor(13); // Magenta for bomb
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
        if (board[y][x] == Elements::Empty)
            board[y][x] = Elements::Wall;

        x = rand() % boardSize;
        y = rand() % boardSize;
        if (board[y][x] == Elements::Empty)
            board[y][x] = Elements::Enemy;

        x = rand() % boardSize;
        y = rand() % boardSize;
        if (board[y][x] == Elements::Empty)
            board[y][x] = Elements::Concrete;
    }
    board[boardSize - 1][boardSize - 1] = Elements::Exit;
    board[playerY][playerX] = Elements::Player;
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

        outFile << bombCount << endl;
        for (int i = 0; i < bombCount; i++)
        {
            outFile << bombsX[i] << " " << bombsY[i] << " " << bombRemainingMoves[i] << endl;
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
                board[y][x] = static_cast<Elements>(temp);
            }
        }

        inFile >> bombCount;
        for (int i = 0; i < bombCount; i++)
        {
            inFile >> bombsX[i] >> bombsY[i] >> bombRemainingMoves[i];
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
    board[playerY][playerX] = Elements::Empty;

    int oldX = playerX, oldY = playerY;

    if ((direction == 'W' || direction == 'w') && playerY > 0)
        playerY--;
    if ((direction == 'S' || direction == 's') && playerY < boardSize - 1)
        playerY++;
    if ((direction == 'A' || direction == 'a') && playerX > 0)
        playerX--;
    if ((direction == 'D' || direction == 'd') && playerX < boardSize - 1)
        playerX++;

    if (board[playerY][playerX] == Elements::Enemy)
    {
        cout << "You encountered an enemy!\n";
        score += 100;
        board[playerY][playerX] = Elements::Empty;
    }

    if (board[playerY][playerX] == Elements::Concrete)
    {
        cout << "You hit a wall!\n";
        playerX = oldX;
        playerY = oldY;
    }

    board[playerY][playerX] = Elements::Player;
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

    int enemyCount, brickCount, concreteCount;
    if (currentDifficulty == Difficulty::Easy)
    {
        enemyCount = boardSize * 2;
        brickCount = boardSize * 3;
        concreteCount = 6;
    }
    else if (currentDifficulty == Difficulty::Medium)
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
        if (board[y][x] == Elements::Empty)
            board[y][x] = Elements::Wall;
    }

    for (int i = 0; i < enemyCount; i++)
    {
        int x = rand() % boardSize;
        int y = rand() % boardSize;
        if (board[y][x] == Elements::Empty)
            board[y][x] = Elements::Enemy;
    }

    for (int i = 0; i < concreteCount; i++)
    {
        int x = rand() % boardSize;
        int y = rand() % boardSize;
        if (board[y][x] == Elements::Empty)
            board[y][x] = Elements::Concrete;
    }

    board[boardSize - 1][boardSize - 1] = Elements::Exit;
    board[playerY][playerX] = Elements::Player;
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
            cout << "Select difficulty:\n1. Easy\n2. Medium\n3. Hard\n";
            int difficultyChoice;
            cin >> difficultyChoice;
            if (difficultyChoice == 1)
                currentDifficulty = Difficulty::Easy;
            else if (difficultyChoice == 2)
                currentDifficulty = Difficulty::Medium;
            else
                currentDifficulty = Difficulty::Hard;
            break;
        case 4:
            showGuide();
            break;
        case 5:
            // نمایش اسکوربورد اینجا
            break;
        case 6:
            isRunning = false;
            break;
        default:
            cout << "Invalid choice. Try again.\n";
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
                    cout << "Saving game...\n";
                    cout << "Enter save file name: ";
                    string filename;
                    cin >> filename;
                    saveGame(filename);
                    break;
                }

                movePlayer(move);

                // Move bombs
                moveBombs();
                handleBombs();

                // Check if the game has ended
                if (board[playerY][playerX] == Elements::Exit)
                {
                    gameEndTime = steady_clock::now();
                    // تابع محاسبه امتیاز می‌تواند در اینجا اضافه شود
                    break;
                }
            }
        }

    } while (choice != 6);

    return 0;
}
