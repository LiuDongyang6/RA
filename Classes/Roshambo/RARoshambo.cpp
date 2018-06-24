#include"RARoshambo.h"
#include"RAUtility.h"
RARoshambo* RARoshambo::create(int num)
{
	auto object = new RARoshambo(num);
	object->initWithSpriteFrameName(StringUtils::format("roshambo%d", num));

	object->setPosition(Point(800, 300) + Point((num - 2) * 200, 0));
	auto touch=EventListenerTouchOneByOne::create();
	touch->onTouchBegan = CC_CALLBACK_2(RARoshambo::onTouchBegan,object);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch,object);
	return object;
}
bool RARoshambo::onTouchBegan(Touch* touch,Event* event)
{
	if (RAUtility::containsTouchLocation(touch, event))
	{
		//·¢ÏûÏ¢
		/////
		auto move_to = MoveTo::create(1.0f, Vec2(800, 300));
		auto scale_to = ScaleTo::create(1.0f, 1.5f);
		auto action = Spawn::createWithTwoActions(move_to, scale_to);
		runAction(action);
		return true;
	}
}