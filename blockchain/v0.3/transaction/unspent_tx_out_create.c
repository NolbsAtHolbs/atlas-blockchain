#include "transaction.h"

/**
 * unspent_tx_out_create - creates a new unspent transaction output
 * @block_hash: hash of the Block containing the transaction output
 * @tx_id: hash of the transaction containing the transaction output
 * @out: pointer to the referenced transaction output
 * Return: ptr to created unspent transaction output upon success, else NULL
 */
unspent_tx_out_t *unspent_tx_out_create(
					uint8_t block_hash[SHA256_DIGEST_LENGTH],
					uint8_t tx_id[SHA256_DIGEST_LENGTH], tx_out_t const *out)
{
	unspent_tx_out_t *unspent;

	if (!block_hash || !tx_id || !out)
		return (NULL);
	unspent = malloc(sizeof(unspent_tx_out_t));
	if (!unspent)
		return (NULL);
	/* copy block hash, tx ID, and tx output */
	memcpy(unspent->block_hash, block_hash, SHA256_DIGEST_LENGTH);
	memcpy(unspent->tx_id, tx_id, SHA256_DIGEST_LENGTH);
	memcpy(&unspent->out, out, sizeof(tx_out_t));
	return (unspent);
}
