#define _POSIX_C_SOURCE 200112L
#include "PluginAPI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

static int TYPE_STRING;
static VMAPI vm;

Handle nativeNetConnect(Handle args, Handle env) {
    Handle hostH = vm.getLeft(args);
    Handle portH = vm.getLeft(vm.getRight(args));
    
    Handle hostStr = vm.evaluate(hostH, env);
    Handle portNum = vm.evaluate(portH, env);
    
    char *hostname = (char*)vm.getUserData(hostStr);
    int port = vm.getLiteral(portNum);
    
    if (!hostname) return vm.createLiteral(-1);
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return vm.createLiteral(-1);
    
    struct hostent *he = gethostbyname(hostname);
    if (he == NULL) {
        close(sock);
        return vm.createLiteral(-1);
    }
    
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr = *((struct in_addr *)he->h_addr_list[0]);
    
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        close(sock);
        return vm.createLiteral(-1);
    }
    
    return vm.createLiteral(sock);
}

Handle nativeNetListen(Handle args, Handle env) {
    Handle portH = vm.getLeft(args);
    Handle portNum = vm.evaluate(portH, env);
    int port = vm.getLiteral(portNum);
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return vm.createLiteral(-1);
    
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    
    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        close(sock);
        return vm.createLiteral(-1);
    }
    
    if (listen(sock, 5) < 0) {
        close(sock);
        return vm.createLiteral(-1);
    }
    
    return vm.createLiteral(sock);
}

Handle nativeNetAccept(Handle args, Handle env) {
    Handle sockH = vm.getLeft(args);
    Handle sockNum = vm.evaluate(sockH, env);
    int sock = vm.getLiteral(sockNum);
    
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    int client_sock = accept(sock, (struct sockaddr *)&client, &client_len);
    
    return vm.createLiteral(client_sock);
}

Handle nativeNetSend(Handle args, Handle env) {
    Handle sockH = vm.getLeft(args);
    Handle strH = vm.getLeft(vm.getRight(args));
    
    Handle sockNum = vm.evaluate(sockH, env);
    Handle strNode = vm.evaluate(strH, env);
    
    int sock = vm.getLiteral(sockNum);
    char *content = (char*)vm.getUserData(strNode);
    
    if (sock < 0 || !content) return vm.createLiteral(0);
    
    ssize_t sent = send(sock, content, strlen(content), 0);
    return vm.createLiteral(sent);
}

Handle nativeNetRecv(Handle args, Handle env) {
    Handle sockH = vm.getLeft(args);
    Handle sizeH = vm.getLeft(vm.getRight(args));
    
    Handle sockNum = vm.evaluate(sockH, env);
    Handle sizeNum = vm.evaluate(sizeH, env);
    
    int sock = vm.getLiteral(sockNum);
    int max_size = vm.getLiteral(sizeNum);
    
    if (sock < 0 || max_size <= 0) return vm.createForeign(TYPE_STRING, 0, -1, -1, NULL);
    
    char *buffer = malloc(max_size + 1);
    ssize_t received = recv(sock, buffer, max_size, 0);
    
    if (received < 0) {
        free(buffer);
        return vm.createForeign(TYPE_STRING, 0, -1, -1, NULL);
    }
    
    buffer[received] = '\0';
    return vm.createForeign(TYPE_STRING, 0, -1, -1, buffer);
}

Handle nativeNetClose(Handle args, Handle env) {
    Handle sockH = vm.getLeft(args);
    Handle sockNum = vm.evaluate(sockH, env);
    int sock = vm.getLiteral(sockNum);
    
    if (sock >= 0) close(sock);
    return vm.createLiteral(1);
}

void initPlugin(VMAPI providedApi) {
    vm = providedApi;
    TYPE_STRING = vm.registerType("String");
    vm.registerNative("net.connect", nativeNetConnect);
    vm.registerNative("net.listen", nativeNetListen);
    vm.registerNative("net.accept", nativeNetAccept);
    vm.registerNative("net.send", nativeNetSend);
    vm.registerNative("net.recv", nativeNetRecv);
    vm.registerNative("net.close", nativeNetClose);
}
