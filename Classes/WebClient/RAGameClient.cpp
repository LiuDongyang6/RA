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

	ServerAddr.sin_family = PF_INET;
	ServerAddr.sin_port = htons(PORTS);
	ServerAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	ClientSocket = socket(PF_INET, SOCK_STREAM, 0);

	if (ClientSocket == INVALID_SOCKET)
	{
		WSACleanup();	// �ͷ��׽�����Դ
		return false;
	}
	unsigned long ul = 1;
	int ret = ioctlsocket(ClientSocket, FIONBIO, (unsigned long*)&ul);	// ���óɹ�
	if (ret == SOCKET_ERROR)		// ����ʧ��
	{
		outfile << "���÷�����ʧ��\n";
		closesocket(ClientSocket);	// �ر��׽���
		WSACleanup*();	// �ͷ��׽�����Դ
		return false;
	}
	outfile << "���÷������ɹ�\n";
	
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
			outfile << "����ʧ��\n";
			closesocket(ClientSocket);	// �ر��׽���
			WSACleanup();	// �ͷ��׽�����Դ
			return false;
		}
	}

	outfile << "�ͻ������ӳɹ�!\n";
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
���ƣ��̴߳���
��������Ҫ���������̵߳Ĵ���һ�����շ����ݵ��̣߳�һ�����������߳�
		�������ܵ�WebGameScene��schedule��Լ��ÿһִ֡��һ��
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
���ƣ���Ϸ��������
������������Ϸ���ݵ��߳�
*/
DWORD WINAPI GameClient::gameSendThread(LPVOID lpParam)
{
	GameClient *Client = static_cast<GameClient *>(lpParam);

	const int PACKAGE = 28;		// ÿ�η��Ͱ��ĳ���
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
���ƣ���Ϸ��������
������������Ϸ���ݵ��߳�
*/
DWORD WINAPI GameClient::gameRecvThread(LPVOID lpParam)
{
	GameClient *Client = static_cast<GameClient *>(lpParam);
	const int PACKAGE = 28;		// ÿ�η��Ͱ��ĳ���
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
				// ��������
				int direct_2;
				int still_2;
				int put_bubble_2 = 0;
				ZeroMemory(recvData, sizeof(recvData));

				int temp = 0;
				
				// ����

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
���ƣ�������
�������������̣߳�ר������pop���У����ﵽ���ƽ�ɫ��Ŀ��
*/
DWORD WINAPI GameClient::consumer(LPVOID lpParam)
{

}


/*
���ƣ����յ���
������ר���������յ��߷ֲ���Ϣ
		���õ�һ��ִֻ��һ��
*/
void GameClient::acceptProps()
{

}

/*
���ƣ������߳�
�����������̣߳�����Ϸ��ʼǰ��
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
���ƣ��̴߳��������
�������� CharacterSelect �и����շ�����
*/
int GameClient::clientProcessRoomData(int which)
{
	// ����	���ݱȽϼ򵥣�ֻ�������ڷ���ĺ���
	char sendBuf[8];
	sprintf(sendBuf, "%d", which);
	send(ClientSocket, sendBuf, strlen(sendBuf) + sizeof(char), 0);

	// ����	������΢����
	/*
	���ո�ʽ: int	#size	Rooms[which].playerlist		�м������
	string #name	Rooms[which].playerlist.nickname	ÿ���м���|�ָ�
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

	// �������ݵĴ��� ��Ҫ���յ������ݱ��ػ�
	if (str.size() < 8)
		return 0;
	
	if (size == Rooms[which].playerList.size())	// ����Ѿ�������ϣ��Ͳ��ٴ���
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
���ƣ�������
�������������ݵ��̹߳���
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
���ƣ��������ݷ���
������ר�ŷ����������ݵ��̣߳����� select ģʽ����
*/
DWORD WINAPI GameClient::chatSendThread(LPVOID lpParam)		// �����ҷ�����Ϣ
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
���ƣ��������ݽ���
������ר�Ž����������ݵ��̣߳����� select ģʽ����
*/
DWORD WINAPI GameClient::chatRecvThread(LPVOID lpParam)  //�����ҽ�����Ϣ
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
���ƣ���������
�������ͷ��׽��֣������ڴ�
*/
GameClient::~GameClient()
{
	delete[]prop;
	closesocket(ClientSocket);

	WSACleanup();
}