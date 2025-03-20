#include "transaction.h"

int process_utxo(llist_node_t node, unsigned int index, void *ctx);
void create_outputs(transaction_t *tx, tx_context_t *context,
					EC_KEY const *receiver, uint32_t amount);
void *free_context(tx_context_t *context);
int sign_tx_input(llist_node_t node, unsigned int index, void *ctx);

/**
 * sign_tx_input - signs a tx input
 * @node: pointer to the transaction input node to sign
 * @index: unused index parameter
 * @ctx: pointer to the transaction context containing sender key and ID
 * Return: 0
 */
int sign_tx_input(llist_node_t node, unsigned int index, void *ctx)
{
	(void)index;
	tx_context_t *context = (tx_context_t *)ctx;
	tx_in_sign((tx_in_t *)node, context->tx_id, context->sender, context->selected_utxo);
	return (0);
}

/**
 * process_utxo - collects UTXOs and creates inputs
 * @node: UTXO node
 * @index: unused
 * @ctx: transaction context
 * Return: always 0
 */
int process_utxo(llist_node_t node, unsigned int index, void *ctx)
{
	tx_context_t *context = (tx_context_t *)ctx;
	unspent_tx_out_t *utxo = (unspent_tx_out_t *)node;
	tx_in_t *input;

	(void)index;

	if (!memcmp(utxo->out.pub, context->sender_pub, EC_PUB_LEN))
	{
		llist_add_node(context->selected_utxo, utxo, ADD_NODE_REAR);
		context->balance += utxo->out.amount;
		/* create input right after collecting UTXO */
		input = tx_in_create(utxo);
		if (input)
			llist_add_node(context->selected_utxo, input, ADD_NODE_REAR);
	}
	return (0);
}

/**
 * create_outputs - creates outputs for receiver and change (if needed)
 * @tx: transaction to add outputs to
 * @context: transaction context
 * @receiver: receiver's public key
 * @amount: amount to send
 */
void create_outputs(transaction_t *tx, tx_context_t *context,
					EC_KEY const *receiver, uint32_t amount)
{
	uint8_t receiver_pub[EC_PUB_LEN];

	/* create output to receiver */
	ec_to_pub(receiver, receiver_pub);
	llist_add_node(tx->outputs,
				   tx_out_create(amount, receiver_pub),
				   ADD_NODE_REAR);
	/* create change output if there's leftover balance */
	if (context->balance > amount)
		llist_add_node(tx->outputs,
				tx_out_create(context->balance - amount, context->sender_pub),
				ADD_NODE_REAR);
}

/**
 * free_context - frees transaction context memory
 * @context: transaction context
 * Return: always NULL
 */
void *free_context(tx_context_t *context)
{
	if (context)
	{
		if (context->selected_utxo)
			llist_destroy(context->selected_utxo, 0, NULL);
		free(context);
	}
	return (NULL);
}

/**
 * transaction_create - creates a new transaction
 * @sender: sender's private key
 * @receiver: receiver's public key
 * @amount: amount to send
 * @all_unspent: list of all unspent transaction outputs
 * Return: pointer to created transaction, or NULL on failure
 */
transaction_t *transaction_create(EC_KEY const *sender,
								  EC_KEY const *receiver,
								  uint32_t amount,
								  llist_t *all_unspent)
{
	transaction_t *tx = NULL;
	tx_context_t *context = NULL;

	if (!sender || !receiver || !amount || !all_unspent)
		return (NULL);
	context = calloc(1, sizeof(tx_context_t));
	if (!context)
		return (NULL);

	ec_to_pub(sender, context->sender_pub); /* setup tx context */

	context->sender = sender;
	context->balance = 0;
	context->selected_utxo = llist_create(MT_SUPPORT_FALSE);

	tx = calloc(1, sizeof(transaction_t)); /* collect UTXOs, create inputs */
	if (!tx)
		return (free_context(context), NULL);

	tx->inputs = llist_create(MT_SUPPORT_FALSE);
	llist_for_each(all_unspent, process_utxo, context);

	if (context->balance < amount) /* check balance */
		return (free_context(context), NULL);

	tx->outputs = llist_create(MT_SUPPORT_FALSE); /* create outputs */
	create_outputs(tx, context, receiver, amount);

	transaction_hash(tx, tx->id); /* compute tx ID, sign inputs */
	memcpy(context->tx_id, tx->id, SHA256_DIGEST_LENGTH);
	llist_for_each(tx->inputs, sign_tx_input, context);

	free_context(context);
	return (tx);
}
