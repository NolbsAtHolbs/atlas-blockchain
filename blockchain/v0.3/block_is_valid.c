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
	},
	NULL, /* Initialize transactions to NULL */
	ATLAS_SCHOOL_SHA256 /* Set hash */
};


static int validate_tx_wrapper(void *node, unsigned int index, void *arg)
{
	(void)index;
	return transaction_is_valid((transaction_t *)node, (llist_t *)arg);
}

/**
 * block_is_valid - checks the validity of a block
 * @block: pointer to block to validate
 * @prev_block: pointer to previous block
 * @all_unspent: list of all unspent transaction outputs
 * Return: 0 if valid, -1 otherwise
 */
int block_is_valid(block_t const *block, block_t const *prev_block,
				   llist_t *all_unspent)
{
	uint8_t computed_hash[SHA256_DIGEST_LENGTH];
	uint8_t prev_hash[SHA256_DIGEST_LENGTH];

	if (!block || !all_unspent)
		return (-1);

	if (block->info.index == 0)
	{ /* validate genesis block */
		if (prev_block || memcmp(block, &GENESIS_BLOCK,
				sizeof(GENESIS_BLOCK)))
			return (-1);
	}
	else
	{ /* validate regular block */
		if (!prev_block || block->info.index != prev_block->info.index + 1)
			return (-1);
		block_hash(prev_block, prev_hash);
		if (memcmp(prev_block->hash, prev_hash, SHA256_DIGEST_LENGTH) != 0 ||
			memcmp(block->info.prev_hash, prev_hash,
					SHA256_DIGEST_LENGTH) != 0)
			return (-1);
	}

	if (block->data.len > BLOCKCHAIN_DATA_MAX) /* check block data size */
		return (-1);

	block_hash(block, computed_hash); /* validate block hash */
	if (memcmp(block->hash, computed_hash, SHA256_DIGEST_LENGTH) != 0)
		return (-1);
	/* validate txs */
	if (!block->transactions || llist_size(block->transactions) < 1)
		return (-1);
	if (coinbase_is_valid(llist_get_head(block->transactions),
						  block->info.index) == 0)
		return (-1);
	if (llist_for_each(block->transactions, validate_tx_wrapper, all_unspent) != 0)
		return (-1);
	if (!hash_matches_difficulty(block->hash, block->info.difficulty))
		return (-1);
	return (0);
}
