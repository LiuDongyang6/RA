#ifndef __RAMAP__H__
#define __RAMAP__H__

#include "cocos2d.h"


class RAMap:public cocos2d::Node
{
public:
    static bool init();

	static cocos2d::Point glCoordToTileCoord(cocos2d::Point gl_cood);

	static bool cannotBuildNormal(cocos2d::Point build_point, int size);

	static cocos2d::Point cannotBuildOil(cocos2d::Point build_point, int size);

	void moveMap(float dt);

	static void testForCoord(void);

	static void setMovePosition(void);

	static void mapInit(void);

	static void sureToBuildNormal(cocos2d::Point pos, int size);

	static void sureToBuildOil(cocos2d::Point pos, int size);


	//地图移动速度
	static const float speed;

	static const int accurancy;

	static cocos2d::TMXTiledMap* &getMap() { return _tiledMap; };

protected:
	static cocos2d::TMXTiledMap *_tiledMap;
	static cocos2d::Point diff;
	static std::map<cocos2d::Point, bool> collision;
	static std::map<cocos2d::Point, bool> oil;
};

#endif // __AB__

