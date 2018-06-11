#ifndef __LOADING_SCENE_H__
#define __LOADING_SCENE_H__

#include "cocos2d.h"

class Loading : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void menuLastpageCallback(cocos2d::Ref* pSender);

	CREATE_FUNC(Loading);
};

#endif // __LOADING_SCENE_H__
