#include "blockchain.h"

int append_tx_id(transaction_t *transaction,
								 unsigned int index, uint8_t *buffer);

/**
 * block_hash - computes the hash of a block's info and transaction IDs
 * @block: pointer to the block to hash
 * @hash_buf: buffer to store the resulting hash
 * Return: pointer to hash_buf, or NULL on failure
 */
uint8_t *block_hash(block_t const *block,
					uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	uint8_t *buffer = NULL;
	size_t block_size = 0, total_size = 0;
	size_t tx_count = 0;

	if (!block || !hash_buf)
		return (NULL);

	/* calculate block size and tx count */
	block_size = sizeof(block->info) + block->data.len;
	if (block->transactions)
		tx_count = llist_size(block->transactions);

	/* calculate total size to allocate */
	total_size = block_size + (tx_count * SHA256_DIGEST_LENGTH);

	buffer = calloc(1, total_size);
	if (!buffer)
		return (NULL);

	/* copy block info and data to buffer */
	memcpy(buffer, &block->info, sizeof(block->info));
	memcpy(buffer + sizeof(block->info), block->data.buffer, block->data.len);

	/* append tx IDs if present */
	if (tx_count)
		llist_for_each(block->transactions,
			(node_func_t)append_tx_id,
			buffer + block_size);

	SHA256(buffer, total_size, hash_buf); /* compute SHA256 hash */

	free(buffer);
	return (hash_buf);
}

/**
 * append_tx_id - appends transaction ID to buffer
 * @transaction: pointer to the transaction
 * @index: index of the transaction
 * @buffer: buffer to append the transaction ID
 * Return: always 0
 */
int append_tx_id(transaction_t *transaction, unsigned int index,
				 uint8_t *buffer)
{
	memcpy(buffer + (index * SHA256_DIGEST_LENGTH), transaction->id,
		   SHA256_DIGEST_LENGTH);
	return (0);
}
