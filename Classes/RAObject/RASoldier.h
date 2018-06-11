#ifndef __RASOLDIER_H__
#define __RASOLDIER_H__
#include"RAObject\RAObject.h"
USING_NS_CC;

class RASoldier :public RAObject
{
public:
	RASoldier(int id) :
		RAObject(id),
		range_(RAUtility::RAgetProperty(id, "range").asInt()),
		speed_(RAUtility::RAgetProperty(id, "speed").asInt()),
		hit_(RAUtility::RAgetProperty(id, "hit").asInt()),
		attack_speed_(RAUtility::RAgetProperty(id, "attack_speed").asFloat()),
	    animation_(3,Vector<SpriteFrame*>(0)),
		power_cost_(RAUtility::RAgetProperty(id, "power").asInt()),
		capital_cost_(RAUtility::RAgetProperty(id, "capital").asInt())
	    {}
	//
	bool initWithIdAndLocation(int id,Point location);
	//delete the building and remove power cost
	virtual bool annihilation()override;
	//
	void findRoadAndLetGo();
	//
	void findRoadAndLetGoForFight();
	//called by my soldier
	bool onTouchBegan(Touch* touch, Event* event) ;
	//
	void doAttack();
	//
	void sufferAttack(float attack_speed, int damage, RASoldier* attacker) override;
	//
	void stopCurrentBehavior(Ref* pSender=0);
	//
	void runTo(Point point);
	//
	void runToFight(RAObject* object);
	~RASoldier()override {	}
private:
	std::vector<Vector<SpriteFrame*>> animation_;
	const int range_;
	const int speed_;
	const int hit_;
	const float attack_speed_;
	const int power_cost_;
	const int capital_cost_;
	Point destination;
	Point next_step=Point(-1,-1);
	RAObject* AimEnemy=NULL;
	//get a repeat forever action
	Action* getAction(int number, float dt);
};

class RAFairy :public RASoldier
{
public:
	RAFairy() :
		RASoldier(id) {}

	static RAObject* create(Point location);
	static const int id = 3;
};
class RAAssassin :public RASoldier
{
public:
	RAAssassin() :
		RASoldier(id) {}

	static RAObject* create(Point location);
	static const int id = 4;
};
class RALancer :public RASoldier
{
public:
	RALancer() :
		RASoldier(id) {}

	static RAObject* create(Point location);
	static const int id = 5;
};
class RAGeneral :public RASoldier
{
public:
	RAGeneral() :
		RASoldier(id) {}

	static RAObject* create(Point location);
	static const int id = 6;
};
#endif // !__RASOLDIER_H__

