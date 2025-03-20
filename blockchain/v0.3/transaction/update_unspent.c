#include "transaction.h"

static int process_transaction(transaction_t *tx, unsigned int idx, ul_t *ctx);
static int clean_spent_outputs(tx_in_t *input, unsigned int idx, ul_t *ctx);
static int match_spent_output(unspent_tx_out_t *unspent, tx_in_t *input);
static int add_unspent_output(tx_out_t *output, unsigned int idx, ul_t *ctx);

/**
 * update_unspent - Updates the list of unspent transaction outputs
 * @transactions: list of valid transactions
 * @block_hash: hash of the block containing the transactions
 * @all_unspent: list of existing unspent outputs
 *
 * Return: pointer to the updated list of unspent transaction outputs
 */
llist_t *update_unspent(
	llist_t *transactions, uint8_t block_hash[SHA256_DIGEST_LENGTH],
	llist_t *all_unspent)
{
	ul_t ctx;

	if (!transactions || !all_unspent || !block_hash)
		return (NULL);

	ctx.unspent = all_unspent;
	memcpy(ctx.hash, block_hash, SHA256_DIGEST_LENGTH);

	/* Process each transaction */
	llist_for_each(transactions, (node_func_t)process_transaction, &ctx);

	return (all_unspent);
}

/**
 * process_transaction - Processes a transaction's inputs and outputs
 * @tx: transaction to process
 * @idx: index of transaction
 * @ctx: context containing unspent list and block info
 *
 * Return: 0 on success
 */
static int process_transaction(transaction_t *tx, unsigned int idx, ul_t *ctx)
{
	(void)idx;

	memcpy(ctx->tx_id, tx->id, SHA256_DIGEST_LENGTH);

	/* Remove spent outputs and add new ones */
	llist_for_each(tx->inputs, (node_func_t)clean_spent_outputs, ctx);
	llist_for_each(tx->outputs, (node_func_t)add_unspent_output, ctx);

	return (0);
}

/**
 * clean_spent_outputs - Removes spent outputs from unspent list
 * @input: transaction input
 * @idx: index of input
 * @ctx: context containing unspent list and block info
 *
 * Return: 0 on success
 */
static int clean_spent_outputs(tx_in_t *input, unsigned int idx, ul_t *ctx)
{
	(void)idx;

	llist_remove_node(ctx->unspent,
		(node_ident_t)(int (*)(void *, void *))match_spent_output,
		input, 1, NULL);

	return (0);
}

/**
 * match_spent_output - Checks if an output matches an input
 * @unspent: unspent transaction output
 * @input: transaction input
 *
 * Return: 1 if match, 0 otherwise
 */
static int match_spent_output(unspent_tx_out_t *unspent, tx_in_t *input)
{
	return (!memcmp(unspent->block_hash, input->block_hash, SHA256_DIGEST_LENGTH) &&
			!memcmp(unspent->tx_id, input->tx_id, SHA256_DIGEST_LENGTH) &&
			!memcmp(unspent->out.hash, input->tx_out_hash, SHA256_DIGEST_LENGTH));
}

/**
 * add_unspent_output - Adds a new unspent output to the list
 * @output: transaction output to add
 * @idx: index of output
 * @ctx: context containing unspent list and block info
 *
 * Return: 0 on success
 */
static int add_unspent_output(tx_out_t *output, unsigned int idx, ul_t *ctx)
{
	unspent_tx_out_t *new;

	(void)idx;

	new = unspent_tx_out_create(ctx->hash, ctx->tx_id, output);
	if (!new)
		return (1);

	llist_add_node(ctx->unspent, new, ADD_NODE_REAR);

	return (0);
}
