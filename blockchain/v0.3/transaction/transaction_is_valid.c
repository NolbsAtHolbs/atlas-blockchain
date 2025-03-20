#include "transaction.h"

static int verify_input(void *node, unsigned int index, void *ctx);
static int match_utxo(void *node, void *input);
static int sum_output_amount(void *node, unsigned int index, void *ctx);

/**
 * transaction_is_valid - checks whether a transaction is valid
 * @transaction: pointer to the transaction to verify
 * @all_unspent: list of all unspent transaction outputs to date
 * Return: 1 if the transaction is valid, otherwise 0
 */
int transaction_is_valid(transaction_t const *transaction, llist_t *all_unspent)
{
	uint8_t computed_hash[SHA256_DIGEST_LENGTH] = {0};
	tv_t context = {0};

	if (!transaction || !all_unspent)
		return (0);

	/* compute hash and compare with stored transaction ID */
	transaction_hash(transaction, computed_hash);
	if (memcmp(transaction->id, computed_hash, SHA256_DIGEST_LENGTH) != 0)
		return (0);

	memcpy(context.tx_id, transaction->id, SHA256_DIGEST_LENGTH);
	context.unspent = all_unspent;
	context.input = 0;
	context.output = 0;

	/* verify all inputs and sum input amount */
	if (llist_for_each(transaction->inputs, verify_input, &context) != 0)
		return (0);

	/* sum output amounts */
	llist_for_each(transaction->outputs, sum_output_amount, &context.output);

	/* inputs must match outputs */
	return (context.input == context.output);
}

/**
 * verify_input - verifies a transaction input
 * @node: pointer to the transaction input (as void *)
 * @index: unused
 * @ctx: transaction context (contains transaction ID and UTXO list)
 * Return: 0 if valid, otherwise 1
 */
static int verify_input(void *node, unsigned int index, void *ctx)
{
	tv_t *context = (tv_t *)ctx;
	tx_in_t *input = (tx_in_t *)node;
	unspent_tx_out_t *utxo = NULL;
	EC_KEY *key = NULL;

	(void)index;

	/* find matching UTXO */
	utxo = llist_find_node(context->unspent, match_utxo, input);
	if (!utxo)
		return (1);
	/* verify signature using the UTXO's public key */
	key = ec_from_pub(utxo->out.pub);
	if (!key || !ec_verify(key, context->tx_id, SHA256_DIGEST_LENGTH, &input->sig))
	{
		if (key)
			EC_KEY_free(key);
		return (1);
	}
	/* update input total */
	context->input += utxo->out.amount;

	EC_KEY_free(key);
	return (0);
}

/**
 * match_utxo - matches an unspent output to a transaction input
 * @node: unspent output node
 * @input: transaction input
 * Return: 1 if it matches, otherwise 0
 */
static int match_utxo(void *node, void *input)
{
	unspent_tx_out_t *utxo = (unspent_tx_out_t *)node;
	tx_in_t *in = (tx_in_t *)input;

	return (!memcmp(utxo->block_hash, in->block_hash, SHA256_DIGEST_LENGTH) &&
			!memcmp(utxo->tx_id, in->tx_id, SHA256_DIGEST_LENGTH) &&
			!memcmp(utxo->out.hash, in->tx_out_hash, SHA256_DIGEST_LENGTH));
}

/**
 * sum_output_amount - sums the amounts from all transaction outputs
 * @node: pointer to the transaction output (as void *)
 * @index: unused
 * @ctx: total output amount
 * Return: always 0
 */
static int sum_output_amount(void *node, unsigned int index, void *ctx)
{
	tx_out_t *output = (tx_out_t *)node;

	(void)index;

	*(uint32_t *)ctx += output->amount;
	return (0);
}
