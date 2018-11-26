//
//  Utils.hpp
//  PeerJet
//
//  Created by Compy on 11/25/18.
//  Copyright © 2018 peerjet. All rights reserved.
//

#ifndef Utils_hpp
#define Utils_hpp

#include <cstdint>
#include <time.h>
#include <stdio.h>

class Utils {
public:
    static void updateUnixTime();
    static uint64_t getUnixTime();
    
private:
    static uint64_t unixTime;
    static uint64_t baseTime;
};

#endif /* Utils_hpp */
