#define _CRT_SECURE_NO_WARNINGS
#include "RAGameClient.h"
#include <string>
#include <WinSock2.h>
#include <fstream>
#include "../PlayScene.h"
#include 
#include 
#include <random>
#include <cstring>
#include <mutex>
#include "ui/CocosGUI.h"

using namespace ui;

#define CHATPACKAGE 1024
USING_NS_CC;
// using ����

using std::queue;
using std::fstream;
using std::string;
using std::endl;

#define PORTS 1236			// ���ö˿ںţ��� Server һ��

extern std::vector<RoomInfo> Rooms;
std::queue<packageInfo> GameClient::recvQueue;
struct PlayerInfo myPlayerInfo;
bool getMsg = false;
std::string Msg;
extern ChatBox *chatting;
static int whichRoom;
int RoomPlayers;
int whichPlayers;
extern int win;

std::mutex g_lock;

static inline GameScene* getGameScene()
{
	auto scene = Director::getInstance()->getRunningScene();
	return dynamic_cast<GameScene*>(scene->getChildByTag(10));
}

/*
���ƣ���ʼ��
��������Ҫ�������ӵ������
*/
bool GameClient::init()
{
	std::fstream outfile;
	outfile.open("e:\\log2.txt", std::ios::app);

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return false;
	}

	ServerAddr.sin_family
}