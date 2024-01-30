#include <iostream>
#include <filesystem>
#include <string>
#include <opencv2/opencv.hpp>
#include "../include/tilesMap.h"

int main(){
    
    TileSet tileSet(1, {48, 48});

    Tile tile1(TileImage("../data/set_1/1.png"), TileSides(1, "0000"));
    Tile tile2(TileImage("../data/set_1/2.png", tile1.getImage()), TileSides(1, "0101"));
    Tile tile3(TileImage("../data/set_1/3.png", tile1.getImage()), TileSides(1, "1001"));
    Tile tile4(TileImage("../data/set_1/4.png", tile1.getImage()), TileSides(1, "1111"));
    Tile tile5(TileImage("../data/set_1/5.png", tile1.getImage()), TileSides(1, "0100"));
    Tile tile6(TileImage("../data/set_1/6.png", tile1.getImage()), TileSides(1, "1011"));

    tileSet.addTile(tile1);
    tileSet.addTile(tile2);
    tileSet.addTile(tile3);
    tileSet.addTile(tile4);
    tileSet.addTile(tile5);
    tileSet.addTile(tile6);

    auto a = tileSet.getTilesBySides("\0", "\0", "\0", "1");
    tileSet.saveCurrentTileSet("current_tile_set");

    TileMapGenerator mapGenerator;
    mapGenerator.generateMap(tileSet, {20,20});

    cv::imwrite("abb.png", mapGenerator.getMap());
    cv::waitKey();
    return 0;
}