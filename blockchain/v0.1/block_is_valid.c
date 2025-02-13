#include "blockchain.h"

/**
 * block_is_valid - Validates a block in the blockchain
 * @block: Pointer to the block to validate
 * @prev_block: Pointer to the previous block in the chain
 * Return: 0 if valid, -1 otherwise
 */
int block_is_valid(block_t const *block, block_t const *prev_block)
{
	uint8_t computed_hash[SHA256_DIGEST_LENGTH];

	int i;

	if (!block)
		return (-1);
	if (block->info.index == 0)
	{ /* validate genesis block */
		if (prev_block)
			return (-1);
		if (block->data.len != 16 ||
			memcmp(block->data.buffer, "Holberton School", 16) != 0)
			return (-1);
		for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
			if (block->info.prev_hash[i] != 0)
				return (-1);
		if (memcmp(block->hash, ATLAS_SCHOOL_SHA256,
				   SHA256_DIGEST_LENGTH) != 0)
			return (-1);
	}
	else
	{ /* validate non-genesis block */
		if (!prev_block)
			return (-1);
		if (block->info.index != prev_block->info.index + 1)
			return (-1);
		if (memcmp(block->info.prev_hash, prev_block->hash,
				   SHA256_DIGEST_LENGTH) != 0)
			return (-1);
	}

	if (block->data.len > BLOCKCHAIN_DATA_MAX)
		return (-1);
	block_hash(block, computed_hash); /* validate hash */
	if (memcmp(block->hash, computed_hash, SHA256_DIGEST_LENGTH) != 0)
		return (-1);

	return (0);
}
