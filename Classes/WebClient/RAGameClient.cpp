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


/*
名称：线程处理
描述：主要负责两大线程的处理，一个是收发数据的线程，一个是消费者线程
		本函数受到WebGameScene的schedule制约，每一帧执行一次
*/
bool GameClient::gameThreadProcess(GameScene * gs)
{
	static bool flag = false;

	runningGameScene = gs;
	
	if (flag == false)
	{
		acceptProps();
		flag = true;
	}

	HANDLE sendThread, recvThread, consumerThread;
	sendThread = CreateThread(NULL, 0,
		static_cast<LPTHREAD_START_ROUTINE>(GameClient::gameSendThread),
		static_cast<LPVOID>(this), 0,
		NULL
	);
	recvThread = CreateThread(NULL, 0,
		static_cast<LPTHREAD_START_ROUTINE>(GameClient::gameSendThread),
		static_cast<LPVOID>(this), 0,
		NULL
	);
	consumerThread = CreateThread(NULL, 0,
		static_cast<LPTHREAD_START_ROUTINE>(GameClient::gameSendThread),
		static_cast<LPVOID>(this), 0,
		NULL
	);
	CloseHandle(sendThread);
	CloseHandle(recvThread);
	CloseHandle(consumerThread);

	return true;
}


/*
名称：游戏发送数据
描述：发送游戏数据的线程
*/
DWORD WINAPI GameClient::gameSendThread(LPVOID lpParam)
{
	GameClient *Client = static_cast<GameClient *>(lpParam);

	const int PACKAGE = 28;		// 每次发送包的长度
	char sendData[1024];
	packageInfo * theReserved = nullptr;
	packageInfo * theSend;
	static int times = 0;
	while (true)
	{
		fd_set wfd;
		FD_ZERO(&wfd);
		FD_SET(Client->ClientSocket, &wfd);
		int sel = select(0, NULL, &wfd, NULL, NULL);
		if (sel > 0)
		{
			if (FD_ISSET(Client->ClientSocket, &wfd))
			{
				theSend = new packageInfo[1];

				
			}
		}
	}
}


/*
名称：游戏接收数据
描述：接收游戏数据的线程
*/
DWORD WINAPI GameClient::gameRecvThread(LPVOID lpParam)
{
	GameClient *Client = static_cast<GameClient *>(lpParam);
	const int PACKAGE = 28;		// 每次发送包的长度
	char recvData[PACKAGE * 100];

	while (true)
	{
		fd_set rfd;
		FD_ZERO(&rfd);
		FD_SET(Client->ClientSocket, &rfd);
		int sel = select(0, &rfd, NULL, NULL, NULL);
		if (sel > 0)
		{
			if (FD_ISSET(Client->ClientSocket, &rfd))
			{
				// 接收数据
				int direct_2;
				int still_2;
				int put_bubble_2 = 0;
				ZeroMemory(recvData, sizeof(recvData));

				int temp = 0;
				
				// 接收

				int recvByte = 0;
				int ret;
				while (recvByte < PACKAGE)
				{
					ret = recv(Client->ClientSocket, recvData, PACKAGE * 100, 0);
					if (ret == SOCKET_ERROR)
						continue;
					recvByte += ret;
				}

				if (recvData[0] == '#')
				{
					win = 1;
					break;
				}
				int len = recvByte / PACKAGE;
				recvData[PACKAGE] = '\0';


				// producer
				for (int i = 0; i < len; i++)
				{
					packageInfo temp;
					sscanf(recvData + i * PACKAGE, "%f %f %d %d %d", &temp.Posx, &temp.Posy,
						&temp.still, &temp.direct, &temp.putButtle);

					g_lock.lock();
					recvQueue.push(temp);
					g_lock.unlock();
				}
			}
		}
	}
}


/*
名称：消费者
描述：开的新线程，专门用来pop队列，来达到控制角色的目的
*/
DWORD WINAPI GameClient::consumer(LPVOID lpParam)
{

}


/*
名称：接收道具
描述：专门用来接收道具分布信息
		作用单一，只执行一次
*/
void GameClient::acceptProps()
{

}

/*
名称：处理线程
描述：处理线程（在游戏开始前）
*/
int GameClient::clientProcessBefore(int flag, int which)
{
	static int curNum;

	int whichRoom = -1;
	if (flag = 1)
		whichRoom = which;

	char sendBuf[16];
	ZeroMemory(sendBuf, sizeof(sendBuf));
	sprintf(sendBuf, "%d %d", flag, whichRoom);
	int sec = send(ClientSocket, sendBuf, strlen(sendBuf) + sizeof(char), 0);

	char recvBuf[10000];
	int ret = recv(ClientSocket, recvBuf, sizeof(recvBuf), 0);
	if (ret > 0)
		sscanf(recvBuf, "%d %d %d %d", &curNum, &Rooms[0].curNum, &Rooms[1].curNum, &Rooms[2].curNum, &Rooms[3].curNum);

	return curNum;
}


