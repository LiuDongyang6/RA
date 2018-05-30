#include "tiled_map.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}


const float RAMap::speed = 50;
const int RAMap::accurancy = 50;
TMXTiledMap * RAMap::_tiledMap;
Point RAMap::diff;
std::map<Point, bool> RAMap::collision;
std::map<Point, bool> RAMap::oil;
std::vector<Point> RAMap::pre_positions;
std::vector<Point> RAMap::dests;

// on "init" you need to initialize your instance
bool RAMap::init()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//创建地图
	_tiledMap = TMXTiledMap::create("map1.tmx");

	mapInit();
	testForCoord();
	setMovePosition();
	_tiledMap->schedule(schedule_selector(RAMap::moveMap));
    return true;
}

//获取所有地图信息
void RAMap::mapInit(void) {
	TMXLayer *_collision = _tiledMap->getLayer("collision");
	TMXLayer *_oil = _tiledMap->getLayer("oil");
	int num = _tiledMap->getMapSize().width;
	for (int x = 0; x < num; x++) {
		for (int y = 0; y < num; y++) {
			Point pos = Point(x, y);
			int tileGid_c = _collision->getTileGIDAt(Point(x, y));
			if (tileGid_c) 
				collision[pos] = 1;
			else
				collision[pos] = 0;
			int tileGid_o = _oil->getTileGIDAt(Point(x, y));
			if (tileGid_o)
				oil[pos] = 1;
			else
				oil[pos] = 0;
		}
	}
}

//测试坐标
void RAMap::testForCoord(void) {

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [&](Touch* touch, Event* event) {
		Point pos1 = touch->getLocation();
		Point tf = cannotBuildOil(pos1, 3);
		log("%f, %f", tf.x, tf.y);
		return true;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, _tiledMap);
}

// OpenGL坐标转换为Tiledmap坐标
Point RAMap::glCoordToTileCoord(Point gl_coord) {
	//求map原点坐标  
	int map_width = _tiledMap->getMapSize().width;
	int map_height = _tiledMap->getMapSize().height;
	float map_zero_x = _tiledMap->getPosition().x + _tiledMap->getContentSize().width / 2;
	float map_zero_y = _tiledMap->getPosition().y + _tiledMap->getContentSize().height;
	int tile_width = (map_zero_x - _tiledMap->getPosition().x) / map_width * 2;
	int tile_height = (map_zero_y - _tiledMap->getPosition().y) / map_height;
	//O为地图的原点，A是要求的点，oa向量的值  
	float OA_x = gl_coord.x - map_zero_x;
	float OA_y = gl_coord.y - map_zero_y;
	//最大坐标
	int max_coord = _tiledMap->getContentSize().height / tile_height;
	//将GL坐标系中x,y轴的单位向量分解到瓦片坐标系
	//向量i=(tile_width/2, -tile_height/2)
	//向量j=(-tile_width/2, -tile_height/2)
	//向量OA=mi+nj
	float m = OA_x / tile_width - OA_y / tile_height;
	float n = -(OA_x / tile_width + OA_y / tile_height);
	//越界检测
	if (m<0 || n<0 || m>max_coord || n>max_coord) {
		m = -1;
		n = -1;
	}
	if (m - static_cast<int>(m) > 0.5)
		m = static_cast<int>(m) + 1;
	else
		m = static_cast<int>(m);
	if (n - static_cast<int>(n) > 0.5)
		n = static_cast<int>(n) + 1;
	else
		n = static_cast<int>(n);
	return Point(m, n);
}

//设置地图移动方向
void RAMap::setMovePosition(void) {
	//创建拖动监听
	auto listener_map_move = EventListenerMouse::create();
	////绑定触摸&计算拖动的偏移量
	listener_map_move->onMouseMove = [=](Event* event) {
		EventMouse* cursor = static_cast<EventMouse*>(event);
		float cursor_x = cursor->getCursorX();
		float cursor_y = cursor->getCursorY();
		auto node = _tiledMap;
		auto currentPos = node->getPosition();
		Size winSize = Director::getInstance()->getWinSize();
		if (cursor) {
			diff = Point(0, 0);
			if (cursor_x < accurancy) {
				diff.x = 1;
				if (cursor_y < accurancy)
					diff.y = 1;
				else if (cursor_y > winSize.height - accurancy)
					diff.y = -1;
				else
					diff.y = 0;
			}
			else if (cursor_x > winSize.width - accurancy) {
				diff.x = -1;
				if (cursor_y < accurancy)
					diff.y = 1;
				else if (cursor_y > winSize.height - accurancy)
					diff.y = -1;
				else
					diff.y = 0;
			}
			else {
				diff.x = 0;
				if (cursor_y < accurancy)
					diff.y = 1;
				else if (cursor_y > winSize.height - accurancy)
					diff.y = -1;
				else
					diff.y = 0;
			}
		}
	};
	//对地图添加监听
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener_map_move, _tiledMap);
}

