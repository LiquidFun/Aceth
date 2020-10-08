#include <iostream>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <fstream>
#include <string>
#include <thread>
#include <array>
#include <algorithm>
#include <random>
#include <ctime>
#include <time.h>
#include <math.h>
#include <sstream>
#include <cstring>

using namespace std;

#ifdef _WIN32

#include "concol.h"
#include <conio.h>
#include <windows.h>

#else // Unix, add alternatives for windows methods used elsewhere
#include <unistd.h>
#include <sys/ioctl.h>

#include "ansicol.h"
#include "cursor.h"
#define FALSE false
void CopyFile(string from, string to, bool discard) {
    ifstream  src(from, std::ios::binary);
    ofstream  dst(to, std::ios::binary);

    dst << src.rdbuf();
}
void Sleep(long long ms) {
    sleep(ms/1000.0); 
}
uint64_t GetTickCount()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)(ts.tv_nsec / 1000000) + ((uint64_t)ts.tv_sec * 1000ull);
}
#endif

using namespace eku;

//#include "mingw.thread.h"


void menu(); //Prints menu and depending on which menu has been chosen runs few other functions
inline void allTheInfoFunctions(); //Checks if any of the info buttons has been pressed (easier to add new info buttons as you just have to actualize one function)
void getMap(); //Saves map from .txt file into array
void printMap(); //Prints map with all the colors
void movePlayer(); //Moves player if he presses wasd
inline void generalInfo(); //Prints out general info below the map
void reqHelp(); //Prints out commands after pressing h
void interact(); //Does something when pressing e
void goToXY(int x,int y); //Goes to the coordinates ON THE SCREEN to x and y
void defineColor(int x, int y);
void refreshMap();
void printEnemyStats(); //Prints a wiki of enemy stats with a quick search option
void printMapCharInfo(); //Prints a wiki of all character types available on the map
void createChar(); //Creates the player and initializes all his stats
void updateCharStats(); //Checks if you have equipped other items and updates your char stats accordingly
void initSpells(); //Loads all the spells from the respective spell file
void initInventory(); //Loads all the items from the respective item file
void initStartItems(); //Gives the player his starting items (usually tunica and boots)
void printInventory(); //Prints the inventory upon pressing i with an option to equip and unequip items
void loadEnemies(); //Loads all the enemies form the respective enemy file
void battle(int xE, int yE); //Simulates the battle with options to choose what to do next
void checkForEnemy(); //Checks all 4 directions from the player if there is an enemy nearby, if found calls battle
void respawn(); //Upon dying the player is re-spawned to his starting position
void checkForLevel(); //Checks of the player has enough xp to level up and if found so, the level increases
void printShop(); //Prints the shop
void shopSellInventory();
void shopBuyInventory();
void shopPotionShop();
void shopSpellShop();
void drinkPotion(); //Checks if one of the potion keys has been presses and consumes a potion.
void randomChestDrop(); //Calculates a random chest drop and adds it to the player inventory
void randomTreasureDrop(); //Gives 500-100 gold loot
void countEnemiesOnMap(); //Counts enemies on the map every 10 moves
int selectRandomItem(); //Gives a random item, used for random chest drop
void reopenMenu(); //Reopens the menu while in-game after pressing m
void spawnRandomEnemies();
void spawnInCaveEnemies(string spawnDef);
void despawnRandomLowLevelEnemies();
void enemyMovement();
void calculateTime();
void checkForEnemyThread();
void printPopup();
void loadNewMap(); //
void checkForDoor();
void loadMapFirstTime();
void saveMap();
void saveSlot();
void saveCharToFile();
void saveItemsToFile();
void saveMapToFile();
void loadSaveFile();
void loadSaveSlot();
void devOnlyDecreaseVisibility();
void toggleInterfaceInfo();
void toggleShowTips();
void clearScreen();
bool checkIfNear(char lookingFor, int x, int y); //Looks for specific characters on the map, mainly used for checking if there is water nearby for swamp creatures


time_t checkTime = time(0);
time_t start = time(0) - 60;
time_t start2 = time(0) - 60;
time_t start3 = time(0) - 60;


int startTime = time(0);
int loadedTime = 2400000; //When loading a game the time gone, x % 24 (if it divides by 24 it resets to 0) this has to be so high because when subtracting manually from the clock it decreases this number
int inGameTime = 12;
int oldTime = -1;


string loadMapFile = "/maps/map";
string loadMapFileTxt = ".txt";
const int SIZE_OF_MAP = 500;
char mapL [SIZE_OF_MAP][SIZE_OF_MAP];
char mapLe [SIZE_OF_MAP][SIZE_OF_MAP];


const string loadItemsFile = "data/itemList52.tsv";
const string loadSpellFile = "data/spellList13.tsv";
const string loadEnemyFile = "data/enemyList77.tsv";
const int SOY = 10;
const int SOX = 15;
const int ITEM_AMOUNT = 52;
const int ENEMY_AMOUNT = 77;
const int SPELL_AMOUNT = 13;
const int SMALL_POTION = 5;
const int ITEMS_IN_SHOP = 6;
const int SPELLS_IN_SHOP = 4;

int mapAmount;
string* mapName;
int* mapCoordX;
int* mapCoordY;

char enemyChar[ENEMY_AMOUNT];
string enemyName[ENEMY_AMOUNT];
int enemyAttack[ENEMY_AMOUNT];
int enemyDefense[ENEMY_AMOUNT];
int enemyHp[ENEMY_AMOUNT];
int enemyWorth[ENEMY_AMOUNT];
int enemyXPWorth[ENEMY_AMOUNT];

int enemyLimitPerMap = 1300;
int enemyCountOnMap = 0;
int doEvery10Moves = 10;


vector<char> forestEnemy0_1    {'w', 's', 'b'};
vector<char> forestEnemy2_4    {'7', '&', 'l', 'f', '=', 'p'};
vector<char> forestEnemy5_8    {'7', '&', 'l', 'f', 'Z', '#', 'k', 'o', 'x', '-', 'J'};
vector<char> forestEnemy9_12   {'Z', '#', 'k', 'o', 'x', '-', 'O', '+', 'u', 'S', '<', 'W'};
vector<char> forestEnemy13_15  {'Z', 'O', '+', 'u', 'S', '<', 'B', 'U', 'D', '5', '2'};
vector<char> forestEnemy16_18  {'B', 'U', '5', 'Q', '@', 'T', '2'};
vector<char> forestEnemy19_20  {'B', '5', 'Q', '@', 'T', '2', 'X'};
vector<char> nearWaterEnemy0_3    {'c', 'n', 'C'};
vector<char> nearWaterEnemy4_8    {'!', 'y', 'C'};
vector<char> nearWaterEnemy9_14   {'!', 'F', '1'};
vector<char> nearWaterEnemy15_20  {'F', '1', 'K'};
vector<char> snowEnemy0_2     {'G', '=', 'w', 's'};
vector<char> snowEnemy3_7     {'t', '=', 'g', '~', 'Y', ':'};
vector<char> snowEnemy8_11    {'%', 'u', '+', 'O', '~', 'W', 'Y'};
vector<char> snowEnemy12_15   {'+', 'O', 'Q'};
vector<char> snowEnemy16_20   {'X', 'W', '4'};


int itemCount[ITEM_AMOUNT];
string itemName[ITEM_AMOUNT];
int itemAttack[ITEM_AMOUNT];
int itemDefense[ITEM_AMOUNT];
int itemPrice[ITEM_AMOUNT];
char itemType[ITEM_AMOUNT];
bool itemEquiped[ITEM_AMOUNT];
int itemLevel[ITEM_AMOUNT];


int shopItemCount[ITEM_AMOUNT];
int shopSpellCount[SPELL_AMOUNT];
int smallPotionCount;
int mediumPotionCount;
int greaterPotionCount;


bool spellLearned[SPELL_AMOUNT];
string spellName[SPELL_AMOUNT];
int spellManaCost[SPELL_AMOUNT];
int spellDamage[SPELL_AMOUNT];
double spellAddAttack[SPELL_AMOUNT];
double spellPercentAttack[SPELL_AMOUNT];
double spellAddDefense[SPELL_AMOUNT];
double spellPercentDefense[SPELL_AMOUNT];
int spellHeal[SPELL_AMOUNT];
int spellPrice[SPELL_AMOUNT];
int spellLevel[SPELL_AMOUNT];


string charName;
int charAttack;
int charDefense;
int charMaxHp;
int charCurrHp;
int charXP;
int charXPreq = 10;
int charLevel;
int charMoney;
int charCurrMana;
int charMaxMana;
int charSmallPotionCount = 1;
int charMediumPotionCount = 0;
int charGreaterPotionCount = 0;
int baseCharAttack;
int baseCharDefense;
int baseCharMaxHp;
int charAditHp = 0;


int timeForCheckEnemy = time(0);
int enemyDirThread = 0;
bool allowThreads = false;
int doorCoordX, doorCoordY;
int mainCoordX, mainCoordY;
int V = 4; //Visibility
bool exitSaveSlot = false;
bool showTips = true;
string currentSaveSlot = "1";
bool toggleMoreInterfaceInfo = false;
int menuS = 0;
int x, y;
int myRan = time(0), myRandom2;
int playerCoord[2];
char keyPress;
string gameName = "Aceth";


string menuOption[] = {"New Game", "Load Game", "Exit Game"};
string battleOption[] = {"Attack", "Use Spell", "Run (-25% money)"};

int main()
{
    while (true)
    {
        loadEnemies();
        initSpells();
        initInventory();
        menu();
        if (menuS == 2) {return 0;}
        while (true)
        {
            updateCharStats();
            devOnlyDecreaseVisibility();
            printMap();
            generalInfo();
            allowThreads = true;
            refreshMap();
            keyPress = getch();
            allowThreads = false;
            movePlayer();
            checkForEnemy();
            checkForLevel();
            updateCharStats();
            interact();
            printInventory();
            drinkPotion();
            updateCharStats();
            countEnemiesOnMap();
            spawnRandomEnemies();
            saveCharToFile();
            saveItemsToFile();
            saveMap();
            allTheInfoFunctions();
            despawnRandomLowLevelEnemies();
            reopenMenu();
            if (menuS == 2) return 0;
        }
    }
}

void getMap()
{
    ifstream getMapInput("saves/SaveSlot_"+currentSaveSlot+loadMapFile+loadMapFileTxt);
    ifstream getMapEInput("saves/SaveSlot_"+currentSaveSlot+loadMapFile+"e"+loadMapFileTxt);
    if (getMapEInput.is_open()) {/*nothing, didn't know how to check if it isn't open*/}
    else memset(mapLe, '.', SIZE_OF_MAP*SIZE_OF_MAP);
    for (int y = 0; y < SIZE_OF_MAP; y++)
    {
                for (int x = 0; x < SIZE_OF_MAP; x++)
                {
                    getMapInput >> mapL[x][y];
                    getMapEInput >> mapLe[x][y];
                }
    }
    getMapInput.close();
    getMapEInput.close();
}

void printMap() //reworked to print 15x10 field of map //REWORKED TO NOT BE LAGGY WOW... WITH 1 LINE gg wp //reworked now its 31x21 with the player in the middle
{
    goToXY(0, 0);
    y = max(playerCoord[1] - SOY, 0);
    while (y < min(playerCoord[1] + SOY + 1, SIZE_OF_MAP))
    {
                x = max(playerCoord[0] - SOX, 0);
                while (x < min(playerCoord[0] + SOX + 1, SIZE_OF_MAP))
                {
                    if (playerCoord[0] - x >= V * 2 - 1 || x - playerCoord[0] >= V * 2 - 1 || playerCoord[1] - y >= V + 1 || y - playerCoord[1] >= V + 1)
                    {
                        if (mapL[x][y] == '`')
                            cout << black;
                        else
                            cout << gray;
                        if (playerCoord[0] - x >= V * 3 - 1 || x - playerCoord[0] >= V * 3 - 1 || playerCoord[1] - y >= V * 1.5 + 1 || y - playerCoord[1] >= V * 1.5 + 1)
                            settextcolor(black);
                        cout << mapL[x][y];
                    }
                    else if (mapLe[x][y] != '.' && mapLe[x][y] != '#')
                    {
                        cout << red << mapLe[x][y];
                    }
                    else if (x == playerCoord[0] && y == playerCoord[1])
                    {
                        cout << aqua << 'P';
                    }
                    else
                    {
                        defineColor(x, y);
                        cout << mapL[x][y];
                    }
                    setcolor(dark_white, black);
                    x++;
                }
        cout << "        |  ";
        if (max(playerCoord[1] - SOY, 0) == y - 0) cout << dark_white << "Your Name: " << aqua << charName << dark_white << "  ";
        if (max(playerCoord[1] - SOY, 0) == y - 1) cout << dark_white << "Your Attack: " << red << charAttack << "A" << dark_white << "  ";
        if (max(playerCoord[1] - SOY, 0) == y - 2) cout << dark_white << "Your Defense: " << green << charDefense << "D" << dark_white << "  ";
        if (max(playerCoord[1] - SOY, 0) == y - 3) cout << dark_white << "Your HP: " << yellow << charCurrHp << dark_white << "/" << yellow << charMaxHp << dark_white << "  ";
        if (max(playerCoord[1] - SOY, 0) == y - 4) cout << dark_white << "Your Mana: " << blue << charCurrMana << dark_white << "/" << blue << charMaxMana << dark_white << "  ";
        if (max(playerCoord[1] - SOY, 0) == y - 5) cout << dark_white << "Your Money: " << white << charMoney << dark_white << "  ";
        if (max(playerCoord[1] - SOY, 0) == y - 6) cout << dark_white << "Your Level: " << yellow << charLevel << dark_white << "  ";
        if (max(playerCoord[1] - SOY, 0) == y - 7) cout << dark_white << "Your XP: " << white << charXP << dark_white << "/" << white << charXPreq << dark_white << "   ";
        if (max(playerCoord[1] - SOY, 0) == y - 8) cout << dark_white << "In-game time: " << white << inGameTime << "   ";
        if (max(playerCoord[1] - SOY, 0) == y - 10) {cout << white << charSmallPotionCount << dark_white << " small healing potion";
            if (charSmallPotionCount > 1 || charSmallPotionCount == 0)
            {
                cout << "s";
            }
            cout << " (+"<<red<<"5"<<dark_white<<"HP)   ";
        }
        if (max(playerCoord[1] - SOY, 0) == y - 11) {cout << white << charMediumPotionCount << dark_white << " medium healing potion";
            if (charMediumPotionCount > 1 || charMediumPotionCount == 0)
            {
                cout << "s";
            }
            cout << " (+"<<red<<"25"<<dark_white<<"HP)   ";
        }
        if (max(playerCoord[1] - SOY, 0) == y - 12) {cout << white << charGreaterPotionCount << dark_white << " greater healing potion";
            if (charGreaterPotionCount > 1 || charGreaterPotionCount == 0)
            {
                cout << "s";
            }
            cout << " (+"<<red<<"125"<<dark_white<<"HP)   ";
        }
        if (max(playerCoord[1] - SOY, 0) == y - 14 && toggleMoreInterfaceInfo == true) {cout << dark_white << playerCoord[0] << " x coord       ";}
        if (max(playerCoord[1] - SOY, 0) == y - 15 && toggleMoreInterfaceInfo == true) {cout << dark_white << playerCoord[1] << " y coord       ";}
        if (max(playerCoord[1] - SOY, 0) == y - 16 && toggleMoreInterfaceInfo == true) {cout << dark_white << enemyCountOnMap << " amount of enemies       ";}
        if (max(playerCoord[1] - SOY, 0) == y - 17 && toggleMoreInterfaceInfo == true) {cout << dark_white << V << " visibility       ";}
        cout << endl;
        y++;
    }
}

