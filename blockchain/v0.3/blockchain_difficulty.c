#include "blockchain.h"

#define DIFF_ADJ_INT 5
#define BLK_GEN_INT 1

/**
 * blockchain_difficulty - computes the difficulty for the next block
 * @blockchain: pointer to the blockchain
 * Return: the difficulty to assign to the next block
 */
uint32_t blockchain_difficulty(blockchain_t const *blockchain)
{
	block_t *last, *adj;
	size_t size;
	uint32_t idx, exp_time, act_time, diff;

	if (!blockchain)
		return (0);
	size = llist_size(blockchain->chain);
	if (size == 0)
		return (0); /* vvv get most recent block vvv */
	last = llist_get_node_at(blockchain->chain, size - 1);
	if (!last)
		return (0);
	idx = last->info.index;
	if (idx == 0) /* if chain has only genesis block, return its difficulty */
		return (last->info.difficulty);
	/* if not at an adj boundary or too few blks, return last blk's difclty */
	if (idx % DIFF_ADJ_INT != 0 || idx < DIFF_ADJ_INT)
		return (last->info.difficulty);
	/* adjustment boundary - compare block at index = idx + 1 - DIFF_ADJ_INT */
	adj = llist_get_node_at(blockchain->chain,
		size - DIFF_ADJ_INT);
	if (!adj)
		return (last->info.difficulty);
	/* expected time = BLK_GEN_INT * DIFF_ADJ_INT */
	/* actual time = last->timestamp - adj->timestamp */
	exp_time = BLK_GEN_INT * DIFF_ADJ_INT;
	act_time = last->info.timestamp - adj->info.timestamp;
	diff = last->info.difficulty;
	/* if actual time < half the expected time -> difficulty++ */
	/* if actual time > double the expected time -> difficulty-- */
	if (act_time < (exp_time / 2))
		diff++;
	else if (act_time > (exp_time * 2))
	{ /* avoid underflow - diff shouldn't go below 0 */
		if (diff > 0)
			diff--;
	}
	return (diff);
}
