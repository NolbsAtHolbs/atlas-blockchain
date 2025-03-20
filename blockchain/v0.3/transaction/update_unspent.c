#include "blockchain.h"

static int remove_spent_unspent(void *node, void *arg);
static int add_new_unspent(void *node, unsigned int idx, void *arg);

/**
 * add_new_unspent - adds new unspent transaction outputs to list
 * @node: current transaction being processed
 * @index: index of the transaction
 * @arg: pointer to a struct containing block hash and new unspent list
 * Return: 0 on success, -1 on failure
 */
static int add_new_unspent(void *node, unsigned int index, void *arg)
{
	transaction_t *tx = (transaction_t *)node;
	llist_t *new_unspent = (llist_t *)arg;
	int i;
	unspent_tx_out_t *unspent;

	for (i = 0; i < llist_size(tx->outputs); i++)
	{
		unspent = unspent_tx_out_create(((tx_out_t *)llist_get_node_at(tx->outputs, i)),
										tx->id, NULL);
		if (!unspent)
			return (-1);
		llist_add_node(new_unspent, unspent, ADD_NODE_REAR);
	}
	return (0);
}

/**
 * remove_spent_unspent - removes spent outputs from the list
 * @node: current unspent transaction output being checked
 * @arg: list of validated transaction inputs
 * Return: 0 on success, 1 if the output should be removed
 */
static int remove_spent_unspent(void *node, void *arg)
{
	tx_in_t *input = (tx_in_t *)arg;
	unspent_tx_out_t *unspent = (unspent_tx_out_t *)node;

	if (!memcmp(unspent->tx_id, input->tx_id, SHA256_DIGEST_LENGTH) &&
	    !memcmp(unspent->out.hash, input->tx_out_hash, SHA256_DIGEST_LENGTH))
		return (1);
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
	int i, j;
	transaction_t *tx;
	tx_in_t *input;

	if (!transactions || !block_hash || !all_unspent)
		return (NULL);

	new_unspent = llist_create(MT_SUPPORT_FALSE);
	if (!new_unspent)
		return (NULL);

	for (i = 0; i < llist_size(transactions); i++)
	{
		tx = llist_get_node_at(transactions, i);

		for (j = 0; j < llist_size(tx->inputs); j++)
		{
			input = llist_get_node_at(tx->inputs, j);
			llist_remove_node(all_unspent, (node_ident_t)remove_spent_unspent, input, 1, NULL);
		}
	}

	llist_for_each(transactions, (node_func_t)add_new_unspent, new_unspent);

	llist_destroy(all_unspent, 1, (node_dtor_t)free);
	return (new_unspent);
}
