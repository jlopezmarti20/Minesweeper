#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Game.h"
#include "TextureManager.h"
#include "Timer.h"
using namespace std;

// load and store the leaderboard
struct Player {
    string name;
    int time;

    bool operator==(const Player& other) const {
        return name == other.name && time == other.time;
    }
};

bool comparePlayers(Player p1, Player p2) {
    return p1.time < p2.time;
}

vector<sf::Sprite> Digits();
unordered_map<string, sf::Sprite> Sprites();
unordered_map<string, vector<vector<sf::Sprite>>> GetSpriteVect(Board myBoard);

int main()
{
    //displayLeaderboard(leaderboard);
    // made the board up here so I could use GetBoardWidth() and GetWindowHeight() for the welcome window
    string boardFile = "/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/board_config.cfg";
    Board gameBoard(boardFile);

    sf::Font temp;
    temp.loadFromFile("/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/font.ttf");

    sf::RenderWindow welcomeWindow(sf::VideoMode(gameBoard.GetBoardWidth(), gameBoard.GetWindowHeight()), "Minesweeper", sf::Style::Close);

    sf::Text title;
    title.setFont(temp);
    title.setString("WELCOME TO MINESWEEPER!");
    title.setCharacterSize(24);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold | sf::Text::Underlined);
    sf::FloatRect titleRect = title.getLocalBounds();
    title.setOrigin(titleRect.left + titleRect.width / 2.0f,
                    titleRect.top + titleRect.height / 2.0f);
    title.setPosition(sf::Vector2f(gameBoard.GetBoardWidth() / 2.0f, gameBoard.GetWindowHeight() / 2.0f - 150));

    // enter your name
    sf::Text name1;
    name1.setFont(temp);
    name1.setString("Enter your name:");
    name1.setCharacterSize(20);
    name1.setFillColor(sf::Color::White);
    name1.setStyle(sf::Text::Bold);
    sf::FloatRect name1Rect = name1.getLocalBounds();
    name1.setOrigin(name1Rect.left + name1Rect.width / 2.0f,
                    name1Rect.top + name1Rect.height / 2.0f);
    name1.setPosition(sf::Vector2f(gameBoard.GetBoardWidth() / 2.0f, gameBoard.GetWindowHeight() / 2.0f - 75));

    sf::Text userInput;
    userInput.setFont(temp);
    userInput.setCharacterSize(18);
    userInput.setFillColor(sf::Color::Yellow);
    userInput.setStyle(sf::Text::Bold);
    sf::FloatRect userInputRect = userInput.getLocalBounds();
    userInput.setOrigin(userInputRect.left + userInputRect.width / 2.0f,
                        userInputRect.top + userInputRect.height / 2.0f);
    userInput.setPosition(sf::Vector2f((gameBoard.GetBoardWidth() / 2.0f) - userInputRect.width, gameBoard.GetWindowHeight() / 2.0f - 45));

    string x = "";

    while(welcomeWindow.isOpen()) {
        sf::Event event;
        while(welcomeWindow.pollEvent(event)) {
            if(event.type == sf::Event::Closed){
                welcomeWindow.close();
                return 0;
            }
            else if(event.type == sf::Event::TextEntered){
                if(event.text.unicode >= 32 && event.text.unicode < 128) {
                    if (x.size() < 10) {
                        if (isalpha(event.text.unicode)){
                            x += event.text.unicode;
                            userInput.setString(x + "|");
                        }
                    }
                }
            }
            else if(event.type == sf::Event::KeyPressed){
                if(event.key.code == sf::Keyboard::BackSpace) {
                    if(!x.empty()) {
                        x.pop_back();
                        userInput.setString(x);
                    }
                }
                if(event.key.code == sf::Keyboard::Enter){
                    if (x.size() >= 1) {
                        x[0] = toupper(x[0]);
                        for (unsigned int i=1; i<x.size(); i++) {
                            x[i] = tolower(x[i]);
                        }
                        welcomeWindow.close();
                    }
                }
            }

        }
        welcomeWindow.clear(sf::Color::Blue);
        welcomeWindow.draw(title);
        welcomeWindow.draw(name1);
        welcomeWindow.draw(userInput);
        welcomeWindow.display();
    }

    // start up the game
    bool playStatus = true;
    bool isPaused = false;
