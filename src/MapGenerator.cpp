#include <iostream>
#include <fstream>
#include <random>
#include <ctime>
#include <cstdlib>
#include <stdio.h>
#include <cstring>
#include <string>
using namespace std;


int forestAmount = 670;
int forestDensity = 4500;
int rockAmount = 300;
int bushAmount = 400;
int chestAmount = 40;
int lakeAmount = 11;
int lakeDensity = 200000;
int castleSize = 90;
int snowAmount = 1600;
int snowDecline = 20;

int villageAmount = 4; //excluding the spawn area village. Each village has ~5 houses, adjust the houseAmount respectively
int villageHouseAmount = 5; //villageAmount * villageHouseAmount MUST BE SMALLER than houseAmount
int houseAmount = 75;
int caveAmount = 40;
int castleAmount = 5;

int aditMap = 0;

string* aditMapNames;
int* aditMapX;
int* aditMapY;

bool cont = false;
int aditHouseAmount = 0;
int const EMS = 500; //entiremapsize
int const SIZE = 79; //size of template
int ehCoordX;
int ehCoordY;
int pX, pY;
int enough = 0;
int gl;
int posX = SIZE / 2, posY = SIZE / 2;
int hcX[5], hcY[5];

char entireMap[EMS][EMS];
char entireHouseMap[EMS][EMS];
char mapL[SIZE][SIZE];

void replaceWithSnow();
void createLake();
int dirToCentre();
int dirToCentreEMS_One();
int dirToCentreEMS_Two();
void updateMapInfo();
void createForest();
void createSpawn();
void createHouseMap();
void createCave();
void createCastle();
void fillMapLToEMS();

int ypr, xpr;

default_random_engine randomGenerator(time(0));
uniform_int_distribution<int> ranStart(-SIZE / 2 - 15, EMS - 1);
uniform_int_distribution<int> ranStartL(-50, 500);
uniform_int_distribution<int> ranCoord(1, 499);

