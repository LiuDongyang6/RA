#include"RAObject.h"
#include"RASoldier.h"
#include"RABuilding.h"

USING_NS_CC;

//
//RAObject
//
void RAObject::stopSufferAttack(RASoldier* attacker)
{
	attacking_me_.erase(attacker);
	unschedule(StringUtils::format("ATK_%05d", attacker->getCount()));
}

bool RAObject::toBeOrNotToBe()//this should be called after getting attacked
{
	if (hp_ < 0)
	{
		NotificationCenter::getInstance()->postNotification("DIE", this);
		annihilation();
		return false;
	}
	else return true;
}

bool RAObject::annihilation()
{
	//顺序不能颠倒
	//释放tilemap占地
	RAMap::destroyNormalBuildings(getPosition(), covering_);
	//
	NotificationCenter::getInstance()->removeAllObservers(this);
	//
	hp_bar->removeFromParentAndCleanup(true);
	//remove children后会导致construct button析构函数触发，停止观察者模式
	removeAllChildrenWithCleanup(true);
	//上下颠倒会delete自己，则power_cost_会变为未知
	removeFromParentAndCleanup(true);
	return true;
}

bool RAObject::initWithSpriteFrameNameAndLocation(const std::string& filename, Point location)
{
	//
	Sprite::initWithSpriteFrameName(filename);
	//building
	if (category_ == 100||category_==2)
	{
		setAnchorPoint(Vec2(0.5, 0));
		RAMap::sureToBuildNormal(location, covering_);
	}
	//soldier
	else
	{
		location = RAMap::soldierBirth(location, covering_);
		setAnchorPoint(Vec2(0.5, 0));
	}
	setPosition(location);
	RAMap::getMap()->addChild(this, category_);
	//initialize hp_bar
	hp_bar = Sprite::create("hp_bar.png");
	hp_bar->setPosition(getPosition());
	hp_bar->setContentSize(Size(getContentSize().width,5));
	hp_bar->setAnchorPoint(Vec2(0.5, 3.0));
	RAMap::getMap()->addChild(hp_bar,99);
	//
	return true;
}

void RAObject::changeControl(bool mine)
{
	under_my_control = mine;
	if (mine)//我方侵占对方单位
	{
		RAPlayer::enemies.erase(this);
		if (category_ == 100)//建筑
		{
			//增加用电量
			RAPlayer::consumePower(static_cast<RABuilding*>(this)->getPowerCost());
		}
		else//士兵
		{
			auto p = static_cast<RASoldier*>(this);
			p->stopCurrentBehavior();
			RAPlayer::all_soldiers_.insert(p);
		}
	}
	else//敌方侵占我方单位
	{
		RAPlayer::enemies.insert(this);
		if (category_ == 100)
		{
			RAPlayer::resumePower(static_cast<RABuilding*>(this)->getPowerCost());
		}
		else
		{
			auto p = static_cast<RASoldier*>(this);
			p->stopCurrentBehavior();
			RAPlayer::all_soldiers_.insert(p);
		}
	}
}
//
//RAConstructButton
//
typedef RAObject*(*CreateType)(Point);
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
		//if constructable
		if (RAMap::cannotBuildNormal(RAUtility::getPositionInMap(point), covering_))
		{
			tempObject->setPosition(point-Vec2(0, tempObject->getContentSize().height / 2));
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
			//resources cost
			RAPlayer::consumePower(power_cost_);
			RAPlayer::consumeCapital(capital_cost_);
		}
		else
		{

		}
	else //soldier
	{
		auto object = CreateWiki[id](this->getParent()->getPosition());
		//resources cost
		RAPlayer::consumePower(power_cost_);
		RAPlayer::consumeCapital(capital_cost_);
		//record in archives
		RAPlayer::all_soldiers_.insert(static_cast<RASoldier*>(object));
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