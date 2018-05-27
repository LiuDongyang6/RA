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
		hp_(original_hp_),
		power_cost_(RAUtility::RAgetProperty(id, "power").asInt()),
		capital_cost_(RAUtility::RAgetProperty(id, "capital").asInt()){}
	~RAObject()override {}
	virtual bool initWithFile(const std::string& filename)override;
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
		capital_cost_(RAUtility::RAgetProperty(id, "capital").asInt()){}
	static RAConstructButton* create(Node* parent,Widget* UI, int IdToConstruct)
	{
		RAConstructButton* button = new RAConstructButton(UI, IdToConstruct);
		button->initButton();
		button->autorelease();
		parent->addChild(button);
		return button;
	}
	void onTouchEnded(Touch* touch, Event* type)
	{
		Vec2 origin = RAMap::getMap()->getPosition();
		button_->onTouchEnded(touch, type);
		Sprite* object = CreateWiki[id]();
		int category = (RAUtility::RAgetProperty(id,"category").asInt());
		if (category == 0)
		{
			auto point = touch->getLocation();
			object->setPosition(point-origin);
		}
		else
			object->setPosition(-1*origin);
		RAMap::getMap()->addChild(object,category);
	}
	bool onTouchBegan(Touch* touch, Event* event)
	{
		if (!button_->isBright())return false;
		return (button_->onTouchBegan(touch, event));
	}
	void initButton()
	{
		button_->setTouchEnabled(0);
		auto listener = EventListenerTouchOneByOne::create();
		listener->onTouchBegan = CC_CALLBACK_2(RAConstructButton::onTouchBegan, this);
		listener->onTouchEnded = CC_CALLBACK_2(RAConstructButton::onTouchEnded, this);
		Director::getInstance()->getEventDispatcher()->
			addEventListenerWithSceneGraphPriority(listener, button_);
		//abandoned,now we use observer patter
		//schedule(schedule_selector(RAConstructButton::checkConstructable),0.05f);
		NotificationCenter::getInstance()->addObserver(this,
			callfuncO_selector(RAConstructButton::checkConstructable),
			"RESOURCE_CHANGE",
			NULL);
	}
	//judge if capital and power can afford the object
	void checkConstructable(Ref* pSender)
	{
		if (button_->isBright())
		{
			if ((RAPlayer::getCapital() < capital_cost_) || (RAPlayer::getPower() < power_cost_))//不能建造
			{
				button_->setBright(false);
			}
		}
		else
		{
			if (!((RAPlayer::getCapital() < capital_cost_) || (RAPlayer::getPower() < power_cost_)))
				button_->setBright(true);
		}
	}
	//a map containing all creat functions
	//initialized in RAInitialAll
	typedef Sprite*(*CreateType)();
	static std::unordered_map<int, CreateType> CreateWiki;
private:
	Button * button_;
	//id of object the button constructs
	const int id;
	//
	int capital_cost_;
	//
	int power_cost_;
};

#endif // __RA_SOLDIER__
