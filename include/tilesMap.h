#pragma once
#include <iostream>
#include <filesystem>
#include <string>
#include <array>
#include <vector>
#include <numeric>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

class TileImage{
private:
    cv::Mat m_img;

public:
    TileImage() = default;
    TileImage(const std::string& path);
    TileImage(const std::string& path, cv::Mat background);

    //rotate clockwise sides by 90 degrees n times
    void rotate90Deg(uint32_t n);

    cv::Mat& getImage();
};

class TileSides{
private:
    std::array<std::string, 4> m_sides; //[0]-up, [1]-right, [2]-bottom, [3]-left
    uint32_t m_n; //count of side's features

public:
    TileSides();
    //count - count of side's features
    //features - features of all sides
    //           need features.size() = count*4
    TileSides(uint32_t count, const std::string& features);

    //count - count of side's features
    //up, ..., left - features for every side
    //                for example up.size() = count
    TileSides(uint32_t count, std::string up, std::string right, std::string bottom, std::string left);

    //rotate clockwise sides by 90 degrees n times
    void rotate90Deg(uint32_t n);

    uint32_t getCountFeatures() const noexcept;
    std::array<std::string, 4> getSides() const noexcept;

    bool operator==(const TileSides& right) const noexcept;
    bool operator!=(const TileSides& right) const noexcept;

};

class Tile{
private:
    TileImage m_img;
    TileSides m_sides;
    uint32_t m_chanse; //chanse for choose this tile
public:
    Tile() = default;
    Tile(TileImage img, TileSides sides, uint32_t chanse = 1);

    void rotate90Deg(uint32_t n);
    //dst - where the copy will be moved
    void createCopy(Tile& dst);

    uint32_t getCountFeatures() const noexcept;
    uint32_t getChanse() const noexcept;

    decltype(std::declval<TileImage>().getImage()) getImage();
    decltype(std::declval<TileSides>().getSides()) getSides() const;

    bool operator==(Tile& right) noexcept;
    bool operator!=(Tile& right) noexcept;
};

class TileSet{
private:
    std::vector<Tile> m_tiles;
    std::array<std::map<std::string, std::vector<size_t>>, 4> m_tileSides;
    bool m_sorted = 0;
    uint32_t m_features; //count features
    cv::Size m_tileSize; //width and height of all tiles

private:
    std::vector<Tile> _generateTiles(Tile& tile);
    std::vector<size_t> _getTilesIdBySides(const std::string sides[4]);
    std::vector<Tile> _getTilesBySides(const std::string sides[4]);
    void _sortTileSides();

public:
    TileSet(uint32_t countFeatures, cv::Size tileSize);
    //add tile to tileset
    void addTile(Tile tile);

    cv::Size getTileSize() const;
    //get vector of suitable tiles by it sides
    std::vector<Tile> getTilesBySides(const std::string& up, const std::string& right, const std::string& bottom, const std::string& left);
    //get vector of suitable tiles by it sides
    std::vector<Tile> getTilesBySides(const std::string sides[4]);
    //get vector of suitable tiles's id by it sides
    std::vector<size_t> getTilesIdBySides(const std::string sides[4]);
    Tile getTileById(size_t id) const;
    //saves tiles as images in directory
    void saveCurrentTileSet(const std::string& directory);
};

class TileMapGenerator{
private:
    cv::Mat m_mapImage;
    std::vector<std::vector<size_t>> m_tileMap;
    std::vector<std::vector<int>> m_visitedMap; //for disable infinity loop in generateMap()
    cv::Size m_mapSize;
    cv::Size m_tileSize;

    std::vector<std::pair<uint32_t, uint32_t>> m_insertTilePlaces; //queue for inserting tile on the map

private:
    bool _isValidMapCoords(const std::pair<uint32_t, uint32_t>& coord) const;
    std::array<std::string, 4> _getNeighbourSides(const TileSet& tileSet, std::pair<uint32_t, uint32_t> tileCoords) const;
    void _generateImage(TileSet& tileSet);
    //return true if can do next step or false if can't do next step
    uint32_t _doGenerateStep(TileSet& tileSet);
    //rewrite or create new maps
    void _initMaps();

public:
    TileMapGenerator()=default;

    //sizeMap - map's size where width and height means count tiles by x and y coords
    void generateMap(TileSet& tileSet, cv::Size sizeMap);
    //generate map and save every generated tile on map to saveDirectory as separated image
    void generateMap_saveSteps(TileSet& tileSet, cv::Size sizeMap, const std::string& saveDirectory);

    //get last generated map
    cv::Mat getMap();
};