#include "tiled_map.h"
#include"RAObject/RASoldier.h"
#include "SimpleAudioEngine.h"

#define random(a,b) (rand()%(b-a+1)+a)
USING_NS_CC;

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}


const float RAMap::speed = 30;
const int RAMap::accurancy = 50;
TMXTiledMap * RAMap::_tiledMap;
Point RAMap::diff;
std::map<Point, bool> RAMap::collision;
std::map<Point, bool> RAMap::oil;
std::map<Point, bool> RAMap::soldier_collision;
std::vector<Point> RAMap::routines;

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
			soldier_collision[pos] = 0;
		}
	}
}

//测试坐标
void RAMap::testForCoord(void) {

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [&](Touch* touch, Event* event) {
		auto dest = touch->getLocation();
		auto tile = glCoordToTileCoord(dest);
		auto relate_1 = tileCoordToRelatedCoord(tile);
		auto relate = relatedCoordToTileCoord(relate_1);/*
		log("-------------tile %f, %f", tile.x, tile.y);
		log("-------------relate %f, %f", relate_1.x, relate_1.y);
		log("-------------%f, %f", relate.x, relate.y);
		log("-------------soldier collision %d %f,%f", 
			soldier_collision[Point(tile.x - 3, tile.y - 3)], tile.x - 3, tile.y - 3);*/
		auto oil = cannotBuildOil(relate_1, 4);
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
	float map_zero_y = _tiledMap->getPosition().y + _tiledMap->getContentSize().height - 12.5;
	int tile_width = (map_zero_x - _tiledMap->getPosition().x) / map_width * 2;
	int tile_height = (map_zero_y  + 12.5 - _tiledMap->getPosition().y) / map_height;
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

//瓦片坐标转相对坐标
Point RAMap::tileCoordToRelatedCoord(Point tile_coord) {
	//求map原点坐标  
	int map_width = _tiledMap->getMapSize().width;
	int map_height = _tiledMap->getMapSize().height;
	float map_zero_x = _tiledMap->getContentSize().width / 2;
	float map_zero_y = _tiledMap->getContentSize().height - 12.5;
	//显示尺寸与属性不同
	float tile_width = map_zero_x / map_width;
	float tile_height = ((map_zero_y + 12.5) / map_height) / 2;
	return Point((map_zero_x + tile_coord.x * tile_width - tile_coord.y * tile_width),
		(map_zero_y - tile_coord.x * tile_height - tile_coord.y * tile_height));
}

//相对坐标转瓦片坐标
Point RAMap::relatedCoordToTileCoord(Point related_coord) {
	//求map原点坐标  
	int map_width = _tiledMap->getMapSize().width;
	int map_height = _tiledMap->getMapSize().height;
	float map_zero_x = _tiledMap->getContentSize().width / 2;
	float map_zero_y = _tiledMap->getContentSize().height - 12.5;
	int tile_width = 50;
	int tile_height = 25;
	//O为地图的原点，A是要求的点，oa向量的值  
	float OA_x = related_coord.x - map_zero_x;
	float OA_y = related_coord.y - map_zero_y;
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
bool RAMap::cannotBuildNormal(Point build_point, int size) {
	Point tile_coord = relatedCoordToTileCoord(build_point);
	if (tile_coord.x <= 0)
		return false;
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			if (collision[tile_coord] || soldier_collision[tile_coord])
				return false;
			tile_coord.y++;
		}
		tile_coord.y -= size;
		tile_coord.x++;
	}
	return true;
}

//判断是否可以建造油井
Point RAMap::cannotBuildOil(Point build_point, int size) {
	Point tile_coord = relatedCoordToTileCoord(build_point);
	if (oil[tile_coord]) {
		while (oil[tile_coord]) {
			if (oil[Point(tile_coord.x + 1, tile_coord.y)])
				tile_coord.x++;
			else if (oil[Point(tile_coord.x, tile_coord.y + 1)])
				tile_coord.y++;
			else 
			{	
				log("%f, %f", tile_coord.x, tile_coord.y);
				return tileCoordToRelatedCoord(tile_coord);
			}
		}
	}
	else
		return Point(-1000, -1000);
}

//确定建造普通建筑
void RAMap::sureToBuildNormal(Point build_point, int size) {
	Point tile_coord = relatedCoordToTileCoord(build_point);
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			collision[tile_coord] = 1;
			tile_coord.y--;

		}
		tile_coord.y += size;
		tile_coord.x--;
	}
}

