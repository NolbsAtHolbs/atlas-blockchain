#include "transaction.h"

int find_utxo(void *node, void *input);
int validate_input(llist_node_t node, unsigned int index, void *ctx);

/**
 * find_utxo - finds a matching utxo for a transaction input
 * @node: utxo node to compare
 * @index: unused
 * @input: transaction input to match
 * Return: 0 if no match, 1 if match
 */
int find_utxo(void *node, void *input)
{
	unspent_tx_out_t *utxo = node;
	tx_in_t *tx_input = input;

	if (!memcmp(utxo->block_hash, tx_input->block_hash, SHA256_DIGEST_LENGTH) &&
		!memcmp(utxo->tx_id, tx_input->tx_id, SHA256_DIGEST_LENGTH) &&
		!memcmp(utxo->out.hash, tx_input->tx_out_hash, SHA256_DIGEST_LENGTH))
		return (0);
	return (1);
}


/**
 * validate_input - validates a single input by checking utxo and signature
 * @node: input node
 * @index: unused
 * @ctx: context holding unspent outputs
 * Return: 0 if valid, 1 if invalid
 */
int validate_input(llist_node_t node, unsigned int index, void *ctx)
{
	tx_in_t *input = (tx_in_t *)node;
	llist_t *all_unspent = (llist_t *)ctx;
	unspent_tx_out_t *utxo;
	EC_KEY *sender;

	(void)index;
	if (!llist_find_node(all_unspent, find_utxo, input))
		return (1);
	/* find actual utxo to get the public key */
	utxo = llist_find_node(all_unspent, find_utxo, input);
	if (!utxo)
		return (1);
	sender = ec_from_pub(utxo->out.pub);
	if (!sender || !ec_verify(sender, input->tx_id, SHA256_DIGEST_LENGTH,
		&input->sig))
	{
		if (sender)
			EC_KEY_free(sender);
		return (1);
	}
	EC_KEY_free(sender);
	return (0);
}

/**
 * transaction_is_valid - verifies a transaction
 * @transaction: transaction to verify
 * @all_unspent: list of all unspent transaction outputs
 * Return: 1 if valid, 0 otherwise
 */
int transaction_is_valid(transaction_t const *transaction,
						 llist_t *all_unspent)
{
	uint8_t hash[SHA256_DIGEST_LENGTH];
	uint32_t total_in = 0, total_out = 0;
	int i;

	transaction_hash(transaction, hash);
	if (memcmp(hash, transaction->id, SHA256_DIGEST_LENGTH) != 0)
		return (0);
	if (!transaction || !all_unspent)
		return (0);
	if (!transaction_hash(transaction, hash) ||
		memcmp(hash, transaction->id, SHA256_DIGEST_LENGTH))
		return (0);
	/* validate all inputs */
	if (llist_for_each(transaction->inputs, validate_input, all_unspent))
		return (0);
	for (i = 0; i < llist_size(transaction->inputs); i++)
	{ /* sum inputs and outputs */
		tx_in_t *input = llist_get_node_at(transaction->inputs, i);
		unspent_tx_out_t *utxo = llist_find_node(
												all_unspent, find_utxo, input);
		if (!utxo)
			return (0);
		total_in += utxo->out.amount;
	}
	for (i = 0; i < llist_size(transaction->outputs); i++)
	{
		tx_out_t *output = llist_get_node_at(transaction->outputs, i);
		total_out += output->amount;
	}
	return (total_in == total_out); /* ensure total input = total output */
}