//    string boardFile = "board_config.cfg";
//    Board gameBoard(boardFile);
    auto digit = Digits();
    auto mySprite = Sprites();
    auto spriteVect = GetSpriteVect(gameBoard);
    sf::RenderWindow gameWindow(sf::VideoMode(gameBoard.GetBoardWidth(), gameBoard.GetWindowHeight()), "Minesweeper");

    // to be used later in the leaderboard
    vector<Player> players;

    // set up the timer
    Timer timer;
    timer.Start();

    while (gameWindow.isOpen()) {
        sf::Event gameEvent;
        sf::Vector2i mousePos = sf::Mouse::getPosition(gameWindow);

        // handle events
        while (gameWindow.pollEvent(gameEvent)){
            if (gameEvent.type == sf::Event::Closed) {
                gameWindow.close();
            }
            if ((gameEvent.type == sf::Event::MouseButtonPressed && sf::Mouse::isButtonPressed(sf::Mouse::Right)) && gameBoard.face == "happyFace") {
                // if the user right clicks for flagging
                if (playStatus){
                    if ((mousePos.x >= 0 && mousePos.x <= gameBoard.GetBoardWidth()) && (mousePos.y >= 0 && mousePos.y <= gameBoard.GetBoardHeight())) {
                        // enters if the user clicked within the bounds of the board
                        // finds the tile specific to the mouse position
                        int row = mousePos.y / 32;
                        int col = mousePos.x / 32;

                        if (!gameBoard.tileVect[row][col].isFlagged && !gameBoard.tileVect[row][col].isRevealed) {
                            // if the tile is not flagged or revealed
                            gameBoard.tileVect[row][col].isFlagged = true; // make the tile flagged
                            gameBoard.flagCount++;
                        }
                        else if (gameBoard.tileVect[row][col].isFlagged){
                            gameBoard.tileVect[row][col].isFlagged = false; // remove flag
                            gameBoard.flagCount--;
                        }
                    }
                }
            }
        }

        // get rid of anything already on the window for a new game and reset the timer
        gameWindow.clear();

        // draw the board
        for (int row = 0; row < gameBoard.numRows; row++) {
            for (int col = 0; col < gameBoard.numCols; col++) {

                gameWindow.draw(spriteVect["revealedTiles"][row][col]);
                // display the number of adjacent mines if these tiles are revealed and aren't mines
                if (!isPaused) {
                    if (!gameBoard.tileVect[row][col].hasMine &&
                        gameBoard.tileVect[row][col].numAdjacents == 1)
                        gameWindow.draw(spriteVect["nums1"][row][col]);
                    if (!gameBoard.tileVect[row][col].hasMine &&
                        gameBoard.tileVect[row][col].numAdjacents == 2)
                        gameWindow.draw(spriteVect["nums2"][row][col]);
                    if (!gameBoard.tileVect[row][col].hasMine &&
                        gameBoard.tileVect[row][col].numAdjacents == 3)
                        gameWindow.draw(spriteVect["nums3"][row][col]);
                    if (!gameBoard.tileVect[row][col].hasMine &&
                        gameBoard.tileVect[row][col].numAdjacents == 4)
                        gameWindow.draw(spriteVect["nums4"][row][col]);
                    if (!gameBoard.tileVect[row][col].hasMine &&
                        gameBoard.tileVect[row][col].numAdjacents == 5)
                        gameWindow.draw(spriteVect["nums5"][row][col]);
                    if (!gameBoard.tileVect[row][col].hasMine &&
                        gameBoard.tileVect[row][col].numAdjacents == 6)
                        gameWindow.draw(spriteVect["nums6"][row][col]);
                    if (!gameBoard.tileVect[row][col].hasMine &&
                        gameBoard.tileVect[row][col].numAdjacents == 7)
                        gameWindow.draw(spriteVect["nums7"][row][col]);
                    if (!gameBoard.tileVect[row][col].hasMine &&
                        gameBoard.tileVect[row][col].numAdjacents == 8)
                        gameWindow.draw(spriteVect["nums8"][row][col]);

                    // show a mine if the tile has a mine
                    if (gameBoard.tileVect[row][col].hasMine) gameWindow.draw(spriteVect["mines"][row][col]);

                    // if the tile has not been clicked, display the hidden tile image
                    if (!gameBoard.tileVect[row][col].isRevealed) gameWindow.draw(spriteVect["hiddenTiles"][row][col]);

                    // if the tile is flagged, display the flag image
                    if (gameBoard.tileVect[row][col].isFlagged) gameWindow.draw(spriteVect["flags"][row][col]);

                    // shows the mines after the game is over
                    if (gameBoard.isRevealed && gameBoard.tileVect[row][col].hasMine) {
                        gameWindow.draw(spriteVect["mines"][row][col]);
                        timer.Pause();
                    }

                    if (playStatus) {
                        sf::FloatRect selectedLoc = spriteVect["hiddenTiles"][row][col].getGlobalBounds();
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
                            selectedLoc.contains(mousePos.x, mousePos.y)) {
                            gameBoard.LeftClick(row, col);
                            if (gameBoard.tileVect[row][col].hasMine) {
                                timer.Pause();
                            }
                        }
                    }
                }
                else {
                    gameWindow.draw(spriteVect["revealedTiles"][row][col]);
                }
            }
        }

        // draw counter
        int digitSize = 21;
        int count = gameBoard.numMines - gameBoard.flagCount;

        // digit 10 is the negative sign
        digit[10].setPosition(0, gameBoard.GetBoardHeight());
        if (count < 0) {
            gameWindow.draw(digit[10]);
            count *= -1;
        }

        int onesPlace = count % 10;
        count /= 10;
        digit[onesPlace].setPosition(digitSize * 3, gameBoard.GetBoardHeight());
        gameWindow.draw(digit[onesPlace]);

        int tensPlace = count % 10;
        count /= 10;
        digit[tensPlace].setPosition(digitSize * 2, gameBoard.GetBoardHeight());
        gameWindow.draw(digit[tensPlace]);

        int hundredsPlace = count % 10;
        count /= 10;
        digit[hundredsPlace].setPosition(digitSize, gameBoard.GetBoardHeight());
        gameWindow.draw(digit[hundredsPlace]);

        // draw the faces
        mySprite[gameBoard.face].setPosition(sf::Vector2f((gameBoard.GetBoardWidth() / 2.0f) - 32, gameBoard.GetBoardHeight() + 16));
        gameWindow.draw(mySprite[gameBoard.face]);
        sf::FloatRect faceSelect = mySprite[gameBoard.face].getGlobalBounds();
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && faceSelect.contains(mousePos.x, mousePos.y)) {
            Board tempBoard(boardFile);
            gameBoard = tempBoard;
            timer.Reset();
            if (playStatus){
                timer.Start();
            }
        }
        if (gameBoard.numRevealed == gameBoard.numRows * gameBoard.numCols - gameBoard.numMines && gameBoard.face != "deadFace") {
            gameBoard.face = "winFace";
            timer.Pause();
            for (int row = 0; row < gameBoard.numRows; row++) {
                for (int col = 0; col < gameBoard.numCols; col++) {
                    if (gameBoard.tileVect[row][col].hasMine) {
                        gameBoard.tileVect[row][col].isFlagged = true;
                        gameBoard.flagCount = gameBoard.numMines;
                    }
                }
            }
            Player currplayer;
            currplayer.name = x + "*";
            currplayer.time = timer.get_elapsed_time();
            if (find_if(players.begin(), players.end(), [&currplayer](const Player& player) {
                return player == currplayer;
            }) == players.end()) {
                // Player not found, add to vector
                players.push_back(currplayer);
            }
        }

        // draw the debug button
        int buttonSize = 64;
        mySprite["debug"].setPosition(gameBoard.GetBoardWidth() - 304, gameBoard.GetBoardHeight() + 16);
        gameWindow.draw(mySprite["debug"]);
        sf::FloatRect debugSelect = mySprite["debug"].getGlobalBounds();
        if ((sf::Mouse::isButtonPressed(sf::Mouse::Left) && debugSelect.contains(mousePos.x, mousePos.y)) && gameBoard.face == "happyFace") {
            while (gameWindow.pollEvent(gameEvent)) {
                if (gameEvent.type == sf::Event::MouseButtonPressed) {
                    if (!gameBoard.isRevealed) gameBoard.isRevealed = true;
                    else gameBoard.isRevealed = false;
                }
            }
        }

        // draw the pause and play button
        if (playStatus){
            mySprite["pause"].setPosition(gameBoard.GetBoardWidth() - 240, gameBoard.GetBoardHeight() + 16);
            gameWindow.draw(mySprite["pause"]);
            sf::FloatRect pauseSelect = mySprite["pause"].getGlobalBounds();
            if ((sf::Mouse::isButtonPressed(sf::Mouse::Left) && pauseSelect.contains(mousePos.x, mousePos.y)) && gameBoard.face == "happyFace") {
                while (gameWindow.pollEvent(gameEvent)) {
                    if (gameEvent.type == sf::Event::MouseButtonPressed) {
                        playStatus = false;
                        isPaused = true;
                        timer.Pause();
                    }
                }
            }
        }
        else {
            mySprite["play"].setPosition(gameBoard.GetBoardWidth() - 240, gameBoard.GetBoardHeight() + 16);
            gameWindow.draw(mySprite["play"]);
            sf::FloatRect pauseSelect = mySprite["play"].getGlobalBounds();
            if ((sf::Mouse::isButtonPressed(sf::Mouse::Left) && pauseSelect.contains(mousePos.x, mousePos.y)) && gameBoard.face == "happyFace") {
                while (gameWindow.pollEvent(gameEvent)) {
                    if (gameEvent.type == sf::Event::MouseButtonPressed) {
                        playStatus = true;
                        isPaused = false;
                        timer.Resume();
                    }
                }
            }
        }

        // draw the leaderboard button
        mySprite["leaderboard"].setPosition(gameBoard.GetBoardWidth() - 176, gameBoard.GetBoardHeight() + 16);
        gameWindow.draw(mySprite["leaderboard"]);
        sf::FloatRect leaderSelect = mySprite["leaderboard"].getGlobalBounds();
        if ((sf::Mouse::isButtonPressed(sf::Mouse::Left) && leaderSelect.contains(mousePos.x, mousePos.y))) {
            while (gameWindow.pollEvent(gameEvent)) {
                if (gameEvent.type == sf::Event::MouseButtonPressed) {
                    // stop the timer if the leaderboard is opened
                    timer.Pause();
                    int leaderWidth = gameBoard.GetBoardWidth() / 2;
                    int leaderHeight = gameBoard.GetWindowHeight() / 2;
                    sf::RenderWindow leaderWindow(sf::VideoMode(leaderWidth, leaderHeight), "Leaderboard", sf::Style::Close);

                    sf::Text leaderTitle;
                    leaderTitle.setFont(temp);
                    leaderTitle.setString("LEADERBOARD");
                    leaderTitle.setCharacterSize(20);
                    leaderTitle.setFillColor(sf::Color::White);
                    leaderTitle.setStyle(sf::Text::Bold | sf::Text::Underlined);
                    sf::FloatRect leaderTitleRect = leaderTitle.getLocalBounds();
                    leaderTitle.setOrigin(leaderTitleRect.left + leaderTitleRect.width / 2.0f, leaderTitleRect.top + leaderTitleRect.height / 2.0f);
                    leaderTitle.setPosition(sf::Vector2f(leaderWidth / 2.0f, leaderHeight / 2.0f - 120));

                    //vector<Player> leadplayers = players;
                    ifstream file("files/leaderboard.txt");

                    string line;
                    while (getline(file, line)) {
                        Player player;
                        size_t pos = line.find(",");
                        string timeStr = line.substr(0, pos);
                        int minutes = stoi(timeStr.substr(0, 2));
                        int seconds = stoi(timeStr.substr(3, 2));
                        player.time = minutes * 60 + seconds;
                        player.name = line.substr(pos + 1);
                        players.push_back(player);
                    }

                    sort(players.begin(), players.end(), [](const Player& a, const Player& b) {
                        return a.name < b.name || (a.name == b.name && a.time < b.time);
                    });

                    // Remove duplicates
                    auto end = unique(players.begin(), players.end(), [](const Player& a, const Player& b) {
                        return a.name == b.name && a.time == b.time;
                    });
                    players.erase(end, players.end());

                    sort(players.begin(), players.end(), comparePlayers);

                    // Concatenate players and times into one string with two new lines between each row
                    string leaderboardStr;
                    if (players.size() >= 5){
                        for (int i = 0; i < 5; i++) {
                            int minInt = players[i].time / 60;
                            int secInt = players[i].time % 60;
                            string minStr;
                            string secStr;
                            if (minInt < 10) {
                                minStr = '0' + to_string(minInt);
                            }
                            else {
                                minStr = to_string(minInt);
                            }
                            if (secInt < 10) {
                                secStr = '0' + to_string(secInt);
                            }
                            else {
                                secStr = to_string(secInt);
                            }
                            leaderboardStr += to_string(i + 1) + ".        " + minStr + ":" + secStr + "      " + players[i].name + "\n\n";
                        }
                    }
                    else {
                        for (int i = 0; i < players.size(); i++) {
                            int minInt = players[i].time / 60;
                            int secInt = players[i].time % 60;
                            string minStr;
                            string secStr;
                            if (minInt < 10) {
                                minStr = '0' + to_string(minInt);
                            }
                            else {
                                minStr = to_string(minInt);
                            }
                            if (secInt < 10) {
                                secStr = '0' + to_string(secInt);
                            }
                            else {
                                secStr = to_string(secInt);
                            }
                            leaderboardStr += to_string(i + 1) + ".        " + minStr + ":" + secStr + "      " + players[i].name + "\n\n";
                        }
                    }


                    ofstream outfile("files/leaderboard.txt");
                    for (int i = 0; i < players.size(); i++) {
                        int minInt = players[i].time / 60;
                        int secInt = players[i].time % 60;
                        string minStr;
                        string secStr;
                        if (minInt < 10) {
                            minStr = '0' + to_string(minInt);
                        }
                        else {
                            minStr = to_string(minInt);
                        }
                        if (secInt < 10) {
                            secStr = '0' + to_string(secInt);
                        }
                        else {
                            secStr = to_string(secInt);
                        }
                        outfile << minStr << ":" << secStr << "," << players[i].name << endl;
                    }
                    // draw the leaderboard text
                    sf::Text leaders;
                    leaders.setFont(temp);
                    leaders.setString(leaderboardStr);
                    leaders.setCharacterSize(18);
                    leaders.setFillColor(sf::Color::White);
                    sf::FloatRect centerLeaders = leaders.getLocalBounds();
                    leaders.setOrigin(centerLeaders.left + centerLeaders.width / 2.0f, centerLeaders.top + centerLeaders.height / 2.0f);
                    leaders.setPosition(sf::Vector2f(leaderWidth / 2.0f, leaderHeight / 2.0f + 20));

                    while(leaderWindow.isOpen()){
                        sf::Event leaderEvent;
                        while(leaderWindow.pollEvent(leaderEvent)) {
                            if (leaderEvent.type == sf::Event::Closed){
                                leaderWindow.close();
                            }
                        }
                        leaderWindow.clear(sf::Color::Blue);
                        leaderWindow.draw(leaderTitle);
                        leaderWindow.draw(leaders);
                        leaderWindow.display();
                    }
                }
            }
            //isPaused = false;
            // resume the timer after the leaderboard is closed
            if (playStatus) {
                timer.Resume();
            }
        }

        // draw the timer
        int minTens = 0;
        int minOnes = 0;
        int secTens = 0;
        int secOnes = 0;
        int timeInSeconds = timer.get_elapsed_time();
        if (timeInSeconds >= 60) {
            int timeMinutes = timeInSeconds / 60;
            timeInSeconds %= 60;
            minTens = timeMinutes / 10;
            minOnes = timeMinutes % 10;
            secTens = timeInSeconds / 10;
            secOnes = timeInSeconds % 10;
        }
        else {
            minTens = 0;
            minOnes = 0;
            secTens = timeInSeconds / 10;
            secOnes = timeInSeconds % 10;
        }

        // draw the minutes tens place
        digit[minTens].setPosition(gameBoard.GetBoardWidth() - 97, gameBoard.GetBoardHeight() + 32);
        gameWindow.draw(digit[minTens]);

        // draw the minutes ones place
        digit[minOnes].setPosition(gameBoard.GetBoardWidth() - 97 + 21, gameBoard.GetBoardHeight() + 32);
        gameWindow.draw(digit[minOnes]);

        // draw the seconds tens place
        digit[secTens].setPosition(gameBoard.GetBoardWidth() - 54, gameBoard.GetBoardHeight() + 32);
        gameWindow.draw(digit[secTens]);

        // draw the seconds ones place
        digit[secOnes].setPosition(gameBoard.GetBoardWidth() - 54 + 21, gameBoard.GetBoardHeight() + 32);
        gameWindow.draw(digit[secOnes]);

        // display everything
        gameWindow.display();
    }
    return 0;
}

