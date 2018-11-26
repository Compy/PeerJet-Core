//
//  Crypto.hpp
//  PeerJet
//
//  Created by Compy on 11/24/18.
//  Copyright © 2018 peerjet. All rights reserved.
//

#ifndef Crypto_hpp
#define Crypto_hpp

#include <cstdint>
#include <cstring>
#include <stdio.h>

#ifndef VANILLA_NACL
/* We use libsodium by default. */
#include <sodium.h>
#else
#include <crypto_box.h>
#include <randombytes.h>
#include <crypto_hash_sha256.h>
#include <crypto_hash_sha512.h>
#include <crypto_verify_16.h>
#include <crypto_verify_32.h>
#include <crypto_scalarmult_curve25519.h>
#define crypto_box_MACBYTES (crypto_box_ZEROBYTES - crypto_box_BOXZEROBYTES)
/* I know */
#define sodium_memcmp(a, b, c) memcmp(a, b, c)
#define sodium_memzero(a, c) memset(a, 0, c)
#endif

#define crypto_box_KEYBYTES (crypto_box_BEFORENMBYTES)

#define MAX_CRYPTO_REQUEST_SIZE 1024

#define CRYPTO_PACKET_FRIEND_REQ    32  /* Friend request crypto packet ID. */
#define CRYPTO_PACKET_HARDENING     48  /* Hardening crypto packet ID. */
#define CRYPTO_PACKET_DHTPK         156
#define CRYPTO_PACKET_NAT_PING      254 /* NAT ping crypto packet ID. */

class Crypto {
public:
    static uint8_t comparePublicKeys(const uint8_t *pk1, const uint8_t *pk2);
    static uint32_t randomInt(void);
    static uint64_t random64b(void);
    static bool isPublicKeyValid(const uint8_t *publicKey);
    static int encryptData(const uint8_t *publicKey, const uint8_t *secretKey, const uint8_t *nonce,
                            const uint8_t *plain, uint32_t length, uint8_t *encrypted);
    static int decryptData(const uint8_t *publicKey, const uint8_t *secretKey, const uint8_t *nonce,
                     const uint8_t *encrypted, uint32_t length, uint8_t *plain);
    /* Fast encrypt/decrypt operations. Use if this is not a one-time communication.
     encrypt_precompute does the shared-key generation once so it does not have
     to be preformed on every encrypt/decrypt. */
    static void encryptPrecompute(const uint8_t *publicKey, const uint8_t *secretKey, uint8_t *encKey);
    
    /* Encrypts plain of length length to encrypted of length + 16 using a
     * secret key crypto_box_KEYBYTES big and a 24 byte nonce.
     *
     *  return -1 if there was a problem.
     *  return length of encrypted data if everything was fine.
     */
    static int encryptDataSymmetric(const uint8_t *secretKey, const uint8_t *nonce, const uint8_t *plain, uint32_t length,
                               uint8_t *encrypted);
    
    /* Decrypts encrypted of length length to plain of length length - 16 using a
     * secret key crypto_box_KEYBYTES big and a 24 byte nonce.
     *
     *  return -1 if there was a problem (decryption failed).
     *  return length of plain data if everything was fine.
     */
    static int decryptDataSymmetric(const uint8_t *secretKey, const uint8_t *nonce, const uint8_t *encrypted, uint32_t length,
                               uint8_t *plain);
    
    /* Increment the given nonce by 1. */
    static void incrementNonce(uint8_t *nonce);
    
    /* increment the given nonce by num */
    static void incrementNonceNumber(uint8_t *nonce, uint32_t hostOrderNum);
    
    /* Fill the given nonce with random bytes. */
    static void randomNonce(uint8_t *nonce);
    
    /* Fill a key crypto_box_KEYBYTES big with random bytes */
    static void newSymmetricKey(uint8_t *key);
    
    /*Gives a nonce guaranteed to be different from previous ones.*/
    static void newNonce(uint8_t *nonce);
    
    
    /* Create a request to peer.
     * send_public_key and send_secret_key are the pub/secret keys of the sender.
     * recv_public_key is public key of receiver.
     * packet must be an array of MAX_CRYPTO_REQUEST_SIZE big.
     * Data represents the data we send with the request with length being the length of the data.
     * request_id is the id of the request (32 = friend request, 254 = ping request).
     *
     * return -1 on failure.
     * return the length of the created packet on success.
     */
    static int createRequest(const uint8_t *sendPublicKey, const uint8_t *sendSecretKey, uint8_t *packet,
                       const uint8_t *recvPublicKey, const uint8_t *data, uint32_t length, uint8_t requestId);
    
    /* puts the senders public key in the request in public_key, the data from the request
     in data if a friend or ping request was sent to us and returns the length of the data.
     packet is the request packet and length is its length
     return -1 if not valid request. */
    static int handleRequest(const uint8_t *selfPublicKey, const uint8_t *selfSecretKey, uint8_t *publicKey, uint8_t *data, uint8_t *requestId, const uint8_t *packet, uint16_t length);
};

#endif /* Crypto_hpp */
