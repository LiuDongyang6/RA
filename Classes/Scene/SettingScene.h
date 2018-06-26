#ifndef __SETTING_SCENE_H__
#define __SETTING_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class Setting : public cocos2d::Layer
{
private:
	void menuOkCallback(cocos2d::Ref *pSender);

	cocos2d::ui::CheckBox * createCheckBox(std::function<void(cocos2d::Ref*, cocos2d::ui::CheckBox::EventType)> callback);
	cocos2d::Label * createLabel(const char * text);

	/**
	@brief 用内容为“OK”的label创建一个menu，用于从设置场景返回主场景
	@return cocos2d::Menu*
	*/
	cocos2d::Menu* createText();

	void onEnter();
////////////////////////////////
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	// a selector callback
	void menuSoundToggleCallback(cocos2d::Ref* pSender);
	void menuMusicToggleCallback(cocos2d::Ref* pSender);
	void menuLastpageCallback(cocos2d::Ref* pSender);


	// implement the "static create()" method manually
	CREATE_FUNC(Setting);
};

#endif // __SETTING_SCENE_H__
