#include "../include/tilesMap.h"


TileImage::TileImage(const std::string& path){
    m_img = cv::imread(std::filesystem::current_path().string() + "/" + path, CV_8UC4);

    if(m_img.empty()){
        throw std::runtime_error("Image not created");
    }

    //cv::Mat mask;
    //cv::inRange(img, cv::Scalar(0, 0, 0, 0), cv::Scalar(255, 255, 255, 0), mask);
    //img.setTo(cv::Scalar(0, 0, 0, 255), mask); //set background color instead of trancperency
}

TileImage::TileImage(const std::string& path, cv::Mat background)
{
    background.copyTo(m_img);
    cv::Mat tmpImg = cv::imread(std::filesystem::current_path().string() + "/" + path, CV_8UC4);

    if(tmpImg.empty()){
        throw std::runtime_error("Image not created");
    }

    for(int y = 0; y<tmpImg.size().height; y++){
        for(int x = 0; x<tmpImg.size().width; x++){
            cv::Vec4b& tmpPix = tmpImg.at<cv::Vec4b>(y, x);
            if(tmpPix[3] != 0){
                cv::Vec4b& pix = m_img.at<cv::Vec4b>(y, x);
                pix += tmpPix[3] * static_cast<cv::Vec4i>(tmpPix) / 255;
            }
        }
    }

    // std::array<cv::Mat, 4> channels;
    // cv::split(tmpImg, channels);
    // //cv::add(m_img, tmpImg, m_img, channels[3]);
    // m_img = m_img + channels[3] * tmpImg;
}

void TileImage::rotate90Deg(uint32_t n){
    n = n%4;
    switch (n)
    {
    case 1:
        cv::rotate(m_img, m_img, cv::ROTATE_90_CLOCKWISE);
        break;
    case 2:
        cv::rotate(m_img, m_img, cv::ROTATE_180);
        break;
    case 3:
        cv::rotate(m_img, m_img, cv::ROTATE_90_COUNTERCLOCKWISE);
        break;
    default:
        break;
    }
}

void Tile::createCopy(Tile& dst){
    if(m_img.getImage().empty())
        return;
    
    dst.m_sides = m_sides;
    dst.m_chanse = m_chanse;

    const cv::Mat& thisImg = m_img.getImage();
    cv::Mat& dstImg = dst.getImage();

    dstImg.create(thisImg.size(), thisImg.type());
    thisImg.copyTo(dstImg);
    //dstImg = thisImg.clone();
    
    if(dst.getImage().empty())
        throw std::runtime_error("Tile: createCopy: Image is empty");
}

cv::Mat& TileImage::getImage(){
    return m_img;
}

TileSides::TileSides(){
    for(int i = 0; i<m_sides.size(); i++)
        m_sides[i] = "";
    m_n = 0;
}

TileSides::TileSides(uint32_t count, const std::string& features):m_n(count){
    if(features.size() != m_n * 4)
        throw std::runtime_error("Error: wrong features size or count. ( count * 4 != features.size() ) ");

    for(int i = 0; i<4*m_n; i++){
        m_sides[i/m_n] += features[i];
    }
}

TileSides::TileSides(uint32_t count, std::string up, std::string right, std::string bottom, std::string left):m_n(count){
    if(up.size() != m_n)
        throw std::runtime_error("Error: wrong up size of count. ( count != up.size() )");
    
    if(right.size() != m_n)
        throw std::runtime_error("Error: wrong right size of count. ( count != right.size() )");
    
    if(bottom.size() != m_n)
        throw std::runtime_error("Error: wrong bottom size of count. ( count != bottom.size() )");
    
    if(left.size() != m_n)
        throw std::runtime_error("Error: wrong left size of count. ( count != left.size() )");
    
    m_sides[0] = std::move(up);
    m_sides[1] = std::move(right);
    m_sides[2] = std::move(bottom);
    m_sides[3] = std::move(left);
}

void TileSides::rotate90Deg(uint32_t n){
    n = n%4;
    switch(n){
        case 1:{
            std::string currSide = m_sides[3];
            std::string tmp;
            for(int i = 0; i<4; i++){
                tmp = m_sides[i];
                if(i%2==0)
                    std::reverse(currSide.begin(), currSide.end());
                m_sides[i] = currSide;
                currSide = tmp;
            }
            break;
        }
        case 2:{
            std::swap(m_sides[0], m_sides[2]);
            std::swap(m_sides[1], m_sides[3]);
            for(int i = 0; i<4; i++)
                std::reverse(m_sides[i].begin(), m_sides[i].end());
            break;
        }
        case 3:{
            std::string currSide = m_sides[0];
            std::string tmp;
            for(int i = 3; i>=0; i--){
                tmp = m_sides[i];
                if(i%2)
                    std::reverse(currSide.begin(), currSide.end());
                m_sides[i] = currSide;
                currSide = tmp;
            }
        }
        default:
            break;
    }
}

