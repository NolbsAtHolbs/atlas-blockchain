#include "blockchain.h"

/**
 * blockchain_create - creates a new Blockchain structure
 * Return: pointer to the created blockchain_t, or NULL on failure
 */
blockchain_t *blockchain_create(void)
{
	blockchain_t *bc = NULL;
	block_t *block = NULL;

	bc = calloc(1, sizeof(*bc)); /* allocate blockchain structure */
	if (!bc)
		return (NULL);

	bc->chain = llist_create(MT_SUPPORT_FALSE); /* create block chain list */
	if (!bc->chain)
	{
		free(bc);
		return (NULL);
	}
	/* create unspent outputs list */
	bc->unspent = llist_create(MT_SUPPORT_FALSE);
	if (!bc->unspent)
	{
		llist_destroy(bc->chain, 0, NULL);
		free(bc);
		return (NULL);
	}

	block = calloc(1, sizeof(*block)); /* create genesis block */
	if (!block)
	{
		llist_destroy(bc->chain, 0, NULL);
		free(bc);
		return (NULL);
	}

	block->info.index = 0; /* initialize block info */
	block->info.difficulty = 0;
	block->info.timestamp = 1537578000;
	block->info.nonce = 0;
	memset(block->info.prev_hash, 0, SHA256_DIGEST_LENGTH);

	block->data.len = 16; /* initialize block data */
	memcpy(block->data.buffer, "Holberton School", 16); /* wrong text */

	/* set genesis block hash */
	memcpy(block->hash, ATLAS_SCHOOL_SHA256, SHA256_DIGEST_LENGTH);

	if (llist_add_node(bc->chain, block, ADD_NODE_REAR) == -1)
	{ /* add genesis block to chain */
		free(block);
		llist_destroy(bc->chain, 0, NULL);
		free(bc);
		return (NULL);
	}
	return (bc);
}
