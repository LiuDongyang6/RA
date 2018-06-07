#include"RASoldier.h"
//
//RASoldier
//
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
	
	//record in archives
	RAPlayer::all_soldiers_.insert(this);
	//resources cost
	RAPlayer::consumePower(power_cost_);
	RAPlayer::consumeCapital(capital_cost_);
	return true;
}
bool RASoldier::onTouchBegan(Touch* touch, Event* event)
{
	if (RAUtility::containsTouchLocation(touch, event))
	{
		RAPlayer::selected_soldiers_.clear();

		RAPlayer::selected_soldiers_.insert(this);
		stopAllActions();
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
	auto remove = [&]() {
		auto it = RAPlayer::selected_soldiers_.find(this);
		if ((it != RAPlayer::selected_soldiers_.end()))
			RAPlayer::selected_soldiers_.erase(it);
		RAObject::annihilation(); };
	CallFunc* callFunc = CallFunc::create(remove);
	auto seq = Sequence::create(animate,callFunc,NULL);
	
	runAction(seq);
	return true;
}
void RASoldier::runTo(Point point)
{
	//如不停止会以矢量和移动
	stopAllActions();
	//保存目的地
	destination = point;
	//
	auto repeat = getAction(1, 0.2f);
	runAction(repeat);
	//
	findRoadAndLetGo();

}
void RASoldier::runToFight(RAObject* object)
{
	//如不停止会以矢量和移动
	stopAllActions();
	//保存目的地
	AimEnemy = object;
	//
	auto repeat = getAction(1, 0.2f);
	runAction(repeat);
	//
	findRoadAndLetGoForFight();

}
Action* RASoldier::getAction(int number, float dt)
{
	auto animation = Animation::createWithSpriteFrames(animation_[number], dt);
	auto animate = Animate::create(animation);
	auto repeat = RepeatForever::create(animate);
	return repeat;
}
void RASoldier::findRoadAndLetGo()
{
	RAMap::removeSoldierCollision(getPosition(), covering_);
	auto vec = RAMap::findRoutine(this,destination,covering_);
	Point point = Point(vec[0], vec[1]);
	RAMap::setSoldierCollision(point, covering_);
	if (vec[2] == 0)
	{
		auto move = MoveTo::create(getPosition().getDistance(point) / speed_, point);
		auto call = [&]() {findRoadAndLetGo(); };
		CallFunc* callFunc = CallFunc::create(call);
		auto sequence = Sequence::create(move, callFunc, NULL);
		runAction(sequence);
	}
	else
	{
		auto move = MoveTo::create(getPosition().getDistance(point) / speed_, point);
		runAction(move);
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
		auto vec = RAMap::findRoutine(this, AimEnemy->getPosition(),covering_);
		if (vec[2] == 0)
		{
			Point point = RAUtility::getPositionInMap(Point(vec[0], vec[1]));
			auto move = MoveTo::create(getPosition().getDistance(point) / speed_, point);
			auto call = [&]() {findRoadAndLetGoForFight(); };
			CallFunc* callFunc = CallFunc::create(call);
			auto sequence = Sequence::create(move, callFunc, NULL);
			runAction(sequence);
		}
	}
}
void RASoldier::doAttack()
{
	stopAllActions();

	auto FightAction = getAction(2, 0.2f);

	runAction(FightAction);

	AimEnemy->sufferAttack(attack_speed_, hit_);

	auto func= [&](float dt){
		if (getPosition().distance(this->AimEnemy->getPosition()) > range_)
		{
			AimEnemy->stopSufferAttack();
			stopAllActions();
			unschedule(std::string("isAimInRange"));
		}
	};
	schedule(func, 0.2f, std::string("isAimInRange"));
}
//
//RAFairy
//
bool RAFairy::lauched = 0;
Sprite* RAFairy::create(Point location)
{
	RAFairy* fairy = new RAFairy();

	fairy->initWithIdAndLocation(id,location);

	fairy->autorelease();
	return fairy;
}
//
//RAAssassin
//
bool RAAssassin::lauched = 0;
Sprite* RAAssassin::create(Point location)
{
	RAAssassin* assassin = new RAAssassin();

	assassin->initWithIdAndLocation(id, location);

	assassin->autorelease();
	return assassin;
}
//
//RAGeneral
//
bool RALancer::lauched = 0;
Sprite* RALancer::create(Point location)
{
	RALancer* lancer = new RALancer();

	lancer->initWithIdAndLocation(id, location);

	lancer->autorelease();
	return lancer;
}
//
//RAGeneral
//
bool RAGeneral::lauched = 0;
Sprite* RAGeneral::create(Point location)
{
	RAGeneral* general = new RAGeneral();

	general->initWithIdAndLocation(id, location);

	general->autorelease();

	return general;
}