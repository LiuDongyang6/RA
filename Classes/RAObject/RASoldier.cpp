#include"RASoldier.h"
#include"RABuilding.h"
#include"PlayScene.h"
#include<iomanip>
//
//RASoldier
//
void RASoldier::sufferAttack(float attack_speed, int damage, RASoldier* attacker)
{
	//保存进攻我的对象
	attacking_me_.insert(attacker);
	auto func = [&, damage](float dt)
	{
		if (!this->invicible_)
		{
			this->hp_ -= damage;
			this->hp_bar->setScaleX(float(hp_) / original_hp_);
			this->toBeOrNotToBe();
		}
	};
	schedule(func, attack_speed, StringUtils::format("ATK_%05d", attacker->getCount()));
}
bool RASoldier::initWithIdAndLocation(int id, Point location)
{
	//super initial
	auto  strName = RAUtility::RAgetProperty(id, "name").asString();
	const char* name = strName.c_str();
	RAObject::initWithSpriteFrameNameAndLocation(StringUtils::format("%s(1).png", name),location);
	//initial animation
	auto frame = RAUtility::RAgetProperty(id, "frame_upper_bound");
	auto cache = SpriteFrameCache::getInstance();
	
	int j = 1;
	for (int i = 0; i != 3; ++i)
		for (; j != frame[i].asInt() + 1; ++j)
		{
			auto aFrame = cache->getSpriteFrameByName(StringUtils::format("%s(%d).png", name, j));
			animation_[i].pushBack(aFrame);
		}

	//initial onClick
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = (CC_CALLBACK_2(RASoldier::onTouchBegan, this));
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	
	return true;
}
bool RASoldier::onTouchBegan(Touch* touch, Event* event)
{
	if (RAUtility::containsTouchLocation(touch, event))
	{
		if (under_my_control)
		{
			RAPlayer::selected_soldiers_.clear();
			RAPlayer::selected_soldiers_.push_back(this);
		}
		else
		{
			/**反向遍历是为了避免遍历过程中元素移除
			*从而会导致遍历混乱并报错
			*/
			auto& TempSet = RAPlayer::selected_soldiers_;
			for (auto soldier=TempSet.end();soldier!=TempSet.begin();)
			{
				(*(--soldier))->runToFight(this);
			}
		}
		return true;
	}
	else
		return false;
}
bool RASoldier::annihilation()
{
	//remove from archives
	//从选中士兵和我方士兵中删去
	{
		auto it1 = std::find(RAPlayer::all_soldiers_.begin(), RAPlayer::all_soldiers_.end(), this);
		if (it1 != RAPlayer::all_soldiers_.end())RAPlayer::all_soldiers_.erase(it1);
		auto it2 = std::find(RAPlayer::selected_soldiers_.begin(), RAPlayer::selected_soldiers_.end(), this);
		if (it2 != RAPlayer::selected_soldiers_.end())RAPlayer::selected_soldiers_.erase(it2);

	}
	//die animation
	auto animation = Animation::createWithSpriteFrames(animation_[active_die_?2:0], 1.0f / 8);
	auto animate = Animate::create(animation);
	//一定要以CallFunc的形式调用
	auto tempSprite = Sprite::createWithSpriteFrame(getSpriteFrame());
	tempSprite->setPosition(getPosition());
	tempSprite->setAnchorPoint(getAnchorPoint());
	RAMap::getMap()->addChild(tempSprite, category_);
	auto remove = [=]() { tempSprite->removeFromParent(); };
	CallFunc* callFunc = CallFunc::create(remove);
	auto seq = Sequence::create(animate, callFunc, NULL);
	tempSprite->runAction(seq);
	RAObject::annihilation();
	return true;
}
void RASoldier::runTo(Point point)
{
	//online
	if(under_my_control)
	{
		using namespace std;
		ostringstream os;
		os << setfill('0') << 'r' << setw(6) << object_count_ <<RAUtility::coortostr(point);
		PlayScene::_thisScene->_client->sendMessage(os.str());
	}
	stopCurrentBehavior();
	//保存目的地
	destination = point;
	//跑步动画
	auto repeat = getAction(1, 0.2f);
	runAction(repeat);
	//
	findRoadAndLetGo();

}
void RASoldier::runToFight(RAObject* object)
{
	//online
	if(under_my_control)
	{
		using namespace std;
		ostringstream os;
		os << setfill('0') << 'f' << setw(6) << object_count_ << setw(6) << object->getCount();
		PlayScene::_thisScene->_client->sendMessage(os.str());
	}
	//如不停止会以矢量和移动
	//重置状态
	stopCurrentBehavior();
	//保存目的地
	AimEnemy = object;
	NotificationCenter::getInstance()->
		addObserver(this, callfuncO_selector(RASoldier::stopCurrentBehavior),
			StringUtils::format("DIE",AimEnemy->getCount()),AimEnemy);
	//
	auto repeat = getAction(1, 0.2f);
	runAction(repeat);
	//
	findRoadAndLetGoForFight();

}
Action* RASoldier::getAction(int number, float dt)
{
	auto animation = Animation::createWithSpriteFrames(animation_[number],dt);
	auto animate = Animate::create(animation);
	auto repeat = RepeatForever::create(animate);
	return repeat;
}
void RASoldier::findRoadAndLetGo()
{
	RAMap::removeSoldierCollision(getPosition(), covering_);
	auto vec = RAMap::findRoutineOneByOne(this,destination,covering_);
	next_step = Point(vec[0], vec[1]);
	RAMap::setSoldierCollision(next_step, covering_);
	if (vec[2] == 0)
	{
		auto move = MoveTo::create(getPosition().getDistance(next_step) / speed_, next_step);
		auto call = [&]() {findRoadAndLetGo(); };
		CallFunc* callFunc = CallFunc::create(call);
		auto sequence = Sequence::create(move, callFunc, NULL);
		runAction(sequence);
		hp_bar->runAction(move->clone());
	}
	else
	{
		auto move = MoveTo::create(getPosition().getDistance(next_step) / speed_, next_step);
		runAction(move);
		hp_bar->runAction(move->clone());
	}
}
void RASoldier::findRoadAndLetGoForFight()
{
	if (getPosition().distance(AimEnemy->getCorePoint()) <= range_)
	{
		doAttack();
	}
	else
	{
		RAMap::removeSoldierCollision(getPosition(), covering_);
		auto vec = RAMap::findRoutineOneByOne(this, Point(AimEnemy->getPosition()), covering_);
		next_step = Point(vec[0], vec[1]);
		RAMap::setSoldierCollision(next_step, covering_);
		if (vec[2] == 0)
		{
			auto move = MoveTo::create(getPosition().getDistance(next_step) / speed_, next_step);
			auto call = [&]() {	findRoadAndLetGoForFight(); };
			CallFunc* callFunc = CallFunc::create(call);
			auto sequence = Sequence::create(move, callFunc, NULL);
			runAction(sequence);
			hp_bar->runAction(move->clone());
		}
		else
		{
			auto move = MoveTo::create(getPosition().getDistance(next_step) / speed_, next_step);
			runAction(move);
			hp_bar->runAction(move->clone());
		}
	}
}
void RASoldier::doAttack()
{
	//停止动画
	stopAllActions();

	auto FightAction = getAction(2, attack_speed_/animation_[2].size());

	runAction(FightAction);

	AimEnemy->sufferAttack(attack_speed_, hit_,this);

	auto func= [&](float dt){
		if (getPosition().distance(this->AimEnemy->getPosition()) > range_)
		{
			stopCurrentBehavior();
		}
	};
	schedule(func, 0.2f, std::string("IN_RANGE_CHECK"));
}
void RASoldier::stopCurrentBehavior(Ref* pSender)
{
	stopAllActions();
	//如果上一个指令是攻击
	if (AimEnemy != NULL)
	{
		NotificationCenter::getInstance()->removeObserver(this, "DIE");
		unschedule(std::string("IN_RANGE_CHECK"));
		AimEnemy->stopSufferAttack(this);
		AimEnemy = NULL;
	}
	//上一个指令是移动(or run to fight)
	if (next_step != Point(-1, -1))
	{
		RAMap::removeSoldierCollision(next_step, covering_);
		RAMap::setSoldierCollision(getPosition(), covering_);
		next_step = Point(-1, -1);
	}
}
//
//RAFairy
//
RAObject* RAFairy::create(Point location)
{
	RAFairy* fairy = new RAFairy();

	fairy->initWithIdAndLocation(id,location);

	fairy->autorelease();

	return fairy;
}
//
//RAAssassin
//
RAObject* RAAssassin::create(Point location)
{
	RAAssassin* assassin = new RAAssassin();

	assassin->initWithIdAndLocation(id, location);

	assassin->autorelease();

	return assassin;
}
//
//RAGeneral
//
RAObject* RALancer::create(Point location)
{
	RALancer* lancer = new RALancer();

	lancer->initWithIdAndLocation(id, location);

	lancer->autorelease();

	return lancer;
}
//
//RAGeneral
//
RAObject* RAGeneral::create(Point location)
{
	RAGeneral* general = new RAGeneral();

	general->initWithIdAndLocation(id, location);

	general->autorelease();

	return general;
}
//
//RAAtomicBomb
//
RAObject* RAAtomicBomb::create(Point location)
{
	RAAtomicBomb* object = new RAAtomicBomb();

	object->initWithIdAndLocation(id, location);

	object->autorelease();

	object->initFire();

	return object;
}
void RAAtomicBomb::initFire()
{
	fire_ = Sprite::createWithSpriteFrameName("AtomicBomb_range_attack.png");
	fire_->setAnchorPoint(Vec2(0.5, 0));
	RAMap::getMap()->addChild(fire_, 10000);
	fire_->setScale(0);

	auto in = ScaleTo::create(attack_speed_ , 1);
	auto changeColor = TintTo::create(attack_speed_ , Color3B(180, 53, 34));
	auto fadeout = FadeOut::create(attack_speed_ );
	auto F = fire_;
	auto call = [=]() {
		std::vector<RAObject*> objects;
		for (auto it:RAPlayer::master_table_)
		{
			auto object = it.second;
			if (object->getCorePoint().distance(F->getPosition()) < 1000)
				objects.push_back(object);
		}
		for (auto object : objects)
			object->annihilation();
	};
	auto callFunc = CallFunc::create(call);

	auto call2 = [=]() {
		F->removeFromParent();
	};
	auto callFunc2 = CallFunc::create(call2);
	auto step = Spawn::create(callFunc, fadeout, NULL);
	fire_action_ = Sequence::create(in, changeColor,step,callFunc2, NULL);
	fire_action_->retain();
}
void RAAtomicBomb::doAttack()
{
	//停止动画
	stopAllActions();
	active_die_ = 1;
	annihilation();
}
bool RAAtomicBomb::annihilation()
{

	fire_->setPosition(getCorePoint());
	fire_->runAction(fire_action_);
	fire_action_->release();
	RASoldier::annihilation();
	return 1;
}
//
//RABlackMagician
//
RAObject* RABlackMagician::create(Point location)
{
	RABlackMagician* object = new RABlackMagician();

	object->initWithIdAndLocation(id, location);

	object->autorelease();

	return object;
}
//
//RABomber
//
RAObject* RABomber::create(Point location)
{
	RABomber* object = new RABomber();

	object->initWithIdAndLocation(id, location);

	object->autorelease();

	object->initFire();

	return object;
}
void RABomber::initFire()
{
	fire_ = Sprite::createWithSpriteFrameName("Bomber_range_attack.png");
	RAMap::getMap()->addChild(fire_, 80);
	fire_->setScale(0);

	auto in = ScaleTo::create(attack_speed_ / 4, 1);

	auto fade = FadeOut::create(attack_speed_ / 4);
	auto revolve = RotateBy::create(attack_speed_ / 4, 1080.0f);
	auto out = Spawn::createWithTwoActions(fade, revolve);

	fire_action_ = Sequence::create(in, out, NULL);
	fire_action_->retain();
}
void RABomber::doAttack()
{
	//停止动画
	stopAllActions();

	auto animation = Animation::createWithSpriteFrames(animation_[2],attack_speed_ / animation_[2].size());
	auto animate = Animate::create(animation);
	auto call = [&]() {
		fire_->setPosition(AimEnemy->getCorePoint());
		fire_->setOpacity(255);
		fire_->setScale(0);
		fire_->runAction(fire_action_);
	};
	auto fire_go = CallFunc::create(call);
	auto seq = Sequence::create(animate, fire_go, NULL);
	auto act = RepeatForever::create(seq);
	runAction(act);
	AimEnemy->sufferAttack(attack_speed_, hit_, this);

	auto func = [&](float dt) {
		if (getPosition().distance(this->AimEnemy->getCorePoint()) > range_)
		{
			stopCurrentBehavior();
		}
	};
	schedule(func, 0.2f, std::string("IN_RANGE_CHECK"));
}
bool RABomber::annihilation()
{
	fire_action_->release();
	fire_->removeFromParent();
	return RASoldier::annihilation();
}
//
//RAEngineer
//
RAObject* RAEngineer::create(Point location)
{
	RAEngineer* object = new RAEngineer();

	object->initWithIdAndLocation(id, location);

	object->autorelease();

	return object;
}
void RAEngineer::runToBuildOilField(Point location)
{
	stopCurrentBehavior();
	//保存目的地
	destination = oil_position_= location;
	//
	auto repeat = getAction(1, 0.2f);
	runAction(repeat);
	//
	findRoadAndLetGoForOilField();
}
void RAEngineer::findRoadAndLetGoForOilField()
{
	if (getPosition().distance(destination) <= range_)
	{
		auto test=RAMap::cannotBuildOil(oil_position_, 4);
		if (RAMap::cannotBuildOil(oil_position_,4) != Point(-1000.0f, -1000.0f))
		{
			RAOilField::create(test);
			RAMap::sureToBuildOil(test,4);
		}
	}
	else
	{
		RAMap::removeSoldierCollision(getPosition(), covering_);
		auto vec = RAMap::findRoutineOneByOne(this, Point(destination), covering_);
		next_step = Point(vec[0], vec[1]);
		RAMap::setSoldierCollision(next_step, covering_);
		if (vec[2] == 0)
		{
			auto move = MoveTo::create(getPosition().getDistance(next_step) / speed_, next_step);
			auto call = [&]() {	findRoadAndLetGoForOilField(); };
			CallFunc* callFunc = CallFunc::create(call);
			auto sequence = Sequence::create(move, callFunc, NULL);
			runAction(sequence);
			hp_bar->runAction(move->clone());
		}
		else
		{
			auto move = MoveTo::create(getPosition().getDistance(next_step) / speed_, next_step);
			runAction(move);
			hp_bar->runAction(move->clone());
		}
	}
}
void RAEngineer::doAttack()
{
	stopAllActions();
	//if and only if enemy is a building and is not under my control
	if (AimEnemy->isBuilding() && !AimEnemy->under_my_control)
	{
		//如果不暂停触摸会有莫名其妙的bug
		AimEnemy->getEventDispatcher()->pauseEventListenersForTarget(AimEnemy);
		active_die_ = 1;
		annihilation();
		AimEnemy->changeControl(true);
		AimEnemy->getEventDispatcher()->resumeEventListenersForTarget(AimEnemy);
	}
}
//
//RAWinterSoldier
//
RAObject* RAWinterSoldier::create(Point location)
{
	RAWinterSoldier* object = new RAWinterSoldier();

	object->initWithIdAndLocation(id, location);

	object->autorelease();

	return object;
}
//
//RAWitch
//
RAObject* RAWitch::create(Point location)
{
	RAWitch* object = new RAWitch();

	object->initWithIdAndLocation(id, location);

	object->autorelease();

	return object;
}
//
//RAWizzard
//
RAObject* RAWizzard::create(Point location)
{
	RAWizzard* object = new RAWizzard();

	object->initWithIdAndLocation(id, location);
	object->initWizzard();
	object->autorelease();

	return object;
}
void RAWizzard::initWizzard()
{
	//initial UI
	UI_ = GUIReader::getInstance()->widgetFromJsonFile(RAUtility::RAgetProperty(id, "UIFile").asCString());
	UI_->setPosition(Point(0, 0));
	UI_->retain();
	//init onTouch
	_eventDispatcher->removeEventListenersForTarget(this);
	auto touch = EventListenerTouchOneByOne::create();
	touch->onTouchBegan=CC_CALLBACK_2(RAWizzard::WizzardOnTouch,this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touch, this);
	//init skill
	auto button=(Button*)Helper::seekWidgetByTag(UI_, 0);
	button->setTouchEnabled(false);
	auto touch2 = EventListenerTouchOneByOne::create();
	touch2->onTouchBegan = [=](Touch* touch,Event* event) {
		bool value;
		if (value=button->onTouchBegan(touch, event))
		{
			this->StartSkill();
		}
		return value;
	};
	touch2->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touch2, button);
}
bool RAWizzard::WizzardOnTouch(Touch* touch, Event* event)
{
	bool flag;
	if (flag=RASoldier::onTouchBegan(touch,event))
	{
		if (under_my_control)
		{
			Director::getInstance()->getRunningScene()->getChildByTag(2)->removeChild(RAPlayer::currentUI(), false);
			Director::getInstance()->getRunningScene()->getChildByTag(2)->addChild(this->UI_);
			RAPlayer::currentUI() = UI_;
		}
	}
	return flag;
}
void RAWizzard::StartSkill()
{
	int range = RAUtility::RAgetProperty(id, "skill_range").asInt();
	for (auto soldier : RAPlayer::all_soldiers_)
	{
		if (getPosition().distance(soldier->getPosition()) < range)
		{
			RAWizzardSkill::create(soldier);
		}
	}
}
bool RAWizzard::annihilation()
{
	UI_->removeAllChildrenWithCleanup(true);
	UI_->removeFromParentAndCleanup(true);
	if (RAPlayer::currentUI() == UI_)
	{
		RAPlayer::currentUI() = NULL;
	}
	UI_->release();
	return RASoldier::annihilation();

}
void RAWizzardSkill::create(RASoldier* soldier)
{
	auto ring = new RAWizzardSkill();
	ring->initWithSpriteFrameName("defence_wheel.png");
	ring->setScale((soldier->getContentSize().height + 40) /ring->getContentSize().height);
	ring->setPosition(soldier->getCorePoint());
	RAMap::getMap()->addChild(ring, 80);
	
	soldier->setInvicible();

	auto follow = [=](float dt) 	{
		ring->setPosition(soldier->getCorePoint());
	};
	ring->schedule(follow, std::string("FOLLOW"));

	auto call = [=]() {
		soldier->disSetInvicible();
		ring->removeFromParentAndCleanup(true);
	};
	auto callFunc = CallFunc::create(call);
	auto delay = DelayTime::create(RAUtility::RAgetProperty(13, "skill_lasting").asInt());
	auto couple = Sequence::createWithTwoActions(delay, callFunc);
	ring->runAction(couple);
}