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
	virtual void doAttack();
	//
	void sufferAttack(float attack_speed, int damage, RASoldier* attacker) override;
	//
	void stopCurrentBehavior(Ref* pSender=0);
	//
	void runTo(Point point);
	//
	void runToFight(RAObject* object);
	~RASoldier()override {	}
protected:
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
	//soldier是否主动死亡（意味着可能播放不同的动画)
	bool active_die_ = 0;
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
class RAAtomicBomb :public RASoldier
{
public:
	RAAtomicBomb() :
		RASoldier(id) {}
	void doAttack() override;
	static RAObject* create(Point location);
	static const int id = 7;
};
class RABlackMagician :public RASoldier
{
public:
	RABlackMagician() :
		RASoldier(id) {}

	static RAObject* create(Point location);
	static const int id = 8;
};
class RABomber :public RASoldier
{
public:
	RABomber() :
		RASoldier(id) {}

	static RAObject* create(Point location);
	static const int id = 9;
};
class RAEngineer :public RASoldier
{
public:
	RAEngineer() :
		RASoldier(id) {}
	void doAttack() override;
	static RAObject* create(Point location);
	static const int id = 10;
	void runToBuildOilField(Point pos);
	void findRoadAndLetGoForOilField();
private:
	Point oil_position_;
};
class RAWinterSoldier :public RASoldier
{
public:
	RAWinterSoldier() :
		RASoldier(id) {}

	static RAObject* create(Point location);
	static const int id = 11;
};
class RAWizzard :public RASoldier
{
public:
	RAWizzard() :
		RASoldier(id) {}

	static RAObject* create(Point location);
	static const int id = 13;
};
#endif // !__RASOLDIER_H__

