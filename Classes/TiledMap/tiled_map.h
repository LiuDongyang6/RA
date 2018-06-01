#ifndef __RAMAP__H__
#define __RAMAP__H__

#include "cocos2d.h"
class RASoldier;


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

	static std::map<cocos2d::Point, int> tryEightdirection(cocos2d::Point position,
		cocos2d::Point dest);

	static std::vector<float> findRoutine(RASoldier* soldier, cocos2d::Point dest);

	static void destroyNormalBuildings(cocos2d::Point pos, int size);

	static void destroyOilBuildings(cocos2d::Point pos, int size);

	static void setSoldierCollision(cocos2d::Point pos);

	static void removeSoldierCollision(cocos2d::Point pos);
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