int main()
{
    cout << "──┐ GENERATING WORLD" << endl;
    aditMapNames = new string [houseAmount + caveAmount + castleAmount];
    aditMapX = new int [houseAmount + caveAmount + castleAmount];
    aditMapY = new int [houseAmount + caveAmount + castleAmount];
    memset(entireMap, '.', EMS*EMS);

    //forests
    cout << "  ├─Creating forests" << endl;
    for (int i = 0; i < forestAmount; i++)
    {
        createForest();
        fillMapLToEMS();
    }
    cout << "  ├─Creating rocks" << endl;
    //rocks
    for (int i = 0; i < rockAmount; i++)
    {
        ypr = ranCoord(randomGenerator);
        xpr = ranCoord(randomGenerator);
        entireMap[xpr][ypr] = 'r';
    }
    //chests
    cout << "  ├─Creating chests" << endl;
    for (int i = 0; i < chestAmount; i++)
    {
        ypr = ranCoord(randomGenerator);
        xpr = ranCoord(randomGenerator);
        entireMap[xpr][ypr] = 'C';
    }
    //bushes
    cout << "  ├─Creating bushes" << endl;
    for (int i = 0; i < bushAmount; i++)
    {
        ypr = ranCoord(randomGenerator);
        xpr = ranCoord(randomGenerator);
        entireMap[xpr][ypr] = 'b';
    }
    //lakes
    cout << "  ├─Creating lakes" << endl;
    for (int i = 0; i < lakeAmount; i++)
    {
        createLake();
        fillMapLToEMS();
    }
    //spawn
    cout << "  ├─Creating spawn" << endl;
    createSpawn();
    fillMapLToEMS();

    //spawnhouses
    int ehcX, ehcY;
    int someBodyIUsedToKnow = 212;
    for (int i = 0; i < 5; i++)
    {
        createHouseMap();
        someBodyIUsedToKnow += ranCoord(randomGenerator) % 5;
        ehcX = hcX[i] + someBodyIUsedToKnow;
        someBodyIUsedToKnow -= ranCoord(randomGenerator) % 5;
        ehcY = hcY[i] + someBodyIUsedToKnow;
        if (entireMap[ehcX][ehcY] != 'h' && entireMap[ehcX][ehcY] != 'Q' && entireMap[ehcX][ehcY] != 'c')
        {
            entireMap[ehcX][ehcY] = 'h';
            aditMapX[aditMap] = ehcX;
            aditMapY[aditMap] = ehcY;
            aditMap++;
            aditHouseAmount++;
        }
    }
    cout << "  ├─Creating villages" << endl;
    //create villages
    for (int i = 0; i < villageAmount; i++)
    {
        createSpawn();
        fillMapLToEMS();
        ehcX = ranCoord(randomGenerator);
        ehcY = ranCoord(randomGenerator);
        for (int i = 0; i < villageHouseAmount; i++)
        {
            createHouseMap();
            ehcX += ranCoord(randomGenerator) % 5;
            ehcY -= ranCoord(randomGenerator) % 5;
            if (entireMap[ehcX][ehcY] != 'h' && entireMap[ehcX][ehcY] != 'Q' && entireMap[ehcX][ehcY] != 'c')
            {
                entireMap[ehcX][ehcY] = 'h';
                aditMapX[aditMap] = ehcX;
                aditMapY[aditMap] = ehcY;
                aditMap++;
                aditHouseAmount++;
            }
        }
    }
    //houses
    cout << "  ├─Creating houses" << endl;
    for (int i = 0; i < houseAmount - aditHouseAmount; i++)
    {
        cont = false;
        createHouseMap();
        while (cont == false)
        {
            ehCoordX = ranCoord(randomGenerator);
            ehCoordY = ranCoord(randomGenerator);
            if (entireMap[ehCoordX][ehCoordY] != '-' && entireMap[ehCoordX][ehCoordY] != '#')
            {
                cont = true;
            }
        }
        for (int y = ehCoordY - 1; y < ehCoordY + 2; y++)
        {
            for (int x = ehCoordX - 1; x < ehCoordX + 2; x++)
            {
                    entireMap[x][y] = '.';
            }
        }
        entireMap[ehCoordX][ehCoordY] = 'h';
        aditMapX[aditMap] = ehCoordX;
        aditMapY[aditMap] = ehCoordY;
        aditMap++;
    }

    //caves
    cout << "  ├─Creating caves" << endl;
    for (int i = 0; i < caveAmount; i++)
    {
        cont = false;
        createCave();
        while (cont == false)
        {
            ehCoordX = ranCoord(randomGenerator);
            ehCoordY = ranCoord(randomGenerator);
            if (entireMap[ehCoordX][ehCoordY] != '-' && entireMap[ehCoordX][ehCoordY] != '#')
            {
                cont = true;
            }
        }
        for (int y = ehCoordY - 1; y < ehCoordY + 1; y++)
        {
            for (int x = ehCoordX - 1; x < ehCoordX + 1; x++)
            {
                    entireMap[x][y] = 'r';
            }
        }
        entireMap[ehCoordX][ehCoordY] = 'c';
        aditMapX[aditMap] = ehCoordX;
        aditMapY[aditMap] = ehCoordY;
        aditMap++;
    }

    //castles
    cout << "  ├─Creating castles" << endl;
    for (int i = 0; i < castleAmount; i++)
    {
        cont = false;
        createCastle();
        while (cont == false)
        {
            ehCoordX = ranCoord(randomGenerator);
            ehCoordY = ranCoord(randomGenerator);
            if (entireMap[ehCoordX][ehCoordY] != '-' && entireMap[ehCoordX][ehCoordY] != '#')
            {
                cont = true;
            }
        }
        for (int y = ehCoordY - 2; y < ehCoordY + 3; y++)
        {
            for (int x = ehCoordX - 2; x < ehCoordX + 3; x++)
            {
                if ((x == ehCoordX - 2 || x == ehCoordX + 2 || y == ehCoordY - 2 || y == ehCoordY + 2) && (x != ehCoordX || y != ehCoordY + 2))
                {
                    entireMap[x][y] = '-';
                }
                else
                {
                    entireMap[x][y] = '.';
                }

            }
        }
        entireMap[ehCoordX][ehCoordY] = 'Q';
        aditMapX[aditMap] = ehCoordX;
        aditMapY[aditMap] = ehCoordY;
        aditMap++;
    }

    cout << "  ├─Creating arctic biome" << endl;
    replaceWithSnow();
    //####
    for (int y = 0; y < EMS; y++)
    {
        for (int x = 0; x < EMS; x++)
        {
            if ((x == 0 || x == EMS - 1) || (y == 0 || y == EMS - 1))
            {
                entireMap[x][y] = '#';
            }
        }
    }

    updateMapInfo();
    //Prints map to file
    ofstream outputFile("maps/map.txt");
    for (int y = 0; y < EMS; y++)
    {
        for (int x = 0; x < EMS; x++)
        {
            outputFile << entireMap[x][y];
        }
        outputFile << endl;
    }
    outputFile.close();
    cout << "──┘ GENERATION COMPLETE" << endl;
}
//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//
/*........//........//........//........//........//........//........//........//........//........//........//........//........//........//........//........//........//........//........*/
//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//INT MAIN//

