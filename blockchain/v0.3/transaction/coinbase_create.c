#include "transaction.h"

#define COINBASE_AMOUNT 50

/**
 * coinbase_create - creates a coinbase transaction
 * @receiver: receiver's public key
 * @block_index: index of the block the coinbase tx belongs to
 * Return: pointer to created transaction or NULL on failure
 */
transaction_t *coinbase_create(EC_KEY const *receiver, uint32_t block_index)
{
	transaction_t *tx = NULL;
	tx_in_t *input = NULL;
	tx_out_t *output = NULL;
	uint8_t pub[EC_PUB_LEN];

	if (!receiver)
		return (NULL);

	tx = calloc(1, sizeof(transaction_t)); /* allocate memory for tx */
	if (!tx)
		return (NULL);

		input = calloc(1, sizeof(tx_in_t)); /* create the coinbase input */
	if (!input)
		return (free(tx), NULL);

	/* set tx_out_hash with block_index */
	memcpy(input->tx_out_hash, &block_index, sizeof(block_index));

	/* add the input to the tx */
	tx->inputs = llist_create(MT_SUPPORT_FALSE);
	if (!tx->inputs || llist_add_node(tx->inputs, input, ADD_NODE_REAR) != 0)
		return (free(input), free(tx), NULL);

	ec_to_pub(receiver, pub); /* create the coinbase output */
	output = tx_out_create(COINBASE_AMOUNT, pub);
	if (!output)
		return (free(input), free(tx), NULL);

	tx->outputs = llist_create(MT_SUPPORT_FALSE); /* add output to tx */
	if (!tx->outputs || llist_add_node(tx->outputs, output, ADD_NODE_REAR) != 0)
		return (free(input), free(output), free(tx), NULL);

	transaction_hash(tx, tx->id); /* compute tx ID */
	return (tx);
}
