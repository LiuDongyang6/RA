#pragma once
#include <WinSock2.h>
#include <process.h>				// C �߳�
#include <string>
#include <random>
#include <array>
#pragma comment(lib,"ws2_32.lib")

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
	int order;
};

struct RoomInfo
{
	std::string name;			// ��������
	long id;					// �����
	std::vector<PlayerInfo> playerList;		// ����б�
	int curNum;					// ���䵱ǰ����
};

class GameServer
{
protected:
	enum
	{
		MAX_NUM = 4			// �����������
	};
public:
	GameServer();
	~GameServer();

	int ProcessGameServer();	// �̴߳���

	static int SendMessageToOneClient(int ID, const std::string str);	// ��ĳһ�� Client ������Ϣ

	int CheckSocket();			// ��⵱ǰ���õ�ID��

	static void CleanSocket(int ID);	// ���ID�ŵ��׽���

	static void SendMessageToAllClient(const std::string str, int ID = -1);	// ������ Client ������Ϣ

	// �߳�
public:
	static DWORD WINAPI GameThread(void *data);		// �����߳�
	static DWORD WINAPI sendRoomInfo(void *data);

private:
	SOCKET ListenSocket;		// �ȴ��������ݵ� socket, ��Ϊ��*Server
	static RAClientInformation AcceptSocket[MAX_NUM];	// Client �������Ϣ����Ϊ��*Client
	sockaddr_in Server;			// �󶨵�ַ

	// ���������ݵĴ���
public:
	void GenerateProps();
	static void sendProps(int ID);
	static int prog_map[15][15];
	static std::vector<RoomInfo> Rooms;
	static std::vector<PlayerInfo> allPlayerInfo;
}��