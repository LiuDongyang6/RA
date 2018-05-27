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

bool RABuilding::initWithId(int id)
{
	//initial texture
	auto s = RAUtility::RAgetProperty(id, "texture");
	RAObject::initWithFile(s[0].asString());
	for (int i = 0; i != 3; ++i)
		appearances.push_back(s[i].asString());
	current_appearance_ = 0;
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
	listener->onTouchBegan = CC_CALLBACK_2(RABuilding::onTouchBegan, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

bool RABuilding::annihilation()
{
	if (RAPlayer::currentUI() == UI_)
	{
		RAPlayer::currentUI() = 0;
	}
	UI_->removeFromParent();
	UI_->release();
	RAObject::annihilation();
	//release
	return true;
}

bool  RABuilding::sufferAttack(int damage)
{
	bool survive = RAObject::sufferAttack(damage);
	if (survive)
		changeAppearance();
	return survive;
}

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
Sprite* RAPowerStation::create()
{
	RAPowerStation* powerstation = new RAPowerStation();
	
	powerstation->initWithId(id);
	
	powerstation->autorelease();
	return powerstation;
}

//
//RABase
//
Sprite* RABase::create()
{
	RABase* base = new RABase();

	base->initWithId(id);
	//Initial UI

	base->autorelease();

	return base;
}
