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
// using ……

using std::queue;
using std::fstream;
using std::string;
using std::endl;

#define PORTS 1236			// 设置端口号，与 Server 一致

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
名称：初始化
描述：主要负责连接到服务端
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

	ServerAddr.sin_family = PF_INET;
	ServerAddr.sin_port = htons(PORTS);
	ServerAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	ClientSocket = socket(PF_INET, SOCK_STREAM, 0);

	if (ClientSocket == INVALID_SOCKET)
	{
		WSACleanup();	// 释放套接字资源
		return false;
	}
	unsigned long ul = 1;
	int ret = ioctlsocket(ClientSocket, FIONBIO, (unsigned long*)&ul);	// 设置成功
	if (ret == SOCKET_ERROR)		// 设置失败
	{
		outfile << "设置非阻塞失败\n";
		closesocket(ClientSocket);	// 关闭套接字
		WSACleanup*();	// 释放套接字资源
		return false;
	}
	outfile << "设置非阻塞成功\n";
	
	if (connect(ClientSocket, (LPSOCKADDR)&ServerAddr,
		sizeof(ServerAddr)) < 0)
	{
		fd_set wfd;
		struct timeval tm;

		FD_ZERO(&wfd);
		FD_SET(ClientSocket, &wfd);
		tm.tv_sec = 0.1;
		tm.tv_usec = 0;
		int sel = select(ClientSocket, NULL, &wfd, NULL, &tm);
		if (sel <= 0)
		{
			outfile << "连接失败\n";
			closesocket(ClientSocket);	// 关闭套接字
			WSACleanup();	// 释放套接字资源
			return false;
		}
	}

	outfile << "客户端连接成功!\n";
	outfile.close();
	Rooms.resize(4);
	for (int i = 0; i < 4; i++)
	{
		Rooms[i].name = "Room" + std::to_string(i);
		Rooms[i].id = 10 + i;
		Rooms[i].curNum = 0;
	}
	char tempBuf[8];
	int retc = recv(ClientSocket, tempBuf, sizeof(tempBuf), 0);
	int id;
	sscanf(tempBuf, "%d", &id);
	myPlayerInfo.clientInfo = { NULL, ServerAddr, id, 0, 0, 0 };
	myPlayerInfo.nickname = std::to_string(ServerAddr.sin_addr.S_un.S_addr) +
		std::to_string(ServerAddr.sin_port) + "_" + std::to_string(id);


	return true;
}