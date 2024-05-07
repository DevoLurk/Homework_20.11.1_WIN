#include "Chat.h"

Chat::Chat()
{
    current_screen = NOSCR;
    previos_screen = NOSCR;
}

void Chat::start()
{
    if (current_screen == NOSCR)
        scr_load();

    char ch = ' ';
    std::string str;

    while (true)
    {
        switch (ch)
        {
        case 'y':
            mySleep();
            scr_login();
            break;
        case 'n':
            mySleep();
            scr_newAccount();
            break;
        default:
            mySleep();
            scr_welcome();
            std::getline(std::cin, str);
            ch = str[0];
            break;
        }
    }
}

void Chat::showMailbox()
{
    Mailbox* box = current_user.getMailboxPtr();

    if (!box->getCapity())
    {
        printf("\n   You have no mails\n\n");
        return;
    }

    printf("\n");

    std::string str_out;
    std::string name_out;

    for (auto i{ 0 }; i < box->getCapity(); ++i)
    {
        str_out.assign(box->getMailsArray()[i].getMsg(), 0, 17);    // shortening messages
        str_out += "...";

        if (box->getMailsArray()[i].getAuthor().length() > 11)      // shortening long names
        {
            name_out.append(box->getMailsArray()[i].getAuthor(), 0, 9);
            name_out += "..";
        }
        else
            name_out = box->getMailsArray()[i].getAuthor();

        if (box->getMailsArray()[i].getFlag())  // Unreaded will print grey. Readed will be yellow
        {
            printf("  %3d.", i + 1);
            printf(" %11s: ", name_out.c_str());
            printf("\033[90m");
            printf("%s", str_out.c_str());
            printf("\033[0m");
        }
        else
        {
            printf("  %3d.", i + 1);
            printf(" %11s: ", name_out.c_str());
            printf("\033[33m");
            printf("%s", str_out.c_str());
            printf("\033[0m");
        }
        printf("\n");
    }
    printf("\n");
}

void Chat::keyProcessing()
{
    while (!_kbhit()) // wait for key press
        mySleep(20);

    if (GetKeyState(VK_ESCAPE) & 0x8000)
    {
        mySleep();
        scr_exit();
    }

    if (GetKeyState(VK_TAB) & 0x8000)
    {
        mySleep();
        start();
    }

    if ((GetKeyState(0x50) & 0x8000) && (current_screen != PUBLIC))
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> start1 = std::chrono::high_resolution_clock::now();
        std::chrono::time_point<std::chrono::high_resolution_clock> final1 = start1;
        int duration_miliseconds = 0;

        while (GetKeyState(0x50) & 0x8000)
        {
            final1 = std::chrono::high_resolution_clock::now();
            duration_miliseconds = std::chrono::duration<double, std::milli>(final1 - start1).count();

            if (duration_miliseconds >= 500)
            {
                printf("Switching screen");

                std::chrono::time_point<std::chrono::high_resolution_clock> start2 = std::chrono::high_resolution_clock::now();
                std::chrono::time_point<std::chrono::high_resolution_clock> final2 = start2;
                duration_miliseconds = 0;

                while (duration_miliseconds < 4000) // switch after realize key/four secons
                {
                    final2 = std::chrono::high_resolution_clock::now();
                    duration_miliseconds = std::chrono::duration<double, std::milli>(final2 - start2).count();

                    if (GetKeyState(0x50) & 0x8000)
                        keybd_event(0x50, 0, KEYEVENTF_KEYUP, 0);

                    mySleep(200);

                    if (!(GetKeyState(0x50) & 0x8000))
                        break;
                }

                FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

                if (previos_screen == LOGIN)
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }

                mySleep();
                scr_public();
            }
        }
    }
}

