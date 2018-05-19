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
			this->setTexture(appearances[2]);
			current_appearance_ = 2;
		}
	}
	else if (hp_ < (original_hp_ / 2))
	{
		if (current_appearance_ != 1)
		{
			this->setTexture(appearances[1]);
			current_appearance_ = 1;
		}
	}
	else
	{
		if (current_appearance_ != 0)
		{
			this->setTexture(appearances[0]);
			current_appearance_ = 0;
		}
	}
}

bool RABuilding::initWithFile(const std::string original, const std::string damaged, const std::string dying, const char* UIName)
{
	RAObject::initWithFile(original);

	appearances.push_back(original);
	appearances.push_back(damaged);
	appearances.push_back(dying);
	current_appearance_ = 0;

	UI_ = GUIReader::getInstance()->widgetFromJsonFile(UIName);
	UI_->setPosition(Point(0, 0));
	UI_->retain();

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(RABuilding::onTouchBegan, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	RAPlayer::power() += power_cost_;

	return true;
}

bool RABuilding::annihilation()
{
	RAPlayer::power() -= power_cost_;
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
	if (!RA::containsTouchLocation(touch,event))
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
RAPowerStation* RAPowerStation::create()
{
	RAPowerStation* powerstation = new RAPowerStation();
	
	powerstation->initWithFile("PowerStation0.png", 
		"PowerStation1.png",
		"PowerStation2.png",
		"PowerStation/PowerStationUi.ExportJson");
	
	powerstation->autorelease();
	return powerstation;
}

//
//RABase
//
RABase* RABase::create()
{
	RABase* base = new RABase();

	base->initWithFile("Base0.png", 
		"Base1.png",
		"Base2.png", 
		"Base/BaseUi.ExportJson");
	//Initial UI

	base->autorelease();

	//
	RAConstructButton<RAPowerStation>::initButton(base->UI_, (std::string)"PowerStation");
	//
	RAConstructButton<RABase>::initButton(base->UI_, (std::string)"Button_2");
	//

	return base;
}