uint32_t TileSides::getCountFeatures() const noexcept{
    return m_n;
}

std::array<std::string, 4> TileSides::getSides() const noexcept{
    return m_sides;
}

bool TileSides::operator==(const TileSides& right) const noexcept{
    if(m_n != right.m_n) return 0;
    if(m_sides != right.m_sides) return 0;
    return 1;
}

bool TileSides::operator!=(const TileSides& right) const noexcept{
    return !(*this == right);
}

Tile::Tile(TileImage img, TileSides sides, uint32_t chanse):m_img(img), m_sides(sides), m_chanse(chanse){}

void Tile::rotate90Deg(uint32_t n){
    m_img.rotate90Deg(n);
    m_sides.rotate90Deg(n);
}

uint32_t Tile::getCountFeatures() const noexcept{
    return m_sides.getCountFeatures();
}

uint32_t Tile::getChanse() const noexcept{
    return m_chanse;
}

decltype(std::declval<TileImage>().getImage()) Tile::getImage(){
    return m_img.getImage();
}

decltype(std::declval<TileSides>().getSides()) Tile::getSides() const{
    return m_sides.getSides();
}

bool Tile::operator==(Tile& right) noexcept{
    if(m_sides != right.m_sides) return 0;
    bool q = 0;
    //if(cv::sum(m_img.getImage() != right.m_img.getImage()) == cv::Scalar(0,0,0,0)) return 0;
    return 1;
}

bool Tile::operator!=(Tile& right) noexcept{
    return !(*this == right);
}

TileSet::TileSet(uint32_t countFeatures, cv::Size tileSize)
                :m_features(countFeatures), m_tileSize(tileSize){}

std::vector<Tile> TileSet::_generateTiles(Tile& tile){
    std::vector<Tile> tiles;
    tiles.push_back(tile);

    //tile.createCopy(tmpTile);
    for(int i = 0; i<3; i++){
        Tile tmpTile;
        tiles.back().createCopy(tmpTile);
        tmpTile.rotate90Deg(1);
        tiles.push_back(tmpTile);
    }

    //check if tile have duplicate and delete it
    for(int i = 0; i<tiles.size(); i++){
        for(int j = i + 1; j<tiles.size(); j++){
            if(tiles[i]==tiles[j]){
                tiles.erase(tiles.begin() + j);
                j--;
            }
        }
    }
    return tiles;
}

void TileSet::addTile(Tile tile){
    if(m_features != tile.getCountFeatures()
      || m_tileSize != tile.getImage().size())
    {
        std::string tileName = "";
        for(int i = 0; i<tile.getSides().size(); i++){
            tileName += tile.getSides()[i];
        }
        if(m_features != tile.getCountFeatures()){
            std::cout<<"Error: wrong tile's (\""<<tileName
                    <<"\") count feature. Expected "<<m_features
                    <<" count but tile have "<<tile.getCountFeatures()<<".\n";
        }
        else if(m_tileSize != tile.getImage().size()){
            std::cout<<"Error: wrong tile's (\""<<tileName
                    <<"\") size. Expected "<<m_tileSize
                    <<" size but tile have "<<tile.getImage().size()<<".\n";
        }
        return;
    }
    std::vector<Tile> newTiles = _generateTiles(tile);
    m_tiles.insert(m_tiles.end(), std::make_move_iterator(newTiles.begin()), std::make_move_iterator(newTiles.end()));
    m_sorted = false;
}

void TileSet::_sortTileSides(){
    //clear array
    for(int j = 0; j < m_tileSides.size(); j++){
        m_tileSides[j].clear();
    }

    //insert all indexes of tiles by it side
    for(size_t i = 0; i < m_tiles.size(); i++){
        for(int j = 0; j < m_tileSides.size(); j++){
            const std::string side = m_tiles[i].getSides()[j];
            if(m_tileSides[j].contains(side)){
                m_tileSides[j][side].push_back(i);
            }
            else{
                m_tileSides[j].insert({ side, std::vector<size_t>{i} });
            }
        }
    }

    //sorting every vector in the map
    for(int j = 0; j < m_tileSides.size(); j++){
        for(auto& it: m_tileSides[j]){
            std::sort(it.second.begin(), it.second.end());
        }
    }

    m_sorted = true;
}

cv::Size TileSet::getTileSize() const{
    return m_tileSize;
}

