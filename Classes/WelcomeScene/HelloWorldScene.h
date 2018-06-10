#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#include "cocostudio\CocoStudio.h"
#include "ui\CocosGUI.h"

using namespace cocos2d::ui;
using namespace cocostudio;

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

	void menuItemStartCallback(cocos2d::Ref* pSender);
	void menuItemSettingCallback(cocos2d::Ref* pSender);
	void menuItemHelpCallback(cocos2d::Ref* pSender);

	void onClick(Ref*, TouchEventType type);
	ImageView* m_xiaoruoImg;

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
};

#endif // __HELLOWORLD_SCENE_H__
