#include <stdio.h>
#include "NetMenu.h"
#include "SearchScene.h"
#include "ui/CocosGUI.h"
#include "StartScene.h"

USING_NS_CC;

using namespace ui;

Scene* NetMenu::createScene()
{
    auto scene = Scene::create();
    auto layer = NetMenu::create();
    scene->addChild(layer);
    
    return scene;
}

bool NetMenu::init()
{
    if (!Layer::init())
    {
        return false;
    }
    
    system("ping -c 1 255.255.255.255");
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    auto bg = Sprite::create("Scene/room-back.png");
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

    _selectLevelIndex = 0;

//=============================map select window===========================

    auto pageView = PageView::create();
    pageView->setContentSize(Size(680.0f, 680.0f));
    
    pageView->setPosition(Vec2((visibleSize.width - pageView->getContentSize().width) / 2.0f,
                               (visibleSize.height - pageView->getContentSize().height)*0.6));
    

    for (int i = 0; i < 2; ++i)
    {
        Layout* layout = Layout::create();
        layout->setContentSize(Size(680.0f, 680.0f));
        
        ImageView* imageView = ImageView::create(StringUtils::format("map%d.png", i + 1));
        imageView->setContentSize(Size(680.0f, 680.0f));
        imageView->setPosition(Vec2(layout->getContentSize().width / 2.0f, layout->getContentSize().height / 2.0f));
        
        layout->addChild(imageView);

        pageView->insertPage(layout, i);
    }
    
    pageView->addEventListener([=](Ref* pSender, PageView::EventType type)
                               {
                                   switch (type)
                                   {
                                       case PageView::EventType::TURNING:
                                       {
                                           PageView* pageView = dynamic_cast<PageView*>(pSender);
                                           _selectLevelIndex = pageView->getCurPageIndex();
                                       }
                                           break;
                                       default:break;
                                   }
                               });
    
    auto thisObject = this;

//=============================remote network button===========================

    auto remote_button = Button::create("Scene/RemoteFight.png");
    auto local_button = Button::create("Scene/LocalFight.png");
    remote_button->setScale(1.0);
    
    remote_button->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 5 * 3));

    remote_button->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {

            Color4B black = Color4B(0, 0, 0, 100);
            auto role_layer = LayerColor::create(black, 680, 680);
            role_layer->setPosition(Vec2(origin.x, origin.y));
            
            thisObject->addChild(role_layer, 2);

            remote_button->setVisible(false);
            local_button->setVisible(false);
            
            auto role_selector = Sprite::create("BlankBoard.png");
            role_selector->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
            role_layer->addChild(role_selector);
            
            auto back_button = Button::create("Scene/myClose1.png", "Scene/myClose2.png");
            auto create_button = Button::create("Scene/button_normal.png", "Scene/button_selected.png");
            auto join_button = Button::create("Scene/button_normal.png", "Scene/button_selected.png");
            
            role_selector->addChild(create_button);
            role_selector->addChild(join_button);
            role_layer->addChild(back_button);
            
            create_button->setTitleText("create room");
            create_button->setTitleFontSize(20);
            join_button->setTitleText("join room");
            join_button->setTitleFontSize(20);
            
            
            float widthMargin = visibleSize.width/2 - role_selector->getContentSize().width/2;
            float heightMargin = visibleSize.height/2 - role_selector->getContentSize().height/2;
            
            create_button->setPosition(Vec2(role_selector->getContentSize().width/2, role_selector->getContentSize().height / 3 * 2));
            
            join_button->setPosition(Vec2(role_selector->getContentSize().width/2, role_selector->getContentSize().height / 3));
            
            back_button->setPosition(Vec2(origin.x + visibleSize.width - widthMargin, origin.y + role_selector->getContentSize().height * 7.5 / 9 + heightMargin));
            
            back_button->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
                if (type == Widget::TouchEventType::ENDED) {

                    thisObject->removeChild(role_layer);
                    remote_button->setVisible(true);
                    local_button->setVisible(true);
                    
                }
            });

            join_button->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
                if (type == Widget::TouchEventType::ENDED) {

                    auto client = Client::create(1);
                    client->_with_server = false;
                    auto transition = TransitionSlideInL::create(0.5, SearchScene::createScene(client, _playerName));
                    
                    Director::getInstance()->pushScene(transition);
                }
            });
            
            create_button->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
                if (type == Widget::TouchEventType::ENDED) {

                    auto server = LocalServer::create();
                    this->addChild(server);
                    auto client = Client::create(1);
                    this->addChild(client);
                    client->_filter_mode = true;
                    client->sensitive_word = _playerName;
                    client->_with_server = true;
                    
                    auto transition = TransitionSlideInL::create(0.5, RoomScene::createScene(client, 2, _playerName));
                    Director::getInstance()->pushScene(transition);
                    
                }
            });

        }
    });
    remote_button->setVisible(false);
    this->addChild(remote_button, 1);
    
