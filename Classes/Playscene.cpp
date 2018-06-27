#include "PlayScene.h"
#include "SimpleAudioEngine.h"
#include"ResourceUI/ResourceUI.h"
#include "TiledMap/RAlittle_map.h"
#include"Roshambo\RARoshambo.h"
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
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("roshambo/roshambo.plist");
	RARoshambo::startCompete();
	CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("roshambobgm.mp3");
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

void PlayScene::gameStart(bool topSide)
{
	SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("roshambo / roshambo.plist");
	Director::getInstance()->setAnimationInterval(1.0f / 60);
	RedAlert::getInstance()->initAll(RoomScene::map_num);

	CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("rabgm.mp3",1);
	auto background = Sprite::create("rabg.png");
	auto fight_layer = RAMap::getMap();
	auto ui_layer = Sprite::create("ui_layer.png");
	background->setContentSize(Size(2000, 900));
	background->setPosition(Vec2(800,450));
	fight_layer->setPosition(0, 0);
	ui_layer->setContentSize(Size(500.0, 1000.0));
	ui_layer->setPosition(1200, -50);
	ui_layer->setAnchorPoint(Vec2(0.1,0));
	addChild(background, 0, 0);
	addChild(fight_layer, 1, 1);
	addChild(ui_layer, 2, 2);

	RAResourceUI::init();
	addChild(RAResourceUI::ResourceUI,3);
	littleMap::init(1);
	this->addChild(littleMap::getLittleMap(), 20);

	auto sch = [&](float dt) {
		std::queue<std::string> msgs;
		PlayScene::_thisScene->_client->RAGetMessage(msgs);
		while (!msgs.empty())
		{
			RedAlert::catcher(msgs.front());
			msgs.pop();
		}
	};
	_thisScene->schedule(sch, std::string("RAING"));

	if (topSide)
	{
		RAPlayer::setEdge(1);
		auto base = RABase::create(Point(1200, 2500));
		base->setCount(RAPlayer::getCounter());
		RAPlayer::master_table_.insert({ base->getCount(),base });
		PlayScene::_thisScene->_client->sendMessage(base->birthMessage());
	}
	else
	{
		RAPlayer::setEdge(2);
		auto base = RADefendingBase::create(Point(1200, 1000));
		base->setCount(RAPlayer::getCounter());
		RAPlayer::master_table_.insert({ base->getCount(),base });
		PlayScene::_thisScene->_client->sendMessage(base->birthMessage());
	}
}

// SendMessage:		_gamescene->_client->SendMessage(MOVE_UNIT, getMoveMessage(soldier, soldier->_route.front()));
// executeOrder:	((temp = _gamescene->_client->executeOrder()) != "no")