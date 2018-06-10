#include "RoomScene.h"
#include "LoadingScene.h"

USING_NS_CC;

Scene* Room::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Room::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}


// on "init" you need to initialize your instance
bool Room::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Sprite *bg = Sprite::create("room-back.png");
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

	// add a label shows "Red Alert"
	// create and initialize a label

	auto label = Label::createWithTTF("Red Alert", "fonts/Marker Felt.ttf", 24);

	// position the label on the center of the screen
	label->setPosition(Vec2(origin.x + visibleSize.width - label->getContentSize().width,
		origin.y + visibleSize.height - label->getContentSize().height));

	// add the label as a child to this layer
	this->addChild(label, 1);

	auto startMenuItem = MenuItemImage::create(
		"start-up1.png",
		"start-up1.png",
		CC_CALLBACK_1(Room::startCallback, this));
	startMenuItem->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 3));

	auto lastpageMenuItem = MenuItemImage::create(
		"lastpage.png",
		"lastpage.png",
		CC_CALLBACK_1(Room::menuLastpageCallback, this));

	lastpageMenuItem->setPosition(Vec2(origin.x + lastpageMenuItem->getContentSize().width,
		origin.y + lastpageMenuItem->getContentSize().height));

	auto okMenuItem = MenuItemImage::create(
		"ok-down1.png",
		"ok-up1.png",
		CC_CALLBACK_1(Room::menuOkCallback, this));

	okMenuItem->setPosition(Vec2(origin.x + visibleSize.width - okMenuItem->getContentSize().width,
		origin.y + okMenuItem->getContentSize().height));

	Menu* mn = Menu::create(lastpageMenuItem, okMenuItem, NULL);
	mn->setPosition(Vec2::ZERO);
	this->addChild(mn);

	auto UI = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("ListView_1.ExportJson");
	UI->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	this->addChild(UI);

	return true;
}

void Room::menuLastpageCallback(Ref* pSender)
{
	Director::getInstance()->popScene();
}

void Room::startCallback(Ref* pSender)
{
	auto sc = Loading::createScene();
	Director::getInstance()->pushScene(sc);
}

void Room::menuOkCallback(Ref* pSender)
{
	Director::getInstance()->popScene();
}