void Chat::cmdProcessing()
{
    std::string str;

    if (current_screen == COMMANDS)
    {
        while (true)
        {
            printf("\n\t");

            keyProcessing();
            std::getline(std::cin, str);
            cmd_default(str);

            clearLine();
        }
    }

    if (current_screen == PROFILE)
    {
        Message* msgArr;
        int pos;

        while (true)
        {
            printf("\n\t");

            keyProcessing();
            std::getline(std::cin, str);
            cmd_default(str);

            if (strCmp_read(str, pos))
            {
                if ((pos >= 1) && (pos <= current_user.getMessageCnt())) // if message position relevant
                {
                    --pos; // we print from 1 so user enters nums from 1 (not from 0)
                    msgArr = current_user.getMailboxPtr()->getMailsArray();

                    scr_message(msgArr[pos]);
                }
            }
            clearLine();
        }
    }

    if (current_screen == PRIVATE)
    {
        while (GetKeyState(0x50) & 0x8000) { std::cout << '1' << std::endl; }
        std::flush(std::cout);

        std::string str_msg;

        while (true)
        {
            printf("\n\t");

            keyProcessing();
            std::getline(std::cin, str);
            cmd_default(str);

            if (strCmp_pm(str, str_msg)) // str = login, str_msg = message
            {
                if (sv_userExists(str))
                {
                    sv_sendMessage(str, str_msg);

                    clearLine();
                    printf("\033[36m     Message sended to user\033[0m %s\n", str.c_str());
                    mySleep(1000);
                }
                else
                {
                    clearLine();
                    printf("\033[36m     Cant find user\033[0m %s\n", str.c_str());
                    mySleep(1000);
                }
            }
            clearLine();
        }
    }

    if (current_screen == MESSAG)
    {
        while (true)
        {
            printf("\n\t");

            keyProcessing();
            std::getline(std::cin, str);
            cmd_default(str);

            if (str == "\\back") { mySleep(); scr_profile(); }

            clearLine();
        }
    }

    if (current_screen == PUBLIC)
    {
        std::string name_out;
        std::string msg_to_save;

        if (current_user.getName().length() > 11)      // shortening of long names
        {
            name_out.append(current_user.getName(), 0, 9);
            name_out += "..";
        }
        else
            name_out = current_user.getName();

        while (true)
        {
            printf("%11s : ", name_out.c_str());

            keyProcessing();
            std::getline(std::cin, str);
            cmd_default(str);

            clearLine();

            printf("%11s : ", name_out.c_str());
            printf("%s\n", str.c_str());

            msg_to_save = name_out;
            msg_to_save += " : ";
            msg_to_save += str;

            rememberMail(msg_to_save);
        }
    }
}

void Chat::cmd_default(std::string& str)
{
    if ((str == "\\help") && (current_screen != COMMANDS)) { mySleep(); scr_commands(); }

    if ((str == "\\private") && (current_screen != PRIVATE)) { mySleep(); scr_private(); }

    if ((str == "\\public") && (current_screen != PUBLIC)) { mySleep(); scr_public(); }

    if ((str == "\\profile") && (current_screen != PROFILE)) { mySleep(); scr_profile(); }

    if (str == "\\logout") { mySleep(); start(); }

    if (str == "\\close") { mySleep(); scr_exit(); }
}

void Chat::clearLine()
{
    printf("\033[A");       // line up
    printf("\33[2K\r");     // clear line + move curor
}

bool Chat::strCmp_read(std::string& str, int& num) // find commad and num in string
{
    std::string s_cmd, s_num;

    if (str.length() > 9)
        return false;

    s_cmd.assign(str, 0, 6);

    if (s_cmd == "\\read ")
    {
        int numsize = str.length() - s_cmd.length();
        num = 0;

        s_num.assign(str, s_cmd.length(), numsize);

        for (auto i{ 0 }; i < numsize; i++)
        {
            if ((s_num[i] >= '0') && (s_num[i] <= '9'))
            {
                num = num * 10 + (s_num[i] - '0');
            }
            else
                return false;
        }
        return true;
    }
    return false;
}

bool Chat::strCmp_pm(std::string& str_cmd, std::string& str_msg) // for private messages
{
    if (str_cmd[0] == '\\')
    {
        int pos = str_cmd.find(' ', 2);

        if (pos == -1)            // str do not contain ' ' 
            return false;

        str_msg = str_cmd;
        str_cmd.erase(pos, str_cmd.length() - pos);
        str_msg.erase(0, str_cmd.length() + 1);
        str_cmd.erase(0, 1);

        return true;
    }
    return false;
}

void Chat::mySleep(int time)
{
    std::chrono::milliseconds timespan(time);
    std::this_thread::sleep_for(timespan);
}

void Chat::showUsers()
{
    printf("\n\t");

    for (auto i{ 0 }; i < Usernames.size(); i++)
    {
        if ((i % 8) == 0)
            printf("\n\t");

        printf("\033[32m");
        printf("%s, ", Usernames[i].c_str());

    }
    printf("\b\b.\033[0m\n\n");
}

