#include <iostream>
#include <fstream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string>

using namespace std;

#define BUFFER_SIZE 1024 * 10
#define PORT 25
int server_fd;

void cleanup(int signum)
{
    cout << endl
         << " Stopping server... " << signum << endl;
    shutdown(server_fd, SHUT_RDWR);
    exit(signum);
}
string getHeaderFieldValue(const string &emailData, const string &headerFieldName)
{
    string headerStart = headerFieldName + ": ";
    char *dataPtr   = (char*)emailData.c_str();
    char *headerPtr = (char*)headerStart.c_str();

    bool lineStart  = false;
    bool matched    = false;
    bool matching   = false;

    string value;
    while (*dataPtr != 0)
    {
        if(*dataPtr != '\r'){
            if(matched){
                if(*dataPtr == '\n'){
                    break;
                }else{
                    value.push_back(*dataPtr);
                }
            }
            if((matching || lineStart) && tolower(*dataPtr) == tolower(*headerPtr)){
                matching = true;
                headerPtr++;
            }else{
                matching = true;
                headerPtr = (char*)headerStart.c_str();
            }
            if(*headerPtr == 0){
                matched = true;
            }

            if(*dataPtr == '\n'){
                // Double newline == 
                if(lineStart) break;
                lineStart = true;
            }else{
                lineStart = false;
            }
        }
        dataPtr++;
    }

    return value;
}

// int main(){
//     string data= "O:<bar@example.com>\r\n\r\nContent-Type: multipart/alternative;\r\n boundary=\"--_NmP-7bd80925b63b9578-Part_1\"\r\n From: =?UTF-8?Q?Fred_Foo_=F0=9F=91=BB?= <foo@example.com>\r\nTo: bar@example.com\r\nSubject: =?UTF-8?Q?Hello_=E2=9C=94?=\r\nMessage-ID: <136f1662-99ec-eedb-7e4d-911c6e775bc4@example.com>\r\nDate: Tue, 25 Apr 2023 19:36:52 +0000\r\nMIME-Version: 1.0\r\n\r\n----_NmP-7bd80925b63b9578-Part_1\r\nContent-Type: text/plain; charset=utf-8\r\nContent-Transfer-Encoding: 7bit\r\n\r\nHello world?\r\n----_NmP-7bd80925b63b9578-Part_1\r\nContent-Type: text/html; charset=utf-8\r\nContent-Transfer-Encoding: 7bit\r\n\r\n<b>Hello world?</b>\r\n----_NmP-7bd80925b63b9578-Part_1--";
//     string hea = "from";

//     string val = getHeaderFieldValue(data, hea);

//     cout << val << endl;

//     return 0;
// }

std::string strip(std::string &str)
{
    if  (str.length() != 0)
    {
        auto w = std::string(" ") ;
        auto n = std::string("\n") ;
        auto r = std::string("\t") ;
        auto t = std::string("\r") ;
        auto v = std::string(1 ,str.front()); 
        while((v == w) || (v==t) || (v==r) || (v==n))
        {
            str.erase(str.begin());
            v = std::string(1 ,str.front());
        }
        v = std::string(1 , str.back()); 
        while((v ==w) || (v==t) || (v==r) || (v==n))
        {
            str.erase(str.end() - 1 );
            v = std::string(1 , str.back());
        }
    }

    return str;
}

void handle_connection(int socket)
{
    cout << endl
         << ">> Incoming connection " << socket << endl;

    ofstream log;
    log.open("./con.log");



    string message = "220 smtp.example.com SMTP Ready\r\n";
    send(socket, message.c_str(), message.length(), 0);

    char buffer[BUFFER_SIZE] = {0};
    string line;

    bool readingDATA;
    string data;

    string sender;
    string target;

    while (read(socket, buffer, BUFFER_SIZE) > 0)
    {
        line = string(buffer);
        log << line;
    process:
        if (readingDATA)
        {
            size_t index = line.rfind("\r\n.\r\n");
            if (index != string::npos)
            {
                cout << "- Received " << data.length() << " chars of body." << endl;
                readingDATA = false;
                message = "250 Ok\r\n";
                send(socket, message.c_str(), message.length(), 0);
                data += line.substr(0, index);
            }
            else
            {
                data += line;
            }
        }
        else
        {
            if (line.rfind("HELO") == 0 || line.rfind("EHLO") == 0)
            {
                // cout << "> Receive Hello" << endl;
                message = "250 Ok\r\n";
                send(socket, message.c_str(), message.length(), 0);
            }
            else if (line.rfind("MAIL FROM:") == 0)
            {
                sender = line.substr(10);
                sender = strip(sender);
                cout << "- Sender: " << sender << endl;

                message = "250 Ok\r\n";
                send(socket, message.c_str(), message.length(), 0);
            }
            else if (line.rfind("RCPT TO:") == 0)
            {
                target = line.substr(8);
                target = strip(target);
                cout << "- Target: " << target << endl;

                message = "250 Ok\r\n";
                send(socket, message.c_str(), message.length(), 0);
            }
            else if (line.rfind("DATA") == 0)
            {
                cout << "- Begin Data..." << endl;
                message = "354  Ready\r\n";
                send(socket, message.c_str(), message.length(), 0);
                line = line.substr(4 + 2);

                readingDATA = true;
                goto process;
            }
            else if (line.rfind("QUIT") == 0)
            {
                message = "221 Bye\r\n";
                send(socket, message.c_str(), message.length(), 0);
                break;
            }
            else
            {
                cout << "Not recognized: "
                     << "\x1B[32m" << strip(line) << "\x1B[0m" << endl;
            }
        }
    }

    string date = getHeaderFieldValue(data, "date");
    string to = getHeaderFieldValue(data, "to");
    ofstream myfile;
    string path = "./messages/";
    mkdir(path.c_str(), 0777);
    if (date.length() == 0 || to.length() == 0)
    {
        path += "unknown/";
        mkdir(path.c_str(), 0777);
        path += to_string(rand()) + ".txt";
    }
    else
    {
        path += to + "/";
        mkdir(path.c_str(), 0777);
        path += date + ".txt";
    }

    myfile.open(path);
    myfile << data;
    log.close();
    myfile.close();

    cout << "Wrote email to " << path << endl;

    close(socket);
    cout << "Closed connection " << socket << endl;
}

int main(int argc, char const *argv[])
{
    signal(SIGINT, cleanup);

    // Creating socket file descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    cout << "Listening on port: " << PORT << endl;

    while (true)
    {
        socklen_t addrlen = sizeof(address);
        int new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);

        if (new_socket < 0)
        {
            perror("accept error");
            exit(EXIT_FAILURE);
        }

        handle_connection(new_socket);
    }

    cleanup(2);

    return 0;
}