std::vector<size_t> TileSet::_getTilesIdBySides(const std::string sides[4]){

    std::vector<size_t> tileIndxs; //todo: convert vector to something better for deletions

    //function that find common ints in tileIndxs and given vector
    auto findCommon = [&](const std::vector<size_t>& tmpTileIndxs){
        std::vector<size_t> newTileIndxs;
        for(auto num:tmpTileIndxs){
            auto it = std::lower_bound(tileIndxs.begin(), tileIndxs.end(), num);
            if(it != tileIndxs.end() && *it == num)
                newTileIndxs.push_back(num);
        }
        tileIndxs = std::move(newTileIndxs);
    };
    
    int sum = 0; //sum for check if all sides == ""
    int sizeSide = 4;

    //find tileIndxs
    for(int i = 0; i<sizeSide; i++){
        if(tileIndxs.size() == 0){
            if(sides[i]!=""){
                if(m_tileSides[i].contains(sides[i]))
                    tileIndxs = m_tileSides[i][sides[i]];
                else return std::vector<size_t>(); //todo: maybe do something better
            }
            else sum++;
        }
        else if(sides[i]!=""){
            if(m_tileSides[i].contains(sides[i])){
                const std::vector<size_t>& tmpTileIndxs = m_tileSides[i][sides[i]];
                findCommon(tmpTileIndxs);
            }
            else return std::vector<size_t>();
        }
        else sum++;
    }

    //if all sides == ""
    if(sum == sizeSide){
        tileIndxs.resize(m_tiles.size());
        std::iota(tileIndxs.begin(), tileIndxs.end(), 0);
    }
    
    return tileIndxs;
}

std::vector<Tile> TileSet::_getTilesBySides(const std::string sides[4]){

    std::vector<size_t> tileIndxs = _getTilesIdBySides(sides);
    
    //convert size_t vector to Tile vector
    std::vector<Tile> result;
    result.reserve(tileIndxs.size());
    for(auto i:tileIndxs){
        result.push_back(m_tiles[i]);
    }
    return result;
}

std::vector<Tile> TileSet::getTilesBySides(const std::string& up, const std::string& right, const std::string& bottom, const std::string& left){
    if(!m_sorted) _sortTileSides();

    const std::string sides[] = {up, right, bottom, left};

    return _getTilesBySides(sides);
}

std::vector<Tile> TileSet::getTilesBySides(const std::string sides[4]){
    if(!m_sorted) _sortTileSides();

    return _getTilesBySides(sides);
}

std::vector<size_t> TileSet::getTilesIdBySides(const std::string sides[4]){
    if(!m_sorted) _sortTileSides();

    return _getTilesIdBySides(sides);
}

Tile TileSet::getTileById(size_t id) const{
    return m_tiles[id];
}


void TileSet::saveCurrentTileSet(const std::string& directory){
    int c = 0;
    std::filesystem::create_directories(directory);
    //std::filesystem::remove_all("current_tile_set/*");

    for(auto& i:m_tiles){
        cv::imwrite(directory+"/"+std::to_string(c++)+".png", i.getImage());
    }
}

std::array<std::string, 4> TileMapGenerator::_getNeighbourSides(const TileSet& tileSet, std::pair<uint32_t, uint32_t> tileCoords) const{
    std::array<std::string, 4> result;
    if(tileCoords.second <= 0) result[0] = "";
    else{
        const size_t id = m_tileMap[tileCoords.second - 1][tileCoords.first];
        if(id == 0) result[0] = "";
        else result[0] = tileSet.getTileById(id - 1).getSides()[2]; //2 is bottom(otherwise side)
    }

    if(tileCoords.first >= m_mapSize.width-1) result[1] = "";
    else{
        const size_t id = m_tileMap[tileCoords.second][tileCoords.first + 1];
        if(id == 0) result[1] = "";
        else result[1] = tileSet.getTileById(id - 1).getSides()[3]; //3 is left(otherwise side)
    }

    if(tileCoords.second >= m_mapSize.height-1) result[2] = "";
    else{
        const size_t id = m_tileMap[tileCoords.second + 1][tileCoords.first];
        if(id == 0) result[2] = "";
        else result[2] = tileSet.getTileById(id - 1).getSides()[0]; //0 is up(otherwise side)
    }

    if(tileCoords.first <= 0) result[3] = "";
    else{
        const size_t id = m_tileMap[tileCoords.second][tileCoords.first - 1];
        if(id == 0) result[3] = "";
        else result[3] = tileSet.getTileById(id - 1).getSides()[1]; //1 is right(otherwise side)
    }
    return result;
}

