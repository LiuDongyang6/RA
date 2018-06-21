#include "RAlittle_map.h"
#include "tiled_map.h"
#include"RAUtility.h"

using namespace cocos2d;

Sprite* littleMap::little_map;

bool littleMap::init(int num)
{
	chooseMap(num);
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [&](Touch* touch, Event* event)
	{
		if (RAUtility::containsTouchLocation(touch, event))
		{
			auto pos = little_map->convertToNodeSpace(touch->getLocation());
			auto tiled_map = RAMap::getMap();
			auto prop_x = tiled_map->getContentSize().width / little_map->getContentSize().width;
			auto prop_y = tiled_map->getContentSize().height / little_map->getContentSize().height;
			auto visibleSize = Director::getInstance()->getVisibleSize();
			tiled_map->setPosition(Point(-prop_x * pos.x + visibleSize.width / 2, 
				-prop_y * pos.y + visibleSize.height / 2));
			return true;
		}
		return false;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, little_map);
	return true;
}

Sprite* littleMap::getLittleMap()
{
	return little_map;
}

void littleMap::chooseMap(int num) {
	if (num == 1)
	{
		auto visibleSize = Director::getInstance()->getVisibleSize();
		Vec2 origin = Director::getInstance()->getVisibleOrigin();
		little_map = Sprite::create("littlemap.png");
		little_map->setAnchorPoint(Point(0, 0));
		auto tiled_map = RAMap::getMap();
		little_map->setScaleX(tiled_map->getContentSize().width / 16000);
		little_map->setScaleY(tiled_map->getContentSize().height / 9000);
		little_map->setPosition(50, visibleSize.height - little_map->getContentSize().height / 5 - 80);
	}
}