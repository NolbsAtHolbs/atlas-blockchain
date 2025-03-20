#include "blockchain.h"

/**
 * block_destroy - frees the memory used by a block
 * @block: pointer to the block to delete
 */
void block_destroy(block_t *block)
{
	if (!block)
		return;

	/* destroy tx list */
	llist_destroy(block->transactions, 1, (node_dtor_t)transaction_destroy);

	/* can free allocated sub-objs before freeing block if applicable */
	free(block);
}
