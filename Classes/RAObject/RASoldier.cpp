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


	return true;
}
bool RASoldier::onTouchBegan(Touch* touch, Event* event)
{
	if (RAUtility::containsTouchLocation(touch, event))
	{
		RAPlayer::selected_soldiers_.clear();

		RAPlayer::selected_soldiers_.pushBack(this);
		stopAllActions();
		return true;
	}
	else
		return false;
}
bool RASoldier::annihilation()
{
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
	stopAllActions();
	//
	auto animation = Animation::createWithSpriteFrames(animation_[1], 0.2f);
	auto animate = Animate::create(animation);
	auto repeat = RepeatForever::create(animate);
	//
	auto move = MoveTo::create(getPosition().getDistance(point)/speed_, point);

	runAction(repeat);
	runAction(move);
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