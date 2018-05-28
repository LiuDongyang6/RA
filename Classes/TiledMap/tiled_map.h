#ifndef __RAMAP__H__
#define __RAMAP__H__

#include "cocos2d.h"

class RAMap
{
public:
    static bool init();

	// OpenGL����ת��ΪTiledmap����
	static cocos2d::Point glCoordToTileCoord(cocos2d::Point gl_cood);

	//�ж��Ƿ���Խ�����ͨ����
	static bool cannotBuildNormal(cocos2d::Point build_point, int size);

	//�����콨������Ƭ��Ϊ����ͨ��
	static void sureToBuild(cocos2d::Point build_point, int size);

	//�ƶ���ͼ
	static void moveMap(void);
    
	//��������
	static void testForCoord(void);

	//��ͼ�ƶ��ٶ�
	static const float speed;

	static const int accurancy;

	static cocos2d::TMXTiledMap* &getMap() { return _tiledMap; };

private:
	static cocos2d::TMXTiledMap *_tiledMap;
	static cocos2d::TMXLayer *_collision;
	static cocos2d::Point diff;
};

#endif // __ABMYXJJ__
