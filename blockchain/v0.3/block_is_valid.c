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
 * @all_unspent: list of all unspent transaction outputs
 * Return: 0 if valid, -1 otherwise
 */
int block_is_valid(block_t const *block, block_t const *prev_block,
				   llist_t *all_unspent)
{
	uint8_t computed_hash[SHA256_DIGEST_LENGTH];
	uint8_t prev_hash_computed[SHA256_DIGEST_LENGTH];
	transaction_t *coinbase = NULL;
	size_t tx_count = 0;

	if (!block || !all_unspent)
		return (-1);

	if (block->info.index == 0)
	{ /* genesis block checks */
		if (prev_block)
			return (-1);
		if (memcmp(block, &GENESIS_BLOCK, sizeof(GENESIS_BLOCK)) != 0)
			return (-1);
	}
	else
	{ /* non-genesis block checks */
		if (!prev_block)
			return (-1);
		if (block->info.index != prev_block->info.index + 1)
			return (-1);
		block_hash(prev_block, prev_hash_computed);
		if (memcmp(prev_block->hash, prev_hash_computed,
			SHA256_DIGEST_LENGTH) != 0)
			return (-1);
		if (memcmp(block->info.prev_hash, prev_block->hash,
			SHA256_DIGEST_LENGTH) != 0)
			return (-1);
	} /* check that the block contains at least one tx */
	tx_count = llist_size(block->transactions);
	if (tx_count < 1)
		return (-1);
	coinbase = llist_get_head(block->transactions); /* 1st tx - valid coinbs */
	if (!coinbase || !coinbase_is_valid(coinbase, block->info.index))
		return (-1);
	if (llist_for_each(block->transactions, /* validate all other txs */
			(node_func_t)transaction_is_valid, all_unspent) != 0)
		return (-1);
	block_hash(block, computed_hash);
	if (memcmp(block->hash, computed_hash, SHA256_DIGEST_LENGTH) != 0)
		return (-1);
	if (!hash_matches_difficulty(block->hash, block->info.difficulty))
		return (-1);
	return (0);
}
