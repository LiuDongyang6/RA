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
	//initial selectBox
	selectBox = LayerColor::create(Color4B(145, 150, 134, 255));
	selectBox->setOpacity(70);
	selectBox->retain();
	//initial on touch
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(RedAlert::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(RedAlert::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(RedAlert::onTouchEnded, this);
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
	selectBox->setPosition(RAUtility::getPositionInMap(touch->getLocation()));
	selectBox->setContentSize(Size(0, 0));
	RAMap::getMap()->addChild(selectBox);
	
	return true;
}
void RedAlert::onTouchMoved(Touch* touch, Event* event)
{
	selectBox->setContentSize(Size(touch->getLocation() - touch->getStartLocation()));
}
void RedAlert::onTouchEnded(Touch* touch, Event* event)
{
	//unmoved
	if (touch->getStartLocation() == touch->getLocation())
		selectedSoldiersMove(touch);
	//moved
	else
	{
		auto position = selectBox->getPosition();
		auto size = selectBox->getContentSize();
		auto rect = Rect(MIN(position.x,position.x+size.width),MIN(position.y, position.y + size.height),abs(size.width),abs(size.height));
		//尽管移动了，但范围内没有我方单位
		//则不会清空
		bool cleared=0;
		for (auto soldier : RAPlayer::all_soldiers_)
		{
			auto position = soldier->getPosition();
			if (rect.containsPoint(soldier->getPosition()))
			{
				if (!cleared)
				{
					RAPlayer::selected_soldiers_.clear();
					cleared = 1;
				}
				RAPlayer::selected_soldiers_.insert(soldier);
			}
		}
	}
	RAMap::getMap()->removeChild(selectBox);
}