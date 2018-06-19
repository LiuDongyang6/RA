#ifndef __RA_OBJECT__
#define __RA_OBJECT__

#include"RAPlayer.h"
#include"cocostudio\CocoStudio.h"
#include"ui\CocosGUI.h"
#include"RAUtility.h"
#include"TiledMap\tiled_map.h"

USING_NS_CC;
using namespace cocostudio;
using namespace cocos2d::ui;

class RASoldier;

class RAObject : public cocos2d::Sprite
{
public:
	RAObject(int id) :
		original_hp_(RAUtility::RAgetProperty(id, "original_hp").asInt()),
		hp_(RAUtility::RAgetProperty(id, "original_hp").asInt()),
		covering_(RAUtility::RAgetProperty(id, "covering").asInt()),
		category_(RAUtility::RAgetProperty(id, "category").asInt()),
		object_count_(RAPlayer::getCounter()),
		id_(id)
	{}
	~RAObject()override {}
	bool initWithSpriteFrameNameAndLocation(const std::string& filename,Point location);
	virtual void sufferAttack(float attack_speed,int damage,RASoldier* attacker)=0;
	void stopSufferAttack(RASoldier* attacker);
	virtual bool annihilation();
	void changeControl(bool mine);
	int getCount() { return object_count_; }
	int getId() { return id_; };
	bool isBuilding();
	bool under_my_control=true;
protected:
	const int covering_;
	int hp_;
	const int original_hp_;
	bool toBeOrNotToBe();
	const int category_;
	std::unordered_set<RASoldier*> attacking_me_;
	const int object_count_;
	Sprite* hp_bar;
	const int id_;
};


class RAConstructButton: public Node
{
public:
	RAConstructButton(Widget* UI, int IdToConstruct) :
		button_((Button*)Helper::seekWidgetByTag(UI, IdToConstruct)),
		id(IdToConstruct),
		power_cost_(RAUtility::RAgetProperty(id, "power").asInt()),
		capital_cost_(RAUtility::RAgetProperty(id, "capital").asInt()),
		category_((RAUtility::RAgetProperty(IdToConstruct, "category").asInt())),
		covering_((RAUtility::RAgetProperty(IdToConstruct, "covering").asInt()))
	{}
	static RAConstructButton* create(Node* parent, Widget* UI, int IdToConstruct);
	void onTouchMoved(Touch* touch, Event* type);
	void onTouchEnded(Touch* touch, Event* type);
	bool onTouchBegan(Touch* touch, Event* event);
	void initButton();
	//judge if capital and power can afford the object
	void checkConstructable(Ref* pSender);
	//a map containing all creat functions
	//initialized in RAInitialAll
	typedef RAObject*(*CreateType)(Point);
	static std::unordered_map<int, CreateType> CreateWiki;
	~RAConstructButton()
	{ NotificationCenter::getInstance()->removeAllObservers(this); }
	static std::unordered_set<int>LaunchRecord;
	static bool LaunchTest(int id);
private:
	Button * button_;
	//id of object the button constructs
	const int id;
	//
	const int capital_cost_;
	//
	const int power_cost_;
	//
	const int category_;
	//
	const int covering_;
	//
	Sprite* tempObject;
};

#endif // __RA_SOLDIER__ 