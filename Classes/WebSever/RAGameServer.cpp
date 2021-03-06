#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <fstream>
#include "RAGameServer.h"

using std::cout;
using std::string;

volatile static int count = 0;
static int playerNum = 0;		// 玩家总数
std::vector<PlayerInfo> GameServer::allPlayerInfo;
int GameServer::prog_map[15][15];
std::vector<RoomInfo> GameServer::Rooms;

RAClientInformation GameServer::AcceptSocket[GameServer::MAX_NUM];
/*
名称：构造函数
描述：用于 Socket 初始化
*/

GameServer::GameServer()
{
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)	// 判断一下有没有这个版本
	{
		cout << "无法加载套接字协议栈.\n";
		fflush(stdout);
		return;
	}

	// 设置监听套接字
	ListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// IPv4 STREAM TCP

	if (ListenSocket == INVALID_SOCKET)
	{
		cout << "套接字初始化错误，错误号: " << WSAGetLastError() << "\n";
		fflush(stdout);
		return;
	}

	int port = 1236;

	// 设置服务器的信息
	Server.sin_family = PF_INET;
	Server.sin_port = htons(port);
	Server.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	int errornum;

	// 绑定
	bind(ListenSocket, (LPSOCKADDR)&Server, sizeof(Server));
	while ((errornum = WSAGetLastError()) == 10048)
	{
		port++;
		Server.sin_port = htons(port);
		int retval = bind(ListenSocket, (LPSOCKADDR)&Server, sizeof(Server));
		cout << "绑定错误，错误号10048\n";
		fflush(stdout);
	}

	cout << "服务器的IP为:" << Server.sin_addr.S_un.S_addr
		<< " 端口号为: " << port << "\n";
	fflush(stdout);

	// 监听		有 listen 就有 accept
	if (listen(ListenSocket, 10) == SOCKET_ERROR)
	{
		cout << "监听出错，错误号: " << WSAGetLastError() << "\n";
		fflush(stdout);
		return;
	}

	// 将所有信息初始化
	for (int i = 0; i < MAX_NUM; i++)
		AcceptSocket[i].ClientSock = INVALID_SOCKET;

	cout << "网络初始化成功\n";
	fflush(stdout);

	// 其他初始化操作
	playerNum = 0;
	Rooms.resize(4);
	for (int i = 0; i < 4; i++)
	{
		Rooms[i].name = "Room" + std::to_string(i);
		Rooms[i].id = 100 + i;
		Rooms[i].curNum = 0;
	}
	GenerateProps();

	return;
}

/*
名称：析构函数
描述：用于资源释放
*/
GameServer::~GameServer()
{
	if (ListenSocket != INVALID_SOCKET)
		closesocket(ListenSocket);
	cout << "服务器关闭\n";
	fflush(stdout);
	WSACleanup();

	return;
}

/*
名称：检测ID
描述：用于检测当前没有用的ID号
*/
int GameServer::CheckSocket()
{
	for (int i = 0; i < MAX_NUM; i++)
	{
		if (AcceptSocket[i].ClientSock == INVALID_SOCKET)
			return i;
	}
	return -1;
}

/*
名称：线程处理
描述：为每一个新玩家创建一个接受线程
如果人数达到上限，就拒绝
*/
int GameServer::ProcessGameServer()
{
	while (true)
	{
		int index = CheckSocket();
		sockaddr_in Client;

		int ClntLen = sizeof(Client);

		if (index != -1)	// 玩家未满
		{
			cout << "等待 Client 连接...\n";
			fflush(stdout);
			AcceptSocket[index].ClientSock = accept(
				ListenSocket,
				(struct sockaddr*)&AcceptSocket[index].Client,
				&ClntLen);

			AcceptSocket[index].ID = index;
			// 传输ID，其他的不用
			std::string msg = std::to_string(AcceptSocket[index].ID);
			SendMessageToOneClient(AcceptSocket[index].ID, msg);
			cout << "#ID :" << msg << "\n";

			cout << "连接成功\n";
			fflush(stdout);
			playerNum++;
			// 至此 Client 与 Server 连接成功

			cout << "新玩家加入，IP地址为: " << inet_ntoa(AcceptSocket[index].Client.sin_addr)
				<< "  端口号为: " << ntohs(AcceptSocket[index].Client.sin_port) << "\n";
			fflush(stdout);

			// 创建接收者线程
			struct PlayerInfo temp;
			temp.clientInfo = { NULL, AcceptSocket[index].Client, AcceptSocket[index].ID,0,0,0 };
			temp.nickname = std::to_string(AcceptSocket[index].Client.sin_addr.S_un.S_addr) + "_" +
				std::to_string(AcceptSocket[index].Client.sin_port) + "_" + std::to_string(AcceptSocket[index].ID);
			allPlayerInfo.push_back(temp);


			CreateThread(NULL, 0,
				static_cast<LPTHREAD_START_ROUTINE>(GameServer::sendRoomInfo),	// 线程点函数
				(LPVOID)&AcceptSocket[index], 0,								// 参数
				nullptr);
		}
		else	// 玩家已满
		{

			SOCKET Accept = accept(ListenSocket,
				(struct sockaddr*)&Client, &ClntLen);

			if (Accept == INVALID_SOCKET)
			{
				cout << "连接错误\n";
				fflush(stdout);
				break;
			}

			cout << "非法请求的玩家的IP地址为: " << inet_ntoa(Client.sin_addr)
				<< " 端口号为: " << ntohs(Client.sin_port) << "\n";
			fflush(stdout);
			send(Accept, "当前用户已满", strlen("当前用户已满") + sizeof(char), 0);

			closesocket(Accept);

			cout << "非法连接玩家已断开\n";
			fflush(stdout);
			break;
		}
	}
	return 0;
}

