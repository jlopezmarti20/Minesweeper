#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Random.h"
#include "Game.h"
using namespace std;

// constructors
Board::Board(std::string fileName) {
    ifstream configFile;
    configFile.open(fileName);
    string fileExtension = fileName.substr(fileName.length() - 3);

    if (fileExtension != "cfg") {
        cout << "Invalid fileName" << endl;
    }
    else if (fileExtension == "cfg") {
        // file contains columns, rows, and number of mines
        if (configFile.is_open()) {
            while (!configFile.eof()) {
                string width;
                configFile >> width;
                numCols = stoi(width);

                string height;
                configFile >> height;
                numRows = stoi(height);

                string Mines;
                configFile >> Mines;
                numMines = stoi(Mines);
            }
        }
        else {cout << "can't open";}

        if (numMines > numCols * numRows) {
            // number of mines can't exceed the number of tiles
            numMines = numCols * numRows;
        }

        bool minesAreBlank = false;
        int formerMines = numMines;
        if (numMines > (numRows * numCols) / 2) {
            numMines = (numRows * numCols) - numMines;
            minesAreBlank = true;
        }

        // make 1D array with initial values
        bool* mineArray = new bool[numCols * numRows];
        for (int i = 0; i < numRows * numCols; i++) {
            if (minesAreBlank) {
                mineArray[i] = 1;
            }
            else {
                mineArray[i] = 0;
            }
        }

        // set mine locations
        int* mineIndices = new int[numMines];
        for(int i = 0; i < numMines; i++) {
            mineIndices[i] = Random::Int(0, numRows * numCols - 1);

            for (int j = 0; j < i; j++) {
                // checks that an index hasn't been duplicated
                if (mineIndices[j] == mineIndices[i]) {
                    mineIndices[i] = Random::Int(0, numRows* numCols - 1);
                    j = 0; // restart the loop
                }
            }
        }

        // fill in mines
        for (int i = 0; i < numMines; i++) {
            if (minesAreBlank) {
                mineArray[mineIndices[i]] = 0;
            }
            else {
                mineArray[mineIndices[i]] = 1;
            }
        }

        if (minesAreBlank) numMines = formerMines;

        // make 2D vector and fill it with default values
        Tile myTile;
        vector<Tile> blankRowOfTiles;
        for (int i = 0; i < numCols; i++) blankRowOfTiles.push_back(myTile);
        for (int i = 0; i < numRows; i++) tileVect.push_back(blankRowOfTiles);

        // fill in tileVect with mines
        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++) {
                tileVect[i][j].hasMine = mineArray[i * numCols + j];
            }
        }

        // delete dynamically allocated memory
        delete[] mineArray;
        delete[] mineIndices;
        configFile.close();
    }


    // fill in the vector of tiles
    for (int row = 0; row < numRows; row++) {
        for (int col = 0; col < numCols; col++) {
            // sets the neighboring tiles for each tile
            // also checks that there is a tile present in each direction
            if (row + 1 != numRows) tileVect[row][col].neighbors["N"] = &tileVect[row+1][col];
            if (col - 1 != -1) tileVect[row][col].neighbors["E"] = &tileVect[row][col - 1];
            if (row - 1 != -1) tileVect[row][col].neighbors["S"] = &tileVect[row - 1][col];
            if (col + 1 != numCols) tileVect[row][col].neighbors["W"] = &tileVect[row][col + 1];
            if ((row + 1 != numRows) && (col - 1 != -1)) tileVect[row][col].neighbors["NE"] = &tileVect[row + 1][col - 1];
            if ((row - 1 != -1) && (col - 1 != -1)) tileVect[row][col].neighbors["SE"] = &tileVect[row - 1][col - 1];
            if ((row - 1 != -1) && (col + 1 != numCols)) tileVect[row][col].neighbors["SW"] = &tileVect[row - 1][col + 1];
            if ((row + 1 != numRows) && (col + 1 != numCols)) tileVect[row][col].neighbors["NW"] = &tileVect[row + 1][col + 1];

            // check if each of the adjacent tiles have a mine
            // if they do, add to NumAdjacents
            if (tileVect[row][col].neighbors["N"] != nullptr) {
                if (tileVect[row][col].neighbors["N"]->hasMine) {
                    tileVect[row][col].numAdjacents++;}
            }
            if (tileVect[row][col].neighbors["E"] != nullptr) {
                if (tileVect[row][col].neighbors["E"]->hasMine) {
                    tileVect[row][col].numAdjacents++;}
            }
            if (tileVect[row][col].neighbors["S"] != nullptr) {
                if (tileVect[row][col].neighbors["S"]->hasMine) {
                    tileVect[row][col].numAdjacents++;}
            }
            if (tileVect[row][col].neighbors["W"] != nullptr) {
                if (tileVect[row][col].neighbors["W"]->hasMine) {
                    tileVect[row][col].numAdjacents++;}
            }
            if (tileVect[row][col].neighbors["NE"] != nullptr) {
                if (tileVect[row][col].neighbors["NE"]->hasMine) {
                    tileVect[row][col].numAdjacents++;}
            }
            if (tileVect[row][col].neighbors["NW"] != nullptr) {
                if (tileVect[row][col].neighbors["NW"]->hasMine) {
                    tileVect[row][col].numAdjacents++;}
            }
            if (tileVect[row][col].neighbors["SE"] != nullptr) {
                if (tileVect[row][col].neighbors["SE"]->hasMine) {
                    tileVect[row][col].numAdjacents++;}
            }
            if (tileVect[row][col].neighbors["SW"] != nullptr) {
                if (tileVect[row][col].neighbors["SW"]->hasMine) {
                    tileVect[row][col].numAdjacents++;}
            }
        }
    }
}

