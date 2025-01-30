#include "hblk_crypto.h"

/**
 * ec_sign - signs a given set of bytes using a given EC_KEY private key
 * @key: pointer to the EC_KEY structure (private key) to use
 * @msg: pointer to the bytes to sign
 * @msglen: length of msg
 * @sig: pointer to sig_t struct to store the signature
 * Return: pointer to sig->sig upon success, or NULL upon failure
 */
uint8_t *ec_sign(EC_KEY const *key, uint8_t const *msg, size_t msglen, sig_t *sig)
{
	unsigned int sig_len;

	if (!key || !msg || !sig)
		return (NULL);
	sig_len = SIG_MAX_LEN;
	if (!ECDSA_sign(0, msg, msglen, sig->sig, &sig_len, (EC_KEY *)key))
		return (NULL);
	sig->len = (uint8_t)sig_len;
	return (sig->sig);
}
