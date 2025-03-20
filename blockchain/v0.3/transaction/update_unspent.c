#include "transaction.h"

static int remove_spent_unspent(void *node, void *arg)
{
	tx_in_t *input = (tx_in_t *)arg;
	unspent_tx_out_t *unspent = (unspent_tx_out_t *)node;

	if (!memcmp(unspent->tx_id, input->tx_id, SHA256_DIGEST_LENGTH) &&
	    !memcmp(unspent->out.hash, input->tx_out_hash, SHA256_DIGEST_LENGTH))
		return (1);
	return (0);
}

static int add_new_unspent(void *node, unsigned int index, void *arg)
{
	transaction_t *tx = (transaction_t *)node;
	llist_t *new_unspent = (llist_t *)arg;
	int i;
	unspent_tx_out_t *unspent;
	tx_out_t *out;

	(void)index; /* Suppress unused parameter warning */

	for (i = 0; i < llist_size(tx->outputs); i++)
	{
		out = llist_get_node_at(tx->outputs, i);
		if (!out)
			continue;
		unspent = unspent_tx_out_create(tx->id, tx->id, out);
		if (!unspent)
			return (-1);
		llist_add_node(new_unspent, unspent, ADD_NODE_REAR);
	}
	return (0);
}

/**
 * update_unspent - updates unspent transaction outputs
 * @transactions: list of validated transactions
 * @block_hash: hash of the validated block
 * @all_unspent: current list of unspent transaction outputs
 * Return: new list of unspent transaction outputs
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
