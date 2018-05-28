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
		RAObject(id){}

	bool initWithId(int id);
	//when attacked or repaired;
	void changeAppearance();
	//delete the building and remove power cost
	virtual bool annihilation()override;

	virtual bool sufferAttack(int damage)override;

	bool onTouchBegan(Touch* touch, Event* event);

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
	RAPowerStation() :
		RABuilding(id) {}

	static Sprite* create();
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

	static Sprite* create();
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

	static Sprite* create();
	static const int id = 2;

};

#endif

