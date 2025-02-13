#include "blockchain.h"

static const block_t GENESIS_BLOCK = {
	{ /* info */
		0,              /* index */
		0,              /* difficulty */
		1537578000,     /* timestamp */
		0,              /* nonce */
		{0}             /* prev_hash -> 32 bytes of 0 */
	},
	{ /* data */
		"Holberton School", /* buffer */
		16                  /* length */
	}, /* hash -> 32-byte SHA-256 result */
	ATLAS_SCHOOL_SHA256
};

/**
 * block_is_valid - Validates a block in the blockchain
 * @block: Pointer to the block to validate
 * @prev_block: Pointer to the previous block in the chain
 * Return: 0 if valid, -1 otherwise
 */
int block_is_valid(block_t const *block, block_t const *prev_block)
{
	uint8_t computed_hash[SHA256_DIGEST_LENGTH];
	uint8_t prev_hash_computed[SHA256_DIGEST_LENGTH];

	if (!block)
		return (-1);
	if (block->info.index == 0)
	{ /* genesis block checks: prev_block must be NULL */
		if (prev_block)
			return (-1);
		/* compare entire block to known GENESIS_BLOCK or compare fields */
		if (memcmp(block, &GENESIS_BLOCK, sizeof(GENESIS_BLOCK)) != 0)
			return (-1);
	}
	else
	{ /* non-genesis checks */
		if (!prev_block)
			return (-1);
		if (block->info.index != prev_block->info.index + 1)
			return (-1);
		/* recompute the prev_block’s hash and compare... */
		block_hash(prev_block, prev_hash_computed);
		if (memcmp(prev_block->hash, prev_hash_computed,
				   SHA256_DIGEST_LENGTH) != 0)
			return (-1);
		if (memcmp(block->info.prev_hash, prev_hash_computed,
				   SHA256_DIGEST_LENGTH) != 0)
			return (-1);
	}
	if (block->data.len > BLOCKCHAIN_DATA_MAX) /* data length check */
		return (-1);
	block_hash(block, computed_hash); /* recompute this block’s hash */
	if (memcmp(block->hash, computed_hash, SHA256_DIGEST_LENGTH) != 0)
		return (-1);
	return (0);
}
