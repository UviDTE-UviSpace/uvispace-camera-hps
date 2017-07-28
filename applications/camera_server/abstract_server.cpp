#include "abstract_server.hpp"

abstract_server::abstract_server(int port) : port(port) {
    // Setup socket address structure
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(this->port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Create socket
    this->sock = socket(PF_INET, SOCK_STREAM, 0);
    if (!this->sock) {
        throw server_init_error("Socket creation failed");
    }

    // Set socket to immediately reuse port when the application closes
    int reuse = 1;
    if (setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        throw server_init_error("Socket configuration failed");
    }

    // Call bind to associate the socket with our local address and port
    if (bind(this->sock, (const struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        throw server_init_error("Socket binding failed");
    }

    // Convert the socket to listen for incoming connections
    if (listen(this->sock, SOMAXCONN) < 0) {
        throw server_init_error("Socket listening failed");
    }
}

void abstract_server::run() {
    int client;
    struct sockaddr_in client_addr;
    socklen_t clientlen = sizeof(client_addr);

    while (true) {
        client = accept(this->sock, (struct sockaddr *) &client_addr, &clientlen);
        if (client > 0) {
            this->client_connected = true;
            handle(client);
        }
    }

    return;
}

void abstract_server::handle(int client) {
    try {
        while (this->client_connected) {
            std::string request = this->get_request(client);
            std::string response = this->process_request(request);
            this->send_response(client, response);
        }
    } catch (server_handling_error& e) {
        this->disconnect_client();
    }
    close(client);
    return;
}

std::string abstract_server::get_request(int client) {
    char* rx = new char[32];
    ssize_t nread = recv(client, rx, 32, 0);
    if (nread < 0) {
        throw server_handling_error("Error reading request");
    }

    // Be sure to use append in case we have binary data
    std::string request = "";
    request.append(rx, nread);

    // Remove line breaks
    request.erase(std::remove(request.begin(), request.end(), '\n'), request.end());
    request.erase(std::remove(request.begin(), request.end(), '\r'), request.end());
    delete[] rx;
    return request;
}

std::string abstract_server::process_request(std::string request) {
    if (request == "quit") {
        return this->disconnect_client();
    }
    return "unknown command\n";
}

std::string abstract_server::disconnect_client() {
    this->client_connected = false;
    return "bye\n";
}

void abstract_server::send_response(int client, std::string response) {
    ssize_t nwritten = send(client, response.c_str(), response.length(), MSG_NOSIGNAL);
    if (nwritten < 0) {
        throw server_handling_error("Error sending request");
    }
    return;
}
