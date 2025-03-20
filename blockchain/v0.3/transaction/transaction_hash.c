#include "transaction.h"

/**
 * transaction_hash - computes the id (hash) of a transaction
 * @transaction: pointer to the transaction to compute the hash of
 * @hash_buf: buffer in which to store the computed hash
 * Return: pointer to hash_buf, or NULL on failure
 */
uint8_t *transaction_hash(transaction_t const *transaction,
						  uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	size_t total_size = 0, offset = 0;
	tx_in_t *in;
	tx_out_t *out;
	uint8_t *buffer;
	int i;

	if (!transaction || !hash_buf)
		return (NULL);
	/* calculate buffer size (3 fixed-size fields) */
	total_size += llist_size(transaction->inputs) * (SHA256_DIGEST_LENGTH * 3);
	total_size += llist_size(transaction->outputs) * SHA256_DIGEST_LENGTH;

	buffer = malloc(total_size);
	if (!buffer)
		return (NULL);

	for (i = 0; i < llist_size(transaction->inputs); i++)
	{ /* copy inputs to buffer */
		in = llist_get_node_at(transaction->inputs, i);
		memcpy(buffer + offset, in->block_hash, SHA256_DIGEST_LENGTH);
		offset += SHA256_DIGEST_LENGTH;
		memcpy(buffer + offset, in->tx_id, SHA256_DIGEST_LENGTH);
		offset += SHA256_DIGEST_LENGTH;
		memcpy(buffer + offset, in->tx_out_hash, SHA256_DIGEST_LENGTH);
		offset += SHA256_DIGEST_LENGTH;
	}
	/* then copy outputs to buffer */
	for (i = 0; i < llist_size(transaction->outputs); i++)
	{
		out = llist_get_node_at(transaction->outputs, i);
		memcpy(buffer + offset, out->hash, SHA256_DIGEST_LENGTH);
		offset += SHA256_DIGEST_LENGTH;
	}
	/* hash the buffer */
	if (!sha256((int8_t const *)buffer, total_size, hash_buf))
	{
		free(buffer);
		return (NULL);
	}
	free(buffer);
	return (hash_buf);
}
