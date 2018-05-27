#ifndef _RA_PLAYER_H_
#define _RA_PLAYER_H_
#include"cocos2d.h"
#include"cocostudio\CocoStudio.h"
#include"ui\CocosGUI.h"

USING_NS_CC;
using namespace cocostudio;
using namespace cocos2d::ui;

class RASoldier;

class RAPlayer
{
public:
	static const int getCapital() { return capital_; }
	static const int getPower() {return power_; }

	//player's capital will increase by quantity
	static void resumeCapital(const int quantity);
	//player's power will increase by quantity
	static void resumePower(const int quantity);
	//player's capital will decrease by quantity
	static void consumeCapital(const int quantity);
	//player's power will decrease by quantity
	static void consumePower(const int quantity);

	static Widget*& currentUI() { return current_UI_; }

	static Vector<RASoldier*> selected_soldiers_;
private:
	static int capital_;
	static int power_;
	static Widget* current_UI_;
};

#endif // !_RA_PLAYER_H_