//确定建造油井 
void RAMap::sureToBuildOil(Point build_point, int size) {
	Point tile_coord = relatedCoordToTileCoord(build_point);
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
	Point tile_coord = relatedCoordToTileCoord(des_pos);
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
	Point tile_coord = relatedCoordToTileCoord(des_pos);
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
void RAMap::setSoldierCollision(Point pos, const int size) {
	Point tile_coord = relatedCoordToTileCoord(pos);
	for (int x = 0; x != size; x++) {
		for (int y = 0; y != size; y++) {
			soldier_collision[Point(tile_coord.x - x, tile_coord.y - y)] = 1;
		}
	}
}

//士兵离开这格
void RAMap::removeSoldierCollision(Point pos, const int size) {
	Point tile_coord = relatedCoordToTileCoord(pos);
	for (int x = 0; x != size; x++) {
		for (int y = 0; y != size; y++) {
			soldier_collision[Point(tile_coord.x - x, tile_coord.y - y)] = 0;
		}
	}
}

//不想解释
std::map<Point, int> RAMap::tryEightdirection(Point position, Point dest, const int size) {
	std::map<Point, int> open_list;
	bool cannotmove = 0;
	for (int x = 0; x != size; x++) {
		for (int y = 0; y != size; y++) {
			if (collision[Point(position.x - 1 - x, position.y - y)]) {
				cannotmove = 1;
				goto left;
			}
		}
	}
left:	if (cannotmove)//1
		open_list[Point(position.x - 1, position.y)] = 10000;
	else
		open_list[Point(position.x - 1, position.y)] =
		10 + 10 * (abs(dest.x - position.x + 1) + abs(dest.y - position.y));
	cannotmove = 0;
	for (int x = 0; x != size; x++) {
		for (int y = 0; y != size; y++) {
			if (collision[Point(position.x + 1 - x, position.y - y)]) {
				cannotmove = 1;
				goto right;
			}
		}
	}
right:	if (cannotmove)//2
		open_list[Point(position.x + 1, position.y)] = 10000;
	else
		open_list[Point(position.x + 1, position.y)] =
		10 + 10 * (abs(dest.x - position.x - 1) + abs(dest.y - position.y));
	cannotmove = 0;
	for (int x = 0; x != size; x++) {
		for (int y = 0; y != size; y++) {
			if (collision[Point(position.x - x, position.y - 1 - y)]) {
				cannotmove = 1;
				goto down;
			}
		}
	}
down:	if (cannotmove)//3
		open_list[Point(position.x, position.y - 1)] = 10000;
	else
		open_list[Point(position.x, position.y - 1)] =
		10 + 10 * (abs(dest.x - position.x) + abs(dest.y - position.y + 1));
	cannotmove = 0;
	for (int x = 0; x != size; x++) {
		for (int y = 0; y != size; y++) {
			if (collision[Point(position.x - x, position.y + 1 - y)]) {
				cannotmove = 1;
				goto up;
			}
		}
	}
up:	if (cannotmove)//4
		open_list[Point(position.x, position.y + 1)] = 10000;
	else
		open_list[Point(position.x, position.y + 1)] =
		10 + 10 * (abs(dest.x - position.x) + abs(dest.y - position.y - 1));
	cannotmove = 0;
	for (int x = 0; x != size; x++) {
		for (int y = 0; y != size; y++) {
			if (collision[Point(position.x + 1 - x, position.y + 1 - y)]) {
				cannotmove = 1;
				goto upright;
			}
		}
	}
upright:	if (cannotmove)//5
		open_list[Point(position.x + 1, position.y + 1)] = 10000;
	else
		open_list[Point(position.x + 1, position.y + 1)] =
		14 + 10 * (abs(dest.x - position.x - 1) + abs(dest.y - position.y - 1));
	cannotmove = 0;
	for (int x = 0; x != size; x++) {
		for (int y = 0; y != size; y++) {
			if (collision[Point(position.x - 1 - x, position.y - 1 - y)]) {
				cannotmove = 1;
				goto downleft;
			}
		}
	}
downleft:	if (cannotmove)//6
		open_list[Point(position.x - 1, position.y - 1)] = 10000;
	else
		open_list[Point(position.x - 1, position.y - 1)] =
		14 + 10 * (abs(dest.x - position.x + 1) + abs(dest.y - position.y + 1));
	cannotmove = 0;
	for (int x = 0; x != size; x++) {
		for (int y = 0; y != size; y++) {
			if (collision[Point(position.x -1 - x, position.y + 1 - y)]) {
					cannotmove = 1;
					goto upleft;
			}
		}
	}
upleft:	if (cannotmove)//7
		open_list[Point(position.x - 1, position.y + 1)] = 10000;
	else
		open_list[Point(position.x - 1, position.y + 1)] =
		14 + 10 * (abs(dest.x - position.x + 1) + abs(dest.y - position.y - 1));
	cannotmove = 0;
	for (int x = 0; x != size; x++) {
		for (int y = 0; y != size; y++) {
			if (collision[Point(position.x + 1 - x, position.y - 1 - y)]) {
				cannotmove = 1;
				goto downright;
			}
		}
	}
downright:	if (cannotmove)//8
		open_list[Point(position.x + 1, position.y - 1)] = 10000;
	else
		open_list[Point(position.x + 1, position.y - 1)] =
		14 + 10 * (abs(dest.x - position.x - 1) + abs(dest.y - position.y + 1));
	return open_list;
}

int RAMap::aStar(Point so_tilecoord, Point dest_tile, const int size) {
	auto open_list_1 = tryEightdirection(so_tilecoord, dest_tile, size);
	auto next_step = open_list_1.begin();
	auto start = open_list_1.begin();
	for (start; start != open_list_1.end(); start++) {
		if (start->second < next_step->second)
			next_step = start;
	}
	if (next_step->second == 10000) {
		routines.push_back(tileCoordToRelatedCoord(so_tilecoord));
		aStar(so_tilecoord, dest_tile, size);
		return 1;
	}
	auto open_list_2 = tryEightdirection(next_step->first, dest_tile, size);
	auto third_step = open_list_2.begin();
	auto start_2 = open_list_2.begin();
	for (start_2; start_2 != open_list_2.end(); start_2++) {
		if (start_2->second < third_step->second)
			third_step = start_2;
	}
	bool isfind = 0;
	if (third_step->second != 10000) {
		for (auto pos : open_list_1) {
			if (pos.first == third_step->first) {
				isfind = 1;
				break;
			}
		}
	}
	if (isfind) {
		routines.push_back(tileCoordToRelatedCoord(third_step->first));
		if (third_step->first == dest_tile)
			return 1;
		else {
			aStar(third_step->first, dest_tile, size);
			return 1;
		}
	}
	else {
		routines.push_back(tileCoordToRelatedCoord(next_step->first));
		if (next_step->first == dest_tile)
			return 1;
		else {
			aStar(next_step->first, dest_tile, size);
			return 1;
		}
	}
}

//士兵寻路
std::vector<Point> RAMap::findRoutineAllAtOnce(RASoldier* soldier, Point &dest, const int size) {
	routines.clear();
	Point dest_tile = relatedCoordToTileCoord(dest);
	Point so_related_coord = Point(soldier->getPosition().x, soldier->getPosition().y);
	Point so_tilecoord = relatedCoordToTileCoord(soldier->getPosition());
	bool cannotmove_1 = 0;
	for (int x = 0; x != size; x++) {
		for (int y = 0; y != size; y++) {
			if (collision[Point(dest_tile.x - x, dest_tile.y - y)] ||
				soldier_collision[Point(dest_tile.x - x, dest_tile.y - y)]) {
				cannotmove_1 = 1;
			}
		}
	}
	if (cannotmove_1) {
		for (int a = 1; a + dest_tile.x < 128 && dest_tile.x - a >0 
			&& dest_tile.y - a > 0 && a + dest_tile.y < 128; a++) {
			bool cannotmove_2 = 0;
			for (int x = 0; x != size; x++) {
				for (int y = 0; y != size; y++) {
					if (collision[Point(dest_tile.x + a - x, dest_tile.y  + a - y)] ||
						soldier_collision[Point(dest_tile.x + a - x, dest_tile.y +a - y)]) {
						cannotmove_2 = 1;
					}
				}
			}
			if (cannotmove_2) {
				dest_tile = Point(dest_tile.x + a, dest_tile.y + a);
				break;
			}
			cannotmove_2 = 0;
			for (int x = 0; x != size; x++) {
				for (int y = 0; y != size; y++) {
					if (collision[Point(dest_tile.x + a - x, dest_tile.y - a - y)] ||
						soldier_collision[Point(dest_tile.x + a - x, dest_tile.y - a - y)]) {
						cannotmove_2 = 1;
					}
				}
			}
			if (cannotmove_2) {
				dest_tile = Point(dest_tile.x + a, dest_tile.y - a);
				break;
			}
			cannotmove_2 = 0;
			for (int x = 0; x != size; x++) {
				for (int y = 0; y != size; y++) {
					if (collision[Point(dest_tile.x - a - x, dest_tile.y - a - y)] ||
						soldier_collision[Point(dest_tile.x - a - x, dest_tile.y - a - y)]) {
						cannotmove_2 = 1;
					}
				}
			}
			if (cannotmove_2) {
				dest_tile = Point(dest_tile.x - a, dest_tile.y - a);
				break;
			}
			cannotmove_2 = 0;
			for (int x = 0; x != size; x++) {
				for (int y = 0; y != size; y++) {
					if (collision[Point(dest_tile.x - a - x, dest_tile.y + a - y)] ||
						soldier_collision[Point(dest_tile.x - a - x, dest_tile.y + a - y)]) {
						cannotmove_2 = 1;
					}
				}
			}
			if (cannotmove_2) {
				dest_tile = Point(dest_tile.x - a, dest_tile.y + a);
				break;
			}
			cannotmove_2 = 0;
			for (int x = 0; x != size; x++) {
				for (int y = 0; y != size; y++) {
					if (collision[Point(dest_tile.x + a - x, dest_tile.y - y)] ||
						soldier_collision[Point(dest_tile.x + a - x, dest_tile.y - y)]) {
						cannotmove_2 = 1;
					}
				}
			}
			if (cannotmove_2) {
				dest_tile = Point(dest_tile.x + a, dest_tile.y);
				break;
			}
			cannotmove_2 = 0;
			for (int x = 0; x != size; x++) {
				for (int y = 0; y != size; y++) {
					if (collision[Point(dest_tile.x - a - x, dest_tile.y - y)] ||
						soldier_collision[Point(dest_tile.x - a - x, dest_tile.y - y)]) {
						cannotmove_2 = 1;
					}
				}
			}
			if (cannotmove_2) {
				dest_tile = Point(dest_tile.x - a, dest_tile.y);
				break;
			}
			cannotmove_2 = 0;
			for (int x = 0; x != size; x++) {
				for (int y = 0; y != size; y++) {
					if (collision[Point(dest_tile.x - x, dest_tile.y - a - y)] ||
						soldier_collision[Point(dest_tile.x - x, dest_tile.y - a - y)]) {
						cannotmove_2 = 1;
					}
				}
			}
			if (cannotmove_2) {
				dest_tile = Point(dest_tile.x, dest_tile.y - a);
				break;
			}
			cannotmove_2 = 0;
			for (int x = 0; x != size; x++) {
				for (int y = 0; y != size; y++) {
					if (collision[Point(dest_tile.x - x, dest_tile.y + a - y)] ||
						soldier_collision[Point(dest_tile.x - x, dest_tile.y + a - y)]) {
						cannotmove_2 = 1;
					}
				}
			}
			if (cannotmove_2) {
				dest_tile = Point(dest_tile.x, dest_tile.y + a);
				break;
			}
		}
	}
	aStar(so_tilecoord, dest_tile, size);
	return routines;
}

std::vector<float> RAMap::findRoutineOneByOne(RASoldier* soldier, Point &dest, const int size) {
	std::vector<float> answer = { -1, -1, -1 };
	Point dest_tile = relatedCoordToTileCoord(dest);
	Point so_related_coord = Point(soldier->getPosition().x, soldier->getPosition().y);
	Point so_tilecoord = relatedCoordToTileCoord(soldier->getPosition());
	bool cannotmove_1 = 0;
	for (int x = 0; x != size; x++) {
		for (int y = 0; y != size; y++) {
			if (collision[Point(dest_tile.x - x, dest_tile.y - y)] ||
				soldier_collision[Point(dest_tile.x - x, dest_tile.y - y)]) {
				cannotmove_1 = 1;
			}
		}
	}
	if (cannotmove_1) {
		for (int a = 1; a + dest_tile.x < 128 && dest_tile.x - a >0
			&& dest_tile.y - a > 0 && a + dest_tile.y < 128; a++) {
			bool cannotmove_2 = 0;
			for (int x = 0; x != size; x++) {
				for (int y = 0; y != size; y++) {
					if (collision[Point(dest_tile.x + a - x, dest_tile.y + a - y)] ||
						soldier_collision[Point(dest_tile.x + a - x, dest_tile.y + a - y)]) {
						cannotmove_2 = 1;
					}
				}
			}
			if (cannotmove_2) {
				dest_tile = Point(dest_tile.x + a, dest_tile.y + a);
				break;
			}
			cannotmove_2 = 0;
			for (int x = 0; x != size; x++) {
				for (int y = 0; y != size; y++) {
					if (collision[Point(dest_tile.x + a - x, dest_tile.y - a - y)] ||
						soldier_collision[Point(dest_tile.x + a - x, dest_tile.y - a - y)]) {
						cannotmove_2 = 1;
					}
				}
			}
			if (cannotmove_2) {
				dest_tile = Point(dest_tile.x + a, dest_tile.y - a);
				break;
			}
			cannotmove_2 = 0;
			for (int x = 0; x != size; x++) {
				for (int y = 0; y != size; y++) {
					if (collision[Point(dest_tile.x - a - x, dest_tile.y - a - y)] ||
						soldier_collision[Point(dest_tile.x - a - x, dest_tile.y - a - y)]) {
						cannotmove_2 = 1;
					}
				}
			}
			if (cannotmove_2) {
				dest_tile = Point(dest_tile.x - a, dest_tile.y - a);
				break;
			}
			cannotmove_2 = 0;
			for (int x = 0; x != size; x++) {
				for (int y = 0; y != size; y++) {
					if (collision[Point(dest_tile.x - a - x, dest_tile.y + a - y)] ||
						soldier_collision[Point(dest_tile.x - a - x, dest_tile.y + a - y)]) {
						cannotmove_2 = 1;
					}
				}
			}
			if (cannotmove_2) {
				dest_tile = Point(dest_tile.x - a, dest_tile.y + a);
				break;
			}
			cannotmove_2 = 0;
			for (int x = 0; x != size; x++) {
				for (int y = 0; y != size; y++) {
					if (collision[Point(dest_tile.x + a - x, dest_tile.y - y)] ||
						soldier_collision[Point(dest_tile.x + a - x, dest_tile.y - y)]) {
						cannotmove_2 = 1;
					}
				}
			}
			if (cannotmove_2) {
				dest_tile = Point(dest_tile.x + a, dest_tile.y);
				break;
			}
			cannotmove_2 = 0;
			for (int x = 0; x != size; x++) {
				for (int y = 0; y != size; y++) {
					if (collision[Point(dest_tile.x - a - x, dest_tile.y - y)] ||
						soldier_collision[Point(dest_tile.x - a - x, dest_tile.y - y)]) {
						cannotmove_2 = 1;
					}
				}
			}
			if (cannotmove_2) {
				dest_tile = Point(dest_tile.x - a, dest_tile.y);
				break;
			}
			cannotmove_2 = 0;
			for (int x = 0; x != size; x++) {
				for (int y = 0; y != size; y++) {
					if (collision[Point(dest_tile.x - x, dest_tile.y - a - y)] ||
						soldier_collision[Point(dest_tile.x - x, dest_tile.y - a - y)]) {
						cannotmove_2 = 1;
					}
				}
			}
			if (cannotmove_2) {
				dest_tile = Point(dest_tile.x, dest_tile.y - a);
				break;
			}
			cannotmove_2 = 0;
			for (int x = 0; x != size; x++) {
				for (int y = 0; y != size; y++) {
					if (collision[Point(dest_tile.x - x, dest_tile.y + a - y)] ||
						soldier_collision[Point(dest_tile.x - x, dest_tile.y + a - y)]) {
						cannotmove_2 = 1;
					}
				}
			}
			if (cannotmove_2) {
				dest_tile = Point(dest_tile.x, dest_tile.y + a);
				break;
			}
		}
	}
	auto open_list_1 = tryEightdirection(so_tilecoord, dest_tile, size);
	auto next_step = open_list_1.begin();
	auto start = open_list_1.begin();
	while (start != open_list_1.end())
	{
		if (next_step->second > start->second)
		{
			next_step = start;
		}
		start++;
	}
	if (next_step->second == 10000)
	{
		answer[0] = soldier->getPosition().x;
		answer[1] = soldier->getPosition().y;
		answer[2] = 0;
		return answer;
	}
	auto open_list_2 = tryEightdirection(next_step->first, dest_tile, size);
	auto third_step = open_list_2.begin();
	start = open_list_1.begin();
	bool isfind = 0;
	while (start != open_list_1.end())
	{
		if (start->first == third_step->first)
		{
			isfind = 1;
			break;
		}
		start++;
	}
	if (isfind)
	{
		answer[0] = tileCoordToRelatedCoord(third_step->first).x;
		answer[1] = tileCoordToRelatedCoord(third_step->first).y;
		if (third_step->first == dest_tile)
			answer[2] = 1;
		else
			answer[2] = 0;
		return answer;
	}
	else 
	{
		answer[0] = tileCoordToRelatedCoord(next_step->first).x;
		answer[1] = tileCoordToRelatedCoord(next_step->first).y + soldier->getScaleY(); 
		if (next_step->first == dest_tile)
			answer[2] = 1;
		else
			answer[2] = 0;
		return answer;
	}
	return answer;
}

//将建筑物建在中心
Point RAMap::setCenter(Point pos) {
	Point tile_coord = relatedCoordToTileCoord(
		Point(pos.x , pos.y));
	return tileCoordToRelatedCoord(tile_coord);
}

//出现士兵
Point RAMap::soldierBirth(Point build_pos, const int size) {
	Point build_tile = relatedCoordToTileCoord(build_pos);
	int direction = random(1, 8);
	if (direction == 1) {
		for (int x = build_tile.x; x != 128 - size; x++) {
			if (collision[Point(x, build_tile.y)] || soldier_collision[Point(x, build_tile.y)])
				continue;
			else {
				bool canreach = 1;
				for (int i = 0; i != size; i++) {
					for (int j = 0; j != size; j++) {
						if (collision[Point(x - i, build_tile.y - j)] ||
							soldier_collision[Point(x - i, build_tile.y - j)]) {
							canreach = 0;
							continue;
						}
					}
				}
				if (canreach) {
					auto answer = tileCoordToRelatedCoord(Point(x, build_tile.y));
					setSoldierCollision(answer, size);
					return answer;
				}
			}
		}
		direction++;
	}
	if (direction == 2) {
		for (int x = build_tile.x; x != 0; x--) {
			if (collision[Point(x, build_tile.y)] || soldier_collision[Point(x, build_tile.y)])
				continue;
			else {
				bool canreach = 1;
				for (int i = 0; i != size; i++) {
					for (int j = 0; j != size; j++) {
						if (collision[Point(x - i, build_tile.y - j)] ||
							soldier_collision[Point(x - i, build_tile.y - j)]) {
							canreach = 0;
							continue;
						}
					}
				}
				if (canreach) {
					auto answer = tileCoordToRelatedCoord(Point(x, build_tile.y));
					setSoldierCollision(answer, size);
					return answer;
				}
			}
		}
		direction++;
	}
	if (direction == 3) {
		for (int y = build_tile.y; y != 128 - size; y++) {
			if (collision[Point(build_tile.x, y)] || soldier_collision[Point(build_tile.x, y)])
				continue;
			else {
				bool canreach = 1;
				for (int i = 0; i != size; i++) {
					for (int j = 0; j != size; j++) {
						if (collision[Point(build_tile.x - i, y - j)] ||
							soldier_collision[Point(build_tile.x - i, y - j)]) {
							canreach = 0;
							continue;
						}
					}
				}
				if (canreach) {
					auto answer = tileCoordToRelatedCoord(Point(build_tile.x, y));
					setSoldierCollision(answer, size);
					return answer;
				}
			}
		}
		direction++;
	}
	if (direction == 4) {
		for (int y = build_tile.y; y != 0; y--) {
			if (collision[Point(build_tile.x, y)] || soldier_collision[Point(build_tile.x, y)])
				continue;
			else {
				bool canreach = 1;
				for (int i = 0; i != size; i++) {
					for (int j = 0; j != size; j++) {
						if (collision[Point(build_tile.x - i, y - j)] ||
							soldier_collision[Point(build_tile.x - i, y - j)]) {
							canreach = 0;
							continue;
						}
					}
				}
				if (canreach) {
					auto answer = tileCoordToRelatedCoord(Point(build_tile.x, y));
					setSoldierCollision(answer, size);
					return answer;
				}
			}
		}
		direction++;
	}
	if (direction == 5) {
		for (int x = build_tile.x, y = build_tile.y; x != 128 - size, y != 128 - size; x++, y++) {
			if (collision[Point(x, y)] || soldier_collision[Point(x, y)])
				continue;
			else {
				bool canreach = 1;
				for (int i = 0; i != size; i++) {
					for (int j = 0; j != size; j++) {
						if (collision[Point(x - i, y - j)] || soldier_collision[Point(x - i, y - j)]) {
							canreach = 0;
							continue;
						}
					}
				}
				if (canreach) {
					auto answer = tileCoordToRelatedCoord(Point(x, y));
					setSoldierCollision(answer, size);
					return answer;
				}
			}
		}
		direction++;
	}
	if (direction == 6) {
		for (int x = build_tile.x, y = build_tile.y; x != 128 - size, y != 0; x++, y--) {
			if (collision[Point(x, y)] || soldier_collision[Point(x, y)])
				continue;
			else {
				bool canreach = 1;
				for (int i = 0; i != size; i++) {
					for (int j = 0; j != size; j++) {
						if (collision[Point(x - i, y - j)] || soldier_collision[Point(x - i, y - j)]) {
							canreach = 0;
							continue;
						}
					}
				}
				if (canreach) {
					auto answer = tileCoordToRelatedCoord(Point(x, y));
					setSoldierCollision(answer, size);
					return answer;
				}
			}
		}
		direction++;
	}
	if (direction == 7) {
		for (int x = build_tile.x, y = build_tile.y; x != 0, y != 128 - size; x--, y++) {
			if (collision[Point(x, y)] || soldier_collision[Point(x, y)])
				continue;
			else {
				bool canreach = 1;
				for (int i = 0; i != size; i++) {
					for (int j = 0; j != size; j++) {
						if (collision[Point(x - i, y - j)] || soldier_collision[Point(x - i, y - j)]) {
							canreach = 0;
							continue;
						}
					}
				}
				if (canreach) {
					auto answer = tileCoordToRelatedCoord(Point(x, y));
					setSoldierCollision(answer, size);
					return answer;
				}
			}
		}
		direction++;
	}
	if (direction == 8) {
		for (int x = build_tile.x, y = build_tile.y; x != 0, y != 0; x--, y--) {
			if (collision[Point(x, y)] || soldier_collision[Point(x, y)])
				continue;
			else {
				bool canreach = 1;
				for (int i = 0; i != size; i++) {
					for (int j = 0; j != size; j++) {
						if (collision[Point(x - i, y - j)] || soldier_collision[Point(x - i, y - j)]) {
							canreach = 0;
							continue;
						}
					}
				}
				if (canreach) {
					auto answer = tileCoordToRelatedCoord(Point(x, y));
					setSoldierCollision(answer, size);
					return answer;
				}
			}
		}
	}
}