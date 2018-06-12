#pragma once
#include <WinSock2.h>
#include <process.h>
#include <string>
#include <fstream>
#include "../PlayScene.h"
#include <cocos2d.h>
#pragma comment(lib,"ws2+32.lib")
#include <queue>		// ������ݵĶ���
#include "ChatBox.h"	// ������

class GameScene;

struct packageInfo
{
	float Posx;
	float Posy;
	int still;
	int direct;
	int putButtle;
};

struct RAClientInformation			// Client �����Ϣ
{
	SOCKET ClientSock;			// �׽���
	sockaddr_in Client;			// Client IP port
	int ID;						// Server �����ID����
	DWORD RecvThreadID;			// Server ������յ��̺߳�
	DWORD SendThreadID;			// Server ���䷢�͵��̺߳�
	bool Active;
};

struct PlayerInfo
{
	std::string nickname;
	struct RAClientInformation clientInfo;
};

struct RoomInfo
{
	std::string name;		// ��������
	long id;				// �����
	std::vector<PlayerInfo> playerList;		// ����б�
	int curNum;				// ���䵱ǰ����
};


class GameClient
{
public:
	bool virtual init();
	void acceptProps();
	~GameClient();
	int clientProcessBefore(int flag, int which);
	int clientProcessRoomData(int which);

	bool gameThreadProcess(GameScene * gs);		// �����ͻ�����
	static DWORD WINAPI gameSendThread(LPVOID lpParam);		// �շ���Ϣ
	static DWORD WINAPI gameRecvThread(LPVOID lpParam);		// �շ���Ϣ

	static DWORD WINAPI consumer(LPVOID lpParam);		// ������

	void chat(int whichRoom);
	static DWORD WINAPI chatSendThread(LPVOID lpParam);		// �����ҷ�����Ϣ
	static DWORD WINAPI chatRecvThread(LPVOID lpParam);		// �����ҽ�����Ϣ

	// SOCKET ���
public:
	SOCKET ClientSocket;
	struct sockaddr_in ServerAddr;

	// �������ݴ���

private:
	static std::queue <packageInfo> recvQueue;
	HANDLE hMutex;
	GameScene * runningGameScene;
	char *prop;
};