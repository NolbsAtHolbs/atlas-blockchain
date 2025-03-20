#include "transaction.h"

/**
 * is_utxo_spent_by_transactions - checks if UTXO is spent by any input
 * @u: the UTXO
 * @transactions: list of transactions
 * Return: 1 if spent, 0 otherwise
 */
static int is_utxo_spent_by_transactions(unspent_tx_out_t *u,
					 llist_t *transactions)
{
	transaction_t *tx;
	tx_in_t *in;
	size_t i, j, tx_count, in_count;
	int spent = 0;

	tx_count = llist_size(transactions);
	for (i = 0; i < tx_count && !spent; i++)
	{
		tx = llist_get_node_at(transactions, i);
		if (!tx)
			continue;
		in_count = llist_size(tx->inputs);
		for (j = 0; j < in_count && !spent; j++)
		{
			in = llist_get_node_at(tx->inputs, j);
			if (!in)
				continue;
			if (!memcmp(in->block_hash, u->block_hash, SHA256_DIGEST_LENGTH) &&
			    !memcmp(in->tx_id, u->tx_id, SHA256_DIGEST_LENGTH) &&
			    !memcmp(in->tx_out_hash, u->out.hash, SHA256_DIGEST_LENGTH))
				spent = 1;
		}
	}
	return (spent);
}

/**
 * copy_keep_unspent - copy old UTXOs that are not spent
 * @all: old unspent
 * @txs: transactions
 * @dst: new unspent
 * Return: 0 on success, 1 on failure
 */
static int copy_keep_unspent(llist_t *all, llist_t *txs, llist_t *dst)
{
	unspent_tx_out_t *u, *u_copy;
	size_t i, count;
	int spent;

	count = llist_size(all);
	for (i = 0; i < count; i++)
	{
		u = llist_get_node_at(all, i);
		if (!u)
			continue;
		spent = is_utxo_spent_by_transactions(u, txs);
		if (spent)
			continue;
		u_copy = malloc(sizeof(*u_copy));
		if (!u_copy)
			return (1);
		memcpy(u_copy, u, sizeof(*u_copy));
		if (llist_add_node(dst, u_copy, ADD_NODE_REAR) == -1)
		{
			free(u_copy);
			return (1);
		}
	}
	return (0);
}

/**
 * add_new_utxos - add new UTXOs from each tx's outputs
 * @txs: transactions
 * @block_hash: block hash
 * @dst: new unspent
 * Return: 0 on success, 1 on failure
 */
static int add_new_utxos(llist_t *txs,
			 uint8_t block_hash[SHA256_DIGEST_LENGTH],
			 llist_t *dst)
{
	transaction_t *tx;
	tx_out_t *out;
	unspent_tx_out_t *u;
	size_t i, j, tx_count, out_count;

	tx_count = llist_size(txs);
	for (i = 0; i < tx_count; i++)
	{
		tx = llist_get_node_at(txs, i);
		if (!tx)
			continue;
		out_count = llist_size(tx->outputs);
		for (j = 0; j < out_count; j++)
		{
			out = llist_get_node_at(tx->outputs, j);
			if (!out)
				continue;
			u = unspent_tx_out_create(block_hash, tx->id, out);
			if (!u)
				return (1);
			if (llist_add_node(dst, u, ADD_NODE_REAR) == -1)
			{
				free(u);
				return (1);
			}
		}
	}
	return (0);
}

/**
 * update_unspent - updates the list of all unspent transaction outputs
 * @transactions: validated transactions
 * @block_hash: block hash
 * @all_unspent: current unspent
 * Return: new updated unspent, or NULL on failure
 */
llist_t *update_unspent(llist_t *transactions,
	uint8_t block_hash[SHA256_DIGEST_LENGTH],
	llist_t *all_unspent)
{
	llist_t *new_unspent;
	int err;

	new_unspent = llist_create(MT_SUPPORT_FALSE);
	if (!new_unspent)
		return (NULL);

	err = copy_keep_unspent(all_unspent, transactions, new_unspent);
	if (!err)
		err = add_new_utxos(transactions, block_hash, new_unspent);

	if (err)
	{
		llist_destroy(new_unspent, 1, free);
		return (NULL);
	}
	llist_destroy(all_unspent, 1, free);
	return (new_unspent);
}
