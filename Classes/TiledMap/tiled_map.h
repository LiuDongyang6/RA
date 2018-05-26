#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class TiledMap : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

	// OpenGL����ת��ΪTiledmap����
	cocos2d::Point glCoordToTileCoord(cocos2d::Point gl_cood);

	//�ж��Ƿ���Խ�����ͨ����
	bool cannotBuildNormal(cocos2d::Point build_point, int size);

	//�����콨������Ƭ��Ϊ����ͨ��
	void sureToBuild(cocos2d::Point build_point, int size);

    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

	//�ƶ���ͼ
	void moveMap(void);
    
	//��������
	void testForCoord(void);

    // implement the "static create()" method manually
    CREATE_FUNC(TiledMap);

	//��ͼ�ƶ��ٶ�
	const float speed = 30;

	const int accurancy = 200;

private:
	cocos2d::TMXTiledMap *_tiledMap;
	cocos2d::TMXLayer *_collision;
	cocos2d::Point diff;
};

#endif // __HELLOWORLD_SCENE_H__
