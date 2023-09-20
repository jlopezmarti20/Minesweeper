#pragma once
#include <string>
#include <vector>
#include "Tile.h"
using namespace std;

struct Board {
    int numCols = 0;
    int numRows = 0;
    int numMines = 0;
    int numRevealed = 0;
    int flagCount = 0;
    bool isRevealed = false;

    string face = "happyFace";
    vector<vector<Tile>> tileVect;

    //constructor
    Board(string fileName);

    // accessors
    int GetBoardWidth();
    int GetBoardHeight();
    int GetWindowHeight();

    // actions
    vector<vector<sf::Sprite>> FilledWith(sf::Sprite sprite);
    void ClickMine(int row, int col);
    void ClickNum(int row, int col);
    void ClickZero(int row, int col);
    void LeftClick(int row, int col);
    void PausePlay();
};
