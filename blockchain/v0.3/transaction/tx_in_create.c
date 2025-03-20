#include "transaction.h"

/**
 * tx_in_create - creates a new transaction input
 * @unspent: ptr to the unspent transaction output to convert to input
 * Return: ptr to the created transaction input upon success, else NULL
 */
tx_in_t *tx_in_create(unspent_tx_out_t const *unspent)
{
	tx_in_t *in;

	if (!unspent)
		return (NULL);
	in = calloc(1, sizeof(tx_in_t));
	if (!in)
		return (NULL);
	/* copy data from unspent transaction output */
	memcpy(in->block_hash, unspent->block_hash, SHA256_DIGEST_LENGTH);
	memcpy(in->tx_id, unspent->tx_id, SHA256_DIGEST_LENGTH);
	memcpy(in->tx_out_hash, unspent->out.hash, SHA256_DIGEST_LENGTH);
	memset(&in->sig, 0, sizeof(sig_t)); /* zero out the signature structure */
	return (in);
}