//=============================rlocal network button===========================
    local_button->setScale(1.0);
    
    local_button->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 5 * 2));
    
    local_button->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {

            Color4B black = Color4B(0, 0, 0, 100);
            auto role_layer = LayerColor::create(black, 680, 680);
            role_layer->setPosition(Vec2(origin.x, origin.y));
            
            thisObject->addChild(role_layer, 2);
            
            remote_button->setVisible(false);
            local_button->setVisible(false);
            
            auto role_selector = Sprite::create("BlankBoard.png");
            role_selector->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
            role_layer->addChild(role_selector);
            
            auto back_button = Button::create("Scene/myClose1.png", "Scene/myClose2.png");
            auto create_button = Button::create("Scene/button_normal.png","Scene/button_selected.png");
            auto join_button = Button::create("Scene/button_normal.png", "Scene/button_selected.png");
            
            role_selector->addChild(create_button);
            role_selector->addChild(join_button);
            role_layer->addChild(back_button);
            
            create_button->setTitleText("create room");
            create_button->setTitleFontSize(20);
            join_button->setTitleText("join room");
            join_button->setTitleFontSize(20);

            float widthMargin = visibleSize.width/2 - role_selector->getContentSize().width/2;
            float heightMargin = visibleSize.height/2 - role_selector->getContentSize().height/2;
            
            create_button->setPosition(Vec2(role_selector->getContentSize().width/2, role_selector->getContentSize().height / 3 * 2));
            
            join_button->setPosition(Vec2(role_selector->getContentSize().width/2, role_selector->getContentSize().height / 3));
            
            back_button->setPosition(Vec2(origin.x + visibleSize.width - widthMargin, origin.y + role_selector->getContentSize().height * 7.5 / 9 + heightMargin));
            
            back_button->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
                if (type == Widget::TouchEventType::ENDED) {

                    thisObject->removeChild(role_layer);
                    remote_button->setVisible(true);
                    local_button->setVisible(true);
                    
                }
            });
            
            join_button->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
                if (type == Widget::TouchEventType::ENDED) {
                    auto client = Client::create(2);
                    client->_with_server = false;
                    auto transition = TransitionSlideInL::create(0.5, SearchScene::createScene(client, _playerName));
                    
                    Director::getInstance()->pushScene(transition);
                }
            });
            
            create_button->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
                if (type == Widget::TouchEventType::ENDED) {

                    auto server = LocalServer::create();
                    this->addChild(server);
                    auto client = Client::create(2);
                    this->addChild(client);
                    client->_filter_mode = true;
                    client->sensitive_word = _playerName;
                    client->_with_server = true;
                    
                    auto transition = TransitionSlideInL::create(0.5, RoomScene::createScene(client, 2, _playerName));
                    Director::getInstance()->pushScene(transition);
                    
                }
            });

        }
    });
    local_button->setVisible(false);
    this->addChild(local_button, 1);

	//return button
	auto return_button = Button::create("Scene/backNormal.png", "Scene/backSelected.png");
	return_button->setScale(1.0);

	return_button->setPosition(Vec2(origin.x + visibleSize.width / 4, origin.y + visibleSize.height*0.2));
    
    
    return_button->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            
            auto transition = TransitionSlideInR::create(0.5, Start::createScene());
            
            Director::getInstance()->replaceScene(transition);
        }
    });
    return_button->setVisible(false);
    this->addChild(return_button, 1);
    

    
	auto label = Label::createWithTTF("Input Your Name: ", "fonts/Marker Felt.ttf", 24);
	// position the label on the center of the screen
	label->setPosition(origin / 2 + visibleSize / 2);

	// add the label as a child to this layer
	this->addChild(label, 1);

    cocos2d::ui::TextField* textField = cocos2d::ui::TextField::create("input words here", "Arial", 30);
    textField->setMaxLengthEnabled(true);
    textField->setMaxLength(20);
    textField->setPosition(Point(visibleSize.width/2,visibleSize.height/3 + 50));
    textField->addEventListener(CC_CALLBACK_2(NetMenu::textFieldEvent, this));
    this->addChild(textField,3);
    
    auto inputBox = Sprite::create("InputBox.png");
    inputBox->setPosition(Point(visibleSize.width/2,visibleSize.height/3 + 50));
    inputBox->setScale(0.6, 0.6);
    this->addChild(inputBox,1);
    
    auto pleaseStartButton = Button::create("Scene/button_normal.png", "Scene/button_selected.png");
    pleaseStartButton->setScale(1.0);
    pleaseStartButton->setPosition(Vec2(origin.x + visibleSize.width /2, origin.y + visibleSize.height*0.5 - 230));
    
    pleaseStartButton->setTitleText("input your name");
    pleaseStartButton->setTitleFontSize(20);
    pleaseStartButton->setVisible(false);
    _pleaseStartButton = pleaseStartButton;
    
    this->addChild(pleaseStartButton,3);
    
    auto nameStartButton = Button::create("Scene/button_normal.png", "Scene/button_selected.png");
    nameStartButton->setScale(1.0);
    nameStartButton->setPosition(Vec2(origin.x + visibleSize.width /2, origin.y + visibleSize.height*0.5 - 230));
    
    nameStartButton->setTitleText("Start");
    nameStartButton->setTitleFontSize(20);

    auto name = &_playerName;
    
    nameStartButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            if(_playerName.size() < 4)
                _nameStartButton->setTitleText("At least 4 character");
            else{
            
                pleaseStartButton->setVisible(false);
                _pleaseStartButton->setVisible(false);
                textField->setVisible(false);
                inputBox->setVisible(false);
                nameStartButton->setVisible(false);
				label->setVisible(false);
            
                local_button->setVisible(true);
                remote_button->setVisible(true);
                return_button->setVisible(true);
            }
        }
    });

    nameStartButton->setVisible(false);
    _nameStartButton = nameStartButton;

    this->addChild(nameStartButton,3);

    return true;
}


