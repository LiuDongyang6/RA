#ifndef __ROOM_SCENE_H__
#define __ROOM_SCENE_H__

#include "cocos2d.h"

#include "cocostudio\CocoStudio.h"
#include "ui\CocosGUI.h"

class Room : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void menuLastpageCallback(cocos2d::Ref* pSender);
	void startCallback(Ref* pSender);
	void menuOkCallback(cocos2d::Ref* pSender);


	// implement the "static create()" method manually
	CREATE_FUNC(Room);
};

#endif // __ROOM_SCENE_H__
