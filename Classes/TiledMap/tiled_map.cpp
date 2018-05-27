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
std::map<cocos2d::Point, bool> RAMap::collision;
std::map<cocos2d::Point, bool> RAMap::oil;

// on "init" you need to initialize your instance
bool RAMap::init()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//������ͼ
	_tiledMap = TMXTiledMap::create("map1.tmx");
	mapInit();
	testForCoord();
	setMovePosition();
	_tiledMap->schedule(schedule_selector(RAMap::moveMap));
    return true;
}

//��ȡ���е�ͼ��Ϣ
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

//��������
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

// OpenGL����ת��ΪTiledmap����
Point RAMap::glCoordToTileCoord(Point gl_coord) {
	//��mapԭ������  
	int map_width = _tiledMap->getMapSize().width;
	int map_height = _tiledMap->getMapSize().height;
	float map_zero_x = _tiledMap->getPosition().x + _tiledMap->getContentSize().width / 2;
	float map_zero_y = _tiledMap->getPosition().y + _tiledMap->getContentSize().height;
	int tile_width = (map_zero_x - _tiledMap->getPosition().x) / map_width * 2;
	int tile_height = (map_zero_y - _tiledMap->getPosition().y) / map_height;
	//OΪ��ͼ��ԭ�㣬A��Ҫ��ĵ㣬oa������ֵ  
	float OA_x = gl_coord.x - map_zero_x;
	float OA_y = gl_coord.y - map_zero_y;
	//�������
	int max_coord = _tiledMap->getContentSize().height / tile_height;
	//��GL����ϵ��x,y��ĵ�λ�����ֽ⵽��Ƭ����ϵ
	//����i=(tile_width/2, -tile_height/2)
	//����j=(-tile_width/2, -tile_height/2)
	//����OA=mi+nj
	float m = OA_x / tile_width - OA_y / tile_height;
	float n = -(OA_x / tile_width + OA_y / tile_height);
	//Խ����
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

//���õ�ͼ�ƶ�����
void RAMap::setMovePosition(void) {
	//�����϶�����
	auto listener_map_move = EventListenerMouse::create();
	////�󶨴���&�����϶���ƫ����
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
	//�Ե�ͼ��Ӽ���
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener_map_move, _tiledMap);
}

//�ƶ���ͼ
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

//�ж��Ƿ���Խ�����ͨ����
bool RAMap::cannotBuildNormal(cocos2d::Point build_point/*GL ����*/, int size) {
	//��GL����ת��Ϊ��Ƭ����  
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

//�ж��Ƿ���Խ����;�
Point RAMap::cannotBuildOil(cocos2d::Point build_point, int size) {
	//��GL����ת��Ϊ��Ƭ����  
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

//ȷ��������ͨ����
void RAMap::sureToBuildNormal(cocos2d::Point build_point, int size) {
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

//ȷ�������;�
void RAMap::sureToBuildOil(cocos2d::Point build_point, int size) {
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