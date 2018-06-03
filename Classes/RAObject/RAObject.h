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

class RAObject : public cocos2d::Sprite
{
public:
	RAObject(int id) :
		original_hp_(RAUtility::RAgetProperty(id, "original_hp").asInt()),
		hp_(RAUtility::RAgetProperty(id, "original_hp").asInt()),
		power_cost_(RAUtility::RAgetProperty(id, "power").asInt()),
		capital_cost_(RAUtility::RAgetProperty(id, "capital").asInt()){}
	~RAObject()override {}
	virtual bool initWithSpriteFrameName(const std::string& filename)override;
	virtual bool sufferAttack(int damage);//return survive or not
	virtual bool annihilation();
protected:
	const int power_cost_;
	const int capital_cost_;
	int hp_;
	const int original_hp_;
	bool toBeOrNotToBe();
};


class RAConstructButton: public Node
{
public:
	RAConstructButton(Widget* UI, int IdToConstruct) :
		button_((Button*)Helper::seekWidgetByTag(UI, IdToConstruct)),
		id(IdToConstruct),
		power_cost_(RAUtility::RAgetProperty(id, "power").asInt()),
		capital_cost_(RAUtility::RAgetProperty(id, "capital").asInt()),
		category_((RAUtility::RAgetProperty(IdToConstruct, "category").asInt()))
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
	typedef Sprite*(*CreateType)();
	static std::unordered_map<int, CreateType> CreateWiki;
	~RAConstructButton()
	{ NotificationCenter::getInstance()->removeAllObservers(this); }
private:
	Button * button_;
	//id of object the button constructs
	const int id;
	//
	int capital_cost_;
	//
	int power_cost_;
	//
	int category_;
	//
	Sprite* tempObject;
};

#endif // __RA_SOLDIER__
