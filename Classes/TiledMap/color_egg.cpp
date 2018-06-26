#include "color_egg.h"

using namespace cocos2d;

bool colorEgg::init() {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto back_color = LayerColor::create(Color4B(30, 160, 120, 10));
	back_color->setCascadeOpacityEnabled(true);
	back_color->setOpacity(55);
	this->addChild(back_color, 10);
	auto word = Sprite::create("egg.png");
	word->setPosition(visibleSize.width / 2, -1000);
	word->setScale(2.0f);
	this->addChild(word, 20);
	MoveTo* moveTo = MoveTo::create(20, Point(visibleSize.width / 2, visibleSize.height));
	auto func = [&]() {this->removeFromParent(); };
	auto callFunc = CallFunc::create(func);
	auto actions = Sequence::create(moveTo, callFunc, NULL);
	word->runAction(actions);
	return true;
}