/*
名称：线程处理房间相关
描述：在 CharacterSelect 中负责收发数据
*/
int GameClient::clientProcessRoomData(int which)
{
	// 发送	内容比较简单，只发送所在房间的号码
	char sendBuf[8];
	sprintf(sendBuf, "%d", which);
	send(ClientSocket, sendBuf, strlen(sendBuf) + sizeof(char), 0);

	// 接收	内容稍微复杂
	/*
	接收格式: int	#size	Rooms[which].playerlist		有几个玩家
	string #name	Rooms[which].playerlist.nickname	每个中间以|分隔
	*/

	char recvBuf[1024];
	int ret = recv(ClientSocket, recvBuf, sizeof(recvBuf), 0);

	int size;
	std::string str;
	char temp[1024];
	ZeroMemory(temp, sizeof(temp));

	if (ret > 0)
		sscanf(recvBuf, "%d %s", &size, temp);

	str = temp;

	// 接收数据的处理 需要把收到的数据本地化
	if (str.size() < 8)
		return 0;
	
	if (size == Rooms[which].playerList.size())	// 如果已经接收完毕，就不再处理
		return 0;

	for (int i = 0; i < size; i++)
	{
		static int pos = 0;
		std::string str1;
		while (pos < str.size() && str[pos] != '|')
		{
			str1 += str[pos];
			pos++;
		}
		pos++;
		struct PlayerInfo temp;
		temp.nickname = str1;
		if (temp.nickname.size() == 0)
			return 0;
		Rooms[which].playerList.push_back(temp);
	}
}

/*
名称：聊天室
描述：聊天数据的线程管理
*/
void GameClient::chat(int Room)
{
	whichRoom = Room;

	HANDLE sendThread, recvThread;
	sendThread = CreateThread(NULL, 0, chatSendThread, (LPVOID)this, 0, NULL);
	CloseHandle(sendThread);
	recvThread = CreateThread(NULL, 0, chatRecvThread, (LPVOID)this, 0, NULL);
	CloseHandle(recvThread);
}

/*
名称：聊天数据发送
描述：专门发送聊天数据的线程，采用 select 模式管理
*/
DWORD WINAPI GameClient::chatSendThread(LPVOID lpParam)		// 聊天室发送消息
{
	GameClient *Client = static_cast<GameClient *>(lpParam);

	char sendBuf[CHATPACKAGE];

	while (true)
	{
		fd_set write;
		FD_ZERO(&write);
		FD_SET(Client->ClientSocket, &write);
		int sel = select(0, NULL, &write, NULL, NULL);
		if (sel > 0)
		{
			if (FD_ISSET(Client->ClientSocket, &write))
			{

				ZeroMemory(sendBuf, sizeof(sendBuf));
				char tempMsg[1024];
				ZeroMemory(tempMsg, sizeof(tempMsg));


				if (!chatting->cur_msg.empty())
				{
					strcpy(tempMsg, chatting->cur_msg.c_str());
					tempMsg[chatting->cur_msg.size()] = '\0';
					sprintf(sendBuf, "%d %s", whichRoom, tempMsg);
				}
				else
					continue;

				send(Client->ClientSocket, sendBuf, strlen(sendBuf) + sizeof(char), 0);

				if (chatting->cur_msg == "#GO_TO_GAME_SCENE!")
				{
					break;
				}

			}

		}

	}

	return 0;
}


/*
名称：聊天数据接收
描述：专门接收聊天数据的线程，采用 select 模式管理
*/
DWORD WINAPI GameClient::chatRecvThread(LPVOID lpParam)  //聊天室接受消息
{
	GameClient *Client = static_cast<GameClient *>(lpParam);
	char recvBuf[CHATPACKAGE];
	while (true)
	{

		fd_set read;
		FD_ZERO(&read);
		FD_SET(Client->ClientSocket, &read);
		int sel = select(0, &read, NULL, NULL, NULL);
		if (sel > 0)
		{
			if (FD_ISSET(Client->ClientSocket, &read))
			{
				Msg.clear();
				ZeroMemory(recvBuf, sizeof(recvBuf));

				int ret = recv(Client->ClientSocket, recvBuf, CHATPACKAGE, 0);
				if (ret == SOCKET_ERROR)
					continue;

				if (recvBuf[0] == '@')
				{
					sscanf(recvBuf, "@ %d %d", &RoomPlayers, &whichPlayer);
					break;
				}

				Msg = recvBuf;


			}
		}
	}
	return 0;
}

/*
名称：析构函数
描述：释放套接字，清理内存
*/
GameClient::~GameClient()
{
	delete[]prop;
	closesocket(ClientSocket);

	WSACleanup();
}