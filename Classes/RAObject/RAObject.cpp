#include"RAObject.h"

USING_NS_CC;


//
//RAObject
//
bool RAObject::sufferAttack(int damage)
{
	hp_ -= damage;
	return toBeOrNotToBe();
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
	//恢复资源，发布消息
	RAPlayer::resumePower(power_cost_);
	//2，3颠倒会delete自己，则power_cost_会变为未知
	removeFromParentAndCleanup(true);
	return true;
}

bool RAObject::initWithSpriteFrameName(const std::string& filename)
{
	Sprite::initWithSpriteFrameName(filename);
	RAPlayer::consumePower(power_cost_) ;
	RAPlayer::consumeCapital(capital_cost_);
	return true;
}

//
//RAConstructButton
//
typedef Sprite*(*CreateType)();
std::unordered_map<int, CreateType> RAConstructButton::CreateWiki;

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
		tempObject = CreateWiki[id]();
		tempObject->setPosition(touch->getLocation());
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
void RAConstructButton::onTouchEnded(Touch* touch, Event* type)
{
	//revert to normal texture
	button_->onTouchEnded(touch, type);
	//
	if (category_ == 100)//building
		if (tempObject->isVisible())
		{
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
			RAPlayer::resumeCapital(capital_cost_);

		}
	else //soldier
	{
		tempObject->setPosition(this->getParent()->getPosition() - Vec2(50, 50));
		tempObject->retain();
		tempObject->removeFromParentAndCleanup(false);
		RAMap::getMap()->addChild(tempObject, category_);
		tempObject->release();
		tempObject->setVisible(true);
		//
		//RAMap::setSoilderCollision(tempObject->getPosition() + RAMap::getMap()->getPosition());
	}
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