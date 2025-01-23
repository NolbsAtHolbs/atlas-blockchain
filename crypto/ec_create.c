#include "hblk_crypto.h"

/**
 * ec_create - Creates a new EC key pair using the secp256k1 curve.
 * Return: pointer to an EC_KEY structure containing both public and private
 *         keys, or NULL upon failure.
 */
EC_KEY *ec_create(void)
{
	EC_KEY *key = NULL;
	/* create new EC_KEY object by specifying the curve */
	key = EC_KEY_new_by_curve_name(NID_secp256k1);
	if (!key)
		return (NULL);
	if (!EC_KEY_generate_key(key))
	{ /* generate the public and private key pair */
		EC_KEY_free(key);
		return (NULL);
	}
	return (key);
}
