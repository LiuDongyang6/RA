#ifndef _RA_BUILDING_
#define _RA_BUILDING_

#include"RAObject.h"

USING_NS_CC;
//
//RABuilding
//
class RABuilding : public RAObject
{
public:

	RABuilding(int id) :
		RAObject(id),
		power_cost_(RAUtility::RAgetProperty(id, "power").asInt()),
		capital_cost_(RAUtility::RAgetProperty(id, "capital").asInt())
	{}

	bool initWithIdAndLocation(int id,Point location);
	//when attacked or repaired;
	void changeAppearance();
	//delete the building and remove power cost
	virtual bool annihilation()override;

	//virtual bool sufferAttack(int damage)override;

	bool onTouchBegan(Touch* touch, Event* event);

	~RABuilding()override {}
	
	Widget* UI_;

protected:
	std::vector<std::string> appearances;
	//取值0，1或2，为appearances脚标
	int current_appearance_;
	const int power_cost_;
	const int capital_cost_;
};


//
//RAPowerStation
//
class RAPowerStation :public RABuilding
{
public:
	RAPowerStation() :
		RABuilding(id) {}

	static Sprite* create(Point location);
	static const int id = 1;
};

//
//RABase
//
class RABase :public RABuilding
{
public:
	RABase() :
		RABuilding(id) {}

	static Sprite* create(Point location);
	static const int id = 0;

};
//
//RABarrack
//
class RABarrack :public RABuilding
{
public:
	RABarrack() :
		RABuilding(id) {}

	static Sprite* create(Point location);
	static const int id = 2;

};

#endif