vector<sf::Sprite> Digits() {
    vector<sf::Sprite> digits;
    int digitSize = 21;
    string digitsImg = "/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/images/digits";
    sf::Sprite digit0(TextureManager::GetTexture(digitsImg));
    sf::Sprite digit1(TextureManager::GetTexture(digitsImg));
    sf::Sprite digit2(TextureManager::GetTexture(digitsImg));
    sf::Sprite digit3(TextureManager::GetTexture(digitsImg));
    sf::Sprite digit4(TextureManager::GetTexture(digitsImg));
    sf::Sprite digit5(TextureManager::GetTexture(digitsImg));
    sf::Sprite digit6(TextureManager::GetTexture(digitsImg));
    sf::Sprite digit7(TextureManager::GetTexture(digitsImg));
    sf::Sprite digit8(TextureManager::GetTexture(digitsImg));
    sf::Sprite digit9(TextureManager::GetTexture(digitsImg));
    sf::Sprite digitNeg(TextureManager::GetTexture(digitsImg));

    sf::IntRect rect0(digitSize * 0, 0, digitSize, 32);
    sf::IntRect rect1(digitSize * 1, 0, digitSize, 32);
    sf::IntRect rect2(digitSize * 2, 0, digitSize, 32);
    sf::IntRect rect3(digitSize * 3, 0, digitSize, 32);
    sf::IntRect rect4(digitSize * 4, 0, digitSize, 32);
    sf::IntRect rect5(digitSize * 5, 0, digitSize, 32);
    sf::IntRect rect6(digitSize * 6, 0, digitSize, 32);
    sf::IntRect rect7(digitSize * 7, 0, digitSize, 32);
    sf::IntRect rect8(digitSize * 8, 0, digitSize, 32);
    sf::IntRect rect9(digitSize * 9, 0, digitSize, 32);
    sf::IntRect rectNeg(digitSize * 10, 0, digitSize, 32);

    digit0.setTextureRect(rect0);
    digit1.setTextureRect(rect1);
    digit2.setTextureRect(rect2);
    digit3.setTextureRect(rect3);
    digit4.setTextureRect(rect4);
    digit5.setTextureRect(rect5);
    digit6.setTextureRect(rect6);
    digit7.setTextureRect(rect7);
    digit8.setTextureRect(rect8);
    digit9.setTextureRect(rect9);
    digitNeg.setTextureRect(rectNeg);

    digits.push_back(digit0);
    digits.push_back(digit1);
    digits.push_back(digit2);
    digits.push_back(digit3);
    digits.push_back(digit4);
    digits.push_back(digit5);
    digits.push_back(digit6);
    digits.push_back(digit7);
    digits.push_back(digit8);
    digits.push_back(digit9);
    digits.push_back(digitNeg);

    return digits;
}

