#include "Scene/EndingScene.h"
#include "Scene/HelloScene.h"


USING_NS_CC;

static int _resultflag;

Scene* EndingScene::createScene(int resultflag)
{
	_resultflag = resultflag;

	auto scene = Scene::create();

	auto layer = EndingScene::create();

	scene->addChild(layer);

	return scene;
}


// on "init" you need to initialize your instance
bool EndingScene::init()
{

	if (!Layer::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();


	//Back to home
	auto homeItem = MenuItemImage::create(
		"Scene/HomeNormal.png", "Scene/HomeSelected.png",
		CC_CALLBACK_1(EndingScene::menuHelloCallback, this));

	if (homeItem == nullptr ||
		homeItem->getContentSize().width <= 0 ||
		homeItem->getContentSize().height <= 0)
	{
		CCLOG("Error to loading home Item!");
	}
	else
	{
		float x = origin.x + visibleSize.width - homeItem->getContentSize().width / 2;
		float y = origin.y + visibleSize.height * 2 / 3 - homeItem->getContentSize().height / 2;
		homeItem->setPosition(Vec2(x, y));
	}

	auto menu = Menu::create(homeItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);


	//close

	auto closeItem = MenuItemImage::create(
		"Scene/myClose1.png",
		"Scene/myClose2.png",
		CC_CALLBACK_1(EndingScene::menuCloseCallback, this));

	if (closeItem == nullptr ||
		closeItem->getContentSize().width <= 0 ||
		closeItem->getContentSize().height <= 0)
	{
		CCLOG("Error to loading close Item!");
	}
	else
	{
		float x = origin.x + visibleSize.width - closeItem->getContentSize().width / 2;
		float y = origin.y + visibleSize.height - closeItem->getContentSize().height / 2;
		closeItem->setPosition(Vec2(x, y));
	}

	auto closeMenu = Menu::create(closeItem, NULL);
	closeMenu->setPosition(Vec2::ZERO);
	this->addChild(closeMenu, 1);

	//=====================label£º"Win or Lose"============================
	// Todo:judge for win/lose
	auto label = Label::createWithTTF("Judge is not done", "fonts/Marker Felt.ttf", 40);

	if (_resultflag)
	{
		label = Label::createWithTTF("Congradulations", "fonts/Marker Felt.ttf", 40);
	}
	else if (!_resultflag)
	{
		label = Label::createWithTTF("Defeated", "fonts/Marker Felt.ttf", 40);
	}

	if (label == nullptr)
	{
		CCLOG("label loading error");
	}
	else
	{
		label->setPosition(Vec2(origin.x + visibleSize.width / 2,
			origin.y + visibleSize.height - label->getContentSize().height));

		this->addChild(label, 1);
	}

	//=====================±³¾°Í¼Æ¬============================
	auto bg = Sprite::create("Scene/room-back.png");

	if (bg == nullptr)
	{
		CCLOG("background picture loading error");
	}
	else
	{
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
		this->addChild(bg);
	}

	return true;
}


void EndingScene::menuCloseCallback(Ref* pSender)
{
	//Close the cocos2d-x game scene and quit the application
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif

	/*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

	//EventCustom customEndEvent("game_scene_close_event");
	//_eventDispatcher->dispatchEvent(&customEndEvent);
}


void EndingScene::menuHelloCallback(Ref *pSender)
{
	Director::getInstance()->pushScene(TransitionFade::create(1, Hello::createScene()));
}