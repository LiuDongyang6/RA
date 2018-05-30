#include"RedAlert.h"
RedAlert* RedAlert::instance_ = NULL;
RedAlert* RedAlert::getInstance()
{
	if (instance_ == NULL)
		instance_ = new RedAlert();
	return instance_;
}
void RedAlert::initAll()
{
	RAUtility::RAObjectProperty=FileUtils::getInstance()->getStringFromFile("RAObjectProperty.json");
	//initial create_function wiki
	auto &wiki = RAConstructButton::CreateWiki;
	wiki.insert({ 0,RABase::create });
	wiki.insert({1,RAPowerStation::create});
	wiki.insert({ 2,RABarrack::create });
	wiki.insert({ 3,RAFairy::create });
	wiki.insert({ 4,RAAssassin::create });
	wiki.insert({ 5,RALancer::create });
	wiki.insert({ 6,RAGeneral::create });

	//initial building sprite frame
	auto framecache = SpriteFrameCache::getInstance();
	framecache->addSpriteFramesWithFile("Buildings.plist");
	//
	RAMap::init();
	//initial on touch
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(RedAlert::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->
		addEventListenerWithSceneGraphPriority(listener, RAMap::getMap()->getChildByTag(1));
}
void RedAlert::selectedSoldiersMove(Touch* touch)
{
	auto point = RAUtility::getPositionInMap(touch->getLocation());
	for (auto soldier : RAPlayer::selected_soldiers_)
	{
		soldier->runTo(point);
	}
}
bool RedAlert::onTouchBegan(Touch* touch, Event* event)
{
	selectedSoldiersMove(touch);
	return false;
}