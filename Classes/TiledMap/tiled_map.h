#ifndef __RAMAP__H__
#define __RAMAP__H__

#include "cocos2d.h"
class RASoldier;


class RAMap:public cocos2d::Node
{
public:
    static bool init();

	static cocos2d::Point glCoordToTileCoord(cocos2d::Point gl_cood);
	
	static cocos2d::Point tileCoordToRelatedCoord(cocos2d::Point tile_coord);

	static cocos2d::Point relatedCoordToTileCoord(cocos2d::Point related_coord);

	static bool cannotBuildNormal(cocos2d::Point build_point, int size);

	static cocos2d::Point cannotBuildOil(cocos2d::Point build_point, int size);

	void moveMap(float dt);

	static void testForCoord(void);

	static void setMovePosition(void);

	static void mapInit(void);

	static void sureToBuildNormal(cocos2d::Point pos, int size);

	static void sureToBuildOil(cocos2d::Point pos, int size);

	static std::map<cocos2d::Point, int> tryEightdirection(cocos2d::Point position,
		cocos2d::Point dest, const int size);

	static std::vector<cocos2d::Point> findRoutineAllAtOnce(RASoldier* soldier, cocos2d::Point &dest, const int size);

	static std::vector<float> findRoutineOneByOne(RASoldier* soldier, cocos2d::Point &dest, const int size);

	static int aStar(cocos2d::Point so_pos, cocos2d::Point dest, const int size);

	static void destroyNormalBuildings(cocos2d::Point pos, int size);

	static void destroyOilBuildings(cocos2d::Point pos, int size);

	static void setSoldierCollision(cocos2d::Point pos, const int size);

	static void removeSoldierCollision(cocos2d::Point pos, const int size);

	static cocos2d::Point setCenter(cocos2d::Point pos);

	static cocos2d::Point soldierBirth(cocos2d::Point build_pos, const int);

	static void changeOilTile(cocos2d::Point pos);

	static void recoverOilTile(cocos2d::Point pos);

	//地图移动速度
	static const float speed;

	static const int accurancy;

	static cocos2d::TMXTiledMap* &getMap() { return _tiledMap; };

protected:
	static cocos2d::TMXTiledMap *_tiledMap;
	static cocos2d::Point diff;
	static std::map<cocos2d::Point, bool> collision;
	static std::map<cocos2d::Point, bool> oil;
	static std::map<cocos2d::Point, bool> soldier_collision;
	static std::vector<cocos2d::Point> routines;
};

#endif // __AB__

