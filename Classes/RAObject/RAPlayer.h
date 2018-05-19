#ifndef _RA_PLAYER_H_
#define _RA_PLAYER_H_
#include"cocos2d.h"
#include"cocostudio\CocoStudio.h"
#include"ui\CocosGUI.h"

USING_NS_CC;
using namespace cocostudio;
using namespace cocos2d::ui;

class RAPlayer
{
public:
	static int& capital() { return capital_; }
	static int& power() { return power_; }
	static Widget*& currentUI() { return current_UI_; }
private:
	static int capital_;
	static int power_;
	static Widget* current_UI_;
	//static Vector<RASoldier> selected_soldiers_;
};

#endif // !_RA_PLAYER_H_
