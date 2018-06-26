#ifndef _RA_PLAYER_H_
#define _RA_PLAYER_H_
#include"cocos2d.h"
#include"cocostudio\CocoStudio.h"
#include"ui\CocosGUI.h"
#include<unordered_set>

USING_NS_CC;
using namespace cocostudio;
using namespace cocos2d::ui;

class RASoldier;
class RAObject;

class RAPlayer
{
public:
	static const int getCapital() { return capital_; }
	static const int getPower() {return power_; }
	//give a unique counter in a game
	static const int getCounter() { return (unit_counter_++)*10+edge; }
	//player's capital will increase by quantity
	static void resumeCapital(const int quantity);
	//player's power will increase by quantity
	static void resumePower(const int quantity);
	//player's capital will decrease by quantity
	static void consumeCapital(const int quantity);
	//player's power will decrease by quantity
	static void consumePower(const int quantity);

	static Widget*& currentUI() { return current_UI_; }

	static std::vector<RASoldier*> selected_soldiers_;
	//all soldiers in my control, used to manipulate my soldiers
	static std::list<RASoldier*> all_soldiers_;
	//enemies' buildings and soldiers, used to caclculate range attack
	static std::list<RAObject*> enemies;
	//
	static std::unordered_map<int, RAObject*> master_table_;
	//
	static void setEdge(int _edge) { edge = _edge; }
private:
	static int capital_;
	static int power_;
	static int unit_counter_;
	static Widget* current_UI_;
	static int edge;
};

#endif // !_RA_PLAYER_H_
