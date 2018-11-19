//
//  Node.hpp
//  PeerJet
//
//  Created by Compy on 11/18/18.
//  Copyright © 2018 peerjet. All rights reserved.
//

#ifndef Node_hpp
#define Node_hpp

#include <stdio.h>
#include <string>

#define PEERJET_KEY_LENGTH      32

typedef struct {
    unsigned char ip[4];
    unsigned short port;
} NetworkAddress;

typedef enum {
    PROXY_TYPE_NONE,
    PROXY_TYPE_HTTP,
    PROXY_TYPE_SOCKS5
} ProxyType;

typedef unsigned char NodeAddress[PEERJET_KEY_LENGTH];

typedef struct {
    /**
     * Is IPv6 enabled?
     */
    bool ipv6Enabled;
    
    /**
     * Is UDP enabled?
     */
    bool udpEnabled;
    
    /**
     * Defines a proxy type to be used when connecting to the network.
     */
    ProxyType proxyType;
    
    /**
     * The location of the proxy if not NONE above
     */
    const char *proxyHost;
    
    /**
     * The port of the proxy if not NONE above
     */
    unsigned short proxyPort;
    
    /**
     * The starting port in the range that we're able to listen for connections on
     */
    unsigned short startPort;
    
    /**
     * The ending port in the range that we're able to listen for connections on
     */
    unsigned short endPort;
    
    /**
     * The TCP port we will listen for TCP connections on
     */
    unsigned short tcpPort;
    
} NodeConfiguration;

typedef enum {
    CONNECTION_TYPE_NONE,
    CONNECTION_TYPE_TCP,
    CONNECTION_TYPE_UDP
} ConnectionType;

typedef enum {
    MESSAGE_TYPE_NORMAL,
    MESSAGE_TYPE_ACTION
} MessageType;

typedef enum {
    USER_STATUS_NONE,
    USER_STATUS_AWAY,
    USER_STATUS_BUSY
} UserStatus;

class Node {
public:
    Node(const NodeConfiguration*);
    NodeAddress* getAddress();
    
    bool setName(std::string name);
    std::string getName();
    
    bool setStatusMessage(std::string statusMessage);
    std::string getStatusMessage();
    
    bool setStatus(UserStatus status);
    UserStatus getStatus();
    
    bool bootstrap(std::string address, unsigned short port, const unsigned short *pubKey);
    
    bool addTcpRelay(std::string address, unsigned short port, const unsigned short *pubKey);
    
    ConnectionType getConnectionStatus();
    
    unsigned int getIterationInterval();
    
    void tick();
private:
    NodeAddress* address;
    std::string name;
    std::string statusMessage;
    UserStatus status;
};

#endif /* Node_hpp */
