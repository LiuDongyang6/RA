#ifndef ChatBox_hpp
#define ChatBox_hpp

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class ChatBox : public Layer 
{
public:
	friend class GameClient;
	virtual bool init();
	mutable ui::TextField* writeText;
	// ���� chatLines ����
	void alignChat();
	CREATE_FUNC(ChatBox);
	void onKeyReleased(EventKeyboard::KeyCode keycode, cocos2d::Event*event);
	void update(float dt);

	mutable Vector<cocos2d::MenuItem*> chatlines;		// �Ի���Ŀ
	void addTextField();
	void addChatLine(const std::string s);

public:
	std::string cur_msg;
};

#endif /* ChatBox_hpp */