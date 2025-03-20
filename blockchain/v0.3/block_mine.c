#include "blockchain.h"

/**
 * block_mine - finds a nonce so that block->hash meets block->info.difficulty
 * @block: pointer to the block to be mined
 */
void block_mine(block_t *block)
{
	if (!block)
		return;
	block->info.nonce = 0;
	while (1)
	{
		block_hash(block, block->hash);
		if (hash_matches_difficulty(block->hash, block->info.difficulty))
			break;
		block->info.nonce++;
	}
}
