#include "Chat.h"

Chat::Chat()
{
    current_user = NULL;
}

Chat::Chat(std::string path) : Chat()
{
    save_path = path;
}

void Chat::start()
{
    scr_load();
    scr_work();
}

bool Chat::findUser(const std::string& name, int& pos)
{
    for (auto i{ 0 }; i < users_array.size(); i++)
        if (users_array[i].getName() == name)
        {
            pos = i;
            return true;
        }

    return false;
}

void Chat::clearLine()
{
    printf("\033[A");       // line up
    printf("\33[2K\r");     // clear line + move curor
}

void Chat::mySleep(int time)
{
    std::chrono::milliseconds timespan(time);
    std::this_thread::sleep_for(timespan);
}

void Chat::clear_screen()
{
    system("cls");
}

void Chat::rememberMail(std::string str) // remember 1000 last mails
{
    if (public_msgArr.size() < 1000)
        public_msgArr.push_back(str);
    else
    {
        public_msgArr.pop_front();
        public_msgArr.push_back(str);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// SAVE - LOAD

bool Chat::save()
{
    std::ofstream fout(save_path, std::ios_base::binary);

    if (!fout.is_open()) // if file is not opened/created
        return false;

    size_t capity;

    capity = public_msgArr.size();
    fout.write((char*)&capity, sizeof(size_t));         // save mails count

    for (auto i{ 0 }; i < public_msgArr.size(); ++i)    // save mails array
        saveStr(public_msgArr[i], fout);

    capity = users_array.size();
    fout.write((char*)&capity, sizeof(size_t));         // save users count

    for (auto i{ 0 }; i < users_array.size(); ++i)      // save Users array
    {
        bool foo;
        Message* msg_arr;

        std::string str_out = users_array[i].getName(); // save login
        saveStr(str_out, fout);

        Hash hash_to_save = pass_table[str_out];
        saveHash(hash_to_save, fout);

        msg_arr = users_array[i].getMailboxPtr()->getMailsArray();
        capity = users_array[i].getMailboxPtr()->getCapity();

        fout.write((char*)&capity, sizeof(size_t));    // save Mailbox size

        for (auto j{ 0 }; j < capity; ++j)
        {
            str_out = msg_arr[j].getAuthor();
            saveStr(str_out, fout);           // save name
            str_out = msg_arr[j].getMsg();
            saveStr(str_out, fout);           // save message text

            foo = msg_arr[j].getFlag();
            fout.write((char*)&foo, sizeof(bool));     // save flag
        }
    }

    fout.close();
    return true;
}

bool Chat::load()
{
    std::ifstream fin(save_path, std::ios_base::binary);

    if (!fin.is_open())                         // if file is not opened
        return false;

    size_t load_data_size;
    std::string load_data;

    fin.read((char*)&load_data_size, sizeof(size_t));

    for (auto i{ 0 }; i < load_data_size; ++i)
    {
        loadStr(load_data, fin);
        public_msgArr.push_back(load_data);
    }

    fin.read((char*)&load_data_size, sizeof(size_t));
    users_array.reserve(load_data_size);

    for (auto i{ 0 }; i < load_data_size; ++i)
    {
        bool foo;
        size_t capity;
        std::string name, txt;
        Hash hash_to_load;

        loadStr(name, fin);
        users_array.emplace_back(name);

        loadHash(hash_to_load, fin);

        pass_table.emplace(std::make_pair(name, hash_to_load));

        fin.read((char*)&capity, sizeof(size_t));

        if (!capity)
            continue;

        for (auto j{ 0 }; j < capity; ++j)
        {
            loadStr(name, fin);
            loadStr(txt, fin);
            fin.read((char*)&foo, sizeof(bool));

            Message msg_scan(txt, name);

            if (foo)
                msg_scan.setReaded();

            users_array[i].receiveMessage(msg_scan);
        }
    }

    fin.close();
    return true;
}

void Chat::saveStr(std::string& str, std::ofstream& fout)
{
    int size = sizeof(char) * str.length();

    fout.write((char*)&size, sizeof(int));
    fout.write(str.c_str(), size);
}

void Chat::loadStr(std::string& str, std::ifstream& fin)
{
    int size;

    fin.read((char*)&size, sizeof(int));
    str.resize(size / sizeof(char));
    fin.read((char*)str.c_str(), size);
}

void Chat::saveHash(Hash& hash, std::ofstream& fout)
{
    fout.write((char*)&hash._part1, sizeof(uint));
    fout.write((char*)&hash._part2, sizeof(uint));
    fout.write((char*)&hash._part3, sizeof(uint));
    fout.write((char*)&hash._part4, sizeof(uint));
    fout.write((char*)&hash._part5, sizeof(uint));
}

void Chat::loadHash(Hash& hash, std::ifstream& fin)
{
    fin.read((char*)&hash._part1, sizeof(uint));
    fin.read((char*)&hash._part2, sizeof(uint));
    fin.read((char*)&hash._part3, sizeof(uint));
    fin.read((char*)&hash._part4, sizeof(uint));
    fin.read((char*)&hash._part5, sizeof(uint));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// SCREENS

void Chat::scr_exit()
{
    clear_screen();
    printf("\n\n\n");
    printf("\t\t\033[36m     Saving data");

    for (auto i{ 0 }; i < 3; ++i)
    {
        mySleep();
        printf(".");
    }

    if (save())
    {
        printf("\n\n\t\t     Complete\033[0m");
        mySleep(250);
    }
    else
    {
        printf("\n\n\t\t     Save error\033[0m");
        mySleep(250);
    }

    printf("\n\n\n\n\n\n");
    exit(0);
}

void Chat::scr_load()
{
    clear_screen();
    printf("\n\n\n");
    printf("\033[36m");
    printf("\t\t      Loadind data");

    for (auto i{ 0 }; i < 3; ++i)
    {
        mySleep(200);
        printf(".");
    }

    if (load())
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
    }
    printf("\033[0m");
}

void Chat::scr_work()
{
    std::string task, userName;

    clear_screen();
    sv_create();

    while (true)
    {
        bool flag;
        int foo, poz;

        sv_gettask(clientSocket, userName, task);

        if(task.length() == 0)
            break;

        printf("\nuserName: %10s  task: %s", userName.c_str(), task.c_str());

        if (task == "updatePrivate")
        {
            int client_msg_cnt, server_msg_cnt;

            sv_readInt(client_msg_cnt, clientSocket);
            findUser(userName, poz);
            server_msg_cnt = users_array[poz].getMessageCnt();

            if (client_msg_cnt == server_msg_cnt)
            {
                flag = false;
                sv_sendBool(flag, clientSocket);
            }
            else
            {
                flag = true;
                sv_sendBool(flag, clientSocket);

                // send difference in messegaes to client
                // notice! we mean server_msg_cnt allways bigger or simular 
                client_msg_cnt = server_msg_cnt - client_msg_cnt;
                sv_sendInt(client_msg_cnt, clientSocket);

                Message* msg_arr = users_array[poz].getMailboxPtr()->getMailsArray();

                for (auto i{ 0 }; i < server_msg_cnt; i++)
                {
                    sv_sendStr(msg_arr[i].getAuthor(), clientSocket);
                    mySleep(20);
                    sv_sendStr(msg_arr[i].getMsg(), clientSocket);
                    mySleep(20);

                    flag = msg_arr[i].getFlag();
                    sv_sendBool(flag, clientSocket);
                }
            }
        }
        else if (task == "updatePublic")
        {
            sv_sendInt(public_msgArr.size(), clientSocket);

            for (auto i{ 0 }; i < public_msgArr.size(); i++)
            {
                sv_sendStr(public_msgArr[i], clientSocket);
                mySleep(20);
            }
        }
        else if (task == "userExists")
        {
            flag = findUser(userName, foo);
            sv_sendBool(flag, clientSocket);
        }
        else if (task == "askspace")
        {
            flag = (users_array.size() == users_array.max_size());
            sv_sendBool(flag, clientSocket);
        }
        else if (task == "checkPass")
        {
            Hash client_hash;

            sv_readHash(client_hash, clientSocket);
            if (pass_table[userName] == client_hash)
                flag = true;
            else
                flag = false;
            sv_sendBool(flag, clientSocket);
        }
        else if (task == "addUser")
        {
            Hash client_hash;

            sv_readHash(client_hash, clientSocket);
            pass_table.emplace(std::make_pair(userName, client_hash));
            users_array.emplace_back(userName);
        }
        else if (task == "rememberPUBMail")
        {
            std::string pub_message;
            sv_readStr(pub_message, clientSocket);

            if (public_msgArr.size() < 1000)
                public_msgArr.push_back(pub_message);
            else
            {
                public_msgArr.pop_front();
                public_msgArr.push_back(pub_message);
            }
        }
        else if (task == "sendMessage")
        {
            std::string private_message, reciever;
            sv_readStr(reciever, clientSocket);
            sv_readStr(private_message, clientSocket);

            findUser(userName, poz);

            Message msg(private_message, userName);
            users_array[poz].receiveMessage(msg);
        }
        else if (task == "updateUsernames")
        {
            sv_sendInt(users_array.size(), clientSocket);

            for (auto i{ 0 }; i < users_array.size(); i++)
            {
                sv_sendStr(users_array[i].getName(), clientSocket);
                mySleep(30);
            }
        }
    }

    printf("\nUSER DISCONNECTED");
    mySleep(300);
    scr_exit();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// SERVER

void Chat::sv_create()
{
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed\n");
        exit(1);
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        printf("Socket creation failed!\n");
        WSACleanup();
        exit(1);
    }

    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_family = AF_INET;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        printf("Socket binding failed!\n");
        closesocket(serverSocket);
        WSACleanup();
        exit(1);
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("Socket is unable to listen for new connections!\n");
        closesocket(serverSocket);
        WSACleanup();
        exit(1);
    }
    else
    {
        printf("Server is listening for new connection\n");
    }

    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

    if (clientSocket == INVALID_SOCKET)
    {
        printf("Server is unable to accept the data from client!");
        closesocket(serverSocket);
        WSACleanup();
    }
    else
    {
        printf("Connection created");
    }
}

void Chat::sv_gettask(const SOCKET& connection_descriptor, std::string& name, std::string& task)
{
    sv_readStr(name, connection_descriptor);
    sv_readStr(task, connection_descriptor);
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