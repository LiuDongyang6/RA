#include "SettingScene.h"
#include "GameAudio.h"
#include "Settings.h"

USING_NS_CC;

Scene* Setting::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Setting::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}


// on "init" you need to initialize your instance
bool Setting::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	
	// ----------------------------------- add background picture -----------------------------------
	Sprite *bg = Sprite::create("Scene/setting-back.png");
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
	this->addChild(bg,-1);

	// ----------------------------------- add a label shows "Red Alert" -----------------------------------
	
	// create and initialize a label
	auto label = Label::createWithTTF("Red Alert", "fonts/Marker Felt.ttf", 24);

	// position the label on the center of the screen
	label->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - label->getContentSize().height));

	// add the label as a child to this layer
	this->addChild(label, 1);

	// ----------------------------------- sound button -----------------------------------
	auto soundOnMenuItem = MenuItemImage::create(
		"Scene/on.png",
		"Scene/on.png");
	auto soundOffMenuItem = MenuItemImage::create(
		"Scene/off.png",
		"Scene/off.png");

	auto soundToggleMenuItem = MenuItemToggle::createWithCallback(CC_CALLBACK_1(Setting::menuSoundToggleCallback, this),
		soundOnMenuItem,
		soundOffMenuItem,
		NULL);
	soundToggleMenuItem->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 3));

	// -----------------------------------  music button -----------------------------------
	auto musicOnMenuItem = MenuItemImage::create(
		"Scene/on.png",
		"Scene/on.png");
	auto musicOffMenuItem = MenuItemImage::create(
		"Scene/off.png",
		"Scene/off.png");
	auto musicToggleMenuItem = MenuItemToggle::createWithCallback(CC_CALLBACK_1(Setting::menuMusicToggleCallback, this),
		musicOnMenuItem,
		musicOffMenuItem,
		NULL);
	musicToggleMenuItem->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));

	// ----------------------------------- lastpage button -----------------------------------
	auto okMenuItem = MenuItemImage::create(
		"Scene/ok-down1.png",
		"Scene/ok-up1.png",
		CC_CALLBACK_1(Setting::menuLastpageCallback, this));

	okMenuItem->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 4));

	// ----------------------------------- add all buttons above to menu -----------------------------------
	Menu* mn = Menu::create(soundToggleMenuItem, musicToggleMenuItem, okMenuItem, NULL);
	mn->setPosition(Vec2::ZERO);
	this->addChild(mn);

	return true;
}

void Setting::menuLastpageCallback(Ref* pSender)
{
	Director::getInstance()->popScene();
}

void Setting::menuSoundToggleCallback(Ref* pSender)
{

}


void Setting::menuMusicToggleCallback(Ref* pSender)
{

}

/////////////////////////////////////////////
cocos2d::Menu* Setting::createText()
{
	const auto buttons = Menu::create();

	const auto backButton = MenuItemLabel::create(
		Label::createWithTTF("OK", Setting::Font::Type::title, Setting::Font::Size::normal),
		CC_CALLBACK_1(Setting::menuOkCallback, this));

	const auto visibleSize = Director::getInstance()->getVisibleSize();
	const auto baseY = visibleSize.height * 0.85f;

	backButton->setPosition(visibleSize.width * 0.5f, visibleSize.height * 0.2f);
	static_cast<Label*>(backButton->getLabel())->enableGlow(Color4B(0, 0, 0, 255 * 0.6f));

	buttons->addChild(backButton, 1);

	buttons->setPosition(0, 0);

	return buttons;
}

void Setting::onEnter()
{
	Layer::onEnter();
	GameAudio::getInstance()->playBgm("Sound/WelcomeScene.mp3");
}

void Setting::menuOkCallback(cocos2d::Ref * pSender)
{
	GameAudio::getInstance()->playEffect("Sound/button.mp3");
	Director::getInstance()->popScene();
}

cocos2d::ui::CheckBox * Setting::createCheckBox(std::function<void(Ref*, ui::CheckBox::EventType)> callback)
{
	auto checkBox = ui::CheckBox::create("Scene/checkbox_normal.png", "Scene/checkbox_active.png");
	checkBox->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	checkBox->addEventListener(callback);
	return checkBox;
}

cocos2d::Label * Setting::createLabel(const char * text)
{
	auto label = Label::createWithTTF(text, Setting::Font::Type::title, Setting::Font::Size::normal);
	label->enableShadow(Color4B(0, 0, 0, 255 * 0.15f), Size(2, -2), 2);
	label->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
	return label;
}
