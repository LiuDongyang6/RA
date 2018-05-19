#ifndef __PLAY_SCENE_H__
#define __PLAY_SCENE_H__

#include "cocos2d.h"
#include"cocostudio\CocoStudio.h"
#include"ui\CocosGUI.h"

#include"Classes\RAObject\RABuilding.h"
#include"Classes\RAObject\RAObject.h"
#include"Classes\RAObject\RAPlayer.h"

USING_NS_CC;
using namespace cocostudio;
using namespace cocos2d::ui;


class PlayScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(PlayScene);

	//void onClick(Ref*, TouchEventType type);

	Button* ps;
};

#endif // __PLAY_SCENE_H__
