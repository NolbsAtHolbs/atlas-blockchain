#include "transaction.h"

#define COINBASE_AMOUNT 50

/**
 * coinbase_is_valid - verifies that a coinbase transaction is valid
 * @coinbase: coinbase transaction to validate
 * @block_index: index of the block the coinbase transaction belongs to
 * Return: 1 if the coinbase transaction is valid, otherwise 0
 */
int coinbase_is_valid(transaction_t const *coinbase, uint32_t block_index)
{
	uint8_t computed_hash[SHA256_DIGEST_LENGTH];
	tx_in_t *input;
	tx_out_t *output;

	if (!coinbase)
		return (0);

	/* tx ID must match computed hash */
	transaction_hash(coinbase, computed_hash);
	if (memcmp(coinbase->id, computed_hash, SHA256_DIGEST_LENGTH) != 0)
		return (0);

	/* tx must have exactly 1 input and 1 output */
	if (llist_size(coinbase->inputs) != 1 || llist_size(coinbase->outputs) != 1)
		return (0);

	/* retrieve input and output */
	input = llist_get_node_at(coinbase->inputs, 0);
	output = llist_get_node_at(coinbase->outputs, 0);

	if (!input || !output)
		return (0);

	/* first 4 bytes of tx_out_hash must match the block index */
	if (memcmp(input->tx_out_hash, &block_index, sizeof(block_index)) != 0)
		return (0);

	/* block_hash, tx_id, and signature must be zeroed */
	if (memcmp(input->block_hash, (uint8_t[SHA256_DIGEST_LENGTH]){0},
								   SHA256_DIGEST_LENGTH) != 0 ||
		memcmp(input->tx_id, (uint8_t[SHA256_DIGEST_LENGTH]){0},
							  SHA256_DIGEST_LENGTH) != 0 ||
		memcmp(&input->sig, &(sig_t){0}, sizeof(sig_t)) != 0)
		return (0);

	if (output->amount != COINBASE_AMOUNT)
		return (0);
	return (1);
}