void fillMapLToEMS()
{
    int startPosX = ranStart(randomGenerator);
    int startPosY = ranStart(randomGenerator);
    for (int y = startPosY, yL = 0; y < EMS && yL < SIZE; y++, yL++)
    {
        for (int x = startPosX, xL = 0; x < EMS && xL < SIZE; x++, xL++)
        {
            /* printf("x=%d y=%d xl=%d yl=%d %c %c\n", x, y, xL, yL, entireMap[x][y], mapL[xL][yL]); */
            if (0 <= x && x < EMS && 0 <= y && y < EMS)
                if (0 <= xL && xL < SIZE && 0 <= yL && yL < SIZE)
                    if (mapL[xL][yL] != '`')
                        entireMap[x][y] = mapL[xL][yL];
        }
    }
}

void createForest()
{
    posX = SIZE / 2;
    posY = SIZE / 2;
    memset(mapL, '`', sizeof mapL);
    /* memset(mapL, '.', SIZE * SIZE); */
    mapL[SIZE / 2][SIZE / 2] = 'f';
    uniform_int_distribution<int> ranStartDir(1, 4);
    uniform_int_distribution<int> ranDir(1, 5);

    int oldDir, dir = ranStartDir(randomGenerator);
    for (int i = 0; i < forestDensity; i++)
    {
        oldDir = dir;
        dir = ranDir(randomGenerator);
        if (dir == 5) dir = dirToCentre();
        if (dir == 1) posY--;
        if (dir == 2) posX++;
        if (dir == 3) posY++;
        if (dir == 4) posX--;
        if (0 <= posX && posX < SIZE && 0 <= posY && posY < SIZE)
            mapL[posX][posY] = 'f';
    }
}

void createSpawn()
{
    posX = SIZE / 2;
    posY = SIZE / 2;
    memset(mapL, '`', SIZE * SIZE);
    mapL[SIZE / 2][SIZE / 2] = '.';
    uniform_int_distribution<int> ranStartDir(1, 4);
    uniform_int_distribution<int> ranDir(1, 6);

    int oldDir, dir = ranStartDir(randomGenerator);
    for (int i = 0; i < 12000; i++)
    {
        oldDir = dir;
        dir = ranDir(randomGenerator);
        if (dir == 5 || dir == 6) dir = dirToCentre();
        if (dir == 1) posY--;
        if (dir == 2) posX++;
        if (dir == 3) posY++;
        if (dir == 4) posX--;
        mapL[posX][posY] = '.';
        if (i == 1000 || i == 2000 || i == 3000 || i == 4000 || i == 5000)
        {
            hcX[(i - 1000) / 1000] = posX;
            hcY[(i - 1000) / 1000] = posY;
        }

    }
}

