#ifndef __PLAY_SCENE_H__
#define __PLAY_SCENE_H__

#include"RedAlert.h"
#include "NetWork/Client.h"
#include "Data/ExternData.h"
#include "Data/LevelData.h"


USING_NS_CC;
using namespace cocostudio;
using namespace cocos2d::ui;


class PlayScene : public cocos2d::Scene
{
public:
	cocos2d::Sprite*           small_map;

	LevelData*                 _inputData;           ///选择人物时的数据
	static PlayScene*          _thisScene;           ///等于This指针
	std::vector<PlayerData>    _playerList;          ///所有远程玩家的信息


	cocos2d::TMXTiledMap*      _tileMap;

	Client*                    _client;              ///服务端指针
	std::string                _localPlayerName;     ///本地玩家的名字
	int                        _localPlayerID;

	std::queue<std::string>    _commands;             ///每次读取的命令

	std::queue<std::string>    msgs;

public:
    static cocos2d::Scene* createScene(LevelData &data, Client* client, std::string playerName);

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(PlayScene);

	void gameStart(bool topSide);
};

#endif // __PLAY_SCENE_H__