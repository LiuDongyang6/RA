#ifndef __START_SCENE_H__
#define __START_SCENE_H__

#include "cocos2d.h"

class Start : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	// callback functions that jump to other scenes 
	void menuSingleplayerCallback(cocos2d::Ref* pSender);
	void menuMultiplayerCallback(cocos2d::Ref* pSender);
	void menuLastpageCallback(cocos2d::Ref* pSender);

	CREATE_FUNC(Start);
};

#endif // __START_SCENE_H__