void createCave()
{
    memset(entireHouseMap, '`', EMS * EMS);
    pX = EMS / 2, pY = EMS / 2;
    entireHouseMap[pX][pY] = '#';
    uniform_int_distribution<int> direction(1, 4);
    uniform_int_distribution<int> randir(1, 6);
    uniform_int_distribution<int> ranCoord(1, EMS - 1);
    uniform_int_distribution<int> ranCoordChest(250, 300);
    int dir = direction(randomGenerator);

    pX += 50;
    pY += 50;

    if (dir == 1) pY--;
    if (dir == 2) pX++;
    if (dir == 3) pY++;
    if (dir == 4) pX--;
    entireHouseMap[pX][pY] = '#';

    while (pX != EMS / 2 || pY != EMS / 2)
    {
        dir = randir(randomGenerator);
        if (dir == 5 || dir == 6) dir = dirToCentreEMS_One();
        if (dir == 1) pY--;
        if (dir == 2) pX++;
        if (dir == 3) pY++;
        if (dir == 4) pX--;
        entireHouseMap[pX][pY] = '#';
    }
    pX += 50;
    pY += 50;
    entireHouseMap[pX][pY] = '#';

    while (pX != EMS / 2 || pY != EMS / 2)
    {
        dir = randir(randomGenerator);
        if (dir == 5 || dir == 6) dir = dirToCentreEMS_Two();
        if (dir == 1) pY--;
        if (dir == 2) pX++;
        if (dir == 3) pY++;
        if (dir == 4) pX--;
        entireHouseMap[pX][pY] = '#';
    }

    //chests
    for (int i = 0; i < 3; i++)
    {
        while (true)
        {
            int x = ranCoordChest(randomGenerator);
            int y = ranCoordChest(randomGenerator);
            enough = 0;
            if (entireHouseMap[x][y] != '#')
            {
                enough++;
            }
            gl = x;
            while (gl > 1)
            {
                if (entireHouseMap[gl][y] == '#')
                {
                    enough++;
                    break;
                }
                gl--;
            }
            gl = x;
            while (gl < 500)
            {
                if (entireHouseMap[gl][y] == '#')
                {
                    enough++;
                    break;
                }
                gl++;
            }
            gl = y;
            while (gl > 1)
            {
                if (entireHouseMap[x][gl] == '#')
                {
                    enough++;
                    break;
                }
                gl--;
            }
            gl = y;
            while (gl < 500)
            {
                if (entireHouseMap[x][gl] == '#')
                {
                    enough++;
                    break;
                }
                gl++;
            }
            if (enough == 5)
            {
                entireHouseMap[x][y] = 'C';
                break;
            }
        }
    }
    while (true)
    {
        int x = ranCoord(randomGenerator);
        int y = ranCoord(randomGenerator);
        enough = 0;
        if (entireHouseMap[x][y] != '#')
        {
            enough++;
        }
        gl = x;
        while (gl > 1)
        {
            if (entireHouseMap[gl][y] == '#')
            {
                enough++;
                break;
            }
            gl--;
        }
        gl = x;
        while (gl < 500)
        {
            if (entireHouseMap[gl][y] == '#')
            {
                enough++;
                break;
            }
            gl++;
        }
        gl = y;
        while (gl > 1)
        {
            if (entireHouseMap[x][gl] == '#')
            {
                enough++;
                break;
            }
            gl--;
        }
        gl = y;
        while (gl < 500)
        {
            if (entireHouseMap[x][gl] == '#')
            {
                enough++;
                break;
            }
            gl++;
        }
        if (enough == 5)
        {
            entireHouseMap[x][y] = 'd';
            break;
        }
    }
    //Prints map to file
    string aditNum = to_string(aditMap);
    aditMapNames[aditMap] = "cave"+aditNum;
    ofstream outputFile("maps/cave"+aditNum+".txt");
    for (int y = 0; y < EMS; y++)
    {
        for (int x = 0; x < EMS; x++)
        {
            outputFile << entireHouseMap[x][y];
        }
        outputFile << endl;
    }
    outputFile.close();
}

