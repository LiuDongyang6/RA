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
	static RAConstructButton* create(Node* parent,Widget* UI, int IdToConstruct)
	{
		RAConstructButton* button = new RAConstructButton(UI, IdToConstruct);
		button->initButton();
		button->autorelease();
		parent->addChild(button);
		return button;
	}
	void onTouchMoved(Touch* touch, Event* type)
	{
		if (category_== 100)//building
		{
			//point 指向(0，50%)
			auto point = touch->getLocation();
			auto halfHeight = (tempObject->getContentSize().height) / 2;
			Vec2 halfPoint = point - Vec2(0, halfHeight);
			//if constructable
			if (RAMap::cannotBuildNormal(halfPoint, 4))
			{
				tempObject->setPosition(point);
				tempObject->setVisible(true);
			}
			else
			{
				tempObject->setVisible(false);
			}
		}
	}
	void onTouchEnded(Touch* touch, Event* type)
	{
		//revert to normal texture
		button_->onTouchEnded(touch, type);
		//
		if (category_ == 100)//building
			if (tempObject->isVisible())
			{
				int category = (RAUtility::RAgetProperty(id, "category").asInt());
				tempObject->retain();
				tempObject->removeFromParentAndCleanup(false);
				tempObject->setPosition(RAUtility::getPositionInMap(tempObject->getPosition()));
				RAMap::getMap()->addChild(tempObject, category_);
				tempObject->release();
				auto halfPoint = touch->getLocation();
				auto height = tempObject->getContentSize().height / 2;
				halfPoint -= Vec2(0, height);
				RAMap::sureToBuildNormal(halfPoint, 4);
			}
			else
			{
				auto it = static_cast<RAObject*>(tempObject);
				it->annihilation();
			}
		else //soldier
		{
			tempObject->setPosition(this->getParent()->getPosition() - Vec2(50, 50));
			tempObject->retain();
			tempObject->removeFromParentAndCleanup(false);
			RAMap::getMap()->addChild(tempObject, category_);
			tempObject->release();
			tempObject->setVisible(true);
		}
		tempObject = NULL;
	}
	bool onTouchBegan(Touch* touch, Event* event)
	{
		if (!button_->isBright())return false;
		if (button_->onTouchBegan(touch, event))
		{
			tempObject = CreateWiki[id]();
			tempObject->setPosition(touch->getLocation());
			//透明可以保证只要不移动就不能建造
			tempObject->setVisible(false);
			//z order 5 used for tempObject
			Director::getInstance()->getRunningScene()->addChild(tempObject,5);
			return true;
		}
		return false;
	}
	void initButton()
	{
		button_->setTouchEnabled(0);
		//touch
		auto listener = EventListenerTouchOneByOne::create();
		listener->onTouchBegan = CC_CALLBACK_2(RAConstructButton::onTouchBegan, this);
		listener->onTouchMoved = CC_CALLBACK_2(RAConstructButton::onTouchMoved, this);
		listener->onTouchEnded = CC_CALLBACK_2(RAConstructButton::onTouchEnded, this);
		Director::getInstance()->getEventDispatcher()->
			addEventListenerWithSceneGraphPriority(listener, button_);
		//
		checkConstructable(this);
		//金钱变化
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