void Chat::clear_screen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void Chat::showHistory()
{
    if (public_msgArr.empty())
        return;

    for (auto i{ 0 }; i < public_msgArr.size(); i++)
        printf("\t%s\n", public_msgArr[i].c_str());
}

void Chat::rememberMail(const std::string& str)
{
    sv_rememberMail(str);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// SCREENS

void Chat::scr_newAccount()
{
    previos_screen = current_screen;
    current_screen = NEWACCOUNT;

    if (sv_askspace())
    {
        clear_screen();
        printf("\n\n\n");
        printf("\t\t\033[36m      Max users count in chat reached\033[0m");

        mySleep();
        scr_exit();
    }

    std::string login;
    std::string pass;

    clear_screen();
    printf("\n\n\n");
    printf("\t\t\033[36m      Creating new account\033[0m\n\n");
    printf("\t\t\033[36m    LOGIN: \033[0m");
    std::cin >> login;

    if (sv_userExists(login))
    {
        clear_screen();
        printf("\n\n\n");
        printf("\t\t\033[36m User\033[0m %s\033[36m already exists\033[0m", login.c_str());
        mySleep(900);
        printf("\n\t\033[36m     Please try again \033[0m");
        mySleep(1200);

        scr_newAccount();
    }

    printf("\t\t\033[36m PASSWORD: \033[0m");
    std::cin >> pass;

    add_salt(pass, salt);
    Hash hash = sha1((char*)pass.c_str(), pass.size());

    sv_addUser(login, hash);
    User u(login);
    current_user = u;

    mySleep();
    scr_commands();
}

void Chat::scr_welcome()
{
    previos_screen = current_screen;
    current_screen = WELLCOME;

    clear_screen();
    printf("\n\n\n");
    printf("\t\t\033[36m          Wellcome to the chat!\033[0m\n\n");
    printf("\t\t\033[36m     do you already have an account?\033[0m\n\n");
    printf("\t\t              type \033[32my\033[0m or \033[31mn\033[0m\n\n");
    printf("\t\t              ");
}

void Chat::scr_login()
{
    previos_screen = current_screen;
    current_screen = LOGIN;

    std::string login;
    std::string pass;

    clear_screen();
    printf("\n\n\n");
    printf("\t\t\033[36m    LOGIN: \033[0m");
    std::cin >> login;

    if (sv_userExists(login))
    {
        current_user = login;
        sv_updateUsernames();
        printf("\t\t\033[36m PASSWORD: \033[0m");
        std::cin >> pass;
        add_salt(pass, salt);

        Hash hash = sha1((char*)pass.c_str(), pass.size());

        if (sv_checkPass(login, hash))
        {
            mySleep();
            scr_commands();
        }
        else
        {
            clear_screen();
            printf("\n\n\n");
            printf("\t\033[36m  Wrong password\033[0m\n");
            mySleep(900);
            printf("\t\033[36m  Please try again\033[0m\n");
            mySleep(1200);

            scr_login();
        }
    }
    else
    {
        clear_screen();
        printf("\n\n\n");
        printf("\t\033[36m  Cant find user:    \033[0m %s\n", login.c_str());
        mySleep(900);
        printf("\t\033[36m  Please try again\033[0m\n");
        mySleep(1500);

        scr_login();
    }
}

void Chat::scr_commands()
{
    previos_screen = current_screen;
    current_screen = COMMANDS;

    clear_screen();
    printf("\n\n");
    printf("\t\t\033[36m     Commands list\033[0m\n\n");
    printf("\t\033[93m\\private\033[0m - send private messages\n\n");
    printf("\t\033[93m\\public\033[0m  - public chat\n\n");
    printf("\t\033[93m\\profile\033[0m - go to profile + check mails\n\n");
    printf("\t\033[93m\\help\033[0m    - see commands list\n\n");
    printf("\t\033[93m\\close\033[0m   - close programm\n\n");
    printf("\t\033[93m\\logout\033[0m  - go to the start screen\n\n");
    printf("\t\033[93m\\read N\033[0m  - read message number N (profile screen)\n\n");
    printf("\t\033[93m\\back\033[0m    - return to profile (message reading screen)\n\n");
    printf("\t\033[93m\\NAME MESSAGE\033[0m  - send message to user (private screen) \n\n");

    printf("\t\t\033[36m     KEYS list\033[0m\n\n");
    printf("\t\033[93mESC\033[0m - close programm\n\n");
    printf("\t\033[93mTAB\033[0m - go to the start screen\n");

    cmdProcessing();
}

void Chat::scr_exit()
{
    previos_screen = current_screen;
    current_screen = EXIT;

    clear_screen();
    printf("\n\n\n");
    printf("\t\t\033[36m     Saving data");

    for (auto i{ 0 }; i < 3; ++i)
    {
        mySleep();
        printf(".");
    }

    printf("\n\n\n\n\n\n");
    sv_closeConnection();
    exit(0);
}

void Chat::scr_profile()
{
    previos_screen = current_screen;
    current_screen = PROFILE;
    Mailbox* box = current_user.getMailboxPtr();

    sv_updatePrivate();
    clear_screen();
    printf("\n\n");
    printf("\t\033[36m Hello \033[0m %s\n", current_user.getName().c_str());
    printf("\t\033[36m You have\033[0m %d\033[36m new mails \033[0m\n\n", box->getUnreadCnt());

    showMailbox();
    cmdProcessing();
}

void Chat::scr_load()
{
    previos_screen = current_screen;
    current_screen = LOAD;

    clear_screen();
    printf("\n\n\n");
    printf("\033[36m");
    printf("\t\t      Setting coonection");

    for (auto i{ 0 }; i < 3; ++i)
    {
        mySleep();
        printf(".");
    }

    if (sv_setConnection())
    {
        mySleep(200);
        printf("\n\t\t       success\n");
        mySleep(500);
        clearLine();
    }
    else
    {
        mySleep(200);
        printf("\n\t\t        falue\n");
        mySleep(500);
        clearLine();
        printf("\033[0m");
        exit(1);
    }
    printf("\033[0m");
}

void Chat::scr_public()
{
    previos_screen = current_screen;
    current_screen = PUBLIC;

    sv_updatePublic();
    clear_screen();

    printf("\n\t\033[36m PUBLIC CHAT BEGINNING\033[0m\n\n");
    if (!public_msgArr.empty())
        printf("\t\033[36m last %d messages loaded\033[0m\n\n", public_msgArr.size());

    showHistory();
    cmdProcessing();
}

void Chat::scr_private()
{
    previos_screen = current_screen;
    current_screen = PRIVATE;

    sv_updatePrivate();
    clear_screen();

    printf("\n\t\033[36m       Currently %d users in chat\033[0m\n\n", Usernames.size());

    showUsers();
    cmdProcessing();
}

void Chat::scr_message(Message& m)
{
    previos_screen = current_screen;
    current_screen = MESSAG;

    clear_screen();
    printf("\n\tMessage from \033[32m %s \033[0m", m.getAuthor().c_str());
    printf("\n\n   %s\n\n", m.getMsg().c_str());
    m.setReaded();

    cmdProcessing();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// SERVER

void Chat::sv_updatePrivate()
{
    bool flag;
    std::string author, message;
    std::string task = "updatePrivate";
    int msg_cnt = current_user.getMessageCnt();

    sv_sendtask(current_user.getName(), task, clientSocket);
    sv_sendInt(msg_cnt, clientSocket);
    sv_readBool(flag, clientSocket);

    // flag = true if server will send messages to client
    if (flag)
    {
        // msg_cnt = number messages to recieve from server
        sv_readInt(msg_cnt, clientSocket);

        for (auto i{ 0 }; i < msg_cnt; i++)
        {
            sv_readStr(author, clientSocket);
            sv_readStr(message, clientSocket);
            sv_readBool(flag, clientSocket);

            Message msg_scan(message, author);
            if (flag)
                msg_scan.setReaded();

            current_user.receiveMessage(msg_scan);
        }
    }
}

void Chat::sv_updatePublic()
{
    std::string task = "updatePublic";
    std::string msg;
    int msg_cnt;

    sv_sendtask(current_user.getName(), task, clientSocket);
    sv_readInt(msg_cnt, clientSocket);

    for (auto i{ 0 }; i < msg_cnt; i++)
    {
        sv_readStr(msg, clientSocket);
        public_msgArr.push_back(msg);
    }
}

void Chat::sv_sendMessage(std::string& reciever, std::string& message)
{
    std::string task = "sendMessage";
    sv_sendtask(current_user.getName(), task, clientSocket);
    sv_sendStr(reciever, clientSocket);
    sv_sendStr(message, clientSocket);
}

bool Chat::sv_askspace()
{
    bool flag;
    std::string task = "askspace";
    sv_sendtask(task, task, clientSocket);
    sv_readBool(flag, clientSocket);
    return flag;
}
bool Chat::sv_userExists(std::string& login)
{
    bool user_exists;
    std::string task = "userExists";
    sv_sendtask(login, task, clientSocket);

    sv_readBool(user_exists, clientSocket);
    return user_exists;
}

void Chat::sv_addUser(std::string& login, Hash& hash)
{
    std::string task = "addUser";
    sv_sendtask(login, task, clientSocket);
    sv_sendHash(hash, clientSocket);
}

bool Chat::sv_checkPass(std::string& login, Hash& hash)
{
    bool isPass;
    std::string task = "checkPass";
    sv_sendtask(current_user.getName(), task, clientSocket);
    mySleep(20);
    sv_sendHash(hash, clientSocket);
    sv_readBool(isPass, clientSocket);
    return isPass;
}

void Chat::sv_rememberMail(const std::string& str)
{
    std::string task = "rememberPUBMail";

    sv_sendtask(current_user.getName(), task, clientSocket);
    sv_sendStr(str, clientSocket);
}

void Chat::sv_updateUsernames()
{
    std::string task = "updateUsernames";
    std::string name;
    int names_cnt = 0;
    Usernames.clear();

    sv_sendtask(current_user.getName(), task, clientSocket);
    sv_readInt(names_cnt, clientSocket);

    for (auto i{ 0 }; i < names_cnt; i++)
    {
        sv_readStr(name, clientSocket);
        Usernames.push_back(name);
    }
}

bool Chat::sv_setConnection()
{
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed\n");
        return false;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        printf("Creation of Socket failed!\n");
        WSACleanup();
        return false;
    }

    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_family = AF_INET;

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        printf("Connection with the server failed!\n");
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }

    return true;
}

