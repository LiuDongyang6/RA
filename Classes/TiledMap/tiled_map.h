#ifndef __RAMAP__H__
#define __RAMAP__H__

#include "cocos2d.h"

class RAMap
{
public:
    static bool init();

	// OpenGL坐标转换为Tiledmap坐标
	static cocos2d::Point glCoordToTileCoord(cocos2d::Point gl_cood);

	//判断是否可以建造普通建筑
	static bool cannotBuildNormal(cocos2d::Point build_point, int size);

	//将建造建筑的瓦片设为不可通过
	static void sureToBuild(cocos2d::Point build_point, int size);

	//移动地图
	static void moveMap(void);
    
	//测试坐标
	static void testForCoord(void);

	//地图移动速度
	static const float speed;

	static const int accurancy;

	static cocos2d::TMXTiledMap* &getMap() { return _tiledMap; };

private:
	static cocos2d::TMXTiledMap *_tiledMap;
	static cocos2d::TMXLayer *_collision;
	static cocos2d::Point diff;
};

#endif // __ABMYXJJ__
