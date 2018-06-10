#include "StartScene.h"
#include "LoadingScene.h"
#include "RoomScene.h"

USING_NS_CC;

Scene* Start::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Start::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}


// on "init" you need to initialize your instance
bool Start::init()
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
	Sprite *bg = Sprite::create("background.png");
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
	label->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - label->getContentSize().height));

	// add the label as a child to this layer
	this->addChild(label, 1);

	// ----------------------------------- add menu items -----------------------------------
	// single-play button
	auto singleStartMenuItem = MenuItemImage::create(
		"single-play.png",
		"single-play.png", 
		CC_CALLBACK_1(Start::menuSingleplayerCallback, this));
	singleStartMenuItem->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height * 2 / 3));
	
	// multi-play button
	auto multiStartMenuItem = MenuItemImage::create(
		"multi-play.png",
		"multi-play.png",
		CC_CALLBACK_1(Start::menuMultiplayerCallback, this));
	multiStartMenuItem->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));

	// lastpage button
	auto lastpageMenuItem = MenuItemImage::create(
		"lastpage.png",
		"lastpage.png",
		CC_CALLBACK_1(Start::menuLastpageCallback, this));

	lastpageMenuItem->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 3));
	
	// ----------------------------------- add all buttons above to scene -----------------------------------
	Menu* mn = Menu::create(singleStartMenuItem, multiStartMenuItem, lastpageMenuItem, NULL);
	mn->setPosition(Vec2::ZERO);
	this->addChild(mn);

	return true;
}

void Start::menuLastpageCallback(Ref* pSender)
{
	Director::getInstance()->popScene();
}

void Start::menuSingleplayerCallback(Ref* pSender)
{
	auto sc = Loading::createScene();
	Director::getInstance()->pushScene(sc);
}


void Start::menuMultiplayerCallback(Ref* pSender)
{
	auto sc = Room::createScene();
	Director::getInstance()->pushScene(sc);
}