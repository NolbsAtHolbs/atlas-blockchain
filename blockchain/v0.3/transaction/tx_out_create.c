#include "transaction.h"

/**
 * tx_out_create - creates a new transaction output
 * @amount: amount to be assigned to the transaction output
 * @pub: receiver's public key
 * Return: ptr to created transaction output on success, else NULL
 */
tx_out_t *tx_out_create(uint32_t amount, uint8_t const pub[EC_PUB_LEN])
{
	tx_out_t *out;
	uint8_t data[sizeof(amount) + EC_PUB_LEN];

	out = malloc(sizeof(tx_out_t)); /* alloc mem for transaction output */
	if (!out)
		return (NULL);
	out->amount = amount; /* set amount, copy public key */
	memcpy(out->pub, pub, EC_PUB_LEN);
	memcpy(data, &amount, sizeof(amount)); /* hash prep */
	memcpy(data + sizeof(amount), pub, EC_PUB_LEN);
	if (!sha256(data, sizeof(data), out->hash))
	{ /* generate hash of the amount and pub */
		free(out);
		return (NULL);
	}
	return (out);
}

