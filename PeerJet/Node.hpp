//
//  Node.hpp
//  PeerJet
//
//  Created by Compy on 11/18/18.
//  Copyright © 2018 peerjet. All rights reserved.
//

#ifndef Node_hpp
#define Node_hpp

#include <cstdint>
#include <stdio.h>
#include <string>

#define PEERJET_KEY_LENGTH      32

class Node;

typedef struct {
    unsigned char ip[4];
    uint16_t port;
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
    uint16_t proxyPort;
    
    /**
     * The starting port in the range that we're able to listen for connections on
     */
    uint16_t startPort;
    
    /**
     * The ending port in the range that we're able to listen for connections on
     */
    uint16_t endPort;
    
    /**
     * The TCP port we will listen for TCP connections on
     */
    uint16_t tcpPort;
    
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
} UserStatusType;

typedef enum {
    LOG_LEVEL_TRACE,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR
} LogLevelType;

typedef enum {
    FILE_CONTROL_RESUME,
    FILE_CONTROL_PAUSE,
    FILE_CONTROL_CANCEL
} FileControlType;

// Callback type definitions
typedef void PJLogCallback(Node* node, LogLevelType level, const std::string& file, uint32_t line, const std::string& func, const std::string& message, void* userData);
typedef void PJConnectionStatusCallback(Node* node, ConnectionType connectionStatus, void* userData);
typedef void PJFriendNameCallback(Node* node, uint32_t friendNumber, const std::string& name, size_t length, void* userData);
typedef void PJFriendStatusMessageCallback(Node* node, uint32_t friendNumber, const std::string& message, size_t length, void* userData);
typedef void PJFriendStatusCallback(Node* node, uint32_t friendNumber, UserStatusType userStatus, void* userData);
typedef void PJFriendConnectionStatusCallback(Node* node, uint32_t friendNumber, ConnectionType connectionStatus, void* userData);
typedef void PJFriendTypingCallback(Node* node, uint32_t friendNumber, bool isTyping, void *userData);
typedef void PJFriendReadReceiptCallback(Node* node, uint32_t friendNumber, uint32_t messageId, void* userData);
typedef void PJFriendRequestCallback(Node* node, const uint8_t* publicKey, const uint8_t* message, size_t length, void* userData);
typedef void PJFriendMessageCallback(Node* node, uint32_t friendNumber, MessageType type, const uint8_t* message, size_t length, void* userData);
typedef void PJFileReceiveControlCallback(Node* node, uint32_t friendNumber, uint32_t fileNumber, FileControlType fileControl, void* userData);
typedef void PJFileChunkRequestCallback(Node* node, uint32_t friendNumber, uint32_t fileNumber, uint64_t position, size_t length, void* userData);
typedef void PJFileReceiveCallback(Node* node, uint32_t friendNumber, uint32_t fileNumber, uint32_t fileKind, uint64_t fileSize, const uint8_t *filename, size_t filenameLength, void* userData);
typedef void PJFileReceiveChunkCallback(Node* node, uint32_t friendNumber, uint32_t fileNumber, uint64_t position, const uint8_t *data, size_t length, void* userData);


typedef void PJFriendLossyPacketCallback(Node* node, uint32_t friendNumber, const uint8_t *data, size_t length, void* userData);
typedef void PJFriendLosslessPacketCallback(Node* node, uint32_t friendNumber, const uint8_t *data, size_t length, void* userData);
// End callback type definitions

class Node {
public:
    Node(NodeConfiguration* config);
    NodeAddress* getAddress();
    
    bool setName(std::string name);
    std::string getName();
    
    bool setStatusMessage(std::string statusMessage);
    std::string getStatusMessage();
    
    bool setStatus(UserStatusType status);
    UserStatusType getStatus();
    
    uint32_t addFriend(const std::string& address, const std::string& message, size_t length);
    uint32_t addFriendNoRequest(const std::string& pubKey);
    bool removeFriend(uint32_t friendNumber);
    uint32_t getFriendByPublicKey(const std::string& pubKey);
    bool friendExists(uint32_t friendNumber);
    size_t friendListSize();
    
    bool bootstrap(const std::string& address, uint16_t port, const std::string& pubKey);
    
    bool addTcpRelay(const std::string& address, uint16_t port, const std::string& pubKey);
    
    void setNoSpam(uint32_t nospam);
    uint32_t getNoSpam();
    
    bool getFriendsPublicKey(uint32_t friendNumber, uint8_t *pubKey);
    uint64_t getFriendLastOnline(uint32_t friendNumber);
    
    std::string getFriendName(uint32_t friendNumber);
    
    ConnectionType getConnectionStatus();
    
    uint32_t getIterationInterval();
    
    void tick();
    
    // Callbacks
    void setLogCallback(PJLogCallback cb);
    void setConnectionStatusCallback(PJConnectionStatusCallback cb);
    void setFriendNameCallback(PJFriendNameCallback cb);
    void setFriendStatusMessageCallback(PJFriendStatusMessageCallback cb);
    void setFriendStatusCallback(PJFriendStatusCallback cb);
    void setFriendConnectionStatusCallback(PJFriendConnectionStatusCallback cb);
    void setFriendTypingCallback(PJFriendTypingCallback cb);
    void setFriendReadReceiptCallback(PJFriendReadReceiptCallback cb);
    void setFriendRequestCallback(PJFriendRequestCallback cb);
    void setFriendMessageCallback(PJFriendMessageCallback cb);
    void setFileReceiveControlCallback(PJFileReceiveControlCallback cb);
    void setFileChunkRequestCallback(PJFileChunkRequestCallback cb);
    void setFileReceiveCallback(PJFileReceiveCallback cb);
    void setFileReceiveChunkCallback(PJFileReceiveChunkCallback cb);
private:
    NodeAddress* address;
    NodeConfiguration* config;
    std::string name;
    std::string statusMessage;
    
    uint32_t nospam;
    UserStatusType status;
    
    PJLogCallback logCallback;
    PJConnectionStatusCallback connectionStatusCallback;
    PJFriendNameCallback friendNameCallback;
    PJFriendStatusMessageCallback friendStatusMessageCallback;
    PJFriendStatusCallback friendStatusCallback;
    PJFriendConnectionStatusCallback friendConnectionStatusCallback;
    PJFriendTypingCallback friendTypingCallback;
    PJFriendReadReceiptCallback friendReadReceiptCallback;
    PJFriendRequestCallback friendRequestCallback;
    PJFriendMessageCallback friendMessageCallback;
    PJFileReceiveControlCallback fileReceiveControlCallback;
    PJFileChunkRequestCallback fileChunkRequestCallback;
    PJFileReceiveCallback fileReceiveCallback;
    PJFileReceiveChunkCallback fileReceiveChunkCallback;
};


#endif /* Node_hpp */
