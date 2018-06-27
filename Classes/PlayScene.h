#ifndef __PLAY_SCENE_H__
#define __PLAY_SCENE_H__

#include"RedAlert.h"
#include "NetWork/Client.h"
#include "Data/ExternData.h"
#include "Data/LevelData.h"


USING_NS_CC;
using namespace cocostudio;
using namespace cocos2d::ui;

struct instruction
{
	instruction(int num,std::string m):count(num),msg(m){}
	int count;
	std::string msg;
};
struct ins_compare
{
	bool operator()(instruction lhs, instruction rhs) {
		return lhs.count > rhs.count;
	}
};
class PlayScene : public cocos2d::Scene
{
public:
	cocos2d::Sprite*           small_map;

	LevelData*                 _inputData;           ///ѡ������ʱ������
	static PlayScene*          _thisScene;           ///����Thisָ��
	std::vector<PlayerData>    _playerList;          ///����Զ����ҵ���Ϣ


	cocos2d::TMXTiledMap*      _tileMap;

	Client*                    _client;              ///�����ָ��
	std::string                _localPlayerName;     ///������ҵ�����
	int                        _localPlayerID;

	std::queue<std::string>    _commands;             ///ÿ�ζ�ȡ������

	static std::queue<std::string>msgs;
	static int                 msg_count;                 ///��Ϣ����
	static int                 received_count;            ///��Ϣ����
	static std::priority_queue<instruction,std::vector<instruction>,ins_compare> ins;          ///������Ϣ����
	static std::vector<chat_message> msg_to_send_twice;
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