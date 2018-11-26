//
//  Utils.cpp
//  PeerJet
//
//  Created by Compy on 11/25/18.
//  Copyright © 2018 peerjet. All rights reserved.
//

#include "Utils.hpp"
#include "NetworkService.hpp"

uint64_t Utils::unixTime = 0;
uint64_t Utils::baseTime = 0;

void Utils::updateUnixTime()
{
    if (Utils::baseTime == 0)
        Utils::baseTime = ((uint64_t)time(NULL) - (NetworkService::getCurrentTimeMonotonic() / 1000ULL));
    
    Utils::unixTime = (NetworkService::getCurrentTimeMonotonic() / 1000ULL) + Utils::baseTime;
}

uint64_t Utils::getUnixTime()
{
    return Utils::unixTime;
}
