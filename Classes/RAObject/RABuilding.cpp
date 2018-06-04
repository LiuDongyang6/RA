#include"RABuilding.h"
#include"PlayScene.h"

USING_NS_CC;

//
//RABuilding
//
void RABuilding::changeAppearance()//called after sufferAttack or repair
{
	if (hp_ < (original_hp_ / 3))
	{
		if ((current_appearance_ != 2))
		{
			this->setSpriteFrame(appearances[2]);
			current_appearance_ = 2;
		}
	}
	else if (hp_ < (original_hp_ / 2))
	{
		if (current_appearance_ != 1)
		{
			this->setSpriteFrame(appearances[1]);
			current_appearance_ = 1;
		}
	}
	else
	{
		if (current_appearance_ != 0)
		{
			this->setSpriteFrame(appearances[0]);
			current_appearance_ = 0;
		}
	}
}

bool RABuilding::initWithIdAndLocation(int id,Point location)
{
	//initial texture
	auto  strName = RAUtility::RAgetProperty(id, "name").asString();
	const char* name = strName.c_str();
	for (int i = 1; i != 4; ++i)
		appearances.push_back(StringUtils::format("%s(%d).png", name,i));
	current_appearance_ = 0;
	RAObject::initWithSpriteFrameNameAndLocation(appearances[0],location);
	//initial UI
	UI_ = GUIReader::getInstance()->widgetFromJsonFile(RAUtility::RAgetProperty(id,"UIFile").asCString());
	UI_->setPosition(Point(0, 0));
	UI_->retain();
	//initial buttons
	auto u = RAUtility::RAgetProperty(id, "UI");
	for (int i = 0; i != u.size(); ++i)
		RAConstructButton::create(this, UI_, u[i].asInt());
	//initial onclick
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(RABuilding::onTouchBegan, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	//cost resources
	RAPlayer::consumePower(power_cost_);
	RAPlayer::consumeCapital(capital_cost_);
	return true;
}

bool RABuilding::annihilation()
{
	auto ui = RAPlayer::currentUI();
	if (RAPlayer::currentUI() == UI_)
	{
		RAPlayer::currentUI() = 0;
	}
	UI_->removeAllChildrenWithCleanup(true);
	UI_->removeFromParentAndCleanup(false);
	//because UI_ may be orphan, so cleanup again;
	UI_->cleanup();
	UI_->release();
	//恢复资源，发布消息
	RAPlayer::resumePower(power_cost_);
	RAObject::annihilation();
	return true;
}

//bool  RABuilding::sufferAttack(int damage)
//{
//	bool survive = RAObject::sufferAttack(damage);
//	if (survive)
//		changeAppearance();
//	return survive;
//}

bool RABuilding::onTouchBegan(Touch* touch, Event* event)
{
	if (!RAUtility::containsTouchLocation(touch,event))
		return false;
	else
	{
		Director::getInstance()->getRunningScene()->getChildByTag(2)->removeChild(RAPlayer::currentUI(),false);
		Director::getInstance()->getRunningScene()->getChildByTag(2)->addChild(this->UI_);
		RAPlayer::currentUI() = UI_;
		return true;
	}
}

//
//RAPowerstation
//
Sprite* RAPowerStation::create(Point location)
{
	RAPowerStation* powerstation = new RAPowerStation();
	
	powerstation->initWithIdAndLocation(id,location);
	
	powerstation->autorelease();
	return powerstation;
}

//
//RABase
//
Sprite* RABase::create(Point location)
{
	RABase* base = new RABase();

	base->initWithIdAndLocation(id, location);
	//Initial UI

	base->autorelease();

	return base;
}

//
//RABarrack
//
Sprite* RABarrack::create(Point location)
{
	RABarrack* base = new RABarrack();

	base->initWithIdAndLocation(id, location);
	//Initial UI

	base->autorelease();

	return base;
}
