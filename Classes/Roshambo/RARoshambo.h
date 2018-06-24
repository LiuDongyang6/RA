#ifndef __RA_ROSHAMBO_H__
#define __RA_ROSHAMBO_H__
#include"cocos2d.h"

USING_NS_CC;

class RARoshambo : public Sprite
{
	RARoshambo(int num) :num_(num), Sprite() {}
	bool onTouchBegan(Touch* touch, Event* event);
	static RARoshambo* create(int num);
private:
	int num_;
};

#endif // !__RA_ROSHAMBO_H__

