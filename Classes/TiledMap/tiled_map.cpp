#include "tiled_map.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* TiledMap::createScene()
{
    return TiledMap::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool TiledMap::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//创建地图
	_tiledMap = TMXTiledMap::create("map1.tmx");
	this->addChild(_tiledMap, -10, 1);
	testForCoord();
	//创建拖动监听
	auto listener_map_move = EventListenerMouse::create();
	////绑定触摸&计算拖动的偏移量
	listener_map_move->onMouseMove = [=](Event* event) {
		EventMouse* cursor = static_cast<EventMouse*>(event);
		float cursor_x = cursor->getCursorX();
		float cursor_y = cursor->getCursorY();
		auto node = getChildByTag(1);
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
		//移动地图
		moveMap();
	};
	//拖动方法移动地图 problem 会显得有点卡
	//listener_map_move->onMouseMove = [=](Event* event) {
	//	EventMouse* click = static_cast<EventMouse*>(event);
	//	if (click->getMouseButton() == EventMouse::MouseButton::BUTTON_MIDDLE) {
	//		float click_x = click->getCursorX();
	//		float click_y = click->getCursorY();
	//		pre_pos = Point(click_x, click_y);
	//		auto now_pos = click->getDelta();
	//		auto diff = now_pos - pre_pos;
	//		log("diff: %f, %f", diff.x, diff.y);
	//		log("now: %f, %f", now_pos.x, now_pos.y);
	//		auto node = getChildByTag(1);
	//		auto currentPos = node->getPosition();
	//		Point mapPos = _tiledMap->getPosition();
	//		Point viewPos = mapPos + diff;
	//		Size winSize = Director::getInstance()->getWinSize();
	//		Size mapSize = _tiledMap->getMapSize();
	//		Size tileSize = _tiledMap->getTileSize();
	//		if (viewPos.x < winSize.width - 105 * tileSize.width || viewPos.x > 0)
	//		{
	//			diff.x = 0;
	//		}
	//		if (viewPos.y < winSize.height - 106 * tileSize.height || viewPos.y >0)
	//		{
	//			diff.y = 0;
	//		}
	//		node->setPosition(currentPos + diff);
	//	}
	//};
	/*listener_map_move->onMouseUp = [&](Event* event) {
		EventMouse* loosen = static_cast<EventMouse*>(event);
		if (loosen->getMouseButton() == EventMouse::MouseButton::BUTTON_MIDDLE) {
			float loosen_x = loosen->getCursorX();
			float loosen_y = loosen->getCursorY();
			auto diff = Point(loosen_x, loosen_y) - pre_pos;
			log("diff: %f, %f", diff.x, diff.y);
			auto node = getChildByTag(1);
			auto currentPos = node->getPosition();
			Point mapPos = _tiledMap->getPosition();
			Point viewPos = mapPos + diff;
			Size winSize = Director::getInstance()->getWinSize();
			Size mapSize = _tiledMap->getMapSize();
			Size tileSize = _tiledMap->getTileSize();
			if (viewPos.x < winSize.width - 105 * tileSize.width || viewPos.x > 0)
			{
				diff.x = 0;
			}

			if (viewPos.y < winSize.height - 106 * tileSize.height || viewPos.y >0)
			{
				diff.y = 0;
			}
			node->setPosition(currentPos + diff);
		}
	};*/
	//对地图添加监听
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener_map_move, this);
    return true;
}
void TiledMap::testForCoord() {
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [&](Touch* touch, Event* event) {
		Point pos1 = touch->getLocation();
		bool tf = cannotBuildNormal(pos1, 3);
		log("%d", tf);
		return true;
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

Point TiledMap::glCoordToTileCoord(Point gl_coord) {
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

void TiledMap::moveMap(void) {
	auto node = getChildByTag(1);
	auto currentPos = node->getPosition();
	Point mapPos = _tiledMap->getPosition();
	Point viewPos = mapPos + speed * diff;
	Size winSize = Director::getInstance()->getWinSize();
	Size mapSize = _tiledMap->getMapSize();
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

bool TiledMap::cannotBuildNormal(cocos2d::Point build_point/*GL 坐标*/, int size) {
	//从GL坐标转化为瓦片坐标  
	Point tile_coord = this->glCoordToTileCoord(build_point);
	if (tile_coord.x <= 0)
		return false;
	_collision = _tiledMap->getLayer("collision");
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			//获得瓦片的GID  
			int tileGid = _collision->getTileGIDAt(tile_coord);
			log("x:%d y:%d pos_x:%f pos_y:%f gid:%d", x, y, tile_coord.x, tile_coord.y, tileGid);
			if (tileGid) {
				Value prop = _tiledMap->getPropertiesForGID(tileGid);
				ValueMap propValueMap = prop.asValueMap();
				bool is_collision = propValueMap["collision"].asBool();
				if (is_collision)
					return false;
			}
			tile_coord.y--;
			if (tile_coord.y < 0)
				return false;
		}
		tile_coord.y += size;
		tile_coord.x--;
		if (tile_coord.x < 0)
			return false;
	}
	return true;
}

void TiledMap::sureToBuild(cocos2d::Point build_point, int size) {
	auto tile_size = _tiledMap->getTileSize();
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			build_point.x += x * tile_size.width;
			build_point.y += y * tile_size.height;
			//从GL坐标转化为瓦片坐标  
			Point tileCoord = this->glCoordToTileCoord(build_point);

		}
	}
}

void TiledMap::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