//移动地图
void RAMap::moveMap(float dt) {

	auto node = _tiledMap;
	auto currentPos = node->getPosition();
	Point mapPos = _tiledMap->getPosition();
	Point viewPos = mapPos + speed * diff;
	Size winSize = Director::getInstance()->getWinSize();
	Size tileSize = _tiledMap->getTileSize();
	if (viewPos.x < winSize.width - 105 * tileSize.width || viewPos.x > 0)
	{
		diff.x = 0;
	}

	if (viewPos.y < winSize.height - 106 * tileSize.height || viewPos.y >0)
	{
		diff.y = 0;
	}
	node->setPosition(currentPos + speed * diff);
}

//判断是否可以建造普通建筑
bool RAMap::cannotBuildNormal(Point build_point/*GL 坐标*/, int size) {
	//从GL坐标转化为瓦片坐标  
	Point tile_coord = glCoordToTileCoord(build_point);
	if (tile_coord.x <= 0)
		return false;
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			log("%f, %f", tile_coord.x,tile_coord.y);
			if (collision[tile_coord])
				return false;
			tile_coord.y--;
		}
		tile_coord.y += size;
		tile_coord.x--;
	}
	return true;
}

//判断是否可以建造油井
Point RAMap::cannotBuildOil(Point build_point, int size) {
	//从GL坐标转化为瓦片坐标  
	Point tile_coord = glCoordToTileCoord(build_point);
	if (tile_coord.x <= 0)
		return false;
	if (oil[tile_coord]) {
		while (oil[tile_coord]) {
			if (oil[Point(tile_coord.x + 1, tile_coord.y)])
				tile_coord.x++;
			else if (oil[Point(tile_coord.x, tile_coord.y + 1)])
				tile_coord.y++;
			else
				return _tiledMap->getLayer("oil")->getPositionAt(tile_coord);
		}
	}
	else
		return Point(-1000, -1000);
}

//确定建造普通建筑
void RAMap::sureToBuildNormal(Point build_point, int size) {
	Point tile_coord = glCoordToTileCoord(build_point);
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			collision[tile_coord] = 1;
			tile_coord.y--;

		}
		tile_coord.y += size;
		tile_coord.x--;
	}
}

//确定建造油井 传入的point是cannotBuildOil返回的
void RAMap::sureToBuildOil(Point build_point, int size) {
	Point tile_coord = glCoordToTileCoord(build_point);
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			oil[tile_coord] = 0;
			tile_coord.y--;
		}
		tile_coord.y += size;
		tile_coord.x--;
	}
}

//普通建筑被摧毁
void RAMap::destroyNormalBuildings(Point des_pos, int size) {
	Point tile_coord = glCoordToTileCoord(des_pos);
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			collision[tile_coord] = 0;
			tile_coord.y--;

		}
		tile_coord.y += size;
		tile_coord.x--;
	}
}

//油田被摧毁
void RAMap::destroyOilBuildings(Point des_pos, int size) {
	Point tile_coord = glCoordToTileCoord(des_pos);
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			oil[tile_coord] = 1;
			tile_coord.y--;

		}
		tile_coord.y += size;
		tile_coord.x--;
	}
}

//士兵占据这格
void RAMap::setSoilderCollision(Point pos) {
	Point tile_coord = glCoordToTileCoord(pos);
	collision[tile_coord] = 1;
}

//士兵离开这格
void RAMap::removeSoilderCollision(cocos2d::Point pos) {
	Point tile_coord = glCoordToTileCoord(pos);
	collision[tile_coord] = 0;
}

