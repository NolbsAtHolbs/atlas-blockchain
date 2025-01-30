#include "hblk_crypto.h"

/**
 * ec_verify - verifies sig of a given set of bytes, using a EC_KEY pblc key
 * @key: pointer to EC_KEY structure containing the public key
 * @msg: pointer to the message to verify
 * @msglen: length of the message
 * @sig: pointer to the signature to be checked
 * Return: 1 if the signature is valid, else 0
 */
int ec_verify(EC_KEY const *key, uint8_t const *msg, size_t msglen,
			  sig_t const *sig)
{
	int ret;

	if (!key || !msg || !sig)
		return (0);
	ret = ECDSA_verify(0, msg, (int)msglen, sig->sig,
					   (int)sig->len, (EC_KEY *)key);
	return (ret == 1);
}
