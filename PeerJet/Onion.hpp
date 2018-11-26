//
//  Onion.hpp
//  PeerJet
//
//  Created by Compy on 11/24/18.
//  Copyright © 2018 peerjet. All rights reserved.
//

#ifndef Onion_hpp
#define Onion_hpp

#include <stdio.h>
#include "Node.hpp"
#include "Crypto.hpp"
#include "NetworkService.hpp"

#define ONION_MAX_PACKET_SIZE 1400

#define ONION_RETURN_1 (PEERJET_KEY_LENGTH + SIZE_IPPORT + crypto_box_MACBYTES)
#define ONION_RETURN_2 (crypto_box_NONCEBYTES + SIZE_IPPORT + crypto_box_MACBYTES + ONION_RETURN_1)
#define ONION_RETURN_3 (crypto_box_NONCEBYTES + SIZE_IPPORT + crypto_box_MACBYTES + ONION_RETURN_2)

#define ONION_SEND_BASE (PEERJET_KEY_LENGTH + SIZE_IPPORT + crypto_box_MACBYTES)
#define ONION_SEND_3 (crypto_box_NONCEBYTES + ONION_SEND_BASE + ONION_RETURN_2)
#define ONION_SEND_2 (crypto_box_NONCEBYTES + ONION_SEND_BASE*2 + ONION_RETURN_1)
#define ONION_SEND_1 (crypto_box_NONCEBYTES + ONION_SEND_BASE*3)

#define ONION_MAX_DATA_SIZE (ONION_MAX_PACKET_SIZE - (ONION_SEND_1 + 1))
#define ONION_RESPONSE_MAX_DATA_SIZE (ONION_MAX_PACKET_SIZE - (1 + ONION_RETURN_3))

#define ONION_PATH_LENGTH 3

typedef struct {
    uint8_t shared_key1[crypto_box_BEFORENMBYTES];
    uint8_t shared_key2[crypto_box_BEFORENMBYTES];
    uint8_t shared_key3[crypto_box_BEFORENMBYTES];
    
    uint8_t public_key1[PEERJET_KEY_LENGTH];
    uint8_t public_key2[PEERJET_KEY_LENGTH];
    uint8_t public_key3[PEERJET_KEY_LENGTH];
    
    NetworkAddress      ip_port1;
    uint8_t             node_public_key1[PEERJET_KEY_LENGTH];
    
    NetworkAddress      ip_port2;
    uint8_t             node_public_key2[PEERJET_KEY_LENGTH];
    
    NetworkAddress      ip_port3;
    uint8_t             node_public_key3[PEERJET_KEY_LENGTH];
    
    uint32_t path_num;
} OnionPath;

class Onion {
public:
private:
    //DHTService* dhtService;
    NetworkService* networkingService;
    uint8_t secretKey[PEERJET_KEY_LENGTH];
    uint64_t timestamp;
    int (*recv_1_function)(void *, NetworkAddress, const uint8_t *, uint16_t);
    void* callbackObject;
};


#endif /* Onion_hpp */
