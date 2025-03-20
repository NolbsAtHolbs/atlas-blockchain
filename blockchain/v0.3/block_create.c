#include "blockchain.h"

/**
 * block_create - creates a new block in the chain
 * @prev: pointer to the previous block
 * @data: buffer of data to duplicate into the new block
 * @data_len: size of data to duplicate, capped by BLOCKCHAIN_DATA_MAX
 * Return: pointer to the newly allocated block, or NULL on failure
 */
block_t *block_create(block_t const *prev,
					  int8_t const *data, uint32_t data_len)
{
	block_t *block;

	/* allocate memory for the new block with calloc to zero-init the memory */
	block = calloc(1, sizeof(*block));
	if (!block)
		return (NULL);

	/* new block's index is prev block's index + 1 */
	block->info.index = prev->info.index + 1;
	block->info.difficulty = 0; /* difficulty starts at 0, adjustable */
	block->info.timestamp = (uint64_t)time(NULL); /* current unix time */
	block->info.nonce = 0; /* num used only once is 0 by default, adjustable */
	/* copy prev block's hash into new block's prev_hash, linking the chain */
	memcpy(block->info.prev_hash, prev->hash, SHA256_DIGEST_LENGTH);

	if (data_len > BLOCKCHAIN_DATA_MAX)
		data_len = BLOCKCHAIN_DATA_MAX; /* if data size xceeds max, clamp it */

	/* copy given data into block's buffer, set data len */
	memcpy(block->data.buffer, data, data_len);
	block->data.len = data_len;

	/* zero out new block's hash field so it's unset at creation */
	memset(block->hash, 0, SHA256_DIGEST_LENGTH);

	/* initialize tx list to an empty list */
	block->transactions = llist_create(MT_SUPPORT_FALSE);
	if (!block->transactions)
	{
		free(block);
		return (NULL);
	}

	return (block);
}