void Chat::sv_closeConnection()
{
    closesocket(clientSocket);
    WSACleanup();
}

void Chat::sv_sendtask(const std::string& username, const std::string& task, const SOCKET& connection_descriptor)
{
    sv_sendStr(username, connection_descriptor);
    sv_sendStr(task, connection_descriptor);
}

void Chat::sv_sendStr(const std::string& str, const SOCKET& connection_descriptor)
{
    send(connection_descriptor, str.c_str(), str.length(), 0);
    mySleep(20);
}

void Chat::sv_readStr(std::string& str, const SOCKET& connection_descriptor)
{
    char buffer[1024];
    memset(buffer, 0, 1024);
    recv(connection_descriptor, buffer, 1024, 0);
    std::string foo(buffer);
    str = foo;
}

void Chat::sv_sendHash(const Hash& hash, const SOCKET& connection_descriptor)
{
    send(connection_descriptor, (char*)&hash._part1, sizeof(uint), 0);
    send(connection_descriptor, (char*)&hash._part2, sizeof(uint), 0);
    send(connection_descriptor, (char*)&hash._part3, sizeof(uint), 0);
    send(connection_descriptor, (char*)&hash._part4, sizeof(uint), 0);
    send(connection_descriptor, (char*)&hash._part5, sizeof(uint), 0);
}

void Chat::sv_readHash(Hash& hash, const SOCKET& connection_descriptor)
{
    recv(connection_descriptor, (char*)&hash._part1, sizeof(uint), 0);
    recv(connection_descriptor, (char*)&hash._part2, sizeof(uint), 0);
    recv(connection_descriptor, (char*)&hash._part3, sizeof(uint), 0);
    recv(connection_descriptor, (char*)&hash._part4, sizeof(uint), 0);
    recv(connection_descriptor, (char*)&hash._part5, sizeof(uint), 0);
}

void Chat::sv_sendBool(const bool& flag, const SOCKET& connection_descriptor)
{
    send(connection_descriptor, (char*)&flag, sizeof(bool), 0);
}

void Chat::sv_readBool(bool& flag, const SOCKET& connection_descriptor)
{
    recv(connection_descriptor, (char*)&flag, sizeof(bool), 0);
}

void Chat::sv_sendInt(const int& integer, const SOCKET& connection_descriptor)
{
    send(connection_descriptor, (char*)&integer, sizeof(int), 0);
}

void Chat::sv_readInt(int& integer, const SOCKET& connection_descriptor)
{
    recv(connection_descriptor, (char*)&integer, sizeof(int), 0);
}