unordered_map<string, sf::Sprite> Sprites() {
    unordered_map<string, sf::Sprite> sprites;


    sf::Sprite happyFaceSprite(TextureManager::GetTexture("/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/images/face_happy"));
    sf::Sprite deadFaceSprite(TextureManager::GetTexture("/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/images/face_lose"));
    sf::Sprite winFaceSprite(TextureManager::GetTexture("/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/images/face_win"));
    sf::Sprite debugSprite(TextureManager::GetTexture("/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/images/debug"));
    sf::Sprite playSprite(TextureManager::GetTexture("/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/images/play"));
    sf::Sprite pauseSprite(TextureManager::GetTexture("/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/images/pause"));
    sf::Sprite leaderboardSprite(TextureManager::GetTexture("/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/images/leaderboard"));

    sprites["happyFace"] = happyFaceSprite;
    sprites["deadFace"] = deadFaceSprite;
    sprites["winFace"] = winFaceSprite;
    sprites["debug"] = debugSprite;
    sprites["play"] = playSprite;
    sprites["pause"] = pauseSprite;
    sprites["leaderboard"] = leaderboardSprite;

    return sprites;
}

unordered_map<string, vector<vector<sf::Sprite>>> GetSpriteVect(Board board) {
    unordered_map<string, vector<vector<sf::Sprite>>> spriteMap;

    sf::Sprite flagSprite(TextureManager::GetTexture("/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/images/flag"));
    sf::Sprite mineSprite(TextureManager::GetTexture("/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/images/mine"));
    sf::Sprite num1Sprite(TextureManager::GetTexture("/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/images/number_1"));
    sf::Sprite num2Sprite(TextureManager::GetTexture("/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/images/number_2"));
    sf::Sprite num3Sprite(TextureManager::GetTexture("/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/images/number_3"));
    sf::Sprite num4Sprite(TextureManager::GetTexture("/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/images/number_4"));
    sf::Sprite num5Sprite(TextureManager::GetTexture("/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/images/number_5"));
    sf::Sprite num6Sprite(TextureManager::GetTexture("/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/images/number_6"));
    sf::Sprite num7Sprite(TextureManager::GetTexture("/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/images/number_7"));
    sf::Sprite num8Sprite(TextureManager::GetTexture("/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/images/number_8"));
    sf::Sprite hiddenTileSprite(TextureManager::GetTexture("/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/images/tile_hidden"));
    sf::Sprite revealedTileSprite(TextureManager::GetTexture("/Users/jesuslopez/Documents/CLionProjects/Minesweeper/files/images/tile_revealed"));

    auto revealedTiles = board.FilledWith(revealedTileSprite);
    auto nums1 = board.FilledWith(num1Sprite);
    auto nums2 = board.FilledWith(num2Sprite);
    auto nums3 = board.FilledWith(num3Sprite);
    auto nums4 = board.FilledWith(num4Sprite);
    auto nums5 = board.FilledWith(num5Sprite);
    auto nums6 = board.FilledWith(num6Sprite);
    auto nums7 = board.FilledWith(num7Sprite);
    auto nums8 = board.FilledWith(num8Sprite);
    auto mines = board.FilledWith(mineSprite);
    auto hiddenTiles = board.FilledWith(hiddenTileSprite);
    auto flags = board.FilledWith(flagSprite);

    for (int row = 0; row < board.numRows; row++){
        for (int col = 0; col < board.numCols; col++){
            revealedTiles[row][col].setPosition(col * 32, row * 32);
            nums1[row][col].setPosition(col * 32, row * 32);
            nums2[row][col].setPosition(col * 32, row * 32);
            nums3[row][col].setPosition(col * 32, row * 32);
            nums4[row][col].setPosition(col * 32, row * 32);
            nums5[row][col].setPosition(col * 32, row * 32);
            nums6[row][col].setPosition(col * 32, row * 32);
            nums7[row][col].setPosition(col * 32, row * 32);
            nums8[row][col].setPosition(col * 32, row * 32);
            mines[row][col].setPosition(col * 32, row * 32);
            hiddenTiles[row][col].setPosition(col * 32, row * 32);
            flags[row][col].setPosition(col * 32, row * 32);
        }
    }

    spriteMap["revealedTiles"] = revealedTiles;
    spriteMap["nums1"] = nums1;
    spriteMap["nums2"] = nums2;
    spriteMap["nums3"] = nums3;
    spriteMap["nums4"] = nums4;
    spriteMap["nums5"] = nums5;
    spriteMap["nums6"] = nums6;
    spriteMap["nums7"] = nums7;
    spriteMap["nums8"] = nums8;
    spriteMap["mines"] = mines;
    spriteMap["hiddenTiles"] = hiddenTiles;
    spriteMap["flags"] = flags;

    return spriteMap;
}