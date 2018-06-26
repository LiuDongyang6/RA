#ifndef __RA_ROSHAMBO_H__
#define __RA_ROSHAMBO_H__
#include"cocos2d.h"

USING_NS_CC;

class RARoshambo : public Sprite
{
public:
	RARoshambo(int num) :num_(num), Sprite() {}
	bool onTouchBegan(Touch* touch, Event* event);
	static RARoshambo* create(int num);
	static void showCard(bool isMe,int num_);
	static void startCompete();
	static RARoshambo* cards[3];
	int num_;
	static int contrast[2];
	static void RoshamboReceive(std::string msg);
};

#endif // !__RA_ROSHAMBO_H__

