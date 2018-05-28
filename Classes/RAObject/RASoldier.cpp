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
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	auto animation = Animation::createWithSpriteFrames(animation_[1],0.2f);
	auto animate = Animate::create(animation);
	auto repeat = RepeatForever::create(animate);
	runAction(repeat);

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
	auto it = RAPlayer::selected_soldiers_.find(this);
	if ((it != RAPlayer::selected_soldiers_.end()))
		RAPlayer::selected_soldiers_.erase(it);
	auto remove = [&]() {removeFromParent(); };
	auto seq = Sequence::create(animate, remove);
	runAction(seq);
	return true;
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