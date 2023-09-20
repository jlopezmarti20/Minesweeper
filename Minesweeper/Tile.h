#pragma once
#include <unordered_map>
#include <string>
using namespace std;

struct Tile {
    bool isRevealed = false;
    bool isFlagged = false;
    bool hasMine = false;
    //bool revealAll;
    unordered_map<string, Tile*> neighbors;
    // the pointer is nullptr if the target tile is off the board
    int numAdjacents = 0;

    Tile(){
        neighbors["N"] = nullptr;
        neighbors["E"] = nullptr;
        neighbors["S"] = nullptr;
        neighbors["W"] = nullptr;
        neighbors["NE"] = nullptr;
        neighbors["SE"] = nullptr;
        neighbors["SW"] = nullptr;
        neighbors["NW"] = nullptr;
    }
};
