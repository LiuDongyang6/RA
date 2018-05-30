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
		attack_speed_(RAUtility::RAgetProperty(id, "attack_speed").asInt()),
	    animation_(3,Vector<SpriteFrame*>(0)){}

	bool initWithId(int id);

	//delete the building and remove power cost
	virtual bool annihilation()override;

	//virtual bool sufferAttack(int damage)override;

	bool doMove(Point);


	bool onTouchBegan(Touch* touch, Event* event);

	bool doAttack();

	~RASoldier()override {
		for (auto vec : animation_)
			vec.clear();
		animation_.clear();
	}

	void runTo(Point point);

	Widget* UI_;

private:
	std::vector<Vector<SpriteFrame*>> animation_;
	const int range_;
	const int speed_;
	const int hit_;
	const int attack_speed_;
};

class RAFairy :public RASoldier
{
public:
	RAFairy() :
		RASoldier(id) {}

	static Sprite* create();
	static const int id = 3;
	static bool lauched;
};
class RAAssassin :public RASoldier
{
public:
	RAAssassin() :
		RASoldier(id) {}

	static Sprite* create();
	static const int id = 4;
	static bool lauched;
};
class RALancer :public RASoldier
{
public:
	RALancer() :
		RASoldier(id) {}

	static Sprite* create();
	static const int id = 5;
	static bool lauched;
};
class RAGeneral :public RASoldier
{
public:
	RAGeneral() :
		RASoldier(id) {}

	static Sprite* create();
	static const int id = 6;
	static bool lauched;
};
#endif // !__RASOLDIER_H__

