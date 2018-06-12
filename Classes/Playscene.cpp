#include "PlayScene.h"
#include "SimpleAudioEngine.h"


USING_NS_CC;
using namespace cocostudio;
using namespace cocos2d::ui;

Scene* PlayScene::createScene()
{
    return PlayScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool PlayScene::init()
{

    if ( !Scene::init() )
    {
        return false;
    }

	RedAlert::getInstance()->initAll();

	auto fight_layer = RAMap::getMap();
	auto ui_layer = LayerColor::create(Color4B(0, 128, 128, 100), 400, 900);
	fight_layer->setPosition(0,0);
	ui_layer->setPosition(1200,0);
	addChild(fight_layer, 1, 1);
	addChild(ui_layer, 2, 2);

	auto base = RABase::create(Point(3000, 3000));


	auto powerstation = RAPowerStation::create(Point(2800,2800));

	RedAlert::HostileObjectAppear(3, Point(2500, 2500));
	RedAlert::HostileObjectAppear(0, Point(1500, 1500));



	return true;
}


void PlayScene::menuCloseCallback(Ref* pSender)
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

