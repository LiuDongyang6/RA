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
    static cocos2d::Scene* createScene(LevelData &data, Client* client, std::string playerName);

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(PlayScene);

};

#endif // __PLAY_SCENE_H__
