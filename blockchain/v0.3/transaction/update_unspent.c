#include "blockchain.h"

static int remove_spent_unspent(void *node, unsigned int idx, void *arg);
static int add_new_unspent(void *node, unsigned int idx, void *arg);

/**
 * add_new_unspent - adds new unspent transaction outputs to list
 * @node: current transaction being processed
 * @idx: index of the transaction
 * @arg: pointer to a struct containing block hash and new unspent list
 * Return: 0 on success, -1 on failure
 */
static int add_new_unspent(void *node, unsigned int idx, void *arg)
{
	transaction_t *tx = node;
	llist_t *new_unspent = ((llist_t **)arg)[0];
	uint8_t *block_hash = ((uint8_t **)arg)[1];
	unspent_tx_out_t *new_utxo;
	int i;

	(void)idx;

	for (i = 0; i < llist_size(tx->outputs); i++)
	{
		new_utxo = unspent_tx_out_create(block_hash, tx->id,
						 llist_get_node_at(tx->outputs, i));
		if (!new_utxo || llist_add_node(new_unspent, new_utxo,
						ADD_NODE_REAR) == -1)
		{
			free(new_utxo);
			return (-1);
		}
	}
	return (0);
}

/**
 * remove_spent_unspent - removes spent outputs from the list
 * @node: current unspent transaction output being checked
 * @idx: index of the unspent transaction output
 * @arg: list of validated transaction inputs
 * Return: 0 on success, 1 if the output should be removed
 */
static int remove_spent_unspent(void *node, unsigned int idx, void *arg)
{
	unspent_tx_out_t *utxo = node;
	llist_t *inputs = arg;
	tx_in_t *input;
	int i;

	(void)idx;

	for (i = 0; i < llist_size(inputs); i++)
	{
		input = llist_get_node_at(inputs, i);
		if (!memcmp(utxo->tx_id, input->tx_id, SHA256_DIGEST_LENGTH) &&
			!memcmp(utxo->block_hash, input->block_hash, SHA256_DIGEST_LENGTH))
			return (1);
	}
	return (0);
}

/**
 * update_unspent - Updates the list of all unspent transaction outputs
 * @transactions: List of validated transactions
 * @block_hash: Hash of the validated Block that contains the transaction list
 * @all_unspent: Current list of unspent transaction outputs
 * Return: A pointer to the new list of unspent transaction outputs
 */
llist_t *update_unspent(llist_t *transactions,
			uint8_t block_hash[SHA256_DIGEST_LENGTH],
			llist_t *all_unspent)
{
	llist_t *new_unspent;
	void *args[2];

	if (!transactions || !block_hash || !all_unspent)
		return (NULL);

	new_unspent = llist_create(MT_SUPPORT_FALSE);
	if (!new_unspent)
		return (NULL);

	/* remove spent UTXOs */
	llist_remove_node(all_unspent, remove_spent_unspent,
			  transactions, 1, (node_dtor_t)free);

	/* carry over remaining unspent outputs */
	llist_for_each(all_unspent, (node_func_t)llist_add_node,
		       new_unspent);

	/* add new UTXOs from processed txs */
	args[0] = new_unspent;
	args[1] = block_hash;
	llist_for_each(transactions, add_new_unspent, args);

	llist_destroy(all_unspent, 1, (node_dtor_t)free);

	return (new_unspent);
}