// accessors
int Board::GetBoardHeight() {
    return numRows * 32;
}

int Board::GetBoardWidth() {
    return numCols * 32;
}

int Board::GetWindowHeight() {
    return numRows * 32 + 100;
}

// actions
vector<vector<sf::Sprite>> Board::FilledWith(sf::Sprite sprite) {
    vector<sf::Sprite> emptyRow;
    vector<vector<sf::Sprite>> filledBoard;
    for (int i = 0; i < numCols; i++) {
        // fill in a row of tiles
        emptyRow.push_back(sprite);}
    for (int i = 0; i < numRows; i++) {
        // add the row of tiles to the board
        filledBoard.push_back(emptyRow);
    }
    return filledBoard;
}

void Board::ClickMine(int row, int col) {
    // function for if the user clicks a mine
    if (!tileVect[row][col].isRevealed) {
        tileVect[row][col].isRevealed = true; // reveal the tile
        numRevealed++;
        face = "deadFace";
        for (int myRow = 0; myRow < numRows; myRow++){
            for (int myCol = 0; myCol < numCols; myCol++) {
                if (tileVect[myRow][myCol].hasMine) {
                    tileVect[myRow][myCol].isFlagged = false;
                    tileVect[myRow][myCol].isRevealed = true;

                }
            }
        }
    }
}

void Board::ClickNum(int row, int col) {
    // function if the user clicks on a tile with a number
    if(!tileVect[row][col].isRevealed){
        tileVect[row][col].isRevealed = true;
        numRevealed++;
    }
}

void Board::ClickZero(int row, int col) {
    // function if a blank tile is clicked
    if ((row < 0 || row >= numRows) || (col < 0 || col >= numCols)) {
        return; // make sure that the click is within bounds
    }
    if (tileVect[row][col].isFlagged) {
        return; // can't reveal a flagged tile
    }
    if (tileVect[row][col].numAdjacents == 0 && !tileVect[row][col].isRevealed){
        tileVect[row][col].isRevealed = true; // reveal surrounding blanks
        numRevealed++;
        // run the function on surrounding tiles
        ClickZero(row + 1, col);
        ClickZero(row - 1, col);
        ClickZero(row, col - 1);
        ClickZero(row, col + 1);
    }
    else {
        ClickNum(row, col); // if the tile isn't blank, should get treated like clicking on a numbered tile
    }
}

void Board::LeftClick(int row, int col) {
    if (tileVect[row][col].isFlagged || face != "happyFace") {
        return; // don't allow a left click on a flag or if the game is over
    }
    else if (tileVect[row][col].hasMine) {
        ClickMine(row, col); // left-clicked on a mine
    }
    else if (tileVect[row][col].numAdjacents != 0){
        ClickNum(row, col); // left-clicked on a numbered tile
    }
    else {
        ClickZero(row, col); // user clicked a blank
    }

}


