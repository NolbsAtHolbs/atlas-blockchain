#include "blockchain.h"

/**
 * blockchain_create - creates a new Blockchain structure
 * Return: pointer to the created blockchain_t, or NULL on failure
 */
blockchain_t *blockchain_create(void)
{
	blockchain_t *bc;
	block_t *block;

	bc = calloc(1, sizeof(*bc)); /* allocate blockchain */
	if (!bc)
		return (NULL);

	/* create linked list for blocks */
	bc->chain = llist_create(MT_SUPPORT_FALSE);
	if (!bc->chain)
		return (free(bc), NULL);

	/* create linked list for unspent tx outputs */
	bc->unspent = llist_create(MT_SUPPORT_FALSE);
	if (!bc->unspent)
		return (llist_destroy(bc->chain, 0, NULL), free(bc), NULL);

	/* allocate memory for genesis block */
	block = calloc(1, sizeof(*block));
	if (!block)
		return (llist_destroy(bc->chain, 0, NULL),
				llist_destroy(bc->unspent, 0, NULL), free(bc), NULL);

	/* initialize genesis block info */
	block->info = (block_info_t){0, 0, 1537578000, 0, {0}};
	/* Initialize genesis block data */
	block->data.len = 16;
	memcpy(block->data.buffer, "Holberton School", 16);
	/* set predefined hash for genesis block */
	memcpy(block->hash, ATLAS_SCHOOL_SHA256, SHA256_DIGEST_LENGTH);

	/* add genesis block to chain */
	if (llist_add_node(bc->chain, block, ADD_NODE_REAR) == -1)
		return (free(block), llist_destroy(bc->chain, 0, NULL),
				llist_destroy(bc->unspent, 0, NULL), free(bc), NULL);

	return (bc);
}
