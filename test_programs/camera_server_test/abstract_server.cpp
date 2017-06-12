#include "abstract_server.hpp"

abstract_server::abstract_server(int port) : port(port) {}

int abstract_server::run() {
    struct sockaddr_in server_addr;

    // setup socket address structure
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(this->port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // create socket
    this->sock = socket(PF_INET,SOCK_STREAM,0);
    if (!this->sock) {
        perror("socket");
        exit(-1);
    }

    // set socket to immediately reuse port when the application closes
    int reuse = 1;
    if (setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt");
        exit(-1);
    }

    // call bind to associate the socket with our local address and port
    if (bind(this->sock,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) {
        perror("bind");
        exit(-1);
    }

    // convert the socket to listen for incoming connections
    if (listen(this->sock,SOMAXCONN) < 0) {
        perror("listen");
        exit(-1);
    }

    // setup client
    int client;
    struct sockaddr_in client_addr;
    socklen_t clientlen = sizeof(client_addr);

    // accept clients
    while ((client = accept(this->sock,(struct sockaddr *)&client_addr,&clientlen)) > 0) {
        handle(client);
    }

}

int abstract_server::handle(int client) {
    std::string request = this->get_request(client);
    std::string response = this->process_request(request);
    this->send_response(client, response);
    close(client);
    return 0;
}

std::string abstract_server::get_request(int client) {
    char* rx = new char[256];
    std::string request = "";
    // read until we get a newline
    while (request.find("\n") == std::string::npos) {
        int nread = recv(client, rx, 257, 0);
        if (nread < 0) {
            if (errno == EINTR)
                // the socket call was interrupted -- try again
                continue;
            else
                // an error occurred, so break out
                std::cerr << "recv error";
                return "";
        }
        // be sure to use append in case we have binary data
        request.append(rx, nread);
    }
    // Remove line breaks
    request.erase(std::remove(request.begin(), request.end(), '\n'), request.end());
    request.erase(std::remove(request.begin(), request.end(), '\r'), request.end());
    // a better server would cut off anything after the newline and
    // save it in a cache
    return request;
}

int abstract_server::send_response(int client, std::string response) {
    // prepare to send response
    const char* ptr = response.c_str();
    int nleft = response.length();
    int nwritten;
    // loop to be sure it is all sent
    while (nleft) {
        if ((nwritten = send(client, ptr, nleft, 0)) < 0) {
            if (errno == EINTR) {
                // the socket call was interrupted -- try again
                continue;
            } else {
                // an error occurred, so break out
                perror("write");
                return -1;
            }
        } else if (nwritten == 0) {
            // the socket is closed
            return -1;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return 0;
}
