#pragma once
#include <WinSock2.h>
#include <process.h>
#include <string>
#include <fstream>
#include "../PlayScene.h"
#include <cocos2d.h>
#pragma comment(lib,"ws2+32.lib")
#include <queue>		// 存放数据的队列
#include "ChatBox.h"	// 聊天室

class GameScene;

struct packageInfo
{
	float Posx;
	float Posy;
	int still;
	int direct;
	int putButtle;
};

struct RAClientInformation			// Client 相关信息
{
	SOCKET ClientSock;			// 套接字
	sockaddr_in Client;			// Client IP port
	int ID;						// Server 分配的ID号码
	DWORD RecvThreadID;			// Server 分配接收的线程号
	DWORD SendThreadID;			// Server 分配发送的线程号
	bool Active;
};

struct PlayerInfo
{
	std::string nickname;
	struct RAClientInformation clientInfo;
};

struct RoomInfo
{
	std::string name;		// 房间名称
	long id;				// 房间号
	std::vector<PlayerInfo> playerList;		// 玩家列表
	int curNum;				// 房间当前人数
};


class GameClient
{
public:
	bool virtual init();
	void acceptProps();
	~GameClient();
	int clientProcessBefore(int flag, int which);
	int clientProcessRoomData(int which);

	bool gameThreadProcess(GameScene * gs);		// 启动客户处理
	static DWORD WINAPI gameSendThread(LPVOID lpParam);		// 收发消息
	static DWORD WINAPI gameRecvThread(LPVOID lpParam);		// 收发消息

	static DWORD WINAPI consumer(LPVOID lpParam);		// 消费者

	void chat(int whichRoom);
	static DWORD WINAPI chatSendThread(LPVOID lpParam);		// 聊天室发送消息
	static DWORD WINAPI chatRecvThread(LPVOID lpParam);		// 聊天室接收消息

	// SOCKET 相关
public:
	SOCKET ClientSocket;
	struct sockaddr_in ServerAddr;

	// 网络数据处理

private:
	static std::queue <packageInfo> recvQueue;
	HANDLE hMutex;
	GameScene * runningGameScene;
	char *prop;
};