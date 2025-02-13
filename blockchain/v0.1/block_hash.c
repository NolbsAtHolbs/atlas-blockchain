#include "blockchain.h"

/**
 * block_hash - computes the hash of a block's info and data
 * @block: pointer to the block to hash (unchanged)
 * @hash_buf: buffer where the resulting hash will be stored
 */
uint8_t *block_hash(block_t const *block,
					uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	uint8_t buf[sizeof(block->info) + BLOCKCHAIN_DATA_MAX];
	size_t len;

	if (!block || !hash_buf)
		return (NULL);

	/* copy the block info into buf first, add block->data content right after */
	memcpy(buf, &block->info, sizeof(block->info));

	/* 'data.len' informs how many bytes in 'data.buffer' to include */
	/* can't exceed BLOCKCHAIN_DATA_MAX when copying */
	len = sizeof(block->info) + block->data.len;
	memcpy(buf + sizeof(block->info), block->data.buffer, block->data.len);

	/* compute SHA-256 on that combined info+data, store result into 'hash_buf' */
	SHA256((const unsigned char *)buf, len, hash_buf);

	return (hash_buf);
}
