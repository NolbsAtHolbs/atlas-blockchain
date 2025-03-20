#include "blockchain.h"

/**
 * block_hash - computes the hash of a block's info, data, and transactions
 * @block: pointer to the block to hash (unchanged)
 * @hash_buf: buffer where the resulting hash will be stored
 * Return: the hashed buffer
 */
uint8_t *block_hash(block_t const *block,
					uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	uint8_t *buf;
	size_t len, tx_len = 0;
	transaction_t *tx;
	int i;

	if (!block || !hash_buf)
		return (NULL);

	/* compute size of all txs */
	for (i = 0; i < llist_size(block->transactions); i++)
	{
		tx = llist_get_node_at(block->transactions, i);
		if (tx)
			tx_len += sizeof(*tx);
	}

	/* allocate memory for block info + data + txs */
	len = sizeof(block->info) + block->data.len + tx_len;
	buf = malloc(len);
	if (!buf)
		return (NULL);

	/* copy block info and data into the buffer */
	memcpy(buf, &block->info, sizeof(block->info));
	memcpy(buf + sizeof(block->info), block->data.buffer, block->data.len);

	/* copy tx data into buffer */
	tx_len = sizeof(block->info) + block->data.len;
	for (i = 0; i < llist_size(block->transactions); i++)
	{
		tx = llist_get_node_at(block->transactions, i);
		if (tx)
			memcpy(buf + tx_len, tx, sizeof(*tx)), tx_len += sizeof(*tx);
	}

	/* compute SHA-256 on combined info + data + txs */
	SHA256((const unsigned char *)buf, len, hash_buf);
	free(buf);
	return (hash_buf);
}
