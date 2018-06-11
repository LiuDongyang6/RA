#include "LoadingScene.h"
#include "PlayScene.h"

USING_NS_CC;

Scene* Loading::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Loading::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}


// on "init" you need to initialize your instance
bool Loading::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// ----------------------------------- add background picture -----------------------------------
	Sprite *bg = Sprite::create("Scene/loading-back.png");
	// position the label on the center of the screen
	bg->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	Size mywinsize = Director::getInstance()->getWinSize();
	float winw = mywinsize.width;
	float winh = mywinsize.height;
	float spx = bg->getTextureRect().getMaxX();
	float spy = bg->getTextureRect().getMaxY();
	bg->setScaleX(winw / spx);
	bg->setScaleY(winw / spx);
	this->addChild(bg, -1);

	// ----------------------------------- add a label shows "Red Alert" -----------------------------------
	// create and initialize a label

	auto label = Label::createWithTTF("Red Alert", "fonts/Marker Felt.ttf", 24);

	// position the label on the center of the screen
	label->setPosition(Vec2(origin.x + visibleSize.width - label->getContentSize().width,
		origin.y + visibleSize.height - label->getContentSize().height));

	// add the label as a child to this layer
	this->addChild(label, 1);

	// ----------------------------------- add lastpage button -----------------------------------
	auto lastpageMenuItem = MenuItemImage::create(
		"Scene/lastpage.png",
		"Scene/lastpage.png",
		CC_CALLBACK_1(Loading::menuLastpageCallback, this));

	lastpageMenuItem->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 3));

	Menu* mn = Menu::create(lastpageMenuItem, NULL);
	mn->setPosition(Vec2::ZERO);
	this->addChild(mn);

	return true;
}

void Loading::menuLastpageCallback(Ref* pSender)
{
	Director::getInstance()->popScene();
}