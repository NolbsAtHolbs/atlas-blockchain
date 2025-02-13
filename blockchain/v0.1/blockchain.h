#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "provided/endianness.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <llist.h>
#include <openssl/sha.h>
#include <time.h>

#define BLOCKCHAIN_DATA_MAX 1024
#define ATLAS_SCHOOL_SHA256 \
"\xc5\x2c\x26\xc8\xb5\x46\x16\x39\x63\x5d\x8e\xdf\x2a\x97\xd4\x8d" \
"\x0c\x8e\x00\x09\xc8\x17\xf2\xb1\xd3\xd7\xff\x2f\x04\x51\x58\x03"
#define FILEHEADER "\x48\x42\x4c\x4b\x30\x2e\x31"
#define END "\x01"

/**
 * struct blockchain_s - Blockchain structure
 * @chain: Linked list of pointers to block_t
 */
typedef struct blockchain_s
{
	llist_t *chain;
} blockchain_t;

typedef struct block_info_s
{
	uint32_t    index;
	uint32_t    difficulty;
	uint64_t    timestamp;
	uint64_t    nonce;
	uint8_t     prev_hash[SHA256_DIGEST_LENGTH];
} block_info_t;

typedef struct block_data_s
{
	int8_t      buffer[BLOCKCHAIN_DATA_MAX];
	uint32_t    len;
} block_data_t;

/**
 * struct block_s - Block structure
 *
 * @info: Block info
 * @data: Block data
 * @hash: 256-bit digest of the Block, to ensure authenticity
 */
typedef struct block_s
{
	block_info_t    info; /* This must stay first */
	block_data_t    data; /* This must stay second */
	uint8_t     hash[SHA256_DIGEST_LENGTH];
} block_t;

blockchain_t *blockchain_create(void);
block_t *block_create(block_t const *prev,
					  int8_t const *data, uint32_t data_len);
void block_destroy(block_t *block);
void blockchain_destroy(blockchain_t *blockchain);
uint8_t *block_hash(block_t const *block,
					uint8_t hash_buf[SHA256_DIGEST_LENGTH]);
int blockchain_serialize(blockchain_t const *blockchain, char const *path);
blockchain_t *blockchain_deserialize(char const *path);
int block_is_valid(block_t const *block, block_t const *prev_block);

#endif
