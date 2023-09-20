#include "TextureManager.h"

unordered_map<string, sf::Texture> TextureManager::myTextures;

void TextureManager::LoadTexture(string textureName) {
    string filePath = textureName + ".png";
    myTextures[textureName].loadFromFile(filePath);
}

sf::Texture& TextureManager::GetTexture(string textureName) {
    if (myTextures.find(textureName) == myTextures.end()) {
        // if the texture isn't already in the map, loads the texture
        LoadTexture(textureName);
    }

    return myTextures[textureName];
}

void TextureManager::Clear() {
    // empties the map
    myTextures.clear();
}
