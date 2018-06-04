#include"RAObject.h"

USING_NS_CC;

int myrandom=0;
//
//RAObject
//
bool RAObject::sufferAttack(float attack_speed,int damage)
{
	auto func = [&,damage](float dt)
	{
		this->hp_ -= damage;
		this->toBeOrNotToBe();
	};
	schedule(func, attack_speed,StringUtils::format("%d%d",myrandom++,myrandom++));
	return true;
}
bool RAObject::stopSufferAttack()
{
	return 0;
}

bool RAObject::toBeOrNotToBe()//this should be called after getting attacked
{
	if (hp_ < 0)
	{
		annihilation();
		return false;
	}
	else return true;
}

bool RAObject::annihilation()
{
	//顺序不能颠倒

	//remove children后会导致construct button析构函数触发，停止观察者模式
	removeAllChildrenWithCleanup(true);
	//2，3颠倒会delete自己，则power_cost_会变为未知
	removeFromParentAndCleanup(true);
	//释放tilemap占地
	//RAMap::destroyNormalBuildings()
	return true;
}

bool RAObject::initWithSpriteFrameNameAndLocation(const std::string& filename, Point location)
{
	//
	Sprite::initWithSpriteFrameName(filename);
	//
	setPosition(location);
	auto halfPoint = location - Point(0, getContentSize().height / 2);
	RAMap::sureToBuildNormal(halfPoint + RAMap::getMap()->getPosition(), covering_);
	return true;
}

//
//RAConstructButton
//
typedef Sprite*(*CreateType)(Point);
std::unordered_map<int, CreateType> RAConstructButton::CreateWiki;
std::unordered_set<int> RAConstructButton::LaunchRecord;
bool RAConstructButton::LaunchTest(int id)
{
	if (LaunchRecord.find(id) == LaunchRecord.end())
	{
		auto strName = RAUtility::RAgetProperty(id, "name").asString();
		const char* name = strName.c_str();
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile(
			StringUtils::format("%s/%s.plist", name, name));
		LaunchRecord.insert(id);
		return false;
	}
	else
		return true;
}
RAConstructButton* RAConstructButton::create(Node* parent, Widget* UI, int IdToConstruct)
{
	RAConstructButton* button = new RAConstructButton(UI, IdToConstruct);
	button->initButton();
	button->autorelease();
	parent->addChild(button);
	return button;
}
bool RAConstructButton::onTouchBegan(Touch* touch, Event* event)
{
	if (!button_->isBright())return false;
	if (button_->onTouchBegan(touch, event))
	{
		auto test=LaunchTest(id);
		std::string ObjectNameS = RAUtility::RAgetProperty(id, "name").asString();
		auto ObjectName = ObjectNameS.c_str();
		std::string SpriteFrameName = StringUtils::format("%s(1).png", ObjectName);
		tempObject = Sprite::createWithSpriteFrameName(SpriteFrameName);
		tempObject->setAnchorPoint(Vec2(0.5, 0));
		tempObject->setPosition(touch->getLocation()-Vec2(0,tempObject->getContentSize().height/2));
		//透明可以保证只要不移动就不能建造
		tempObject->setVisible(false);
		//z order 5 used for tempObject
		Director::getInstance()->getRunningScene()->addChild(tempObject, 5);
		return true;
	}
	return false;
}
void RAConstructButton::onTouchMoved(Touch* touch, Event* type)
{
	if (category_ == 100)//building
	{
		//point 指向(0，50%)
		auto point = touch->getLocation();
		auto halfHeight = (tempObject->getContentSize().height) / 2;
		Vec2 halfPoint = point - Vec2(0, halfHeight);
		//if constructable
		if (RAMap::cannotBuildNormal(halfPoint, covering_))
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
void RAConstructButton::onTouchEnded(Touch* touch, Event* type)
{
	//revert to normal texture
	button_->onTouchEnded(touch, type);
	//
	if (category_ == 100)//building
		if (tempObject->isVisible())
		{
			auto object = CreateWiki[id](RAUtility::getPositionInMap(tempObject->getPosition()));

			RAMap::getMap()->addChild(object, category_);
		}
		else
		{

		}
	else //soldier
	{
		auto object = CreateWiki[id](this->getParent()->getPosition()-Vec2(50, 50));
		RAMap::getMap()->addChild(object, category_);
	}
	tempObject->removeFromParentAndCleanup(true);
	tempObject = NULL;
}
void RAConstructButton::initButton()
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
void RAConstructButton::checkConstructable(Ref* pSender)
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