void devOnlyDecreaseVisibility()
{
    if (keyPress == '[')
    {
        loadedTime--;
        printMap();
    }
    if (keyPress == ']')
    {
        loadedTime++;
        printMap();
    }

}

void movePlayer()
{
    char check;
    switch (keyPress)
    {
        case 'W':
        case 'w':
            check = mapL[playerCoord[0]][playerCoord[1] - 1];
            if (check != '#' && check != 'r' && check != '-' && mapLe[playerCoord[0]][playerCoord[1] - 1] == '.'/* && playerCoord[1] - 1 > 9*/)
            {
                playerCoord[1] -= 1;
            }
            break;
        case 'A':
        case 'a':
            check = mapL[playerCoord[0] - 1][playerCoord[1]];
            if (check != '#' && check != 'r' && check != '-' && mapLe[playerCoord[0] - 1][playerCoord[1]] == '.'/* && playerCoord[0] - 1 > 14*/)
            {
                playerCoord[0] -= 1;
            }
            break;
        case 'S':
        case 's':
            check = mapL[playerCoord[0]][playerCoord[1] + 1];
            if (check != '#' && check != 'r' && check != '-' && mapLe[playerCoord[0]][playerCoord[1] + 1] == '.'/* && playerCoord[1] + 1 < SIZE_OF_MAP - 9*/)
            {
                playerCoord[1] += 1;
            }
            break;
        case 'D':
        case 'd':
            check = mapL[playerCoord[0] + 1][playerCoord[1]];
            if (check != '#' && check != 'r' && check != '-' && mapLe[playerCoord[0] + 1][playerCoord[1]] == '.'/* && playerCoord[0] + 1 < SIZE_OF_MAP - 14*/)
            {
                playerCoord[0] += 1;
            }
            break;
    }
}

void refreshMap()
{
    while (!kbhit())
    {
        calculateTime();
        enemyMovement();
        if (checkTime + 5 < time(0))
        {
            allowThreads = false;
            /* Sleep(50); */
            checkTime = time(0);
            printMap();
            allowThreads = true;
        }
    }
}

inline void generalInfo()
{
    cout << "                                                                                ";
    if (showTips == true) cout << gray << "\nPress "<<white<<"h"<<gray<<" to show a list of keys you can press while ingame." << dark_white;
    cout << "                                                                                ";
}

inline void allTheInfoFunctions()
{
    reqHelp();
    printEnemyStats();
    printMapCharInfo();
    toggleInterfaceInfo();
    toggleShowTips();
}

void reqHelp()
{
    if (keyPress == 'h' || keyPress == 'H')
    {
        clearScreen();
        cout << "You CAN NOT use them here, do that while ingame.\n\n";
        cout << dark_white << "Press " << red << "w" << dark_white << "/" << red << "a" << dark_white << "/" << red << "s" << dark_white << "/" << red << "d" << dark_white << " to move\n";
        cout << dark_white << "Press " << red << "e" << dark_white << " to interact with objects\n";
        cout << dark_white << "Press " << red << "i" << dark_white << " to open your inventory\n";
        cout << dark_white << "Press " << red << "m" << dark_white << " to open the menu\n";
        cout << dark_white << "Press " << red << "," << dark_white << " to show additional interface info (irrelevant for gameplay)\n";
        cout << dark_white << "Press " << red << "." << dark_white << " to toggle tips\n";
        cout << dark_white << "Press " << red << "1" << dark_white << " to drink a small potion\n";
        cout << dark_white << "Press " << red << "2" << dark_white << " to drink a medium potion\n";
        cout << dark_white << "Press " << red << "3" << dark_white << " to drink a greater potion\n";
        cout << dark_white << "Press " << red << "j" << dark_white << " to see a list of all the enemies with a search function\n";
        cout << dark_white << "Press " << red << "k" << dark_white << " to see a list of all the different terrains on the map\n";
        cout << dark_white;
        getch();
        clearScreen();
    }
}

void printEnemyStats()
{
    if (keyPress == 'j' || keyPress == 'J')
    {
        while (true)
        {
            int PES = -1;
            clearScreen();
            cout << dark_white << "Enemies are marked with " << red << "red" << dark_white << " on the map.\n\n";
            for (int i = 0; i < ENEMY_AMOUNT; i++)
            {
                cout << dark_white << "Symbol:" << red << enemyChar[i] << dark_white << ", name: " << enemyName[i] << ". " << red << enemyAttack[i] << "A" << dark_white << ", "<< green << enemyDefense[i] << "D" << dark_white << ", " << yellow << "HP: " << enemyHp[i] << dark_white << ", Worth: " << white << enemyWorth[i] << dark_white << ", XP: " << white << enemyXPWorth[i] << endl;
            }

            //keyPress = getch();
            cout << dark_white << "\n\nSearch for symbol (" << yellow << "ESC" << dark_white << " to exit): ";
            keyPress = getch();
            if (keyPress == 27)
            {
                clearScreen();
                keyPress = ')';
                break;
            }
            else
            {
                for (int i = 0; i < ENEMY_AMOUNT; i++)
                {
                   if (enemyChar[i] == keyPress)
                   {
                       PES = i;
                   }
                }
                if (PES != -1)
                {
                    clearScreen();
                    cout << dark_white << "Symbol:" << red << enemyChar[PES] << dark_white << ", name: " << enemyName[PES] << ". " << red << enemyAttack[PES] << "A" << dark_white << ", "<< green << enemyDefense[PES] << "D" << dark_white << ", " << yellow << "HP: " << enemyHp[PES] << dark_white << ", Worth: " << white << enemyWorth[PES] << dark_white << ", XP: " << white << enemyXPWorth[PES] << endl;
                    getch();
                }
                else
                {
                    clearScreen();
                    cout << dark_white << "No such creature found!";
                    getch();
                }

            }
        }
    }
}

void printMapCharInfo()
{
    if (keyPress == 'k' || keyPress == 'K')
    {
        clearScreen();
        cout << aqua << "P "<<dark_white<<"- Player (you)\n";
        cout << yellow << "z "<<dark_white<<"- Scroll shop\n";
        cout << yellow << "C "<<dark_white<<"- Chest\n";
        cout << yellow << "S "<<dark_white<<"- Shop\n";
        cout << yellow << "p "<<dark_white<<"- Potion shop\n";
        cout << dark_white << ". "<<dark_white<<"- Ground\n";
        cout << white << ", "<<dark_white<<"- Snow\n";
        cout << white << "t "<<dark_white<<"- Snowy forest\n";
        cout << dark_aqua << "i "<<dark_white<<"- Ice\n";
        cout << dark_red << "^ "<<dark_white<<"- Camp-fire\n";
        cout << blue << "- "<<dark_white<<"- Water (unwalkable)\n";
        cout << gray << "r "<<dark_white<<"- Rock (unwalkable)\n";
        cout << dark_yellow << "h "<<dark_white<<"- Building/house (enterable)\n";
        cout << purple << "T "<<dark_white<<"- Treasure 500-1000 Gold\n";
        cout << dark_white << "+ "<<dark_white<<"- Path\n";
        cout << dark_green << "f "<<dark_white<<"- Forest (slower)\n";
        cout << dark_green << "b "<<dark_white<<"- Bush (slower)\n";
        cout << dark_aqua << "w "<<dark_white<<"- Swamp (slower)\n";
        //cout << gray << "~ "<<dark_white<<"- Stony ground, usually in caves\n";
        cout << gray << "# "<<dark_white<<"- Wall (unwalkable)\n";
        cout << dark_yellow << "s "<<dark_white<<"- Ship (enterable)\n";
        cout << dark_yellow << "d "<<dark_white<<"- Door (enterable)\n";
        cout << dark_purple << "c "<<dark_white<<"- Cave (enterable)\n";
        cout << black << "` "<<dark_white<<"- Void (unwalkable)\n";
        getch();
        clearScreen();
    }
}

void toggleInterfaceInfo()
{
    if (keyPress == ',' || keyPress == '<')
    {
        if (toggleMoreInterfaceInfo == false)
        {
            toggleMoreInterfaceInfo = true;
        }
        else if (toggleMoreInterfaceInfo == true)
        {
            toggleMoreInterfaceInfo = false;
        }
        clearScreen();
    }
}

