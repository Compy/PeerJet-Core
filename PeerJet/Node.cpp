//
//  Node.cpp
//  PeerJet
//
//  Created by Compy on 11/18/18.
//  Copyright © 2018 peerjet. All rights reserved.
//

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

uint32_t Node::addFriendNoRequest(const uint8_t *pubKey)
{
    return 0;
}

bool Node::removeFriend(uint32_t friendNumber)
{
    return true;
}

uint32_t Node::getFriendByPublicKey(const uint8_t *pubKey)
{
    return true;
}

bool Node::friendExists(uint32_t friendNumber)
{
    return true;
}

size_t Node::friendListSize()
{
    return 0;
}


