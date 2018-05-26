#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class TiledMap : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

	// OpenGL坐标转换为Tiledmap坐标
	cocos2d::Point glCoordToTileCoord(cocos2d::Point gl_cood);

	//判断是否可以建造普通建筑
	bool cannotBuildNormal(cocos2d::Point build_point, int size);

	//将建造建筑的瓦片设为不可通过
	void sureToBuild(cocos2d::Point build_point, int size);

    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

	//移动地图
	void moveMap(void);
    
	//测试坐标
	void testForCoord(void);

    // implement the "static create()" method manually
    CREATE_FUNC(TiledMap);

	//地图移动速度
	const float speed = 30;

	const int accurancy = 200;

private:
	cocos2d::TMXTiledMap *_tiledMap;
	cocos2d::TMXLayer *_collision;
	cocos2d::Point diff;
};

#endif // __HELLOWORLD_SCENE_H__