//不想解释
std::map<Point, int> RAMap::tryEightdirection(Point position, Point dest) {
	std::map<Point, int> open_list;
	if (collision[Point(position.x - 1, position.y)])//1
		open_list[Point(position.x - 1, position.y)] = 10000;
	else
		open_list[Point(position.x - 1, position.y)] =
		10 + 10 * (abs(dest.x - position.x + 1) + abs(dest.y - position.y));
	if (collision[Point(position.x + 1, position.y)])//2
		open_list[Point(position.x + 1, position.y)] = 10000;
	else
		open_list[Point(position.x + 1, position.y)] =
		10 + 10 * (abs(dest.x - position.x - 1) + abs(dest.y - position.y));
	if (collision[Point(position.x, position.y - 1)])//3
		open_list[Point(position.x, position.y - 1)] = 10000;
	else
		open_list[Point(position.x, position.y - 1)] =
		10 + 10 * (abs(dest.x - position.x) + abs(dest.y - position.y + 1));
	if (collision[Point(position.x, position.y + 1)])//4
		open_list[Point(position.x, position.y + 1)] = 10000;
	else
		open_list[Point(position.x, position.y + 1)] =
		10 + 10 * (abs(dest.x - position.x) + abs(dest.y - position.y - 1));
	if (collision[Point(position.x + 1, position.y + 1)])//5
		open_list[Point(position.x + 1, position.y + 1)] = 10000;
	else
		open_list[Point(position.x + 1, position.y + 1)] =
		14 + 10 * (abs(dest.x - position.x - 1) + abs(dest.y - position.y - 1));
	if (collision[Point(position.x - 1, position.y - 1)])//6
		open_list[Point(position.x - 1, position.y - 1)] = 10000;
	else
		open_list[Point(position.x - 1, position.y - 1)] =
		14 + 10 * (abs(dest.x - position.x + 1) + abs(dest.y - position.y + 1));
	if (collision[Point(position.x - 1, position.y + 1)])//7
		open_list[Point(position.x - 1, position.y + 1)] = 10000;
	else
		open_list[Point(position.x - 1, position.y + 1)] =
		14 + 10 * (abs(dest.x - position.x + 1) + abs(dest.y - position.y - 1));
	if (collision[Point(position.x + 1, position.y - 1)])//8
		open_list[Point(position.x + 1, position.y - 1)] = 10000;
	else
		open_list[Point(position.x + 1, position.y - 1)] =
		14 + 10 * (abs(dest.x - position.x - 1) + abs(dest.y - position.y + 1));
	return open_list;
}

//寻路
std::map<Point, Point> RAMap::findRoutine(std::vector<Point> positions, Point dest) {
	std::map<Point, Point> routine;
	if (dests[0] != dest) {
		pre_positions = positions;
		dests.clear();
		dests.push_back(dest);
	}//如果选定了新的终点
	dest = *(dests.end() - 1);//获取新的终点
	for (auto position : positions) {
		if (find(dests.begin(), dests.end() - 1, position) != dests.end()) {
			routine[position] = position;
		}
		auto position_tile = glCoordToTileCoord(position);
		auto dest_tile = glCoordToTileCoord(dest);
		auto open_list = tryEightdirection(position_tile, dest_tile);
		auto next_step = open_list.begin();
		auto start = open_list.begin();
		for (start; start != open_list.end(); start++) {
			if (start->second < next_step->second)
				next_step = start;
		}
		if (next_step->first == dest_tile) {//dest的collision一定是0
			routine[position] = dest;
			for (int i = 0; i + position_tile.x < 128; i++) {
				auto open_list_2 = tryEightdirection(Point(position_tile.x + i, position_tile.y),
					Point(0, 0));
				auto next_dest = open_list_2.begin();
				auto start_2 = open_list_2.begin();
				for (start_2; start_2 != open_list_2.end(); start_2++) {
					if (start_2->second < next_dest->second)
						next_dest = start_2;
				}
				if (next_dest->second != 10000) {
					dests.push_back(_tiledMap->getLayer("collision")->getPositionAt(next_dest->first));
					dest = _tiledMap->getLayer("collision")->getPositionAt(next_dest->first);
					dest_tile = glCoordToTileCoord(dest);
					break;
				}
			}
			if (dest_tile == position_tile) {//运气很背，前面都没找到的话
				for (int i = 0; i + position_tile.x > 0; i--) {
					auto open_list_2 = tryEightdirection(Point(position_tile.x + i, position_tile.y),
						Point(0, 0));
					auto next_dest = open_list_2.begin();
					auto start_2 = open_list_2.begin();
					for (start_2; start_2 != open_list_2.end(); start_2++) {
						if (start_2->second < next_dest->second)
							next_dest = start_2;
					}
					if (next_dest->second != 10000) {
						dests.push_back(_tiledMap->getLayer("collision")->getPositionAt(next_dest->first));
						dest = _tiledMap->getLayer("collision")->getPositionAt(next_dest->first);
						break;
					}
				}
			}
			continue;
		}
		if (next_step->second == 10000) 
			routine[position] = position;
		else 
			routine[position] = _tiledMap->getLayer("collision")->getPositionAt(next_step->first);
	}
	return routine;
}