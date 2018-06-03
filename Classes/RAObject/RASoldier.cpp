#include"RASoldier.h"
//
//RASoldier
//
bool RASoldier::initWithId(int id)
{
	//super initial
	auto  strName = RAUtility::RAgetProperty(id, "name").asString();
	const char* name = strName.c_str();
	RAObject::initWithSpriteFrameName(StringUtils::format("%s(1).png", name));
	//initial animation
	auto frame = RAUtility::RAgetProperty(id, "frame_upper_bound");
	auto cache = SpriteFrameCache::getInstance();
	
	int j = 1;
	for (int i = 0; i != 2; ++i)
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
	//һ��Ҫ��CallFunc����ʽ����
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
	//�粻ֹͣ����ʸ�����ƶ�
	stopAllActions();
	//����Ŀ�ĵ�
	destination = point;
	//
	auto repeat = getAction(1, 0.2f);
	runAction(repeat);
	//
	findRoadAndLetGo();

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
	auto vec = RAMap::findRoutine(this,destination);
	if (vec[2] == 0)
	{
		Point point = RAUtility::getPositionInMap(Point(vec[0], vec[1]));
		auto move = MoveTo::create(getPosition().getDistance(point) / speed_, point);
		auto call = [&]() {findRoadAndLetGo(); };
		CallFunc* callFunc = CallFunc::create(call);
		auto sequence = Sequence::create(move, callFunc, NULL);
		runAction(sequence);
	}
	else
	{
		int a = 0;
	}
}
//
//RAFairy
//
bool RAFairy::lauched = 0;
Sprite* RAFairy::create()
{
	if (!lauched)
	{
		//asCstring has some bug
		auto strName = RAUtility::RAgetProperty(id, "name").asString();
		const char* name = strName.c_str();
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile(
			StringUtils::format("%s/%s.plist",name,name));
		lauched = 1;
	}
	RAFairy* fairy = new RAFairy();

	fairy->initWithId(id);

	fairy->autorelease();
	return fairy;
}
//
//RAAssassin
//
bool RAAssassin::lauched = 0;
Sprite* RAAssassin::create()
{
	if (!lauched)
	{
		//asCstring has some bug
		auto strName = RAUtility::RAgetProperty(id, "name").asString();
		auto test = SpriteFrameCache::getInstance();
		const char* name = strName.c_str();
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile(
			StringUtils::format("%s/%s.plist", name, name));
		lauched = 1;
	}
	RAAssassin* assassin = new RAAssassin();

	assassin->initWithId(id);

	assassin->autorelease();
	return assassin;
}
//
//RAGeneral
//
bool RALancer::lauched = 0;
Sprite* RALancer::create()
{
	if (!lauched)
	{
		//asCstring has some bug
		auto strName = RAUtility::RAgetProperty(id, "name").asString();
		auto test = SpriteFrameCache::getInstance();
		const char* name = strName.c_str();
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile(
			StringUtils::format("%s/%s.plist", name, name));
		lauched = 1;
	}
	RALancer* lancer = new RALancer();

	lancer->initWithId(id);

	lancer->autorelease();
	return lancer;
}
//
//RAGeneral
//
bool RAGeneral::lauched = 0;
Sprite* RAGeneral::create()
{
	if (!lauched)
	{
		//asCstring has some bug
		auto strName = RAUtility::RAgetProperty(id, "name").asString();
		auto test = SpriteFrameCache::getInstance();
		const char* name = strName.c_str();
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile(
			StringUtils::format("%s/%s.plist", name, name));
		lauched = 1;
	}
	RAGeneral* general = new RAGeneral();

	general->initWithId(id);

	general->autorelease();

	return general;
}