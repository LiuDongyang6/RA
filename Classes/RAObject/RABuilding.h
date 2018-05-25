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

	RABuilding(int hp, int power_cost,int capital_cost) :
		RAObject(hp, power_cost, capital_cost){}

	bool initWithFile(const std::string original,
		const std::string damaged,
		const std::string dying,
		const char* UIName
	);
	//when attacked or repaired;
	void changeAppearance();
	//delete the building and remove power cost
	virtual bool annihilation()override;

	virtual bool sufferAttack(int damage)override;

	bool onTouchBegan(Touch* touch, Event* event);
	//kind() returns 0 when building
	static int kind() { return 0; };

	~RABuilding()override { 
		UI_->release();
	}
	
	Widget* UI_;

protected:
	std::vector<std::string> appearances;
	int current_appearance_;
};


//
//RAPowerStation
//
class RAPowerStation :public RABuilding
{
public:
	RAPowerStation(int hp = original_hp_, int power_cost = power_cost_,int capital_cost=capital_cost_) :
		RABuilding(hp, power_cost,capital_cost) {}

	static RAPowerStation* create();
	static const int original_hp_ = 500;
	static const int power_cost_ = -100;
	static const int capital_cost_ = 100;
};

//
//RABase
//
class RABase :public RABuilding
{
public:
	RABase(int hp = original_hp_, int power_cost = power_cost_,int capital_cost=capital_cost_) :
		RABuilding(hp, power_cost,capital_cost) {}

	static RABase* create();
	static const int original_hp_ = 500;
	static const int power_cost_ = 100;
	static const int capital_cost_ = 100;

private:
	RAConstructButton<RAPowerStation>* power_station_button_;
	RAConstructButton<RABase>* base_button_;

};

#endif

