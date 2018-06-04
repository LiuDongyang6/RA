#include"RAHostileObject.h"
#include"RASoldier.h"
RAHostileObject* RAHostileObject::Appear(int id, Point location)
{
	auto object = new RAHostileObject(id);
	object->initWithSpriteFrameNameAndLocation("Base(1).png", location);
	auto touch = EventListenerTouchOneByOne::create();
	touch->onTouchBegan = CC_CALLBACK_2(RAHostileObject::onTouchBegan, object);
	touch->setSwallowTouches(true);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch, object);
	RAMap::getMap()->addChild(object,100);
	return object;
}
bool RAHostileObject::onTouchBegan(Touch* touch, Event* event)
{
	if (RAUtility::containsTouchLocation(touch, event))
	{
		for (auto soldier : RAPlayer::selected_soldiers_)
		{
			soldier->runToFight(this);
		}
		return true;
	}
	else
		return false;
}