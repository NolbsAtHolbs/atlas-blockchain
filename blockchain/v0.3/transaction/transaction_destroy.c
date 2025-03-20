#include "transaction.h"

/**
 * transaction_destroy - deallocates a transaction structure
 * @transaction: pointer to the transaction to delete
 */
void transaction_destroy(transaction_t *transaction)
{
	if (!transaction)
		return;

	/* free all inputs and outputs */
	llist_destroy(transaction->inputs, 1, free);
	llist_destroy(transaction->outputs, 1, free);

	/* free the transaction itself */
	free(transaction);
}
