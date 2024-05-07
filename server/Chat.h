#ifndef CHAT
#define CHAT

#include<WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include<Windows.h>
#include<conio.h>

#include<fstream>
#include<iostream>
#include<string>
#include<chrono>
#include<thread>
#include<unordered_map>
#include<vector>
#include<deque>

#include"User.h"
#include"myCrypt.h"

#ifdef max // std::numeric_limits<std::streamsize>::max()
#undef max
#endif

#define PORT 50306

class Chat
{
private:
	int current_user;
	std::string save_path{ "ChatData.data" };
	std::vector<User> users_array;
	std::deque<std::string> public_msgArr;
	std::unordered_map<std::string, Hash> pass_table;

	WSADATA wsaData;
	SOCKET serverSocket, clientSocket;
	sockaddr_in serverAddr, clientAddr;
	int clientAddrSize = sizeof(clientAddr);
	// screens
	void scr_load();
	void scr_exit();
	void scr_work();

	// common commands
	void clear_screen();
	void clearLine();
	void myCinClear();
	void rememberMail(std::string str);
	void mySleep(int time = 120);
	bool findUser(const std::string& name, int& pos);

	// save to file functions
	bool save();
	bool load();
	void saveStr(std::string& str, std::ofstream& fout);
	void loadStr(std::string& str, std::ifstream& fin);
	void saveHash(Hash& hash, std::ofstream& fout);
	void loadHash(Hash& hash, std::ifstream& fin);

	// server functions
	bool sv_accept();
	void sv_gettask(const SOCKET& connection_descriptor, std::string& name, std::string& task);

	void sv_sendHash(const Hash& hash, const SOCKET& connection_descriptor);
	void sv_sendBool(const bool& flag, const SOCKET& connection_descriptor);
	void sv_sendInt(const int& integer, const SOCKET& connection_descriptor);
	void sv_sendStr(const std::string& str, const SOCKET& connection_descriptor);

	void sv_readHash(Hash& hash, const SOCKET& connection_descriptor);
	void sv_readBool(bool& flag, const SOCKET& connection_descriptor);
	void sv_readInt(int& integer, const SOCKET& connection_descriptor);
	void sv_readStr(std::string& str, const SOCKET& connection_descriptor);

public:
	Chat();
	Chat(std::string path);
	Chat(Chat& other) = delete;
	Chat& operator=(Chat& other) = delete;
	void sv_create();

	void start();
};

#endif // CHAT