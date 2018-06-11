#pragma once
#include <WinSock2.h>
#include <process.h>				// C 线程
#include <string>
#include <random>
#include <array>
#pragma comment(lib,"ws2_32.lib")

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
	int order;
};

struct RoomInfo
{
	std::string name;			// 房间名称
	long id;					// 房间号
	std::vector<PlayerInfo> playerList;		// 玩家列表
	int curNum;					// 房间当前人数
};

class GameServer
{
protected:
	enum
	{
		MAX_NUM = 4			// 最大上限人数
	};
public:
	GameServer();
	~GameServer();

	int ProcessGameServer();	// 线程处理

	static int SendMessageToOneClient(int ID, const std::string str);	// 向某一个 Client 发送信息

	int CheckSocket();			// 检测当前可用的ID号

	static void CleanSocket(int ID);	// 清空ID号的套接字

	static void SendMessageToAllClient(const std::string str, int ID = -1);	// 向所有 Client 发送信息

	// 线程
public:
	static DWORD WINAPI GameThread(void *data);		// 接收线程
	static DWORD WINAPI sendRoomInfo(void *data);

private:
	SOCKET ListenSocket;		// 等待接收数据的 socket, 此为真*Server
	static RAClientInformation AcceptSocket[MAX_NUM];	// Client 的相关信息，此为真*Client
	sockaddr_in Server;			// 绑定地址

	// 对网络数据的处理
public:
	void GenerateProps();
	static void sendProps(int ID);
	static int prog_map[15][15];
	static std::vector<RoomInfo> Rooms;
	static std::vector<PlayerInfo> allPlayerInfo;
}；