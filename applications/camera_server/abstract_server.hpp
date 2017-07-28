#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <algorithm>
#include <chrono>

class abstract_server {
public:
    abstract_server(int port);
    void run();
    void handle(int client);
    std::string get_request(int client);
    void send_response(int client, std::string response);
protected:
    virtual std::string process_request(std::string request);
private:
    std::string disconnect_client();
    int port;
    int sock;
    bool client_connected;
};

class server_error : public std::runtime_error {
public:
    server_error(const std::string what) : std::runtime_error(what) {}
};

class server_init_error : public server_error {
public:
    server_init_error(const std::string what) : server_error(what) {}
};

class server_handling_error : public server_error {
public:
    server_handling_error(const std::string what) : server_error(what) {}
};