void goToXY(int x, int y)
{
#ifdef _WIN32
    HANDLE h=GetStdHandle(STD_OUTPUT_HANDLE);
    COORD c;
    c.X=x;
    c.Y=y;
    SetConsoleCursorPosition(h,c);
#else
    cout << "\033[" << y << ";" << x << "f";
#endif
}

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[H\033[J";
#endif
}

void toggleShowTips()
{
    if (keyPress == '.' || keyPress == '>')
    {
        if (showTips == false)
        {
            showTips = true;
        }
        else if (showTips == true)
        {
            showTips = false;
        }
        clearScreen();
    }
}

void updateCharStats()
{
    charMaxHp = baseCharMaxHp + charAditHp * pow(1.17, charLevel);
    charAttack = baseCharAttack;
    charDefense = baseCharDefense;

    for (int i = 0; i < ITEM_AMOUNT;i++)
    {
        if (itemEquiped[i] == 1)
        {
            charAttack += itemAttack[i];
            charDefense += itemDefense[i];
        }
    }
}

void createChar()
{
    while (true)
    {
        clearScreen();
        cout << "\n\n\n\n\n"<<"                What is thy name traveller?\n                ";
        settextcolor(aqua);
        getline(cin, charName);
        if (charName.length() > 20)
        {
            cout << white << "Your name is too long!";
        }
        else {break;}
        getch();
        settextcolor(dark_white);
    }

    default_random_engine randomGenerator(time(0) * time(0) * time(0) * time(0));
    uniform_int_distribution<int> ranDefense1(2, 4);
    uniform_int_distribution<int> ranHp(9, 12);
    baseCharAttack = 2;
    baseCharDefense = ranDefense1(randomGenerator);
    baseCharMaxHp = ranHp(randomGenerator);
    charCurrHp = baseCharMaxHp;
    charMaxHp = baseCharMaxHp;
    charMaxMana = 10;
    charCurrMana = charMaxMana;
    charMoney = 0;

    initStartItems();
}

void menu()
{
    concolinit();
    while (true)
    {
        setcolor(dark_white, black);
        clearScreen();
        cout << "                                    MENU";
        cout << "\n\n\n\n\n                                  ";
        if (menuS == 0){setcolor(black, yellow);}
        cout << menuOption[0] << endl;
        setcolor(dark_white, black);
        cout << "                                 ";
        if (menuS == 1){setcolor(black, yellow);}
        cout << menuOption[1] << endl;
        setcolor(dark_white, black);
        cout << "                                 ";
        if (menuS == 2){setcolor(black, yellow);}
        cout << menuOption[2] << endl;
        setcolor(dark_white, black);
        keyPress = getch();
        if ((keyPress == 'w' || keyPress == 'W') && menuS > 0) menuS--;
        if ((keyPress == 's' || keyPress == 'S') && menuS < 2) menuS++;
        if (keyPress == 27) menuS = 2;
        else if (keyPress == '\r' || keyPress == '\n' || keyPress == 'e' || keyPress == 'E')
        {
            if (menuS == 2)
            {
                clearScreen();
                cout << "\n\n\n\n\n\nThank you for playing " << red << gameName << dark_white << "!" << black;
                getch();
                break;
            }
            if (menuS == 1)
            {
                loadSaveSlot();
                if (exitSaveSlot == false)
                {
                    startTime = time(0);
                    checkTime = time(0);
                    loadSaveFile();
                    getMap();
                    /*thread EM (enemyMovement);
                    EM.detach();
                    thread CT (calculateTime);
                    CT.detach();*/
                    clearScreen();
                    break;
                }
                exitSaveSlot = false;
            }
            if (menuS == 0)
            {
                saveSlot();
                if (exitSaveSlot == false)
                {
                    startTime = time(0) - 360;
                    checkTime = time(0);
                    initInventory();
                    createChar();
                    loadMapFirstTime();
                    /*thread EM (enemyMovement);
                    EM.detach();
                    thread CT (calculateTime);
                    CT.detach();*/
                    break;
                }
                exitSaveSlot = false;
            }
        }
    }

}

void reopenMenu()
{
    if (keyPress == 'm' || keyPress == 'M' || keyPress == 27)
    {
        int selectedOption = 1;
        printMap();
        printPopup();
        while (true)
        {
            goToXY(18, 5);
            cout << dark_white << "Are you sure you want to quit to the main";
            goToXY(18, 6);
            cout << "menu (games are saved automatically)?";
            goToXY(18, 8);
            if (selectedOption == 0) setcolor(black,yellow);
            cout << "Yes";
            setcolor(dark_white,black);
            goToXY(18, 9);
            if (selectedOption == 1) setcolor(black,yellow);
            cout << "No";
            setcolor(dark_white,black);

            keyPress = getch();
            if ((keyPress == 'w' || keyPress == 'W') && selectedOption > 0) {selectedOption--;}
            if ((keyPress == 's' || keyPress == 'S') && selectedOption < 1) {selectedOption++;}
            if (keyPress == 27 || keyPress == 'Q' || keyPress == 'q')
            {
                clearScreen();
                break;
            }
            if (keyPress == 'e' || keyPress == 'E' || keyPress == '\n' || keyPress == '\r')
            {
                if (selectedOption == 0)
                {
                    menu();
                    break;
                }
                if (selectedOption == 1)
                {
                    clearScreen();
                    break;
                }
            }
        }
    }
}

void defineColor(int x, int y)
{
    setcolor(dark_white, black);
    if (mapL[x][y] == 'Q')settextcolor(dark_purple);
    else if (mapL[x][y] == 'C')settextcolor(yellow);
    else if (mapL[x][y] == 'S')settextcolor(yellow);
    else if (mapL[x][y] == 'S')settextcolor(yellow);
    else if (mapL[x][y] == 'p')settextcolor(yellow);
    else if (mapL[x][y] == '.')settextcolor(dark_white);
    else if (mapL[x][y] == ',')settextcolor(white);
    else if (mapL[x][y] == 't')settextcolor(white);
    else if (mapL[x][y] == 'i')settextcolor(dark_aqua);
    else if (mapL[x][y] == '^')settextcolor(dark_red);
    else if (mapL[x][y] == '-')settextcolor(dark_blue);
    else if (mapL[x][y] == 'r')settextcolor(gray);
    else if (mapL[x][y] == 'h')settextcolor(dark_yellow);
    else if (mapL[x][y] == 'T')settextcolor(purple);
    else if (mapL[x][y] == '+')settextcolor(dark_white);
    else if (mapL[x][y] == 'b')settextcolor(dark_green);
    else if (mapL[x][y] == 'w')settextcolor(dark_aqua);
    else if (mapL[x][y] == 'f')settextcolor(dark_green);
    else if (mapL[x][y] == 'd')settextcolor(dark_yellow);
    else if (mapL[x][y] == 'c')settextcolor(dark_purple);
    else if (mapL[x][y] == 's')settextcolor(dark_yellow);
    else if (mapL[x][y] == '~')settextcolor(gray);
    else if (mapL[x][y] == '#')settextcolor(gray);
    else if (mapL[x][y] == '`')settextcolor(black);
}

void initSpells()
{
    //memset (spellLearned, 0, SPELL_AMOUNT);
    ifstream spellInput(loadSpellFile);
    for (int i = 0; i < SPELL_AMOUNT; i++)
    {
        spellInput >> spellLearned[i] >> spellName[i] >> spellManaCost[i] >> spellDamage[i] >> spellAddAttack[i] >> spellPercentAttack[i] >> spellAddDefense[i] >> spellPercentDefense[i] >> spellHeal[i] >> spellPrice[i] >> spellLevel[i];
    }
    spellInput.close();
}

void initInventory()
{
    memset (itemCount, 0, ITEM_AMOUNT);
    memset (itemEquiped, 0, ITEM_AMOUNT);
    ifstream itemInput(loadItemsFile);
    for (int i = 0; i < ITEM_AMOUNT; i++)
    {
        itemInput >> itemName[i] >> itemAttack[i] >> itemDefense[i] >> itemPrice[i] >> itemType[i] >> itemLevel[i];
    }
    itemInput.close();
}

void initStartItems()
{
    itemCount[1] = 1;
    itemCount[3] = 1;
    itemEquiped[1] = 1;
    itemEquiped[3] = 1;
}

void printInventory()
{
    if (keyPress == 'i' || keyPress == 'I')
    {
        int selectedItem = 0;
        int selectedItem_i = 0;
        char selectedItemType;
        while (true)
        {
            int numItem = 0;
            int difItems = 0;
            clearScreen();
            concolinit();
            for (int i = 0; i < ITEM_AMOUNT; i++)
            {
                if (itemCount[i] > 0)
                {
                    setcolor(dark_white, black);
                    if (itemEquiped[i] == 1){settextcolor(cyan);}
                    cout << itemCount[i] << " of " << dark_white << "(" << red << itemType[i] << dark_white << ")" ;
                    if (itemEquiped[i] == 1){settextcolor(cyan);}
                    if (selectedItem == numItem){setcolor(black, yellow);selectedItem_i = i;selectedItemType = itemType[i];}
                    cout << itemName[i];
                    setcolor(dark_white, black);
                    if (itemEquiped[i] == 1){settextcolor(cyan);}
                    if (itemAttack[i] != 0){cout << red << " " << itemAttack[i] << "A" << dark_white << ",";}
                    if (itemDefense[i] != 0){cout << green << " " << itemDefense[i] << "D" << dark_white << ",";}
                    if (itemEquiped[i] == 1){settextcolor(cyan);}
                    cout << " Price: " << itemPrice[i]/* << ", it's type: " << itemType[i]*/ << endl;
                    setcolor(dark_white, black);
                    numItem++;
                }
            }
            cout << "\n____________________________________________________________________\n";
            if (showTips == true) cout << gray << "w/s for up/down. e to equip an item. i to exit.\nCyan items are equiped items. The yellow item is the selected item.\nYou can only have one item equiped of each type. (S, H, C, L, B)";
            keyPress = getch();
            allTheInfoFunctions();
            if (keyPress == 'i' || keyPress == 'I' || keyPress == 'q' || keyPress == 'Q' || keyPress == 27)
            {
                keyPress = ')';
                clearScreen();
                break;
            }
            for (int i = 0; i < ITEM_AMOUNT;i++)
            {
                if (itemCount[i] > 0) {difItems++;}
            }
            if ((keyPress == 'w' || keyPress == 'W') && selectedItem > 0) {selectedItem--;}
            if ((keyPress == 's' || keyPress == 'S') && selectedItem < difItems - 1) {selectedItem++;}
            if (keyPress == 'e' || keyPress == 'E')
            {
                if (itemEquiped[selectedItem_i] == 0)
                {
                    for (int i = 0; i < ITEM_AMOUNT;i++)
                    {
                        if (itemEquiped[i] == 1 && itemType[i] == selectedItemType)
                        {
                            itemEquiped[i] = 0;
                        }
                    }
                itemEquiped[selectedItem_i] = 1;
                }
                else if (itemEquiped[selectedItem_i] == 1) {itemEquiped[selectedItem_i] = 0;}
            }
        }
    }
}

void loadEnemies()
{
    ifstream enemyInput(loadEnemyFile);
    for (int i = 0; i < ENEMY_AMOUNT; i++)
    {
        enemyInput >> enemyChar[i] >> enemyName[i] >> enemyAttack[i] >> enemyDefense[i] >> enemyHp[i] >> enemyWorth[i] >> enemyXPWorth[i];
    }
    enemyInput.close();
}

void interact()
{
    if (keyPress == 'e' || keyPress == 'E' || keyPress == '\n' || keyPress == '\r')
    {
        if (mapL[playerCoord[0]][playerCoord[1]] == 'S') printShop();
        else if (mapL[playerCoord[0]][playerCoord[1]] == 'C') randomChestDrop();
        else if (mapL[playerCoord[0]][playerCoord[1]] == 'T') randomTreasureDrop();
        else if (mapL[playerCoord[0]][playerCoord[1]] == 'd') loadNewMap();
        else if (mapL[playerCoord[0]][playerCoord[1]] == 'h') loadNewMap();
        else if (mapL[playerCoord[0]][playerCoord[1]] == 'c') loadNewMap();
        else if (mapL[playerCoord[0]][playerCoord[1]] == 'Q') loadNewMap();
        else if (mapL[playerCoord[0]][playerCoord[1]] == 'p') shopPotionShop();
        else if (mapL[playerCoord[0]][playerCoord[1]] == 'z') shopSpellShop();
    }
}

void checkForEnemy()
{
    if (mapLe[playerCoord[0]][playerCoord[1] - 1] != '.') //Above
    {
        allowThreads = false;
        battle(playerCoord[0], playerCoord[1] - 1);
    }
    if (mapLe[playerCoord[0] - 1][playerCoord[1]] != '.') //To the left
    {
        allowThreads = false;
        battle(playerCoord[0] - 1, playerCoord[1]);
    }
    if (mapLe[playerCoord[0]][playerCoord[1] + 1] != '.') //Below
    {
        allowThreads = false;
        battle(playerCoord[0], playerCoord[1] + 1);
    }
    if (mapLe[playerCoord[0] + 1][playerCoord[1]] != '.') //To the right
    {
        allowThreads = false;
        battle(playerCoord[0] + 1, playerCoord[1]);
    }
    if (mapLe[playerCoord[0]][playerCoord[1]] != '.') //On top
    {
        allowThreads = false;
        battle(playerCoord[0], playerCoord[1]);
    }
}

void battle(int xE, int yE)
{
    default_random_engine randomGenerator(time(0) * time(0) * time(0) * time(0));
    uniform_int_distribution<int> ranRisky(0, 1);
    uniform_real_distribution<double> ranRNG(0.80, 1.20);
    string infoMessages[5] = {" ", " ", " ", " ", " "};
    string stringDamage;
    int enemy_i = 0;
    int selectedAttack = 0;
    double rootA, plusA;
    double nonRndedDmg;
    int damage = 0;
    for (int i = 0; i < ENEMY_AMOUNT; i++)
    {
        if (enemyChar[i] == mapLe[xE][yE])
        {
            enemy_i = i;
        }
    }
    int enemyCurrHp = enemyHp[enemy_i];
    while (true)
    {


        if (enemyCurrHp <= 0)
        {
            cout << green << "\nYou won!\n";
            getch();
            charMoney += enemyWorth[enemy_i];
            charXP += enemyXPWorth[enemy_i];
            mapLe[xE][yE] = '.';
            clearScreen();
            break;
        }
        if (charCurrHp <= 0)
        {
            respawn();
            break;
        }
        ////****



        ////****
        while (true)
        {
            clearScreen();
            cout << "\n\n";
            for (int i = 0; (12 - charName.length() / 2) > i; i++) cout << " ";
            cout << cyan << charName << dark_white << "                                         ";
            for (int i = 0; (15 - enemyName[enemy_i].length() / 2 - charName.length() / 2) > i; i++) cout << " ";
            cout << enemyName[enemy_i] << endl;
            cout << "         " << red << charAttack << "A" << dark_white << ", " << green << charDefense << "D";
            cout << "                                                  " << red << enemyAttack[enemy_i] << "A" << dark_white << ", " << green << enemyDefense[enemy_i] << "D\n";
            cout << "          " << yellow << charCurrHp << dark_white << "/" << yellow << charMaxHp;
            cout << "                                                    " << yellow << enemyCurrHp << dark_white << "/" << yellow << enemyHp[enemy_i] <<  dark_white;
            cout << endl << "________________________________________________________________________________";
            for (int k = 0; k < 25; k++){cout << " ";}
            cout << "|\n";
            for (int k = 0; k < 25; k++){cout << " ";}
            cout << "|\n";
            for (int i = 0; i < 3; i++)
            {
                if (i == selectedAttack) {setcolor(black, yellow);}
                cout << battleOption[i];
                setcolor(dark_white, black);
                for (int k = 0; k < 25 - battleOption[i].length(); k++)
                {
                    cout << " ";
                }
                cout << "|        ";
                if (i >= 2) {cout << gray;}
                cout << infoMessages[i] << endl;
            }
            for (int i = 3; i < 5; i++)
            {
                for (int k = 0; k < 25; k++)
                {
                    cout <<dark_white<< " ";
                }
                cout << "|        " << gray << infoMessages[i] << endl;
            }
            Sleep(300);
            keyPress = getch();
            if ((keyPress == 'w' || keyPress == 'W') && selectedAttack > 0) {selectedAttack--;}
            if ((keyPress == 's' || keyPress == 'S') && selectedAttack < 2) {selectedAttack++;}
            if (keyPress == 'e' || keyPress == 'E' || keyPress == '\n' || keyPress == '\r')
            {
                if (selectedAttack == 0) break;
                if (selectedAttack == 2)
                {
                    size_t found = loadMapFile.find("castle");
                    if (found != string::npos)
                    {
                        printPopup();
                        goToXY(18, 5);
                        cout << red << "You can not run away while fighting in a";
                        goToXY(18, 6);
                        cout << red << "castle!" << dark_white;
                        getch();
                    }
                    else break;
                }
            }
        }
        if (keyPress == 'e' || keyPress == 'E' || keyPress == '\n' || keyPress == '\r')
        {

            if (selectedAttack == 0)
            {
                rootA = (-(pow(charAttack, 1.0/2.0))) + 11.5;
                plusA = pow(charAttack, 1.0/3.15) + 0.33;
                nonRndedDmg = (-pow(enemyDefense[enemy_i], 1.0/(rootA + 1.0))) + plusA;
                damage = lround(nonRndedDmg * ranRNG(randomGenerator) * (charAttack / plusA) * 1.6);
                enemyCurrHp -= damage;

                infoMessages[4] = infoMessages[3];
                infoMessages[3] = infoMessages[2];
                infoMessages[2] = infoMessages[1];
                infoMessages[1] = infoMessages[0];

                std::ostringstream ss;
                ss << damage;
                infoMessages[0] = "You did "+ss.str()+" damage";
            }
            if (selectedAttack == 2)
            {
                clearScreen();
                charMoney *= 0.75;
                damage = 0;
                break;
            }

            ///////**
            clearScreen();
            cout << "\n\n";
            for (int i = 0; (12 - charName.length() / 2) > i; i++) cout << " ";
            cout << cyan << charName << dark_white << "                                         ";
            for (int i = 0; (15 - enemyName[enemy_i].length() / 2 - charName.length() / 2) > i; i++) cout << " ";
            cout << enemyName[enemy_i] << endl;
            cout << "         " << red << charAttack << "A" << dark_white << ", " << green << charDefense << "D";
            cout << "                                                  " << red << enemyAttack[enemy_i] << "A" << dark_white << ", " << green << enemyDefense[enemy_i] << "D\n";
            cout << "          " << yellow << charCurrHp << dark_white << "/" << yellow << charMaxHp;
            cout << "                                                    " << yellow << enemyCurrHp << dark_white << "/" << yellow << enemyHp[enemy_i] <<  dark_white;
            cout << endl << "________________________________________________________________________________";
            for (int k = 0; k < 25; k++){cout << " ";}
            cout << "|\n";
            for (int k = 0; k < 25; k++){cout << " ";}
            cout << "|\n";
            ///////**
            for (int i = 0; i < 3; i++)
            {
                if (i == selectedAttack) {setcolor(black, yellow);}
                cout << battleOption[i];
                setcolor(dark_white, black);
                for (int k = 0; k < 25 - battleOption[i].length(); k++)
                {
                    cout << " ";
                }
                cout << "|        ";
                if (i >= 2) {cout << gray;}
                cout << infoMessages[i] << endl;
            }
            for (int i = 3; i < 5; i++)
            {
                for (int k = 0; k < 25; k++)
                {
                    cout <<dark_white<< " ";
                }
                cout << "|        " << gray << infoMessages[i] << endl;
            }
            Sleep(300);
            //////////////////////ENEMY//////////////////////////



            if (enemyCurrHp > 0)
            {
                rootA = (-pow(enemyAttack[enemy_i], 1.0/2.0)) + 11.5;
                plusA = pow(enemyAttack[enemy_i], 1.0/3.15) + 0.33;
                nonRndedDmg = (-pow(charDefense, 1.0/(rootA + 1.0))) + plusA;
                damage = lround(nonRndedDmg * ranRNG(randomGenerator) * (enemyAttack[enemy_i] / plusA) * 1.6);
                charCurrHp -= damage;

                infoMessages[4] = infoMessages[3];
                infoMessages[3] = infoMessages[2];
                infoMessages[2] = infoMessages[1];
                infoMessages[1] = infoMessages[0];
                //stringDamage = std::to_string(damage);
                //infoMessages[0] = enemyName[enemy_i]+" did "+stringDamage+" damage";
                std::ostringstream ss;
                ss << damage;
                infoMessages[0] = enemyName[enemy_i]+" did "+ss.str()+" damage";
            }
        }
    }
}

void respawn()
{
    printPopup();
    goToXY(18, 5);
    cout << red << "You died and lost half of your money as";
    goToXY(18, 6);
    cout << red <<  "well as half of your current XP!" << dark_white;

    loadMapFile = "/maps/map";
    charMoney /= 2;
    charXP /= 2;
    playerCoord[0] = 259;
    playerCoord[1] = 253;
    charCurrHp = charMaxHp;
    getMap();
    getch();
    clearScreen();
}

void checkForLevel()
{
    if (charXP >= charXPreq && charLevel < 20)
    {
        int optionSelected = 0;
        int diffHP = charMaxHp - charCurrHp;
        charLevel++;
        charXP -= charXPreq;
        charXPreq *= 1.32;
        baseCharMaxHp *= 1.25;
        printMap();
        printPopup();
        while (true)
        {
            goToXY(18, 5);
            setcolor(white, black);
            cout << "            You are now level " << cyan << charLevel << white << "!";
            goToXY(18, 8);
            cout << "Choose which atribute of your character you";
            goToXY(18, 9);
            cout << "want to improve:";
            goToXY(18, 11);
            if (optionSelected == 0)
            {
                setcolor(black, yellow);
                cout << "Additional " << pow(1.17, charLevel) << " HP (Scales with levels).";
            }
            else cout << "Additional " << cyan << pow(1.17, charLevel) << white << " HP (Scales with levels).";
            setcolor(white, black);

            goToXY(18, 12);
            if (optionSelected == 1)
            {
                setcolor(black, yellow);
                cout << "Additional 3 Mana.";
            }
            else cout << "Additional "<<cyan<<"3"<<white<<" Mana.";
            setcolor(white, black);

            goToXY(18, 13);
            if (optionSelected == 2)
            {
                setcolor(black, yellow);
                cout << "Additional 1 Attack";
            }
            else cout << "Additional " << cyan << "1" << white << " Attack";
            setcolor(white, black);

            goToXY(18, 14);
            if (optionSelected == 3)
            {
                setcolor(black, yellow);
                cout << "Additional 2 Defense";
            }
            else cout << "Additional " << cyan << "2" << white << " Defense";
            setcolor(white, black);

            keyPress = getch();
            if ((keyPress == 'w' || keyPress == 'W') && optionSelected > 0) optionSelected--;
            if ((keyPress == 's' || keyPress == 'S') && optionSelected < 3) optionSelected++;
            if (keyPress == 'e' || keyPress == 'E' || keyPress == '\n' || keyPress == '\r')
            {
                if (optionSelected == 0)
                {
                    charAditHp++;
                    break;
                }
                if (optionSelected == 1)
                {
                    charMaxMana += 3;
                    charCurrMana += 3;
                    break;
                }
                if (optionSelected == 2)
                {
                    baseCharAttack++;
                    break;
                }
                if (optionSelected == 3)
                {
                    baseCharDefense += 2;
                    break;
                }
            }
        }
        charMaxHp = baseCharMaxHp + charAditHp * pow(1.17, charLevel);
        charCurrHp = baseCharMaxHp - diffHP;
        clearScreen();
    }
}

void printPopup()
{
    for (int i = 4; i < 21; i++)
    {
        goToXY(17, i);
        for (int j = 17; j < 46 + 17; j++)
        {
            if ((i == 4 && j == 17) || (i == 20 && j == 45 + 17) || (i == 4 && j == 45 + 17) || (i == 20 && j == 17))
            {
                cout << dark_white << "#";
            }
            else if (j == 17 || j == 45 + 17)
            {
                cout << dark_white << "|";
            }
            else if (i == 4 || i == 20)
            {
                cout << dark_white << "-";
            }
            else
            {
                cout << dark_white << " ";
            }
        }
    }
    goToXY(18, 5);
}

void printShop()
{
    char shopAction;
    int shopSelected = 0;
    while (true)
    {
        clearScreen();
        setcolor(dark_white, black);
        cout << "\n\n\n                 ";
        if (shopSelected == 0) setcolor(black, yellow);
        cout << "Buy";
        setcolor(dark_white, black);
        cout << "\n                 ";
        if (shopSelected == 1) setcolor(black, yellow);
        cout << "Sell";
        setcolor(gray, black);
        if (showTips == true) cout << "\n\n\nPress w/s to move the selection up or down. e to select, q to quit.";
        setcolor(dark_white, black);
        keyPress = getch();
        if ((keyPress == 'w' || keyPress == 'W') && shopSelected > 0) {shopSelected--;}
        if ((keyPress == 's' || keyPress == 'S') && shopSelected < 1) {shopSelected++;}
        if (keyPress == 'q' || keyPress == 'Q' || keyPress == 27 || keyPress == 'i' || keyPress == 'I')
        {
            clearScreen();
            break;
        }
        if (keyPress == 'e' || keyPress == 'E' || keyPress == '\r' || keyPress == '\n')
        {
            if (shopSelected == 0)
            {
                shopBuyInventory();
            }
            if (shopSelected == 1)
            {
                shopSellInventory();
            }
        }
    }
}

void drinkPotion()
{
    char otherKeyPress = keyPress;
    int selectedOption = 0;
    if (charCurrHp == charMaxHp && (keyPress == '1' || keyPress == '2' || keyPress == '3'))
    {
        selectedOption = 1;
        printMap();
        printPopup();
        while (true)
        {
            goToXY(18, 5);
            cout << dark_white << "You have full health, are you sure you want";
            goToXY(18, 6);
            cout << "to drink a potion?";
            goToXY(18, 8);
            if (selectedOption == 0) setcolor(black,yellow);
            cout << "Yes";
            goToXY(18, 9);
            setcolor(dark_white,black);
            if (selectedOption == 1) setcolor(black,yellow);
            cout << "No";
            setcolor(dark_white,black);

            //cout << "\n____________________________________________________________________\n" << endl;
            //if (showTips == true) cout << gray << "Press w or s to move the selection or press e to confirm your answer. \nThe answer with the yellow background is the selected answer.\n\n" << dark_white;

            keyPress = getch();
            allTheInfoFunctions();
            if ((keyPress == 'w' || keyPress == 'W') && selectedOption > 0) {selectedOption--;}
            if ((keyPress == 's' || keyPress == 'S') && selectedOption < 1) {selectedOption++;}
            if (keyPress == 27)
            {
                selectedOption = 1;
                break;
            }
            if (keyPress == 'e' || keyPress == 'E' || keyPress == '\n' || keyPress == '\r')
            {
                clearScreen();
                break;
            }
        }
    }
    if (otherKeyPress == '1' && selectedOption == 0)
    {
        if (charSmallPotionCount > 0)
        {
            charSmallPotionCount--;
            for (int i = 0; i < 5; i++)
            {
                if (charCurrHp < charMaxHp)
                {
                    charCurrHp++;
                }
            }
        }
    }
    if (otherKeyPress == '2' && selectedOption == 0)
    {
        if (charMediumPotionCount > 0)
        {
            charMediumPotionCount--;
            for (int i = 0; i < 25; i++)
            {
                if (charCurrHp < charMaxHp)
                {
                    charCurrHp++;
                }
            }
        }
    }
    if (otherKeyPress == '3' && selectedOption == 0)
    {
        if (charGreaterPotionCount > 0)
        {
            charGreaterPotionCount--;
            for (int i = 0; i < 125; i++)
            {
                if (charCurrHp < charMaxHp)
                {
                    charCurrHp++;
                }
            }
        }
    }
}

void shopSellInventory()
{
    int selectedItem = 0;
    int selectedItem_i = 0;
    char selectedItemType;
    while (true)
    {
        int numItem = 0;
        int difItems = 0;
        clearScreen();
        concolinit();
        for (int i = 0; i < ITEM_AMOUNT; i++)
        {
            if (itemCount[i] > 0)
            {
                setcolor(dark_white, black);
                if (itemEquiped[i] == 1){settextcolor(cyan);}
                cout << itemCount[i] << " of " << dark_white << "(" << red << itemType[i] << dark_white << ")" ;
                if (itemEquiped[i] == 1){settextcolor(cyan);}
                if (selectedItem == numItem){setcolor(black, yellow);selectedItem_i = i;selectedItemType = itemType[i];}
                cout << itemName[i];
                setcolor(dark_white, black);
                if (itemEquiped[i] == 1){settextcolor(cyan);}
                if (itemAttack[i] != 0){cout << red << " " << itemAttack[i] << "A" << dark_white << ",";}
                if (itemDefense[i] != 0){cout << green << " " << itemDefense[i] << "D" << dark_white << ",";}
                if (itemEquiped[i] == 1){settextcolor(cyan);}
                cout << " Price: " << itemPrice[i]/* << ", it's type: " << itemType[i]*/ << endl;
                setcolor(dark_white, black);
                numItem++;
            }
        }
        setcolor(dark_white, black);
        cout << "\n____________________________________________________________________\n" << white << "Your money: " << charMoney << endl << endl;
        if (showTips == true) cout << gray << "Press w or s to move the selection or press e to sell an item. q to quit.\nCyan items are equiped items.\nThe item with yellow background is the selected item.\nYou can only have one item equiped of each type. (Sword, Helmet, Chestplate, Leggings, Boots)\n\n";
        cout << yellow << "You get half the money of the item price";
        keyPress = getch();
        allTheInfoFunctions();
        if (keyPress == 'i' || keyPress == 'I') {break;}
        for (int i = 0; i < ITEM_AMOUNT;i++)
        {
            if (itemCount[i] > 0) {difItems++;}
        }
        if ((keyPress == 'w' || keyPress == 'W') && selectedItem > 0) selectedItem--;
        if ((keyPress == 's' || keyPress == 'S') && selectedItem < difItems - 1) selectedItem++;
        if (keyPress == 'q' || keyPress == 'Q' || keyPress == 27 || keyPress == 'i' || keyPress == 'I')
        {
            clearScreen();
            break;
        }
        if (keyPress == 'e' || keyPress == 'E' || keyPress == '\r' || keyPress == '\n')
        {
            if (itemEquiped[selectedItem_i] == 1 && itemCount[selectedItem_i] < 2)
            {
               itemEquiped[selectedItem_i] = 0;
            }
            if (itemCount > 0)
            {
                charMoney += itemPrice[selectedItem_i] / 2;
                itemCount[selectedItem_i]--;
                if (selectedItem == difItems - 1) selectedItem--;
            }
        }
    }
}

void shopBuyInventory()
{
    int selectedItem = 0;
    int selectedItem_i = 0;
    char selectedItemType;
    double seconds_since_start = difftime( time(0), start);
    if (seconds_since_start > 60) // TIMER
    {
        memset(shopItemCount, 0, ITEM_AMOUNT);
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int SRI_levelCap = 0, ranItem_i;
        for (int i = 0; i < ITEM_AMOUNT; i++)
        {
            if (itemLevel[i] <= charLevel + 1)
            {
                SRI_levelCap = i;
            }
        }
        default_random_engine randomGenerator2(time(0) * time(0) * time(0));
        uniform_int_distribution<int> ranItem(0, SRI_levelCap);
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        for (int i = 0; i < ITEMS_IN_SHOP; i++)
        {
            ranItem_i = ranItem(randomGenerator2);
            shopItemCount[ranItem_i]++;
        }
        start = time(0);
    }
    while (true)
    {
        int numItem = 0;
        int difItems = 0;
        clearScreen();
        concolinit();
        for (int i = 0; i < ITEM_AMOUNT; i++)
        {
            if (shopItemCount[i] > 0)
            {
                setcolor(dark_white, black);
                cout << shopItemCount[i] << " of " << dark_white << "(" << red << itemType[i] << dark_white << ")" ;
                if (selectedItem == numItem)
                {
                    setcolor(black, yellow);
                    selectedItem_i = i;
                    selectedItemType = itemType[i];
                }
                cout << itemName[i];
                setcolor(dark_white, black);
                if (itemAttack[i] != 0)cout << red << " " << itemAttack[i] << "A" << dark_white << ",";
                if (itemDefense[i] != 0)cout << green << " " << itemDefense[i] << "D" << dark_white << ",";
                cout << " Price: " << itemPrice[i] << endl;
                setcolor(dark_white, black);
                numItem++;
            }
        }
        setcolor(dark_white, black);
        cout << "\n____________________________________________________________________\n" << white << "Your money: " << charMoney << endl << endl;
        if (showTips == true) cout << gray << "Press w or s to move the selection or press e to buy an item. q to quit.\nThe item with yellow background is the selected item.\n\n" << dark_white << "Shop resets after some time.";
        keyPress = getch();
        allTheInfoFunctions();
        if (keyPress == 'i' || keyPress == 'I') {break;}
        for (int i = 0; i < ITEM_AMOUNT; i++)
        {
            if (shopItemCount[i] > 0) difItems++;
        }
        if ((keyPress == 'w' || keyPress == 'W') && selectedItem > 0) selectedItem--;
        if ((keyPress == 's' || keyPress == 'S') && selectedItem < difItems - 1) selectedItem++;
        if (keyPress == 'q' || keyPress == 'Q' || keyPress == 27 || keyPress == 'i' || keyPress == 'I')
        {
            clearScreen();
            break;
        }
        if (keyPress == 'e' || keyPress == 'E' || keyPress == '\r' || keyPress == '\n')
        {
            if (itemPrice[selectedItem_i] <= charMoney && shopItemCount[selectedItem_i] > 0)
            {
                itemCount[selectedItem_i]++;
                shopItemCount[selectedItem_i]--;
                charMoney -= itemPrice[selectedItem_i];
                if (selectedItem == difItems - 1) selectedItem--;
            }
            else
            {
                cout << red << "You don't have enough money!" << dark_white;
                getch();
            }
        }
    }
}

void shopPotionShop()
{
    int selectedPotion = 0;
    default_random_engine randomGenerator(time(0) * time(0) * time(0));
    uniform_int_distribution<int> ranPotion(3, 7);
    double seconds_since_start2 = difftime( time(0), start2);
    if (seconds_since_start2 > 60) // TIMER
    {
        smallPotionCount = ranPotion(randomGenerator);
        mediumPotionCount = ranPotion(randomGenerator);
        greaterPotionCount = ranPotion(randomGenerator);
        start2 = time(0);
    }
    while (true)
    {
        clearScreen();
        cout << purple << smallPotionCount << dark_white << " of ";
        if (selectedPotion == 0) setcolor(black,yellow);
        cout << "small potions";
        setcolor(dark_white,black);
        cout <<dark_white<<", "<<red<<"5"<<dark_white<<"HP regen, "<<white<<"5"<<dark_white<<" price.\n";
        cout << purple << mediumPotionCount << dark_white << " of ";
        if (selectedPotion == 1) setcolor(black,yellow);
        cout << "medium potions";
        setcolor(dark_white,black);
        cout<<dark_white<<", "<<red<<"25"<<dark_white<<"HP regen, "<<white<<"15"<<dark_white<<" price.\n";
        cout << purple << greaterPotionCount << dark_white << " of ";
        if (selectedPotion == 2) setcolor(black,yellow);
        cout << "greater potions";
        setcolor(dark_white,black);
        cout <<dark_white<<", "<<red<<"125"<<dark_white<<"HP regen, "<<white<<"50"<<dark_white<<" price.\n";

        cout << "\n____________________________________________________________________\n" << white << "Your money: " << charMoney << endl << endl;
        if (showTips == true) cout << gray << "Press w or s to move the selection or press e to buy an item. q to quit.\nThe item with yellow background is the selected potion.\n\n" << dark_white << "Shop resets after some time.";
        keyPress = getch();
        allTheInfoFunctions();
        if ((keyPress == 'w' || keyPress == 'W') && selectedPotion > 0) {selectedPotion--;}
        if ((keyPress == 's' || keyPress == 'S') && selectedPotion < 2) {selectedPotion++;}
        if (keyPress == 'q' || keyPress == 'Q' || keyPress == 27 || keyPress == 'i' || keyPress == 'I')
        {
            clearScreen();
            break;
        }
        if (keyPress == 'e' || keyPress == 'E' || keyPress == '\r' || keyPress == '\n')
        {
            if (5 <= charMoney && smallPotionCount > 0 && selectedPotion == 0)
            {
                charSmallPotionCount++;
                smallPotionCount--;
                charMoney -= 5;
            }
            else if (15 <= charMoney && mediumPotionCount > 0 && selectedPotion == 1)
            {
                charMediumPotionCount++;
                mediumPotionCount--;
                charMoney -= 15;
            }
            else if (50 <= charMoney && greaterPotionCount > 0 && selectedPotion == 2)
            {
                charGreaterPotionCount++;
                greaterPotionCount--;
                charMoney -= 50;
            }
            else if (smallPotionCount == 0 && selectedPotion == 0)
            {
                cout << red << "\nThe shop doesn't have enough small potions!" << dark_white;
                getch();
            }
            else if (mediumPotionCount == 0 && selectedPotion == 1)
            {
                cout << red << "\nThe shop doesn't have enough medium potions!" << dark_white;
                getch();
            }
            else if (greaterPotionCount == 0 && selectedPotion == 2)
            {
                cout << red << "\nThe shop doesn't have enough greater potions!" << dark_white;
                getch();
            }
            else
            {
                cout << red << "\nYou don't have enough money!" << dark_white;
                getch();
            }
        }
    }
}

void shopSpellShop()
{
    //int ranISpell_i = 0;
    int selectedSpell = 0;
    int selectedSpell_i = 0;
    double seconds_since_start = difftime(time(0), start3);
    if (seconds_since_start > 0) // TIMER //////////////////// SET BACK TO 60 AFTER TESTING
    {
        //memset(shopSpellCount, 0, SPELL_AMOUNT);
        for (int i = 0; i < SPELL_AMOUNT; i++)
        {
            shopSpellCount[i] = 0;
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int SRI_levelCap = 0, ranSpell_i = 0;

        //sets the level cap
        for (int i = 0; i < SPELL_AMOUNT; i++)
        {
            if (spellLevel[i] <= charLevel + 3)
            {
                SRI_levelCap = i - 1;
            }
        }
        default_random_engine randomGenerator(time(0) + time(0) + time(0) + GetTickCount() + myRan);
        uniform_int_distribution<int> ranSpell(0, SRI_levelCap);
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        for (int i = 0; i < min(SPELLS_IN_SHOP, SRI_levelCap); i++)
        {
            ranSpell_i = ranSpell(randomGenerator);
            if (shopSpellCount[ranSpell_i] == 0)
            {
                shopSpellCount[ranSpell_i]++;
            }
        }
        start3 = time(0);
    }
    while (true)
    {
        int numSpell = 0;
        int difSpells = 0;
        clearScreen();
        concolinit();
        for (int i = 0; i < SPELL_AMOUNT; i++)
        {
            if (shopSpellCount[i] > 0)
            {
                cout << cyan;
                if (selectedSpell == numSpell)
                {
                    setcolor(black, yellow);
                    selectedSpell_i = i;
                }
                cout << spellName[i];
                setcolor(dark_white, black);
                cout << " Price: " << dark_red << spellPrice[i] << endl;
                setcolor(dark_white, black);
                numSpell++;
            }
        }
        setcolor(dark_white, black);
        cout << "\n____________________________________________________________________\n" << white << "Your money: " << charMoney << endl << endl;
        if (showTips == true) cout << gray << "Press w or s to move the selection. q to quit.\nThe item with yellow background is the selected item.\n\n" << dark_white << "Shop resets after some time.";
        cout << yellow << "\nPress e to see more info about the spell.";
        keyPress = getch();
        allTheInfoFunctions();
        if (keyPress == 'i' || keyPress == 'I') {break;}
        for (int i = 0; i < SPELL_AMOUNT; i++)
        {
            if (shopSpellCount[i] > 0) difSpells++;
        }
        if ((keyPress == 'w' || keyPress == 'W') && selectedSpell > 0) selectedSpell--;
        if ((keyPress == 's' || keyPress == 'S') && selectedSpell < difSpells - 1) selectedSpell++;
        if (keyPress == 'q' || keyPress == 'Q' || keyPress == 27 || keyPress == 'i' || keyPress == 'I')
        {
            clearScreen();
            break;
        }
        if (keyPress == 'e' || keyPress == 'E' || keyPress == '\r' || keyPress == '\n')
        {

            keyPress = getch();
            //explanation of spell here
            if (keyPress == 'e' || keyPress == 'E' || keyPress == '\r' || keyPress == '\n')
            {
                if (spellLearned[selectedSpell_i] == 1)
                {
                    cout << cyan << "\nYou've learnt this spell already!";
                    getch();
                }
                else if (spellPrice[selectedSpell_i] <= charMoney)
                {
                    spellLearned[selectedSpell_i] = 1;
                    charMoney -= spellPrice[selectedSpell_i];
                }
                else
                {
                    cout << red << "\nYou don't have enough money!" << dark_white;
                    getch();
                }
            }
        }
    }
}

void randomChestDrop()
{
    printMap();
    printPopup();
    int chestVariety, chestAdd, chestAdd2;
    default_random_engine randomGenerator(time(0) * time(0) * time(0) * time(0));
    uniform_int_distribution<int> ranPotions(0, 2);
    uniform_int_distribution<int> ranVariety(1, 2);
    chestVariety = ranVariety(randomGenerator) * max(charLevel, 1);
    uniform_int_distribution<int> ranMoney(2 + chestVariety, 5 * chestVariety);
    uniform_int_distribution<int> ranItemChance(0, 1);
    chestAdd = ranPotions(randomGenerator);
    int potionChestCount = chestAdd;
    if (chestAdd != 0)
    {
        goToXY(18, 6);
        cout << dark_white << "You got " << purple << chestAdd << dark_white << " potion";
        if (chestAdd > 1)
        {
            cout << "s";
        }
        cout << ".\n";
    };
    goToXY(18, 5);
    charSmallPotionCount += chestAdd;
    chestAdd = ranMoney(randomGenerator);
    cout << dark_white << "You got " << white << chestAdd << dark_white << " money.\n";
    charMoney += chestAdd;
    chestAdd = ranItemChance(randomGenerator);
    if (chestAdd == 1)
    {
        if (potionChestCount == 0) goToXY(18, 6);
        else goToXY(18, 7);
        chestAdd2 = selectRandomItem();
        itemCount[chestAdd2]++;
        cout << dark_white << "You got a " << white << itemName[chestAdd2] << dark_white << endl;
    }
    getch();
    size_t found = loadMapFile.find("cave");
    if (found != string::npos) mapL[playerCoord[0]][playerCoord[1]] = '`';
    else mapL[playerCoord[0]][playerCoord[1]] = '.';
    clearScreen();
}

void randomTreasureDrop()
{
    printMap();
    printPopup();
    int chestAdd;
    default_random_engine randomGenerator(GetTickCount() - myRan);
    uniform_int_distribution<int> ranMoney(500, 1000);
    chestAdd = ranMoney(randomGenerator);
    goToXY(18, 5);
    cout << dark_white << "You have found a treasure guarded by a Dragon!";
    goToXY(18, 6);
    cout << "You got " << white << chestAdd << dark_white << " money!";
    charMoney += chestAdd;
    getch();
    clearScreen();
}

int selectRandomItem()
{
    int SRI_levelCap = 0, ranItem_i;
    for (int i = 0; i < ITEM_AMOUNT; i++)
    {
        if (itemLevel[i] <= charLevel + 1)
        {
            SRI_levelCap = i;
        }
    }
    default_random_engine randomGenerator2(time(0) * time(0) * time(0));
    uniform_int_distribution<int> ranItem(0, SRI_levelCap);
    ranItem_i = ranItem(randomGenerator2);
    return ranItem_i;
}

void countEnemiesOnMap()
{
    doEvery10Moves++;
    if (doEvery10Moves >= 10)
    {
        enemyCountOnMap = 0;
        for (int y_CEOM = 0; y_CEOM < SIZE_OF_MAP; y_CEOM++)
        {
            for (int x_CEOM = 0; x_CEOM < SIZE_OF_MAP; x_CEOM++)
            {
                if (mapLe[x_CEOM][y_CEOM] != '#' && mapLe[x_CEOM][y_CEOM] !=  '.')
                {
                    enemyCountOnMap++;
                }
            }
        }
        doEvery10Moves = 0;
    }
}

void spawnRandomEnemies()
{
    if (enemyCountOnMap < enemyLimitPerMap)
    {
        bool checkIfNearEnough;
        int ranNum;
        char spawnEnemy = 'w';
        int y_SRFE, x_SRFE;
        myRan -= 17;
        int myRandom;
        /*myRan - GetTickCount() - myRandom - (ranNum - myRandom2) * sqrt(time(0) + 33)*/
        default_random_engine randomGeneratorMess(myRan - GetTickCount() - myRandom - ranNum - myRandom2 * sqrt(time(0) + 33));
        uniform_int_distribution<int> ranCoord(1, SIZE_OF_MAP - 1);
        uniform_int_distribution<int> ranEnemy(0, ENEMY_AMOUNT);
        y_SRFE = ranCoord(randomGeneratorMess);
        myRan -= 177;
        x_SRFE = ranCoord(randomGeneratorMess);
        ranNum = ranEnemy(randomGeneratorMess);

        if (mapL[x_SRFE][y_SRFE] == 'f' && mapLe[x_SRFE][y_SRFE] == '.' && (x_SRFE <= playerCoord[0] - 5 || x_SRFE >= playerCoord[0] + 5) && (y_SRFE <= playerCoord[1] - 5 || y_SRFE >= playerCoord[1] + 5))
        {
            if (charLevel <= 1)
            {
                spawnEnemy = forestEnemy0_1[ranNum % forestEnemy0_1.size()];
                mapLe[x_SRFE][y_SRFE] = spawnEnemy;
            }
            if (charLevel >= 2 && charLevel <= 4)
            {
                spawnEnemy = forestEnemy2_4[ranNum % forestEnemy2_4.size()];
                mapLe[x_SRFE][y_SRFE] = spawnEnemy;
            }
            if (charLevel >= 5 && charLevel <= 8)
            {
                spawnEnemy = forestEnemy5_8[ranNum % forestEnemy5_8.size()];
                mapLe[x_SRFE][y_SRFE] = spawnEnemy;
            }
            if (charLevel >= 9 && charLevel <= 12)
            {
                spawnEnemy = forestEnemy9_12[ranNum % forestEnemy9_12.size()];
                mapLe[x_SRFE][y_SRFE] = spawnEnemy;
            }
            if (charLevel >= 13 && charLevel <= 15)
            {
                spawnEnemy = forestEnemy13_15[ranNum % forestEnemy13_15.size()];
                mapLe[x_SRFE][y_SRFE] = spawnEnemy;
            }
            if (charLevel >= 16 && charLevel <= 18)
            {
                spawnEnemy = forestEnemy16_18[ranNum % forestEnemy16_18.size()];
                mapLe[x_SRFE][y_SRFE] = spawnEnemy;
            }
            if (charLevel >= 19 && charLevel <= 20)
            {
                spawnEnemy = forestEnemy19_20[ranNum % forestEnemy19_20.size()];
                mapLe[x_SRFE][y_SRFE] = spawnEnemy;
            }
        }
        checkIfNearEnough = checkIfNear('-', x_SRFE, y_SRFE);
        if (mapL[x_SRFE][y_SRFE] == '.' && (checkIfNearEnough == true) && mapLe[x_SRFE][y_SRFE] == '.')
        {
            if (charLevel <= 3)
            {
                spawnEnemy = nearWaterEnemy0_3[ranNum % nearWaterEnemy0_3.size()];
                mapLe[x_SRFE][y_SRFE] = spawnEnemy;
            }
            if (charLevel >= 4 && charLevel <= 8)
            {
                spawnEnemy = nearWaterEnemy4_8[ranNum % nearWaterEnemy4_8.size()];
                mapLe[x_SRFE][y_SRFE] = spawnEnemy;
            }
            if (charLevel >= 9 && charLevel <= 14)
            {
                spawnEnemy = nearWaterEnemy9_14[ranNum % nearWaterEnemy9_14.size()];
                mapLe[x_SRFE][y_SRFE] = spawnEnemy;
            }
            if (charLevel >= 15 && charLevel <= 20)
            {
                spawnEnemy = nearWaterEnemy15_20[ranNum % nearWaterEnemy15_20.size()];
                mapLe[x_SRFE][y_SRFE] = spawnEnemy;
            }
        }
        if ((mapL[x_SRFE][y_SRFE] == ',' || mapL[x_SRFE][y_SRFE] == 't' || mapL[x_SRFE][y_SRFE] == 'i') && mapLe[x_SRFE][y_SRFE] == '.')
        {
           if (charLevel <= 2)
            {
                spawnEnemy = snowEnemy0_2[ranNum % snowEnemy0_2.size()];
                mapLe[x_SRFE][y_SRFE] = spawnEnemy;
            }
            if (charLevel >= 3 && charLevel <= 7)
            {
                spawnEnemy = snowEnemy3_7[ranNum % snowEnemy3_7.size()];
                mapLe[x_SRFE][y_SRFE] = spawnEnemy;
            }
            if (charLevel >= 8 && charLevel <= 11)
            {
                spawnEnemy = snowEnemy8_11[ranNum % snowEnemy8_11.size()];
                mapLe[x_SRFE][y_SRFE] = spawnEnemy;
            }
            if (charLevel >= 12 && charLevel <= 15)
            {
                spawnEnemy = snowEnemy12_15[ranNum % snowEnemy12_15.size()];
                mapLe[x_SRFE][y_SRFE] = spawnEnemy;
            }
            if (charLevel >= 16 && charLevel <= 20)
            {
                spawnEnemy = snowEnemy16_20[ranNum % snowEnemy16_20.size()];
                mapLe[x_SRFE][y_SRFE] = spawnEnemy;
            }
        }
    }
}

void spawnInCaveEnemies(string spawnDef)
{
    int caveX, caveY;
    int ranNum;
    char spawnEnemy = '7';
    vector<char> caveEnemy0_1    {'l', 'a', '7', '8', '&', 'e', 's', 'n', 'z', 'e', 'e'};
    vector<char> caveEnemy2_4    {'0', '8', '=', 'o', 'e', 't', 'z', '-', '>', 'e'};
    vector<char> caveEnemy5_8    {'0', 'j', 'u', 'k', 'M', 'o', 't'};
    vector<char> caveEnemy9_12   {'j', 'u', 'k', 'M', 'O', 'o'};
    vector<char> caveEnemy13_15  {'M', 'O', 'k', 'j'};
    vector<char> caveEnemy16_18  {'O', 'M', 'U', 'B'};
    vector<char> caveEnemy19_20  {'O', 'U', 'B', 'M'};
    ///////////////////////////
    vector<char> castleEnemy0_1    {'G', ':', 'e', 'z', '%', '0'};
    vector<char> castleEnemy2_4    {'%', 'G', ':', 'e', '0'};
    vector<char> castleEnemy5_8    {'%', 'u', '~', 'V', ':', 'e', '0'};
    vector<char> castleEnemy9_12   {'%', 'A', 'u', '~', 'V', 'e'};
    vector<char> castleEnemy13_15  {'A', '4', 'L', '~', 'V'};
    vector<char> castleEnemy16_18  {'E', 'A', '4', 'L', '~', 'V'};
    vector<char> castleEnemy19_20  {'R', 'V', 'E', 'A', '4', 'L'};
    int y_SRFE = 0, x_SRFE = 0;
    myRan -= 17;
    int myRandom;

    default_random_engine randomGeneratorMess(myRan - GetTickCount() - myRandom - (ranNum - myRandom2) * sqrt(time(0) + 33));
    uniform_int_distribution<int> ranCoord(1, SIZE_OF_MAP - 1);
    uniform_int_distribution<int> ranEnemy(0, ENEMY_AMOUNT);
    while (mapL[x_SRFE][y_SRFE] != '.' && spawnDef == "castle")
    {
        y_SRFE = ranCoord(randomGeneratorMess);
        myRan -= 189;
        x_SRFE = ranCoord(randomGeneratorMess);
        myRan -= 11;
    }
    ranNum = ranEnemy(randomGeneratorMess);

    //CASTLES//CASTLES//CASTLES//CASTLES//CASTLES//CASTLES//CASTLES//CASTLES//CASTLES//
    if (mapLe[x_SRFE][y_SRFE] == '.' && spawnDef == "castle")
    {
        if (charLevel <= 1)
        {
            spawnEnemy = castleEnemy0_1[ranNum % castleEnemy0_1.size()];
            mapLe[x_SRFE][y_SRFE] = spawnEnemy;
        }
        if (charLevel >= 2 && charLevel <= 4)
        {
            spawnEnemy = castleEnemy2_4[ranNum % castleEnemy2_4.size()];
            mapLe[x_SRFE][y_SRFE] = spawnEnemy;
        }
        if (charLevel >= 5 && charLevel <= 8)
        {
            spawnEnemy = castleEnemy5_8[ranNum % castleEnemy5_8.size()];
            mapLe[x_SRFE][y_SRFE] = spawnEnemy;
        }
        if (charLevel >= 9 && charLevel <= 12)
        {
            spawnEnemy = castleEnemy9_12[ranNum % castleEnemy9_12.size()];
            mapLe[x_SRFE][y_SRFE] = spawnEnemy;
        }
        if (charLevel >= 13 && charLevel <= 15)
        {
            spawnEnemy = castleEnemy13_15[ranNum % castleEnemy13_15.size()];
            mapLe[x_SRFE][y_SRFE] = spawnEnemy;
        }
        if (charLevel >= 16 && charLevel <= 18)
        {
            spawnEnemy = castleEnemy16_18[ranNum % castleEnemy16_18.size()];
            mapLe[x_SRFE][y_SRFE] = spawnEnemy;
        }
        if (charLevel >= 19 && charLevel <= 20)
        {
            spawnEnemy = castleEnemy19_20[ranNum % castleEnemy19_20.size()];
            mapLe[x_SRFE][y_SRFE] = spawnEnemy;
        }
    }

    //CAVES//CAVES//CAVES//CAVES//CAVES//CAVES//CAVES//CAVES//CAVES//CAVES//CAVES//CAVES//
    if (spawnDef == "cave")
    {
        while (true)
        {
            int gl;
            myRan -= 27;
            caveX = ranCoord(randomGeneratorMess);
            myRan -= 22;
            caveY = ranCoord(randomGeneratorMess);
            int enough = 0;

            if (mapL[caveX][caveY] != '#' && mapL[caveX][caveY] != 'C' && mapL[caveX][caveY] != 'd')
            {
                enough++;
            }
            gl = caveX;
            while (gl > 1)
            {
                if (mapL[gl][caveY] == '#')
                {
                    enough++;
                    break;
                }
                gl--;
            }
            gl = caveX;
            while (gl < 500)
            {
                if (mapL[gl][caveY] == '#')
                {
                    enough++;
                    break;
                }
                gl++;
            }
            gl = caveY;
            while (gl > 1)
            {
                if (mapL[caveX][gl] == '#')
                {
                    enough++;
                    break;
                }
                gl--;
            }
            gl = caveY;
            while (gl < 500)
            {
                if (mapL[caveX][gl] == '#')
                {
                    enough++;
                    break;
                }
                gl++;
            }
            if (enough == 5)
            {
                break;
            }
        }
        if (charLevel <= 1)
        {
            spawnEnemy = caveEnemy0_1[ranNum % caveEnemy0_1.size()];
            mapLe[caveX][caveY] = spawnEnemy;
        }
        if (charLevel >= 2 && charLevel <= 4)
        {
            spawnEnemy = caveEnemy2_4[ranNum % caveEnemy2_4.size()];
            mapLe[caveX][caveY] = spawnEnemy;
        }
        if (charLevel >= 5 && charLevel <= 8)
        {
            spawnEnemy = caveEnemy5_8[ranNum % caveEnemy5_8.size()];
            mapLe[caveX][caveY] = spawnEnemy;
        }
        if (charLevel >= 9 && charLevel <= 12)
        {
            spawnEnemy = caveEnemy9_12[ranNum % caveEnemy9_12.size()];
            mapLe[caveX][caveY] = spawnEnemy;
        }
        if (charLevel >= 13 && charLevel <= 15)
        {
            spawnEnemy = caveEnemy13_15[ranNum % caveEnemy13_15.size()];
            mapLe[caveX][caveY] = spawnEnemy;
        }
        if (charLevel >= 16 && charLevel <= 18)
        {
            spawnEnemy = caveEnemy16_18[ranNum % caveEnemy16_18.size()];
            mapLe[caveX][caveY] = spawnEnemy;
        }
        if (charLevel >= 19 && charLevel <= 20)
        {
            spawnEnemy = caveEnemy19_20[ranNum % caveEnemy19_20.size()];
            mapLe[caveX][caveY] = spawnEnemy;
        }
    }
}

void despawnRandomLowLevelEnemies()
{
    if (loadMapFile == "/maps/map")
    {
        bool contains = false;
        int myRandom;
        default_random_engine randomGenerator(myRan - GetTickCount() - myRandom - myRandom2 * sqrt(time(0) + 33));
        uniform_int_distribution<int> ranCoord(1, SIZE_OF_MAP - 1);
        int deX = ranCoord(randomGenerator) % 300, deY = ranCoord(randomGenerator);
        int deXStart = deX + 200;
        if (charLevel <= 1)
        {
            while (deX < SIZE_OF_MAP - 1 && deXStart > deX)
            {
                contains = false;
                for (int i = 0; i < forestEnemy0_1.size(); i++)
                {
                    if (mapLe[deX][deY] == forestEnemy0_1[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < nearWaterEnemy0_3.size(); i++)
                {
                    if (mapLe[deX][deY] == nearWaterEnemy0_3[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < snowEnemy0_2.size(); i++)
                {
                    if (mapLe[deX][deY] == snowEnemy0_2[i])
                    {
                        contains = true;
                    }
                }
                if (contains == false)
                {
                    mapLe[deX][deY] = '.';
                }
                deX++;
            }
        }
        if (charLevel == 2)
        {
            while (deX < SIZE_OF_MAP - 1 && deXStart > deX)
            {
                contains = false;
                for (int i = 0; i < forestEnemy2_4.size(); i++)
                {
                    if (mapLe[deX][deY] == forestEnemy2_4[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < nearWaterEnemy0_3.size(); i++)
                {
                    if (mapLe[deX][deY] == nearWaterEnemy0_3[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < snowEnemy0_2.size(); i++)
                {
                    if (mapLe[deX][deY] == snowEnemy0_2[i])
                    {
                        contains = true;
                    }
                }
                if (contains == false)
                {
                    mapLe[deX][deY] = '.';
                }
                deX++;
            }
        }
        if (charLevel == 3)
        {
            while (deX < SIZE_OF_MAP - 1 && deXStart > deX)
            {
                contains = false;
                for (int i = 0; i < forestEnemy2_4.size(); i++)
                {
                    if (mapLe[deX][deY] == forestEnemy2_4[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < nearWaterEnemy0_3.size(); i++)
                {
                    if (mapLe[deX][deY] == nearWaterEnemy0_3[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < snowEnemy3_7.size(); i++)
                {
                    if (mapLe[deX][deY] == snowEnemy3_7[i])
                    {
                        contains = true;
                    }
                }
                if (contains == false)
                {
                    mapLe[deX][deY] = '.';
                }
                deX++;
            }
        }
        if (charLevel == 4)
        {
            while (deX < SIZE_OF_MAP - 1 && deXStart > deX)
            {
                contains = false;
                for (int i = 0; i < forestEnemy2_4.size(); i++)
                {
                    if (mapLe[deX][deY] == forestEnemy2_4[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < nearWaterEnemy4_8.size(); i++)
                {
                    if (mapLe[deX][deY] == nearWaterEnemy4_8[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < snowEnemy3_7.size(); i++)
                {
                    if (mapLe[deX][deY] == snowEnemy3_7[i])
                    {
                        contains = true;
                    }
                }
                if (contains == false)
                {
                    mapLe[deX][deY] = '.';
                }
                deX++;
            }
        }
        if (charLevel == 5 || charLevel == 6 || charLevel == 7)
        {
            while (deX < SIZE_OF_MAP - 1 && deXStart > deX)
            {
                contains = false;
                for (int i = 0; i < forestEnemy5_8.size(); i++)
                {
                    if (mapLe[deX][deY] == forestEnemy5_8[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < nearWaterEnemy4_8.size(); i++)
                {
                    if (mapLe[deX][deY] == nearWaterEnemy4_8[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < snowEnemy3_7.size(); i++)
                {
                    if (mapLe[deX][deY] == snowEnemy3_7[i])
                    {
                        contains = true;
                    }
                }
                if (contains == false)
                {
                    mapLe[deX][deY] = '.';
                }
                deX++;
            }
        }
        if (charLevel == 8)
        {
            while (deX < SIZE_OF_MAP - 1 && deXStart > deX)
            {
                contains = false;
                for (int i = 0; i < forestEnemy5_8.size(); i++)
                {
                    if (mapLe[deX][deY] == forestEnemy5_8[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < nearWaterEnemy4_8.size(); i++)
                {
                    if (mapLe[deX][deY] == nearWaterEnemy4_8[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < snowEnemy8_11.size(); i++)
                {
                    if (mapLe[deX][deY] == snowEnemy8_11[i])
                    {
                        contains = true;
                    }
                }
                if (contains == false)
                {
                    mapLe[deX][deY] = '.';
                }
                deX++;
            }
        }
        if (charLevel == 9 || charLevel == 10 || charLevel == 11)
        {
            while (deX < SIZE_OF_MAP - 1 && deXStart > deX)
            {
                contains = false;
                for (int i = 0; i < forestEnemy9_12.size(); i++)
                {
                    if (mapLe[deX][deY] == forestEnemy9_12[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < nearWaterEnemy9_14.size(); i++)
                {
                    if (mapLe[deX][deY] == nearWaterEnemy9_14[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < snowEnemy8_11.size(); i++)
                {
                    if (mapLe[deX][deY] == snowEnemy8_11[i])
                    {
                        contains = true;
                    }
                }
                if (contains == false)
                {
                    mapLe[deX][deY] = '.';
                }
                deX++;
            }
        }
        if (charLevel == 12)
        {
            while (deX < SIZE_OF_MAP - 1 && deXStart > deX)
            {
                contains = false;
                for (int i = 0; i < forestEnemy9_12.size(); i++)
                {
                    if (mapLe[deX][deY] == forestEnemy9_12[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < nearWaterEnemy9_14.size(); i++)
                {
                    if (mapLe[deX][deY] == nearWaterEnemy9_14[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < snowEnemy12_15.size(); i++)
                {
                    if (mapLe[deX][deY] == snowEnemy12_15[i])
                    {
                        contains = true;
                    }
                }
                if (contains == false)
                {
                    mapLe[deX][deY] = '.';
                }
                deX++;
            }
        }
        if (charLevel == 13 || charLevel == 14)
        {
            while (deX < SIZE_OF_MAP - 1 && deXStart > deX)
            {
                contains = false;
                for (int i = 0; i < forestEnemy13_15.size(); i++)
                {
                    if (mapLe[deX][deY] == forestEnemy13_15[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < nearWaterEnemy9_14.size(); i++)
                {
                    if (mapLe[deX][deY] == nearWaterEnemy9_14[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < snowEnemy12_15.size(); i++)
                {
                    if (mapLe[deX][deY] == snowEnemy12_15[i])
                    {
                        contains = true;
                    }
                }
                if (contains == false)
                {
                    mapLe[deX][deY] = '.';
                }
                deX++;
            }
        }
        if (charLevel == 15)
        {
            while (deX < SIZE_OF_MAP - 1 && deXStart > deX)
            {
                contains = false;
                for (int i = 0; i < forestEnemy13_15.size(); i++)
                {
                    if (mapLe[deX][deY] == forestEnemy13_15[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < nearWaterEnemy15_20.size(); i++)
                {
                    if (mapLe[deX][deY] == nearWaterEnemy15_20[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < snowEnemy12_15.size(); i++)
                {
                    if (mapLe[deX][deY] == snowEnemy12_15[i])
                    {
                        contains = true;
                    }
                }
                if (contains == false)
                {
                    mapLe[deX][deY] = '.';
                }
                deX++;
            }
        }
        if (charLevel == 16 || charLevel == 18 || charLevel == 17)
        {
            while (deX < SIZE_OF_MAP - 1 && deXStart > deX)
            {
                contains = false;
                for (int i = 0; i < forestEnemy16_18.size(); i++)
                {
                    if (mapLe[deX][deY] == forestEnemy16_18[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < nearWaterEnemy15_20.size(); i++)
                {
                    if (mapLe[deX][deY] == nearWaterEnemy15_20[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < snowEnemy16_20.size(); i++)
                {
                    if (mapLe[deX][deY] == snowEnemy16_20[i])
                    {
                        contains = true;
                    }
                }
                if (contains == false)
                {
                    mapLe[deX][deY] = '.';
                }
                deX++;
            }
        }
        if (charLevel == 19 || charLevel == 20)
        {
            while (deX < SIZE_OF_MAP - 1 && deXStart > deX)
            {
                contains = false;
                for (int i = 0; i < forestEnemy19_20.size(); i++)
                {
                    if (mapLe[deX][deY] == forestEnemy19_20[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < nearWaterEnemy15_20.size(); i++)
                {
                    if (mapLe[deX][deY] == nearWaterEnemy15_20[i])
                    {
                        contains = true;
                    }
                }
                for (int i = 0; i < snowEnemy16_20.size(); i++)
                {
                    if (mapLe[deX][deY] == snowEnemy16_20[i])
                    {
                        contains = true;
                    }
                }
                if (contains == false)
                {
                    mapLe[deX][deY] = '.';
                }
                deX++;
            }
        }
    }
}

void enemyMovement()
{
    /*while (true)
    {
        while (allowThreads == true)
        {*/
            char moveEnemy;
            int myRandom;
            default_random_engine randomGenerator(myRan - GetTickCount() - myRandom - myRandom2 * sqrt(time(0) + 33));
            uniform_int_distribution<int> ranDir(1, 7);
            uniform_int_distribution<int> ranCoord(1, SIZE_OF_MAP - 1);
            for (int i = 1; i < SIZE_OF_MAP * 5; i++)
            {
                int emX = ranCoord(randomGenerator);
                int emY = ranCoord(randomGenerator);
                int enemyDir = ranDir(randomGenerator);
                //int enemyDir = 1; //TESTING REASONS
                if (enemyDir == 1 && mapLe[emX][emY] != '.' && mapLe[emX][emY] != 'D' && mapLe[emX][emY - 1] == '.' && mapL[emX][emY - 1] != '-' && mapL[emX][emY - 1] != '#' && mapL[emX][emY - 1] != 'r' && mapL[emX][emY - 1] != 'd' && mapL[emX][emY - 1] != 'h' && (emX != playerCoord[0] || emY - 1 != playerCoord[1]))
                {
                    mapLe[emX][emY - 1] = mapLe[emX][emY];
                    mapLe[emX][emY] = '.';
                    if (emX + 16 > playerCoord[0] && emX - 16 < playerCoord[0]) //checks if in render range of player
                    {
                        if (emY < playerCoord[1] + 11 && emY > playerCoord[1] - 11 && /*night:*/ (playerCoord[0] - emX < V * 2 - 1 && emX - playerCoord[0] < V * 2 - 1 && playerCoord[1] - emY < V + 1 && emY - playerCoord[1] < V + 1))
                        {
                            goToXY(emX - playerCoord[0] + 15, emY - playerCoord[1] + 10); //checks if still below line
                            defineColor(emX, emY);
                            //settextcolor(blue);
                            cout << mapL[emX][emY] << dark_white;
                        }
                        if (emY > playerCoord[1] - 10 && emY < playerCoord[1] + 12 && /*night:*/ (playerCoord[0] - emX < V * 2 - 1 && emX - playerCoord[0] < V * 2 - 1 && playerCoord[1] - (emY - 1) < V + 1 && (emY - 1) - playerCoord[1] < V + 1))
                        {
                            goToXY(emX - playerCoord[0] + 15, emY - playerCoord[1] + 9);
                            cout << red << mapLe[emX][emY - 1] << dark_white;
                        }
                        goToXY(0, 22);
                    }
                }
                if (enemyDir == 2 && mapLe[emX][emY] != '.' && mapLe[emX][emY] != 'D' && mapLe[emX + 1][emY] == '.' && mapL[emX + 1][emY] != '-' && mapL[emX + 1][emY] != '#' && mapL[emX + 1][emY] != 'r' && mapL[emX + 1][emY] != 'd' && mapL[emX + 1][emY] != 'h' && (emX + 1 != playerCoord[0] || emY != playerCoord[1]))
                {
                    mapLe[emX + 1][emY] = mapLe[emX][emY];
                    mapLe[emX][emY] = '.';
                    if (emY + 11 > playerCoord[1] && emY - 11 < playerCoord[1])
                    {
                        if (emX > playerCoord[0] - 17 && emX < playerCoord[0] + 16 && /*night:*/ (playerCoord[0] - emX < V * 2 - 1 && emX - playerCoord[0] < V * 2 - 1 && playerCoord[1] - emY < V + 1 && emY - playerCoord[1] < V + 1))
                        {
                            goToXY(emX - playerCoord[0] + 15, emY - playerCoord[1] + 10);
                            defineColor(emX, emY);
                            //settextcolor(blue);
                            cout << mapL[emX][emY] << dark_white;
                        }
                        if (emX < playerCoord[0] + 15 && emX > playerCoord[0] - 17 && /*night:*/ (playerCoord[0] - (emX + 1) < V * 2 - 1 && (emX + 1) - playerCoord[0] < V * 2 - 1 && playerCoord[1] - emY < V + 1 && emY - playerCoord[1] < V + 1))
                        {
                            goToXY(emX - playerCoord[0] + 16, emY - playerCoord[1] + 10);
                            cout << red << mapLe[emX + 1][emY] << dark_white;
                        }
                        goToXY(0, 22);

                    }
                }
                if (enemyDir == 3 && mapLe[emX][emY] != '.' && mapLe[emX][emY] != 'D' && mapLe[emX][emY + 1] == '.' && mapL[emX][emY + 1] != '-' && mapL[emX][emY + 1] != '#' && mapL[emX][emY + 1] != 'r' && mapL[emX][emY + 1] != 'd' && mapL[emX][emY + 1] != 'h' && (emX != playerCoord[0] || emY + 1 != playerCoord[1]))
                {
                    mapLe[emX][emY + 1] = mapLe[emX][emY];
                    mapLe[emX][emY] = '.';

                    if (emX + 16 > playerCoord[0] && emX - 16 < playerCoord[0])
                    {
                        if (emY > playerCoord[1] - 11 && emY < playerCoord[1] + 11 && /*night:*/ (playerCoord[0] - emX < V * 2 - 1 && emX - playerCoord[0] < V * 2 - 1 && playerCoord[1] - emY < V + 1 && emY - playerCoord[1] < V + 1))
                        {
                            goToXY(emX - playerCoord[0] + 15, emY - playerCoord[1] + 10); //checks if still below line
                            defineColor(emX, emY);
                            //settextcolor(blue);
                            cout << mapL[emX][emY] << dark_white;
                        }
                        if (emY > playerCoord[1] - 12 && emY < playerCoord[1] + 10 && /*night:*/ (playerCoord[0] - emX < V * 2 - 1 && emX - playerCoord[0] < V * 2 - 1 && playerCoord[1] - (emY + 1) < V + 1 && (emY + 1) - playerCoord[1] < V + 1))
                        {
                            goToXY(emX - playerCoord[0] + 15, emY - playerCoord[1] + 11);
                            cout << red << mapLe[emX][emY + 1] << dark_white;
                        }
                        goToXY(0, 22);
                    }
                }
                if (enemyDir == 4 && mapLe[emX][emY] != '.' && mapLe[emX][emY] != 'D' && mapLe[emX - 1][emY] == '.' && mapL[emX - 1][emY] != '-' && mapL[emX - 1][emY] != '#' && mapL[emX - 1][emY] != 'r' && mapL[emX - 1][emY] != 'd' && mapL[emX - 1][emY] != 'h' && (emX - 1 != playerCoord[0] || emY != playerCoord[1]))
                {
                    mapLe[emX - 1][emY] = mapLe[emX][emY];
                    mapLe[emX][emY] = '.';
                    if (emY + 11 > playerCoord[1] && emY - 11 < playerCoord[1])
                    {
                        if (emX < playerCoord[0] + 16 && emX > playerCoord[0] - 16 && /*night:*/ (playerCoord[0] - (emX - 1) < V * 2 - 1 && (emX - 1) - playerCoord[0] < V * 2 - 1 && playerCoord[1] - emY < V + 1 && emY - playerCoord[1] < V + 1))
                        {
                            goToXY(emX - playerCoord[0] + 15, emY - playerCoord[1] + 10);
                            defineColor(emX, emY);
                            //settextcolor(blue);
                            cout << mapL[emX][emY] << dark_white;
                        }
                        if (emX > playerCoord[0] - 15 && emX < playerCoord[0] + 17 && /*night:*/ (playerCoord[0] - (emX - 1) < V * 2 - 1 && (emX - 1) - playerCoord[0] < V * 2 - 1 && playerCoord[1] - emY < V + 1 && emY - playerCoord[1] < V + 1))
                        {
                            goToXY(emX - playerCoord[0] + 14, emY - playerCoord[1] + 10);
                            cout << red << mapLe[emX - 1][emY] << dark_white;
                        }
                        goToXY(0, 22);
                    }
                }
            }
        /*}
    }*/
}

bool checkIfNear(char lookingFor, int x, int y)
{
    for (int CIN_y = y - 3; CIN_y < y + 3; CIN_y++)
    {
        for (int CIN_x = x - 3; CIN_x < x + 3; CIN_x++)
        {
            if (mapL[CIN_x][CIN_y] == lookingFor)
            {
                return true;
            }
        }
    }
    return false;
}

void calculateTime()
{
    int myTime = time(0);
    //while (true)
    //{
        //if (/*allowThreads == true &&*/ myTime != time(0))
        //{
    myTime = time(0);
    inGameTime = (loadedTime + ((time(0) - startTime) * 2) / 60) % 24;
    if (inGameTime != oldTime)
    {
        goToXY(56, 8);
        cout << white << inGameTime << dark_white << " ";
        if (loadMapFile == "/maps/map")
        {
            if (inGameTime <= 12)
            {
                V = inGameTime + 4;
            }
            if (inGameTime > 12)
            {
                V = -inGameTime + 28;
            }
        }
        if (toggleMoreInterfaceInfo == true)
        {
            goToXY(42, 17);
            cout << dark_white << V;
        }
        oldTime = inGameTime;
    }
       // }
    //}
}

void loadNewMap()
{
    checkForDoor();
    if (loadMapFile == "/maps/map")
    {
        for (int i = 0; i < mapAmount; i++)
        {
            if (playerCoord[0] == mapCoordX[i] && playerCoord[1] == mapCoordY[i])
            {
                mainCoordX = playerCoord[0];
                mainCoordY = playerCoord[1];
                loadMapFile = "/maps/"+mapName[i];
                getMap();
                checkForDoor();
                saveMap();
                playerCoord[0] = doorCoordX;
                playerCoord[1] = doorCoordY;
                size_t found = mapName[i].find("cave");
                if (found != string::npos)
                {
                    V = 3;
                    memset(mapLe, '.', SIZE_OF_MAP * SIZE_OF_MAP);
                    for (int i = 0; i < 7; i++) spawnInCaveEnemies("cave");
                }
                found = mapName[i].find("castle");
                if (found != string::npos)
                {
                    V = 3;
                    memset(mapLe, '.', SIZE_OF_MAP * SIZE_OF_MAP);
                    for (int i = 0; i < 30; i++) spawnInCaveEnemies("castle");
                    for (int y = 0; y < SIZE_OF_MAP; y++) //Places dragon above treasure
                    {
                        for (int x = 0; x < SIZE_OF_MAP; x++)
                        {
                            if (mapL[x][y] == 'T') mapLe[x][y + 1] = 'D';
                        }
                    }
                }
            }
        }
    }
    else if (playerCoord[0] == doorCoordX && playerCoord[1] == doorCoordY)
    {
        if (inGameTime <= 12)
        {
            V = inGameTime + 4;
        }
        if (inGameTime > 12)
        {
            V = -inGameTime + 28;
        }
        saveMap();
        playerCoord[0] = mainCoordX;
        playerCoord[1] = mainCoordY;
        loadMapFile = "/maps/map";
        getMap();
    }
}

void checkForDoor()
{
    for (int y = 0; y < SIZE_OF_MAP; y++)
    {
        for (int x = 0; x < SIZE_OF_MAP; x++)
        {
            if (mapL[x][y] == 'd')
            {
                doorCoordX = x;
                doorCoordY = y;
                break;
            }
        }
        if (mapL[x][y] == 'd')
        {
            doorCoordX = x;
            doorCoordY = y;
            break;
        }
    }
}

void loadMapFirstTime()
{
    memset(mapL, '?', SIZE_OF_MAP*SIZE_OF_MAP);
    V = 16;
    playerCoord[0] = 259;
    playerCoord[1] = 253;
    getMap();
    for (int i = 0; i < 2000; i++)
    {
        spawnRandomEnemies();
    }
    //boost::thread EM (enemyMovement);
}

void saveSlot()
{
    int selectedSlot = 0;
    while (true)
    {
        clearScreen();
        cout << "Choose your save slot:\n\n";
        if (selectedSlot == 0) setcolor(black,yellow);
        cout << "Save Slot 1";
        setcolor(dark_white,black);
        if (selectedSlot == 1) setcolor(black,yellow);
        cout << "\nSave Slot 2";
        setcolor(dark_white,black);
        if (selectedSlot == 2) setcolor(black,yellow);
        cout << "\nSave Slot 3";
        setcolor(dark_white,black);

        cout << "\n____________________________________________________________________\n" << endl;
        if (showTips == true) cout << gray << "Press W or S to move the selection or press e to use the selected save slot. \nThe slot with yellow background is the selected slot.\n\n" << dark_white;

        keyPress = getch();
        if ((keyPress == 'w' || keyPress == 'W') && selectedSlot > 0) {selectedSlot--;}
        if ((keyPress == 's' || keyPress == 'S') && selectedSlot < 2) {selectedSlot++;}
        if (keyPress == 'q' || keyPress == 'Q' || keyPress == 27) {exitSaveSlot = true; break;}
        if (keyPress == 'e' || keyPress == 'E' || keyPress == '\n' || keyPress == '\r')
        {
            if (selectedSlot == 0) currentSaveSlot = "1";
            if (selectedSlot == 1) currentSaveSlot = "2";
            if (selectedSlot == 2) currentSaveSlot = "3";
            string fileLocation, fileDestination;
            char* fileLoc;
            char* fileDest;
            ifstream fileInput ("maps/MapInfo.txt");
            fileInput >> mapAmount;

            mapName = new string [mapAmount];
            mapCoordX = new int [mapAmount];
            mapCoordY = new int [mapAmount];

            fileLocation = ("saves/SaveSlot_"+currentSaveSlot+"/maps/map"+loadMapFileTxt);
            fileLoc = &fileLocation[0];
            CopyFile("maps/map.txt", fileLoc, FALSE);
            fileLocation = ("saves/SaveSlot_"+currentSaveSlot+"/maps/MapInfo"+loadMapFileTxt);
            fileLoc = &fileLocation[0];
            CopyFile("maps/MapInfo.txt", fileLoc, FALSE);
            for (int i = 0; i < mapAmount; i++)
            {
                fileInput >> mapName[i];
                fileInput >> mapCoordX[i];
                fileInput >> mapCoordY[i];

                fileLocation = ("saves/SaveSlot_"+currentSaveSlot+"/maps/"+mapName[i]+loadMapFileTxt);
                fileDestination = ("maps/"+mapName[i]+loadMapFileTxt);
                fileLoc = &fileLocation[0];
                fileDest = &fileDestination[0];
                CopyFile(fileDest, fileLoc, FALSE);
                //fileLocation = ("saves/SaveSlot_"+currentSaveSlot+"/maps/"+mapName[i]);           fileLoc = &fileLocation[0];  CopyFile("maps/map.txt", fileLoc, FALSE);
            }
            fileInput.close();

            fileLocation = ("saves/SaveSlot_"+currentSaveSlot+"/maps/mape"+loadMapFileTxt);
            fileLoc = &fileLocation[0];
            remove(fileLoc);
            for (int i = 0; i < mapAmount; i++)
            {
                fileLocation = ("saves/SaveSlot_"+currentSaveSlot+"/maps/"+mapName[i]+"e"+loadMapFileTxt);
                fileLoc = &fileLocation[0];
                remove(fileLoc);
            }

            //if placing for loop to copy e files place after this comment (above thing removes all e files)
            break;
        }
    }

}

void saveMap()
{
    ofstream mapFile ("saves/SaveSlot_"+currentSaveSlot+loadMapFile+loadMapFileTxt);
    ofstream mapFileE ("saves/SaveSlot_"+currentSaveSlot+loadMapFile+"e"+loadMapFileTxt);
    for (int SM_y = 0; SM_y < SIZE_OF_MAP;SM_y++)
    {
        for (int SM_x = 0; SM_x < SIZE_OF_MAP;SM_x++)
        {
            mapFile << mapL[SM_x][SM_y];
            mapFileE << mapLe[SM_x][SM_y];
        }
        mapFile << endl;
        mapFileE << endl;
    }
    mapFile.close();
    mapFileE.close();
}

void saveCharToFile()
{
    ofstream saveFile ("saves/SaveSlot_"+currentSaveSlot+"/CharInfo"+loadMapFileTxt);
    saveFile << charName << endl;
    saveFile << playerCoord[0] << " " << playerCoord[1] << endl;
    saveFile << loadMapFile << endl;
    saveFile << baseCharAttack << endl;
    saveFile << charAditHp << endl;
    saveFile << baseCharDefense << endl;
    saveFile << baseCharMaxHp << endl;
    saveFile << charCurrHp << endl;
    saveFile << charMaxHp << endl;
    saveFile << charLevel << endl;
    saveFile << charXP << endl;
    saveFile << charXPreq << endl;
    saveFile << charMoney << endl;
    saveFile << charSmallPotionCount << endl;
    saveFile << charMediumPotionCount << endl;
    saveFile << charGreaterPotionCount << endl;
    saveFile << V << endl;
    saveFile << charMaxMana << endl;
    saveFile << charCurrMana << endl;
    saveFile << showTips << endl;
    saveFile << toggleMoreInterfaceInfo << endl;
    saveFile << mainCoordX << endl;
    saveFile << mainCoordY << endl;
    saveFile << inGameTime << endl;
    saveFile.close();
}

void saveItemsToFile()
{
    ofstream saveFileItems ("saves/SaveSlot_"+currentSaveSlot+"/ItemInfo"+loadMapFileTxt);
    for (int i = 0; i < ITEM_AMOUNT; i++)
    {
        saveFileItems << itemCount[i] << " " << itemEquiped[i] << endl;
    }
    saveFileItems.close();


    ofstream saveFileSpells ("saves/SaveSlot_"+currentSaveSlot+"/SpellInfo"+loadMapFileTxt);
    for (int i = 0; i < SPELL_AMOUNT; i++)
    {
        saveFileSpells << spellLearned[i] << endl;
    }
    saveFileSpells.close();
}

void loadSaveSlot()
{
    int selectedSlot = 0;
    while (true)
    {
        clearScreen();
        cout << "Choose the save slot you want to load:\n\n";
        if (selectedSlot == 0) setcolor(black,yellow);
        cout << "Save Slot 1";
        setcolor(dark_white,black);
        if (selectedSlot == 1) setcolor(black,yellow);
        cout << "\nSave Slot 2";
        setcolor(dark_white,black);
        if (selectedSlot == 2) setcolor(black,yellow);
        cout << "\nSave Slot 3";
        setcolor(dark_white,black);

        cout << "\n____________________________________________________________________\n" << endl;
        cout << gray << "Press W or S to move the selection or press e to load the selected save slot. \nThe slot with yellow background is the selected slot.\n\n" << dark_white;

        keyPress = getch();
        if ((keyPress == 'w' || keyPress == 'W') && selectedSlot > 0) {selectedSlot--;}
        if ((keyPress == 's' || keyPress == 'S') && selectedSlot < 2) {selectedSlot++;}
        if (keyPress == 'q' || keyPress == 'Q' || keyPress == 27) {exitSaveSlot = true; break;}
        if (keyPress == 'e' || keyPress == 'E' || keyPress == '\n' || keyPress == '\r')
        {
            if (selectedSlot == 0) currentSaveSlot = "1";
            if (selectedSlot == 1) currentSaveSlot = "2";
            if (selectedSlot == 2) currentSaveSlot = "3";
            break;
        }
    }
}

void loadSaveFile()
{
    ifstream fileInput ("saves/SaveSlot_"+currentSaveSlot+"/maps/MapInfo.txt");
    fileInput >> mapAmount;

    mapName = new string [mapAmount];
    mapCoordX = new int [mapAmount];
    mapCoordY = new int [mapAmount];
    for (int i = 0; i < mapAmount; i++)
    {
        fileInput >> mapName[i];
        fileInput >> mapCoordX[i];
        fileInput >> mapCoordY[i];
    }
    fileInput.close();

    ifstream saveCharFile ("saves/SaveSlot_"+currentSaveSlot+"/CharInfo"+loadMapFileTxt);
    getline(saveCharFile, charName);
    saveCharFile >> playerCoord[0] >>  playerCoord[1];
    saveCharFile >> loadMapFile;
    saveCharFile >> baseCharAttack;
    saveCharFile >> charAditHp;
    saveCharFile >> baseCharDefense;
    saveCharFile >> baseCharMaxHp;
    saveCharFile >> charCurrHp;
    saveCharFile >> charMaxHp;
    saveCharFile >> charLevel;
    saveCharFile >> charXP;
    saveCharFile >> charXPreq;
    saveCharFile >> charMoney;
    saveCharFile >> charSmallPotionCount;
    saveCharFile >> charMediumPotionCount;
    saveCharFile >> charGreaterPotionCount;
    saveCharFile >> V;
    saveCharFile >> charMaxMana;
    saveCharFile >> charCurrMana;
    saveCharFile >> showTips;
    saveCharFile >> toggleMoreInterfaceInfo;
    saveCharFile >> mainCoordX;
    saveCharFile >> mainCoordY;
    saveCharFile >> loadedTime;
    saveCharFile.close();

    memset(itemCount, 0, ITEM_AMOUNT);
    memset(itemEquiped, 0, ITEM_AMOUNT);
    ifstream saveItemFile ("saves/SaveSlot_"+currentSaveSlot+"/ItemInfo"+loadMapFileTxt);
    for (int i = 0; i < ITEM_AMOUNT; i++)
    {
        saveItemFile >> itemCount[i] >> itemEquiped[i];
    }
    saveItemFile.close();

    ifstream saveSpellFile ("saves/SaveSlot_"+currentSaveSlot+"/SpellInfo"+loadMapFileTxt);
    for (int i = 0; i < SPELL_AMOUNT; i++)
    {
        saveSpellFile >> spellLearned[i];
    }
    saveSpellFile.close();
}