void createHouseMap()
{
    memset(entireHouseMap, '`', EMS * EMS);
    uniform_int_distribution<int> ranLength(8, 16);
    uniform_int_distribution<int> ranWidth(4, 6);
    int houseWidth = ranWidth(randomGenerator);
    int houseLength = ranLength(randomGenerator);

    for (int y = EMS / 2; y < houseWidth + EMS / 2; y++)
    {
        for (int x = EMS / 2; x < houseLength + EMS / 2; x++)
        {
            if ((x == EMS / 2 || x == houseLength + EMS / 2 - 1) || (y == EMS / 2 || y == houseWidth + EMS / 2 - 1))
            {
                entireHouseMap[x][y] = '#';
            }
            else
            {
                entireHouseMap[x][y] = '.';
            }
        }
    }
    uniform_int_distribution<int> ranWDoor(1, houseWidth - 2);
    uniform_int_distribution<int> ranLDoor(1, houseLength - 2);
    int houseWDoor = ranWDoor(randomGenerator), houseLDoor;
    if (houseWDoor == 1 || houseWDoor == houseWidth - 1)
    {
        houseLDoor = ranLDoor(randomGenerator);
    }
    else
    {
        if (houseWDoor % 2 == 0)
        {
            houseLDoor = 1;
        }
        else
        {
            houseLDoor = houseLength - 2;
        }
    }
    entireHouseMap[houseLDoor + EMS / 2][houseWDoor + EMS / 2] = 'd';

    int shops = ranLength(randomGenerator) % 4;
    houseWDoor = ranWDoor(randomGenerator);
    if (houseWDoor == 1 || houseWDoor == houseWidth - 1)
    {
        houseLDoor = ranLDoor(randomGenerator);
    }
    else
    {
        if (houseWDoor % 2 == 0)
        {
            houseLDoor = 1;
        }
        else
        {
            houseLDoor = houseLength - 2;
        }
    }
    if (entireHouseMap[houseLDoor + EMS / 2][houseWDoor + EMS / 2] != 'd')
    {
        if (shops == 0)
        {
            entireHouseMap[houseLDoor + EMS / 2][houseWDoor + EMS / 2] = 'S';
        }
        if (shops == 1)
        {
            entireHouseMap[houseLDoor + EMS / 2][houseWDoor + EMS / 2] = 'p';
        }
        if (shops == 2)
        {
            entireHouseMap[houseLDoor + EMS / 2][houseWDoor + EMS / 2] = 'z';
        }
    }

    //####
    for (int y = 0; y < EMS; y++)
    {
        for (int x = 0; x < EMS; x++)
        {
            if ((x == 0 || x == EMS - 1) || (y == 0 || y == EMS - 1))
            {
                entireHouseMap[x][y] = '#';
            }
        }
    }
    string aditNum = to_string(aditMap);
    aditMapNames[aditMap] = "house"+aditNum;
    ofstream outputFile("maps/house"+aditNum+".txt");
    for (int y = 0; y < EMS; y++)
    {
        for (int x = 0; x < EMS; x++)
        {
            outputFile << entireHouseMap[x][y];
        }
        outputFile << endl;
    }
    outputFile.close();
}

