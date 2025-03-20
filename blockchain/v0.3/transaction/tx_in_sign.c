#include "transaction.h"

static unspent_tx_out_t *find_unspent_output(tx_in_t *in, llist_t *all_unspnt);

/**
 * find_unspent_output - finds unspent output matching input
 * @in: pointer to the transaction input
 * @all_unspent: list of all unspent transaction outputs
 * Return: pointer to matching unspent_tx_out_t, or NULL if not found
 */
static unspent_tx_out_t *find_unspent_output(tx_in_t *in, llist_t *all_unspnt)
{
	unspent_tx_out_t *unspent;
	int i, size;

	if (!in || !all_unspnt)
		return (NULL);
	size = llist_size(all_unspnt);
	for (i = 0; i < size; i++)
	{
		unspent = llist_get_node_at(all_unspnt, i);
		if (memcmp(unspent->out.hash, in->tx_out_hash,
					SHA256_DIGEST_LENGTH) == 0)
			return (unspent);
	}
	return (NULL);
}

/**
 * tx_in_sign - signs a transaction input
 * @in: pointer to the transaction input to sign
 * @tx_id: id of the transaction the input belongs to
 * @sender: pointer to the private key of the sender
 * @all_unspent: list of all unspent transaction outputs
 * Return: pointer to signature on success, NULL on failure
 */
sig_t *tx_in_sign(tx_in_t *in, uint8_t const tx_id[SHA256_DIGEST_LENGTH],
				  EC_KEY const *sender, llist_t *all_unspent)
{
	unspent_tx_out_t *unspent;
	uint8_t pub[EC_PUB_LEN];

	if (!in || !tx_id || !sender || !all_unspent)
		return (NULL);
	unspent = find_unspent_output(in, all_unspent);
	if (!unspent)
		return (NULL);
	if (!ec_to_pub(sender, pub)) /* get public key from sender's private key */
		return (NULL);
	if (memcmp(pub, unspent->out.pub, EC_PUB_LEN) != 0)
		return (NULL); /* check if public key matches the one in the output */
	if (!ec_sign(sender, tx_id, SHA256_DIGEST_LENGTH, &in->sig))
		return (NULL); /* sign the transaction id */
	return (&in->sig);
}
