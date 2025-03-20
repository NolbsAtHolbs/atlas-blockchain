#include "blockchain.h"

/**
 * block_destroy - frees the memory used by a block
 * @block: pointer to the block to delete
 */
void block_destroy(block_t *block)
{
	if (!block)
		return;
	/* can free allocated sub-objs before freeing block if applicable */
	free(block);
}
