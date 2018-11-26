//
//  Node.cpp
//  PeerJet
//
//  Created by Compy on 11/18/18.
//  Copyright © 2018 peerjet. All rights reserved.
//

#include "Crypto.hpp"
#include "Node.hpp"

Node::Node(NodeConfiguration* config) {
    this->config = config;
}

NodeAddress* Node::getAddress()
{
    return this->address;
}

bool Node::setName(const std::string& name)
{
    this->name = name;
    return true;
}

const std::string Node::getName()
{
    return this->name;
}

bool Node::setStatusMessage(const std::string& statusMessage)
{
    this->statusMessage = statusMessage;
    return true;
}

const std::string Node::getStatusMessage()
{
    return this->statusMessage;
}

bool Node::setStatus(UserStatusType status)
{
    this->status = status;
    return true;
}

UserStatusType Node::getStatus()
{
    return this->status;
}

uint32_t Node::addFriend(const std::string &address, const std::string &message, size_t length)
{
    return 0;
}

int32_t Node::addFriendNoRequest(const uint8_t *pubKey)
{
    if (getFriendByPublicKey(pubKey) != -1) {
        return -4;
    }
    if (!Crypto::isPublicKeyValid(pubKey)) {
        return -5;
    }
    if (Crypto::comparePublicKeys(pubKey, getAddress()))
}

int Node::getFriendByPublicKey(const uint8_t *pubKey)
{
    int count = 0;
    for (std::vector<Friend*>::iterator it = this->friends.begin(); it != this->friends.end(); ++it) {
        if (Crypto::comparePublicKeys((*it)->real_pk, pubKey)) {
            return count;
        }
        count++;
    }
    return -1;
}

bool Node::removeFriend(uint32_t friendNumber)
{
    if (friendNumber < 0 || friendNumber >= this->friends.size()) return false;
    this->friends.erase(this->friends.begin() + friendNumber);
    return true;
}

bool Node::friendExists(uint32_t friendNumber)
{
    return friendNumber >= 0 && friendNumber < this->friends.size();
}

size_t Node::friendListSize()
{
    return this->friends.size();
}


