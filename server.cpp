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
#include <regex>

using namespace std;

#define BUFFER_SIZE 1024 * 10
#define PORT 8080
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
    // Regular expression pattern to match the header field
    string pattern = "(" + headerFieldName + ":\\s*)([^\\r\\n]+)";
    regex headerFieldPattern(pattern);
    smatch match;

    // Search for the header field in the email data
    if (regex_search(emailData, match, headerFieldPattern))
    {
        // Extract the value of the header field
        if (match.size() == 3)
        {
            return match[2].str();
        }
    }

    // Return an empty string if the header field is not found
    return "";
}
void handle_connection(int socket)
{
    cout << endl
         << ">> Incoming connection " << socket << endl;

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
    process:
        if (readingDATA)
        {
            size_t index = line.rfind("\r\n.\r\n");
            if (index != string::npos)
            {
                cout << "> End data: " << data.length() << endl;
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
                cout << "> Receive Hello" << endl;
                message = "250 Ok\r\n";
                send(socket, message.c_str(), message.length(), 0);
            }
            else if (line.rfind("MAIL FROM:") == 0)
            {
                sender = line.substr(10);
                cout << "> Receive Sender: " << sender << endl;

                message = "250 Ok\r\n";
                send(socket, message.c_str(), message.length(), 0);
            }
            else if (line.rfind("RCPT TO:") == 0)
            {
                target = line.substr(8);
                cout << "> Receive Target: " << target << endl;

                message = "250 Ok\r\n";
                send(socket, message.c_str(), message.length(), 0);
            }
            else if (line.rfind("DATA") == 0)
            {
                cout << "> Start data: " << endl;
                message = "354  Ready\r\n";
                send(socket, message.c_str(), message.length(), 0);
                line = line.substr(4 + 2);

                readingDATA = true;
                goto process;
            }
            else if (line.rfind("QUIT") == 0)
            {
                cout << "> Bye: " << endl;
                message = "221 Bye\r\n";
                send(socket, message.c_str(), message.length(), 0);
                break;
            }
            else
            {
                cout << "Not recognized: "
                     << "\x1B[32m" << line << "\x1B[0m" << endl;
            }
        }
    }

    string date = getHeaderFieldValue(data, "Date");
    string to = getHeaderFieldValue(data, "To");
    ofstream myfile;
    string path = "./messages/";
    mkdir(path.c_str(), 0777);
    if (date.length() == 0 || to.length() == 0)
    {
        cerr << "Invalid Email";
        path += rand() + ".txt";
    }
    else
    {
        path += to + "/";
        mkdir(path.c_str(), 0777);
        path += date + ".txt";
    }

    myfile.open(path);
    myfile << data;
    myfile.close();

    cout << ">>> Wrote email to " << path << endl;

    close(socket);
    cout << endl
         << "<< Closed connection " << socket << endl;
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
