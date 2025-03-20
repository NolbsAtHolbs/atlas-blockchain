#include "blockchain.h"

/**
 * blockchain_destroy - deletes an existing blockchain
 * @blockchain: pointer to the blockchain to delete
 */
void blockchain_destroy(blockchain_t *blockchain)
{
	if (!blockchain)
		return;

	/* frees each block by calling block_destroy on it. */
	/*'1' means "free each node's data", */
	/* and the (node_dtor_t) block_destroy function used does that. */
	llist_destroy(blockchain->chain, 1, (node_dtor_t)block_destroy);

	/* free unspent list */
	llist_destroy(blockchain->unspent, 1, free);

	free(blockchain);
}