void NetMenu::menuCloseCallback(Ref* pSender)
{

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
    MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.", "Alert");
    return;
#endif
    Director::getInstance()->end();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}


void NetMenu::textFieldEvent(Ref *pSender, cocos2d::ui::TextField::EventType type)
{
    switch (type)
    {
        case cocos2d::ui::TextField::EventType::ATTACH_WITH_IME:
        {
            cocos2d::ui::TextField* textField = dynamic_cast<cocos2d::ui::TextField*>(pSender);
            Size screenSize = CCDirector::getInstance()->getWinSize();
            
            _pleaseStartButton->setVisible(true);
        }
            break;
            
        case cocos2d::ui::TextField::EventType::DETACH_WITH_IME:
        {
            cocos2d::ui::TextField* textField = dynamic_cast<cocos2d::ui::TextField*>(pSender);

            _playerName = textField->getString();
        }
            break;
            
        case cocos2d::ui::TextField::EventType::INSERT_TEXT:
        {
            TextField* textField = dynamic_cast<cocos2d::ui::TextField*>(pSender);
            _pleaseStartButton->setVisible(false);
            _playerName = textField->getString();
            _nameStartButton->setVisible(true);
            
        }
            break;
            
        case cocos2d::ui::TextField::EventType::DELETE_BACKWARD:
        {
            TextField* textField = dynamic_cast<cocos2d::ui::TextField*>(pSender);

            _playerName = textField->getString();
        }
            break;
            
        default:
            break;
    }
}

void NetMenu::editBoxEditingDidBegin(EditBox* editBox){
    
}

void NetMenu::editBoxEditingDidEnd(EditBox* editBox){
    
}

void NetMenu::editBoxTextChanged(EditBox* editBox, const std::string& text){
    
}

void NetMenu::editBoxReturn(EditBox* editBox){
    
}



