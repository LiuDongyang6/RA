#include "PlayScene.h"
#include "SimpleAudioEngine.h"
#include"ResourceUI/ResourceUI.h"
#include "TiledMap/RAlittle_map.h"
#include "Scene/RoomScene.h"

USING_NS_CC;
using namespace cocostudio;
using namespace cocos2d::ui;

static std::string splayerName;
static Client* clients;
PlayScene* PlayScene::_thisScene;

static LevelData* ptr = NULL;

Scene* PlayScene::createScene(LevelData &data, Client* client, std::string playerName)
{
	data.retain();
	
	clients = client;
	splayerName = playerName;

	ptr = &data;
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

	_thisScene = this;
	_client = clients;
	_inputData = ptr;
	_localPlayerName = splayerName;
	_playerList = ptr->player_list;

	for (auto& playerData : _playerList)
	{
		if (_localPlayerName == playerData.player_name)
		{
			_localPlayerID = playerData.player_id;
		}
	}
	RedAlert::getInstance()->initAll(RoomScene::map_num);

	auto fight_layer = RAMap::getMap();
	auto ui_layer = LayerColor::create(Color4B(0, 128, 128, 100), 400, 900);
	fight_layer->setPosition(0, 0);
	ui_layer->setPosition(1200, 0);
	addChild(fight_layer, 1, 1);
	addChild(ui_layer, 2, 2);

	RAResourceUI::init();
	ui_layer->addChild(RAResourceUI::ResourceUI);

	auto base = RABase::create(Point(3000, 3000));

	littleMap::init(RAMap::map_num);
	this->addChild(littleMap::getLittleMap(), 20);

	auto powerstation = RAPowerStation::create(Point(2800, 2800));

	RedAlert::HostileObjectAppear(3, Point(2500, 2500), 30);
	RedAlert::HostileObjectAppear(0, Point(1500, 1500), 40);
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

void PlayScene::gameStart()
{
	RedAlert::getInstance()->initAll(RoomScene::map_num);

	auto fight_layer = RAMap::getMap();
	auto ui_layer = LayerColor::create(Color4B(0, 128, 128, 100), 400, 900);
	fight_layer->setPosition(0, 0);
	ui_layer->setPosition(1200, 0);
	addChild(fight_layer, 1, 1);
	addChild(ui_layer, 2, 2);

	RAResourceUI::init();
	ui_layer->addChild(RAResourceUI::ResourceUI);

	auto base = RABase::create(Point(3000, 3000));

	littleMap::init(RoomScene::map_num);
	this->addChild(littleMap::getLittleMap(), 20);

	auto powerstation = RAPowerStation::create(Point(2800, 2800));

	RedAlert::HostileObjectAppear(3, Point(2500, 2500), 30);
	RedAlert::HostileObjectAppear(0, Point(1500, 1500), 40);

}

// SendMessage:		_gamescene->_client->SendMessage(MOVE_UNIT, getMoveMessage(soldier, soldier->_route.front()));
// executeOrder:	((temp = _gamescene->_client->executeOrder()) != "no")