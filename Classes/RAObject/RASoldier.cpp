#include"RASoldier.h"
#include"RABuilding.h"
//
//RASoldier
//
void RASoldier::sufferAttack(float attack_speed, int damage, RASoldier* attacker)
{
	//保存进攻我的对象
	attacking_me_.insert(attacker);
	auto func = [&, damage](float dt)
	{
		this->hp_ -= damage;
		this->hp_bar->setScaleX(float(hp_) / original_hp_);
		this->toBeOrNotToBe();
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
			RAPlayer::selected_soldiers_.insert(this);
		}
		else
		{
			for (auto soldier : RAPlayer::selected_soldiers_)
			{
				soldier->runToFight(this);
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
	RAPlayer::all_soldiers_.erase(this);
	RAPlayer::selected_soldiers_.erase(this);
	//die animation
	auto animation = Animation::createWithSpriteFrames(animation_[0], 1.0f / 8);
	auto animate = Animate::create(animation);
	//一定要以CallFunc的形式调用
	auto remove = [&]() {RAObject::annihilation(); };
	CallFunc* callFunc = CallFunc::create(remove);
	auto seq = Sequence::create(animate,callFunc,NULL);
	
	runAction(seq);
	return true;
}
void RASoldier::runTo(Point point)
{
	stopCurrentBehavior();
	//保存目的地
	destination = point;
	//跑步动画
	auto repeat = getAction(1, 0.2f);
	runAction(repeat);
	//
	routine_.clear();
	routine_ = RAMap::findRoutine(this, destination, covering_);
	findRoadAndLetGo();

}
void RASoldier::runToFight(RAObject* object)
{
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
	auto vec = RAMap::findRoutine(this,destination,covering_);
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
	if (getPosition().distance(AimEnemy->getPosition()) <= range_)
	{
		doAttack();
	}
	else
	{
		RAMap::removeSoldierCollision(getPosition(), covering_);
		auto vec = RAMap::findRoutine(this, Point(AimEnemy->getPosition()), covering_);
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

	return object;
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

	return object;
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
		auto vec = RAMap::findRoutine(this, Point(destination), covering_);
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
//RAWizzard
//
RAObject* RAWizzard::create(Point location)
{
	RAWizzard* object = new RAWizzard();

	object->initWithIdAndLocation(id, location);

	object->autorelease();

	return object;
}