/*
名称：接受线程
描述：Select模式
首先判断该线程是否可读，如果可读就读取其上的信息
*/

DWORD WINAPI GameServer::GameThread(void *data)	// 传进来具体哪个AcceptSocket[xx]的地址
{
	RAClientInformation *GameSocket = static_cast<RAClientInformation *>(data);
	sendProps(GameSocket->ID);

	while (true)
	{
		// 接受命令

		char recvBuf[28000];

		fd_set Read;	// 基于 select 模式对IO进行管理
		FD_ZERO(&Read);	// 初始化为0
		FD_SET(GameSocket->ClientSock, &Read);	// 将ClientSock加入队列

												// we only care reads
		int sel = select(0, &Read, NULL, NULL, NULL);


		if (FD_ISSET(GameSocket->ClientSock, &Read))
		{
			// 接受客户端的数据
			int result = recv(GameSocket->ClientSock, recvBuf, sizeof(recvBuf), 0);
			cout << "recv 返回值: " << result << "\n";
			fflush(stdout);
			if (result > 0 && result < 28000)
			{
				recvBuf[result] = "\0";
				cout << "玩家" << GameSocket->ID << "发送了消息: "
					<< recvBuf << "\n";
				fflush(stdout);
			}

			if (result == SOCKET_ERROR)
			{
				CleanSocket(GameSocket->ID);
				break;
			}
		}

		// 发送命令
		char sendBuf[28000];
		fd_set write;	// 基于 select 模式对ID进行管理
		FD_ZERO(&write);	// 初始化为0
		FD_SET(GameSocket->ClientSock, &write);	// 将 ClientSock 加入队列
												// we only care read
		select(0, NULL, &write, NULL, NULL);

		if (FD_ISSET(GameSocket->ClientSock, &write))
		{
			// 接受客户端的数据
			strcpy(sendBuf, recvBuf);

			SendMessageToAllClient(sendBuf, GameSocket->ID);
		}
	}
	return 1;
}

/*
名称：单发送
描述：给一个 Client 发送信息
*/
int GameServer::SendMessageToOneClient(int ID, const string str)
{
	if (ID < 0)
		return 0;

	int oneSend = send(AcceptSocket[ID].ClientSock,
		str.c_str(), str.length() + sizeof(char), 0);
	cout << "向玩家" << ID << "发送了" << oneSend << "个bits的消息\n";
	fflush(stdout);
	if (oneSend == SOCKET_ERROR)
	{
		cout << "向玩家" << ID << "发送消息失败\n";
		fflush(stdout);
		return 0;
	}

	return 1;
}

/*
名称：全发送
描述：给所有 Client 发送信息
*/
void GameServer::SendMessageToAllClient(const string str, int ID)
{

	bool flag = true;
	for (int i = 0; i < MAX_NUM; i++)
	{
		if (ID != i)
		{
			if (AcceptSocket[i].ClientSock != INVALID_SOCKET &&
				!SendMessageToOneClient(i, str))
				flag = false;
		}
	}
	if (flag)
		cout << "服务器向全体(除了玩家" << ID << ")发送了消息: "
		<< str << "\n";
	else
		cout << "发送存在问题\n";
	fflush(stdout);
}

