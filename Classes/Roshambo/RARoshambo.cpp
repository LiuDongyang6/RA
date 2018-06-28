#include"RARoshambo.h"
#include"RAUtility.h"
#include"PlayScene.h"
int RARoshambo::contrast[2] = { 0 };
RARoshambo* RARoshambo::cards[3];
RARoshambo* RARoshambo::create(int num)
{
	auto object = new RARoshambo(num);
	object->initWithSpriteFrameName(StringUtils::format("roshambo%d.png", num));

	object->setPosition(Point(800, 300) + Point((num - 2) * 200, 0));
	auto touch=EventListenerTouchOneByOne::create();
	touch->onTouchBegan = CC_CALLBACK_2(RARoshambo::onTouchBegan,object);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch,object);
	return object;
}
void RARoshambo::startCompete()
{
	auto &scene = PlayScene::_thisScene;
	auto sch = [&](float dt) {
		std::queue<std::string> msgs;
		scene->_client->RAGetMessage(msgs);
		while(!msgs.empty())
		{
			RARoshambo::RoshamboReceive(msgs.front());
			msgs.pop();
		}
	};
	scene->schedule(sch, std::string("COMPETING"));
	for (int i = 0; i != 3; ++i)
	{
		cards[i] = RARoshambo::create(i+1);
		PlayScene::_thisScene->addChild(cards[i]);
	}
}
bool RARoshambo::onTouchBegan(Touch* touch,Event* event)
{
	if (RAUtility::containsTouchLocation(touch, event))
	{
		//发消息
		PlayScene::_thisScene->_client->sendMessage(StringUtils::format("$%d", num_));
		for (int i = 0; i != 3; ++i)
		{
			if (i+1 != num_)
			{
				cards[i]->removeFromParent();
			}
		}
		auto move_to = MoveTo::create(1.0f, Vec2(800, 300));
		auto scale_to = ScaleTo::create(1.0f, 1.5f);
		auto action = Spawn::createWithTwoActions(move_to, scale_to);
		runAction(action);
		showCard(true, num_);
		return true;
	}
}
void RARoshambo::showCard(bool isMe, int num)
{
	contrast[isMe] = num;
	if (contrast[!isMe] != 0)//双方都已经出牌
	{
		auto opponent=Sprite::createWithSpriteFrameName(StringUtils::format("roshambo%d.png", contrast[0]));
		PlayScene::_thisScene->addChild(opponent);
		opponent->setRotation(180.0f);
		opponent->setPosition(Point(800, 1200));
		auto moveto = MoveTo::create(1.5f, Point(800, 600));
		auto delay = DelayTime::create(1.0f);
		CallFunc* call;
		if (contrast[0] == contrast[1])
		{
			auto func = [=]() {
				RARoshambo::cards[contrast[1]-1]->removeFromParent();
				RARoshambo::contrast[0] = RARoshambo::contrast[1] = 0;
				opponent->removeFromParent();
				RARoshambo::startCompete();
			};
			call = CallFunc::create(func);
		}
		//我方胜
		else if((contrast[1]-contrast[0]==1)|| (contrast[1] - contrast[0] == -2))
		{
			auto func = [=]() {
				PlayScene::_thisScene->unschedule("COMPETING");
				RARoshambo::cards[contrast[1] - 1]->removeFromParent();
				opponent->removeFromParent();
				RARoshambo::contrast[0] = RARoshambo::contrast[1] = 0;
				PlayScene::_thisScene->gameStart(true);
			};
			call = CallFunc::create(func);
		}
		//我方败
		else 
		{
			auto func = [=]() {
				PlayScene::_thisScene->unschedule("COMPETING");
				RARoshambo::cards[contrast[1] - 1]->removeFromParent();
				opponent->removeFromParent();
				RARoshambo::contrast[0] = RARoshambo::contrast[1] = 0;
				PlayScene::_thisScene->gameStart(false);
			};
			call = CallFunc::create(func);
		}
		auto seq = Sequence::create(moveto, delay, call, NULL);
		opponent->runAction(seq);
	}
	else
	{
		return;
	}

}
void RARoshambo::RoshamboReceive(std::string msg)
{
	log(msg.c_str());
	log(int(msg[1]));
	if (msg[0] == '$')
	{
		showCard(false, msg[1]-'0');
	}
}