void TileMapGenerator::_generateImage(TileSet& tileSet){
    for(size_t x = 0; x<m_tileMap.size(); x++){
        for(size_t y = 0; y<m_tileMap[x].size(); y++){
            if(m_tileMap[x][y]!=0)
            tileSet.getTileById(m_tileMap[x][y]-1).getImage().copyTo(m_mapImage(
                cv::Rect(cv::Size(y*m_tileSize.height, x*m_tileSize.width), m_tileSize))
                );
        }
    }
}

uint32_t TileMapGenerator::_doGenerateStep(TileSet& tileSet){
    //take random tile from vector
    size_t tileIndx = rand()%m_insertTilePlaces.size();
    auto tilePlace = m_insertTilePlaces[tileIndx];
    m_insertTilePlaces[tileIndx] = std::move(m_insertTilePlaces.back());
    m_insertTilePlaces.pop_back();

    //check if tile was visited, then skip it
    //if(m_tileMap[tilePlace.second][tilePlace.first] != 0) return !m_insertTilePlaces.empty();
    if(m_visitedMap[tilePlace.second][tilePlace.first] > 0) return (!m_insertTilePlaces.empty() ? 2 : false );
    m_visitedMap[tilePlace.second][tilePlace.first]++;
    
    auto needSides = _getNeighbourSides(tileSet, tilePlace);
    auto candidateTiles = tileSet.getTilesIdBySides(needSides.data());

    //choosing tile with chanse biases 
    if(candidateTiles.size()!=0){
        uint32_t maxRand = 0;
        size_t chooseId;
        for(auto& i: candidateTiles){
            maxRand += tileSet.getTileById(i).getChanse();
        }
        if(maxRand == 0) chooseId = rand()%candidateTiles.size();
        else{
            uint32_t randNum = rand()%maxRand;
            chooseId = 0;
            while(chooseId<candidateTiles.size()){
                uint32_t chanse = tileSet.getTileById(candidateTiles[chooseId]).getChanse();
                if(chanse <= randNum) randNum -= chanse;
                else break;
                chooseId++;
            }
        }
        m_tileMap[tilePlace.second][tilePlace.first] = candidateTiles[chooseId] + 1;
        //m_tileMap[tilePlace.second][tilePlace.first] = candidateTiles[rand()%candidateTiles.size()] + 1;
    }

    //adding tiles in the queue
    if(tilePlace.second > 0) m_insertTilePlaces.push_back({tilePlace.first, tilePlace.second - 1});
    if(tilePlace.first+1 < m_mapSize.width) m_insertTilePlaces.push_back({tilePlace.first + 1, tilePlace.second});
    if(tilePlace.second+1 < m_mapSize.height) m_insertTilePlaces.push_back({tilePlace.first, tilePlace.second + 1});
    if(tilePlace.first > 0) m_insertTilePlaces.push_back({tilePlace.first - 1, tilePlace.second});
    
    return !m_insertTilePlaces.empty();
}

void TileMapGenerator::_initMaps(){
    m_mapImage = cv::Mat(m_mapSize.height * m_tileSize.height, 
                        m_mapSize.width * m_tileSize.width,
                        CV_8UC4,
                        cv::Scalar(0,0,0,0)
                    );
    m_tileMap = std::vector<std::vector<size_t>>(
                m_mapSize.height, std::vector<size_t>(m_mapSize.width, 0)
                );

    m_visitedMap = std::vector<std::vector<int>>(
                   m_mapSize.height, std::vector<int>(m_mapSize.width, 0)
                   );
}

void TileMapGenerator::generateMap(TileSet& tileSet, cv::Size sizeMap){
    m_mapSize = sizeMap;
    m_tileSize = tileSet.getTileSize();

    _initMaps();

    m_insertTilePlaces.clear();
    m_insertTilePlaces.reserve(m_mapSize.area());
    m_insertTilePlaces.push_back({rand()%m_mapSize.width, rand()%m_mapSize.height});
    
    while(_doGenerateStep(tileSet));

    _generateImage(tileSet);
}

void TileMapGenerator::generateMap_saveSteps(TileSet& tileSet, cv::Size sizeMap, const std::string& saveDirectory){
    m_mapSize = sizeMap;
    m_tileSize = tileSet.getTileSize();

    _initMaps();
    m_insertTilePlaces.clear();
    m_insertTilePlaces.reserve(m_mapSize.area());
    m_insertTilePlaces.push_back({rand()%m_mapSize.width, rand()%m_mapSize.height});
    
    std::filesystem::create_directories(saveDirectory);

    size_t imgCount = 0;
    while(uint32_t ret = _doGenerateStep(tileSet)){
        if(ret != 2){
            _generateImage(tileSet);
            cv::imwrite(saveDirectory + "/" + std::to_string(imgCount) + ".png", getMap());
            imgCount++;
        }
    }

}

cv::Mat TileMapGenerator::getMap(){
    return m_mapImage;
}

