//
//  Node.hpp
//  PeerJet
//
//  Created by Compy on 11/18/18.
//  Copyright © 2018 peerjet. All rights reserved.
//

#ifndef Node_hpp
#define Node_hpp

#include "Config.h"
#include <cstdint>
#include <stdio.h>
#include <string>
#include <vector>

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

typedef uint8_t NodeAddress[PEERJET_KEY_LENGTH];

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

#define FILE_ID_LENGTH 32

struct FileTransfers {
    uint64_t size;
    uint64_t transferred;
    uint8_t status; /* 0 == no transfer, 1 = not accepted, 3 = transferring, 4 = broken, 5 = finished */
    uint8_t paused; /* 0: not paused, 1 = paused by us, 2 = paused by other, 3 = paused by both. */
    uint32_t last_packet_number; /* number of the last packet sent. */
    uint64_t requested; /* total data requested by the request chunk callback */
    unsigned int slots_allocated; /* number of slots allocated to this transfer. */
    uint8_t id[FILE_ID_LENGTH];
};

typedef struct {
    uint8_t real_pk[PEERJET_KEY_LENGTH];
    int friendcon_id;
    
    uint64_t friendrequest_lastsent; // Time at which the last friend request was sent.
    uint32_t friendrequest_timeout; // The timeout between successful friendrequest sending attempts.
    uint8_t status; // 0 if no friend, 1 if added, 2 if friend request sent, 3 if confirmed friend, 4 if online.
    uint8_t info[MAX_FRIEND_REQUEST_DATA_SIZE]; // the data that is sent during the friend requests we do.
    uint8_t name[MAX_NAME_LENGTH];
    uint16_t name_length;
    uint8_t name_sent; // 0 if we didn't send our name to this friend 1 if we have.
    uint8_t statusmessage[MAX_STATUSMESSAGE_LENGTH];
    uint16_t statusmessage_length;
    uint8_t statusmessage_sent;
    UserStatusType userstatus;
    uint8_t userstatus_sent;
    uint8_t user_istyping;
    uint8_t user_istyping_sent;
    uint8_t is_typing;
    uint16_t info_size; // Length of the info.
    uint32_t message_id; // a semi-unique id used in read receipts.
    uint32_t friendrequest_nospam; // The nospam number used in the friend request.
    uint64_t last_seen_time;
    uint8_t last_connection_udp_tcp;
    struct FileTransfers file_sending[MAX_CONCURRENT_FILE_PIPES];
    unsigned int num_sending_files;
    struct FileTransfers file_receiving[MAX_CONCURRENT_FILE_PIPES];
    
    struct {
        int (*function)(Node *node, uint32_t friendNumber, const uint8_t *data, uint16_t len, void *object);
        void *object;
    } lossy_rtp_packethandlers[PACKET_LOSSY_AV_RESERVED];
    
    struct Receipts *receipts_start;
    struct Receipts *receipts_end;
} Friend;

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
    
    bool setName(const std::string& name);
    const std::string getName();
    
    bool setStatusMessage(const std::string& statusMessage);
    const std::string getStatusMessage();
    
    bool setStatus(UserStatusType status);
    UserStatusType getStatus();
    
    uint32_t addFriend(const std::string& address, const std::string& message, size_t length);
    uint32_t addFriendNoRequest(const uint8_t* pubKey);
    bool removeFriend(uint32_t friendNumber);
    uint32_t getFriendByPublicKey(const uint8_t* pubKey);
    bool friendExists(uint32_t friendNumber);
    size_t friendListSize();
    
    bool bootstrap(const std::string& address, uint16_t port, const uint8_t* pubKey);
    
    bool addTcpRelay(const std::string& address, uint16_t port, const uint8_t* pubKey);
    
    void setNoSpam(uint32_t nospam);
    uint32_t getNoSpam();
    
    bool getFriendsPublicKey(uint32_t friendNumber, uint8_t *pubKey);
    uint64_t getFriendLastOnline(uint32_t friendNumber);
    
    const std::string getFriendName(uint32_t friendNumber);
    
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
    std::vector<Friend*> friends;
    
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