void createLake()
{
    posX = SIZE / 2;
    posY = SIZE / 2;
    memset(mapL, '`', SIZE*SIZE);
    mapL[posX][posY] = '-';
    uniform_int_distribution<int> direction(1, 4);
    uniform_int_distribution<int> randir(1, 6);
    int oldDir, dir = direction(randomGenerator);

    for (int i = 0; i < lakeDensity; i++)
    {
        oldDir = dir;
        dir = randir(randomGenerator);
        if (dir == 5 || dir == 6) dir = dirToCentre();
        if (dir == 1) posY--;
        if (dir == 2) posX++;
        if (dir == 3) posY++;
        if (dir == 4) posX--;
        mapL[posX][posY] = '-';
    }
}

void replaceWithSnow()
{
    uniform_int_distribution<int> ranCoord(1, 499);
    int rY, rX;

    while (snowAmount > 0)
    {
        for (int i = 0; i < snowAmount; i++)
        {
            rX = ranCoord(randomGenerator);
            switch (entireMap[rX][rY])
            {
                case 'f':
                   entireMap[rX][rY] = 't';
                   break;
               case '-':
                   entireMap[rX][rY] = 'i';
                   break;
                case '.':
                   entireMap[rX][rY] = ',';
                   break;
            }
        }
        rY++;
        snowAmount -= snowDecline;
    }
}

void updateMapInfo()
{
    ofstream outputFile("maps/MapInfo.txt");
    outputFile << houseAmount + caveAmount + castleAmount << endl;
    for (int i = 0; i < houseAmount + caveAmount + castleAmount; i++)
    {
        outputFile << aditMapNames[i] << " " << aditMapX[i] << " " << aditMapY[i] << endl;
    }
    outputFile.close();
}

int dirToCentre()
{
    uniform_int_distribution<int> chance(1, 2);
    int ch = chance(randomGenerator);


    if (posX > SIZE / 2 && posY > SIZE / 2)
    {
        if (ch == 1) return 1;
        else return 4;
    }
    else if (posX > SIZE / 2 && posY < SIZE / 2)
    {
        if (ch == 1) return 3;
        else return 4;
    }
    else if (posX < SIZE / 2 && posY < SIZE / 2)
    {
        if (ch == 1) return 2;
        else return 3;
    }
    else if (posX < SIZE / 2 && posY > SIZE / 2)
    {
        if (ch == 1) return 1;
        else return 2;
    }
    else if (posX < SIZE / 2)
    {
        return 2;
    }
    else if (posX > SIZE / 2)
    {
        return 4;
    }
    else if (posY < SIZE / 2)
    {
        return 3;
    }
    else if (posY > SIZE / 2)
    {
        return 1;
    }
    return 1;
}

int dirToCentreEMS_One()
{
    uniform_int_distribution<int> chance(1, 2);
    int ch = chance(randomGenerator);


    if (pX > EMS / 2 && pY > EMS / 2)
    {
        if (ch % 3 == 1) return 1;
        else return 4;
    }
    else if (pX > EMS / 2 && pY < EMS / 2)
    {
        if (ch % 3 == 1) return 3;
        else return 4;
    }
    else if (pX < EMS / 2 && pY < EMS / 2)
    {
        if (ch % 3 == 1) return 2;
        else return 3;
    }
    else if (pX < EMS / 2 && pY > EMS / 2)
    {
        if (ch % 3 == 1) return 1;
        else return 2;
    }
    else if (pX < EMS / 2)
    {
        return 2;
    }
    else if (pX > EMS / 2)
    {
        return 4;
    }
    else if (pY < EMS / 2)
    {
        return 3;
    }
    else if (pY > EMS / 2)
    {
        return 1;
    }
    return 1;
}

