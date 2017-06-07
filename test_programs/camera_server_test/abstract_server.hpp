#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <algorithm>

class abstract_server {
public:
    abstract_server(int port);
    int run();
    int handle(int client);
    std::string get_request(int client);
    int send_response(int client, std::string response);
protected:
    std::string virtual process_request(std::string request) = 0;
private:
    int port;
    int sock;
};
