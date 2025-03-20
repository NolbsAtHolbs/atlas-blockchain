#include "blockchain.h"

/**
 * blockchain_create - creates a new Blockchain structure
 * Return: pointer to the created blockchain_t, or NULL on failure
 */
blockchain_t *blockchain_create(void)
{
	blockchain_t *bc = calloc(1, sizeof(*bc));
	block_t *block;

	/* allocate blockchain and create lists for chain and unspent outputs */
	if (!bc || !(bc->chain = llist_create(MT_SUPPORT_FALSE)) ||
		!(bc->unspent = llist_create(MT_SUPPORT_FALSE)))
		return (free(bc), NULL);

	block = calloc(1, sizeof(*block));
	if (!block) /* free everything if block allocation fails */
		return (llist_destroy(bc->chain, 0, NULL),
			llist_destroy(bc->unspent, 0, NULL), free(bc), NULL);

	/* initialize genesis block fields */
	block->info = (block_info_t){0, 0, 1537578000, 0, {0}};
	block->data.len = 16;
	memcpy(block->data.buffer, "Holberton School", 16); /* fixed 16-byte str */
	memcpy(block->hash, ATLAS_SCHOOL_SHA256, SHA256_DIGEST_LENGTH);

	/* add genesis block to chain; free on failure */
	if (llist_add_node(bc->chain, block, ADD_NODE_REAR) == -1)
		return (free(block), llist_destroy(bc->chain, 0, NULL),
			llist_destroy(bc->unspent, 0, NULL), free(bc), NULL);

	return (bc);
}
