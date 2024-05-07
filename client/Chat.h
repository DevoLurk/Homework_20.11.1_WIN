#ifndef CHAT
#define CHAT

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include<Windows.h>
#include<conio.h>


#include<iostream>
#include<string>
#include<chrono>
#include<thread>
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
	enum Screens
	{
		NOSCR,          // not set
		LOAD,           // scr_exit
		EXIT,           // scr_exit
		LOGIN,          // scr_login
		PUBLIC,         // scr_public
		WELLCOME,       // scr_welcome
		PRIVATE,        // scr_private
		PROFILE,        // scr_profile
		COMMANDS,       // scr_commands
		NEWACCOUNT,     // scr_newAccount
		MESSAG          // scr_message
	};

private:
	User current_user;
	Screens current_screen;
	Screens previos_screen;
	std::string salt{ "Y6sEgFOSx38RM8bHnSteSOsEjqZKZgDuLBY5AecmPFMT000bBlRBmxMCiYKN" }; // 30+ characters
	std::vector<std::string> Usernames;
	std::deque<std::string> public_msgArr;

	WSADATA wsaData;
	SOCKET clientSocket;
	sockaddr_in serverAddr;

	// screens
	void scr_load();
	void scr_exit();
	void scr_login();
	void scr_public();
	void scr_welcome();
	void scr_private();
	void scr_profile();
	void scr_commands();
	void scr_newAccount();
	void scr_message(Message& m);

	// common functions
	void clear_screen();
	void showHistory();
	void showMailbox();
	void showUsers();
	void clearLine();
	void cmdProcessing();
	void keyProcessing();
	void rememberMail(const std::string& str);
	void mySleep(int time = 120);
	void cmd_default(std::string& str);
	bool strCmp_read(std::string& str, int& num);
	bool strCmp_pm(std::string& str_cmd, std::string& str_msg);

	// server functions
	bool sv_setConnection();
	void sv_updatePrivate();
	void sv_addUser(std::string& login, Hash& hash);
	void sv_closeConnection();
	void sv_updatePublic();
	void sv_sendMessage(std::string& reciever, std::string& message);
	bool sv_askspace(); // return users_array.size() == users_array.max_size()
	bool sv_userExists(std::string& login); // true if exists
	bool sv_checkPass(std::string& login, Hash& hash);
	void sv_sendtask(const std::string& username, const std::string& task, const SOCKET& connection_descriptor);
	void sv_rememberMail(const std::string& str);
	void sv_updateUsernames();

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
	Chat(Chat& other) = delete;
	Chat& operator=(Chat& other) = delete;

	void start();
};

#endif // CHAT