DWORD WINAPI GameServer::sendRoomInfo(void *data)
{
	RAClientInformation *GameSocket = static_cast<RAClientInformation*>(data);

	// 进入RoomChooseScene
	while (true)
	{
		int flag = 0;
		int whichRoom = -1;
		char buf[16];
		ZeroMemory(buf, sizeof(buf));
		int result = recv(GameSocket->ClientSock, buf, sizeof(buf), 0);
		cout << "#buf: " << buf << "\n";
		sscanf(buf, "%d %d", &flag, &whichRoom);
		if (whichRoom != -1)
		{
			Rooms[whichRoom].curNum++;

			Rooms[whichRoom].playerList.push_back(allPlayerInfo.at(GameSocket->ID));
			if (Rooms[whichRoom].playerList.size() == 1)
				Rooms[whichRoom].playerList.back().order = -1;
			Rooms[whichRoom].playerList.back().order++;
		}

		std::string myRoom = std::to_string(playerNum);
		for (int i = 0; i < MAX_NUM; i++)
			myRoom += " " + std::to_string(Rooms[i].curNum);

		cout << "myRoom : " << myRoom << "\n";
		cout << "myRoom size: " << myRoom.size() << "\n";
		if (SendMessageToOneClient(GameSocket->ID, myRoom) && !flag)
			continue;
		else
			break;
	}



	char sendBuf[1024];
	char recvBuf[1024];
	char Msg[1024];
	int ret, left, idx = 0;
	while (true)
	{
		ZeroMemory(sendBuf, sizeof(sendBuf));
		ZeroMemory(recvBuf, sizeof(recvBuf));
		ZeroMemory(Msg, sizeof(Msg));
		cout << "等待接收:\n";
		ret = recv(GameSocket->ClientSock, recvBuf, sizeof(recvBuf), 0);
		cout << "ret: " << ret << "\n";
		if (ret == 0)
			return 1;
		if (ret == SOCKET_ERROR)
		{
			cout << "接收失败.\n";
			break;
		}
		if (ret < 1024)
			recvBuf[ret] = "\0";
		else
			cout << "buf is full\n";
		cout << "收到消息: " << recvBuf << "\n";
		int whichRoom;
		sscanf(recvBuf, "%d %s", &whichRoom, Msg);
		string msg = Msg;

		if (msg == "#GO_TO_GAME_SCENE!")
		{
			sprintf(sendBuf, "@ %d %d", Rooms[whichRoom].playerList.size(), GameSocket->ID);
			cout << "####房间中有" << Rooms[whichRoom].playerList.size() << "人\n";
			SendMessageToOneClient(Rooms[whichRoom].playerList.at(GameSocket->ID).clientInfo.ID, sendBuf);
			break;
		}
		else
			sprintf(sendBuf, "[%s said]:%s", inet_ntoa(GameSocket->Client.sin_addr), Msg);

		for (int i = 0; i < Rooms[whichRoom].playerList.size(); i++)
		{
			if (i == GameSocket->ID)
				continue;
			SendMessageToOneClient(Rooms[whichRoom].playerList.at(i).clientInfo.ID, msg);
		}
	}
	cout << "已离开 CharacterSelect, count++\n";
	count++;

	CreateThread(NULL, 0,
		static_cast<LPTHREAD_START_ROUTINE>(GameServer::GameThread),	// 线程点函数
		(LPVOID)data, 0,												// 参数
		&GameSocket->RecvThreadID										// 系统分配的ID
	);


	return 0;
}

/*
名称：清理Socket
描述：清理退出游戏的线程
*/
void GameServer::CleanSocket(int ID)
{
	if (ID < 8)
		return;

	char send[20];
	sprintf(send, "#DD i%d", ID);
	SendMessageToAllClient(send, ID);


	cout << " 玩家" << ID << "退出游戏\n";
	fflush(stdout);

	AcceptSocket[ID].Active = false;
	closesocket(AcceptSocket[ID].ClientSock);
	AcceptSocket[ID].ClientSock = INVALID_SOCKET;

	cout << "正在关闭其接收线程: " << AcceptSocket[ID].RecvThreadID << "\n";
	fflush(stdout);
	ExitThread(AcceptSocket[ID].RecvThreadID);


	cout << "关闭成功!\n";
	fflush(stdout);
}


void GameServer::GenerateProps()
{
	static std::random_device rd;
	static std::uniform_int_distribution<int> dist(1, 6);

	for (int i = 0; i < 15; i++)
		for (int j = 0; j < 15; j++)
			prog_map[i][j] = dist(rd);

	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
			cout << prog_map[i][j] = " ";
		cout << "\n";
	}
	cout << "\n";
}


void GameServer::sendProps(int ID)
{
	const int MAX = 2 * (sizeof(int) * 15 * 15 + 10 * sizeof(int));
	char buffer[MAX];
	ZeroMemory(buffer, sizeof(buffer));
	for (int i = 0; i < 15; i++)
		for (int j = 0; j < 15; j++)
		{
			sprintf(buffer + 2 * (i * 15 + j), "%d", prog_map[i][j]);
		}
	string str(buffer);


	fflush(stdout);
	SendMessageToOneClient(ID, str);
}