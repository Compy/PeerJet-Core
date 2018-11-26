//
//  Crypto.cpp
//  PeerJet
//
//  Created by Compy on 11/24/18.
//  Copyright © 2018 peerjet. All rights reserved.
//

#include "Crypto.hpp"
#include "NetworkService.hpp"

uint8_t Crypto::comparePublicKeys(const uint8_t *pk1, const uint8_t *pk2)
{
    return crypto_verify_32(pk1, pk2);
}

/*  return a random number.
 */
uint32_t Crypto::randomInt(void)
{
    uint32_t randnum;
    randombytes((uint8_t *)&randnum , sizeof(randnum));
    return randnum;
}

uint64_t Crypto::random64b(void)
{
    uint64_t randnum;
    randombytes((uint8_t *)&randnum, sizeof(randnum));
    return randnum;
}

/* Check if a Tox public key crypto_box_PUBLICKEYBYTES is valid or not.
 * This should only be used for input validation.
 *
 * return 0 if it isn't.
 * return 1 if it is.
 */
bool Crypto::isPublicKeyValid(const uint8_t *publicKey)
{
    if (publicKey[31] >= 128) /* Last bit of key is always zero. */
        return false;
    
    return true;
}

/* Precomputes the shared key from their public_key and our secret_key.
 * This way we can avoid an expensive elliptic curve scalar multiply for each
 * encrypt/decrypt operation.
 * enc_key has to be crypto_box_BEFORENMBYTES bytes long.
 */
void Crypto::encryptPrecompute(const uint8_t *publicKey, const uint8_t *secretKey, uint8_t *encKey)
{
    crypto_box_beforenm(encKey, publicKey, secretKey);
}

int Crypto::encryptDataSymmetric(const uint8_t *secretKey, const uint8_t *nonce, const uint8_t *plain, uint32_t length, uint8_t *encrypted)
{
    if (length == 0 || !secretKey || !nonce || !plain || !encrypted)
        return -1;
    
    uint8_t temp_plain[length + crypto_box_ZEROBYTES];
    uint8_t temp_encrypted[length + crypto_box_MACBYTES + crypto_box_BOXZEROBYTES];
    
    memset(temp_plain, 0, crypto_box_ZEROBYTES);
    memcpy(temp_plain + crypto_box_ZEROBYTES, plain, length); // Pad the message with 32 0 bytes.
    
    if (crypto_box_afternm(temp_encrypted, temp_plain, length + crypto_box_ZEROBYTES, nonce, secretKey) != 0)
        return -1;
    
    /* Unpad the encrypted message. */
    memcpy(encrypted, temp_encrypted + crypto_box_BOXZEROBYTES, length + crypto_box_MACBYTES);
    return length + crypto_box_MACBYTES;
}

int Crypto::decryptDataSymmetric(const uint8_t *secretKey, const uint8_t *nonce, const uint8_t *encrypted, uint32_t length,
                           uint8_t *plain)
{
    if (length <= crypto_box_BOXZEROBYTES || !secretKey || !nonce || !encrypted || !plain)
        return -1;
    
    uint8_t temp_plain[length + crypto_box_ZEROBYTES];
    uint8_t temp_encrypted[length + crypto_box_BOXZEROBYTES];
    
    memset(temp_encrypted, 0, crypto_box_BOXZEROBYTES);
    memcpy(temp_encrypted + crypto_box_BOXZEROBYTES, encrypted, length); // Pad the message with 16 0 bytes.
    
    if (crypto_box_open_afternm(temp_plain, temp_encrypted, length + crypto_box_BOXZEROBYTES, nonce, secretKey) != 0)
        return -1;
    
    memcpy(plain, temp_plain + crypto_box_ZEROBYTES, length - crypto_box_MACBYTES);
    return length - crypto_box_MACBYTES;
}

int Crypto::encryptData(const uint8_t *publicKey, const uint8_t *secretKey, const uint8_t *nonce,
                 const uint8_t *plain, uint32_t length, uint8_t *encrypted)
{
    if (!publicKey || !secretKey)
        return -1;
    
    uint8_t k[crypto_box_BEFORENMBYTES];
    encryptPrecompute(publicKey, secretKey, k);
    int ret = encryptDataSymmetric(k, nonce, plain, length, encrypted);
    sodium_memzero(k, sizeof k);
    return ret;
}

int Crypto::decryptData(const uint8_t *publicKey, const uint8_t *secretKey, const uint8_t *nonce,
                 const uint8_t *encrypted, uint32_t length, uint8_t *plain)
{
    if (!publicKey || !secretKey)
        return -1;
    
    uint8_t k[crypto_box_BEFORENMBYTES];
    encryptPrecompute(publicKey, secretKey, k);
    int ret = decryptDataSymmetric(k, nonce, encrypted, length, plain);
    sodium_memzero(k, sizeof k);
    return ret;
}