int dirToCentreEMS_Two()
{
    uniform_int_distribution<int> chance(1, 2);
    int ch = chance(randomGenerator);
    if (pX > EMS / 2 && pY > EMS / 2)
    {
        if (ch % 2 == 1) return 4;
        else return 1;
    }
    else if (pX > EMS / 2 && pY < EMS / 2)
    {
        if (ch % 2 == 1) return 4;
        else return 3;
    }
    else if (pX < EMS / 2 && pY < EMS / 2)
    {
        if (ch % 2 == 1) return 3;
        else return 2;
    }
    else if (pX < EMS / 2 && pY > EMS / 2)
    {
        if (ch % 2 == 1) return 2;
        else return 1;
    }
    else if (pX < EMS / 2)
    {
        return 2;
    }
    else if (pX > EMS / 2)
    {
        return 4;
    }
    else if (pY < EMS / 2)
    {
        return 3;
    }
    else if (pY > EMS / 2)
    {
        return 1;
    }
    return 1;
}

void createCastle()
{

    memset(entireHouseMap, '`', EMS * EMS);
    uniform_int_distribution<int> ranLength(8, 20);
    uniform_int_distribution<int> ranWidth(6, 9);
    uniform_int_distribution<int> ranDir(1, 4);
    int oldRoomDir = 0, roomDir;
    int houseWidth = ranWidth(randomGenerator);
    int houseLength = ranLength(randomGenerator);
    int middleCoordX = EMS / 2 + houseLength / 2, middleCoordY = EMS / 2 + houseWidth / 2;
    //Creates one room
    for (int y = EMS / 2; y < houseWidth + EMS / 2; y++)
    {
        for (int x = EMS / 2; x < houseLength + EMS / 2; x++)
        {
            if ((x == EMS / 2 || x == houseLength + EMS / 2 - 1) || (y == EMS / 2 || y == houseWidth + EMS / 2 - 1))
            {
                entireHouseMap[x][y] = '#';
            }
            else
            {
                entireHouseMap[x][y] = '.';
            }
        }
    }
    uniform_int_distribution<int> ranWDoor(1, houseWidth - 2);
    uniform_int_distribution<int> ranLDoor(1, houseLength - 2);

    //responsible for placing the door
    int houseWDoor = ranWDoor(randomGenerator), houseLDoor;
    if (houseWDoor == 1 || houseWDoor == houseWidth - 1)
    {
        houseLDoor = ranLDoor(randomGenerator);
    }
    else
    {
        if (houseWDoor % 2 == 0)
        {
            houseLDoor = 1;
        }
        else
        {
            houseLDoor = houseLength - 2;
        }
    }
    entireHouseMap[houseLDoor + EMS / 2][houseWDoor + EMS / 2] = 'd';

    //Places all next rooms
    for (int i = 0; i < castleSize; i++)
    {
        while (true)
        {
            roomDir = ranDir(randomGenerator);
            if (oldRoomDir != roomDir + 2 % 4) break;
        }
        //roomDir = 1;
        oldRoomDir = roomDir;
        if (roomDir == 1)
        {
            for (int f = 0; f < 10; f++)
            {
                if (entireHouseMap[middleCoordX][middleCoordY] != 'd') entireHouseMap[middleCoordX][middleCoordY] = '.';
                if (entireHouseMap[middleCoordX + 1][middleCoordY] != 'd' && entireHouseMap[middleCoordX + 1][middleCoordY] != '.') entireHouseMap[middleCoordX + 1][middleCoordY] = '#';
                if (entireHouseMap[middleCoordX - 1][middleCoordY] != 'd' && entireHouseMap[middleCoordX - 1][middleCoordY] != '.') entireHouseMap[middleCoordX - 1][middleCoordY] = '#';
                middleCoordY--;
            }
        }
        if (roomDir == 2)
        {
            for (int f = 0; f < 10; f++)
            {
                if (entireHouseMap[middleCoordX][middleCoordY] != 'd') entireHouseMap[middleCoordX][middleCoordY] = '.';
                if (entireHouseMap[middleCoordX][middleCoordY + 1] != 'd' && entireHouseMap[middleCoordX][middleCoordY + 1] != '.') entireHouseMap[middleCoordX][middleCoordY + 1] = '#';
                if (entireHouseMap[middleCoordX][middleCoordY - 1] != 'd' && entireHouseMap[middleCoordX][middleCoordY - 1] != '.') entireHouseMap[middleCoordX][middleCoordY - 1] = '#';
                middleCoordX++;
            }
        }
        if (roomDir == 3)
        {
            for (int f = 0; f < 10; f++)
            {
                if (entireHouseMap[middleCoordX][middleCoordY] != 'd') entireHouseMap[middleCoordX][middleCoordY] = '.';
                if (entireHouseMap[middleCoordX + 1][middleCoordY] != 'd' && entireHouseMap[middleCoordX + 1][middleCoordY] != '.') entireHouseMap[middleCoordX + 1][middleCoordY] = '#';
                if (entireHouseMap[middleCoordX - 1][middleCoordY] != 'd' && entireHouseMap[middleCoordX - 1][middleCoordY] != '.') entireHouseMap[middleCoordX - 1][middleCoordY] = '#';
                middleCoordY++;
            }
        }
        if (roomDir == 4)
        {
            for (int f = 0; f < 10; f++)
            {
                if (entireHouseMap[middleCoordX][middleCoordY] != 'd') entireHouseMap[middleCoordX][middleCoordY] = '.';
                if (entireHouseMap[middleCoordX][middleCoordY + 1] != 'd' && entireHouseMap[middleCoordX][middleCoordY + 1] != '.') entireHouseMap[middleCoordX][middleCoordY + 1] = '#';
                if (entireHouseMap[middleCoordX][middleCoordY - 1] != 'd' && entireHouseMap[middleCoordX][middleCoordY - 1] != '.') entireHouseMap[middleCoordX][middleCoordY - 1] = '#';
                middleCoordX--;
            }
        }
        houseWidth = ranWidth(randomGenerator);
        houseLength = ranLength(randomGenerator);
        for (int y = middleCoordY - houseWidth / 2; y < houseWidth / 2 + middleCoordY; y++)
        {
            for (int x = middleCoordX - houseLength / 2; x < houseLength / 2 + middleCoordX; x++)
            {
                if (((x == middleCoordX - houseLength / 2 || x == houseLength / 2 + middleCoordX - 1) || (y == middleCoordY - houseWidth / 2 || y == houseWidth / 2 + middleCoordY - 1)) && entireHouseMap[x][y] != '.' && entireHouseMap[x][y] != 'd')
                {
                    entireHouseMap[x][y] = '#';
                }
                else if (entireHouseMap[x][y] != 'd')
                {
                    entireHouseMap[x][y] = '.';
                }
            }
        }
        if (i == castleSize - 1 && entireHouseMap[middleCoordX][middleCoordY] != 'd')
        {
            entireHouseMap[middleCoordX][middleCoordY] = 'T';
        }
    }

    uniform_int_distribution<int> ranCoord(1, EMS - 1);
    //chests
    for (int i = 0; i < 4; i++)
    {
        bool cont = false;
        while (cont == false)
        {
            int CCX = ranCoord(randomGenerator); //castleChestX
            int CCY = ranCoord(randomGenerator);
            if (entireHouseMap[CCX][CCY] == '.')
            {
                entireHouseMap[CCX][CCY] = 'C';
                cont = true;
            }

        }
    }

    //####
    for (int y = 0; y < EMS; y++)
    {
        for (int x = 0; x < EMS; x++)
        {
            if ((x == 0 || x == EMS - 1) || (y == 0 || y == EMS - 1))
            {
                entireHouseMap[x][y] = '#';
            }
        }
    }
    string aditNum = to_string(aditMap);
    aditMapNames[aditMap] = "castle"+aditNum;
    ofstream outputFile("maps/castle"+aditNum+".txt");
    for (int y = 0; y < EMS; y++)
    {
        for (int x = 0; x < EMS; x++)
        {
            outputFile << entireHouseMap[x][y];
        }
        outputFile << endl;
    }
    outputFile.close();
}
