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

	void sufferAttack(float attack_speed, int damage, RASoldier* attacker) override;

	bool onTouchBegan(Touch* touch, Event* event);

	~RABuilding()override {}
	//used when controler changes
	int getPowerCost() { return power_cost_; }

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

	static RAObject* create(Point location);
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

	static RAObject* create(Point location);
	static const int id = 0;

};
//
//RADefendingBase
//
class RADefendingBase :public RABuilding
{
public:
	RADefendingBase() :
		RABuilding(id) {}

	static RAObject* create(Point location);
	static const int id = 15;

};
//
//RABarrack
//
class RABarrack :public RABuilding
{
public:
	RABarrack() :
		RABuilding(id) {}

	static RAObject* create(Point location);
	static const int id = 2;

};
//
//RAOilField
//
class RAOilField :public RABuilding
{
public:
	RAOilField() :
		RABuilding(id),
		income_value_(RAUtility::RAgetProperty(id,"income").asInt())
	{}
	void changeControl(bool mine) override;
	void initCapitalIncome();
	void getIncome(float dt);
	static RAObject* create(Point location);
	bool annihilation() override;
	static const int id = 14;
protected:
	int income_value_;
};
//
//RANuclearSilo
//
class RANuclearSilo :public RABuilding
{
public:
	RANuclearSilo() :
		RABuilding(id) {}

	static RAObject* create(Point location);
	static const int id = 16;

};
//
//RAManhattan
//
class RAManhattan :public RABuilding
{
public:
	RAManhattan() :
		RABuilding(id) {}

	static RAObject* create(Point location);
	static const int id = 17;

};
#endif