/* Increment the given nonce by 1. */
void Crypto::incrementNonce(uint8_t *nonce)
{
    /* FIXME use increment_nonce_number(nonce, 1) or sodium_increment (change to little endian)
     * NOTE don't use breaks inside this loop
     * In particular, make sure, as far as possible,
     * that loop bounds and their potential underflow or overflow
     * are independent of user-controlled input (you may have heard of the Heartbleed bug).
     */
    uint32_t i = crypto_box_NONCEBYTES;
    uint_fast16_t carry = 1U;
    
    for (; i != 0; --i) {
        carry += (uint_fast16_t) nonce[i - 1];
        nonce[i - 1] = (uint8_t) carry;
        carry >>= 8;
    }
}
/* increment the given nonce by num */
void Crypto::incrementNonceNumber(uint8_t *nonce, uint32_t hostOrderNum)
{
    /* NOTE don't use breaks inside this loop
     * In particular, make sure, as far as possible,
     * that loop bounds and their potential underflow or overflow
     * are independent of user-controlled input (you may have heard of the Heartbleed bug).
     */
    const uint32_t big_endian_num = htonl(hostOrderNum);
    const uint8_t *const num_vec = (const uint8_t *) &big_endian_num;
    uint8_t num_as_nonce[crypto_box_NONCEBYTES] = {0};
    num_as_nonce[crypto_box_NONCEBYTES - 4] = num_vec[0];
    num_as_nonce[crypto_box_NONCEBYTES - 3] = num_vec[1];
    num_as_nonce[crypto_box_NONCEBYTES - 2] = num_vec[2];
    num_as_nonce[crypto_box_NONCEBYTES - 1] = num_vec[3];
    
    uint32_t i = crypto_box_NONCEBYTES;
    uint_fast16_t carry = 0U;
    
    for (; i != 0; --i) {
        carry += (uint_fast16_t) nonce[i - 1] + (uint_fast16_t) num_as_nonce[i - 1];
        nonce[i - 1] = (unsigned char) carry;
        carry >>= 8;
    }
}

/* Fill the given nonce with random bytes. */
void Crypto::randomNonce(uint8_t *nonce)
{
    randombytes(nonce, crypto_box_NONCEBYTES);
}

/* Fill a key crypto_box_KEYBYTES big with random bytes */
void Crypto::newSymmetricKey(uint8_t *key)
{
    randombytes(key, crypto_box_KEYBYTES);
}

/* Gives a nonce guaranteed to be different from previous ones.*/
void Crypto::newNonce(uint8_t *nonce)
{
    randomNonce(nonce);
}

/* Create a request to peer.
 * send_public_key and send_secret_key are the pub/secret keys of the sender.
 * recv_public_key is public key of receiver.
 * packet must be an array of MAX_CRYPTO_REQUEST_SIZE big.
 * Data represents the data we send with the request with length being the length of the data.
 * request_id is the id of the request (32 = friend request, 254 = ping request).
 *
 *  return -1 on failure.
 *  return the length of the created packet on success.
 */
int Crypto::createRequest(const uint8_t *sendPublicKey, const uint8_t *sendSecretKey, uint8_t *packet,
                   const uint8_t *recvPublicKey, const uint8_t *data, uint32_t length, uint8_t requestId)
{
    if (!sendPublicKey || !packet || !recvPublicKey || !data)
        return -1;
    
    if (MAX_CRYPTO_REQUEST_SIZE < length + 1 + crypto_box_PUBLICKEYBYTES * 2 + crypto_box_NONCEBYTES + 1 +
        crypto_box_MACBYTES)
        return -1;
    
    uint8_t *nonce = packet + 1 + crypto_box_PUBLICKEYBYTES * 2;
    newNonce(nonce);
    uint8_t temp[MAX_CRYPTO_REQUEST_SIZE]; // FIXME sodium_memzero before exit function
    memcpy(temp + 1, data, length);
    temp[0] = requestId;
    int len = encryptData(recvPublicKey, sendSecretKey, nonce, temp, length + 1,
                           1 + crypto_box_PUBLICKEYBYTES * 2 + crypto_box_NONCEBYTES + packet);
    
    if (len == -1)
        return -1;
    
    packet[0] = NET_PACKET_CRYPTO;
    memcpy(packet + 1, recvPublicKey, crypto_box_PUBLICKEYBYTES);
    memcpy(packet + 1 + crypto_box_PUBLICKEYBYTES, sendPublicKey, crypto_box_PUBLICKEYBYTES);
    
    return len + 1 + crypto_box_PUBLICKEYBYTES * 2 + crypto_box_NONCEBYTES;
}

/* Puts the senders public key in the request in public_key, the data from the request
 * in data if a friend or ping request was sent to us and returns the length of the data.
 * packet is the request packet and length is its length.
 *
 *  return -1 if not valid request.
 */
int Crypto::handleRequest(const uint8_t *selfPublicKey, const uint8_t *selfSecretKey, uint8_t *publicKey, uint8_t *data,
                   uint8_t *requestId, const uint8_t *packet, uint16_t length)
{
    if (!selfPublicKey || !publicKey || !data || !requestId || !packet)
        return -1;
    
    if (length <= crypto_box_PUBLICKEYBYTES * 2 + crypto_box_NONCEBYTES + 1 + crypto_box_MACBYTES ||
        length > MAX_CRYPTO_REQUEST_SIZE)
        return -1;
    
    if (comparePublicKeys(packet + 1, selfPublicKey) != 0)
        return -1;
    
    memcpy(publicKey, packet + 1 + crypto_box_PUBLICKEYBYTES, crypto_box_PUBLICKEYBYTES);
    const uint8_t *nonce = packet + 1 + crypto_box_PUBLICKEYBYTES * 2;
    uint8_t temp[MAX_CRYPTO_REQUEST_SIZE]; // FIXME sodium_memzero before exit function
    int len1 = decryptData(publicKey, selfSecretKey, nonce,
                            packet + 1 + crypto_box_PUBLICKEYBYTES * 2 + crypto_box_NONCEBYTES,
                            length - (crypto_box_PUBLICKEYBYTES * 2 + crypto_box_NONCEBYTES + 1), temp);
    
    if (len1 == -1 || len1 == 0)
        return -1;
    
    requestId[0] = temp[0];
    --len1;
    memcpy(data, temp + 1, len1);
    return len1;
}
