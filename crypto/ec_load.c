#include "hblk_crypto.h"

/**
 * ec_load - loads an EC key pair from disk
 * @folder: path to the folder containing the key files
 * Return: pointer to the EC_KEY structure, or NULL on failure
 */
EC_KEY *ec_load(char const *folder)
{
	char path[1024];
	FILE *fp;
	EC_KEY *key = NULL, *tmp = NULL;

	if (!folder)
		return (NULL); /* read the PRIVATE KEY from: <folder>/key.pem */
	snprintf(path, sizeof(path), "%s/key.pem", folder);
	fp = fopen(path, "r");
	if (!fp)
		return (NULL);
	key = PEM_read_ECPrivateKey(fp, NULL, NULL, NULL);
	fclose(fp);
	if (!key)
		return (NULL); /* read the PUBLIC KEY from: <folder>/key_pub.pem */
	snprintf(path, sizeof(path), "%s/key_pub.pem", folder);
	fp = fopen(path, "r");
	if (!fp)
	{
		EC_KEY_free(key);
		return (NULL);
	}
	if (!PEM_read_EC_PUBKEY(fp, &tmp, NULL, NULL))
	{
		fclose(fp);
		EC_KEY_free(key);
		return (NULL);
	}
	fclose(fp);
	/* combine by setting public key from tmp into our prvt key structure */
	if (EC_KEY_set_public_key(key, EC_KEY_get0_public_key(tmp)) != 1)
	{
		EC_KEY_free(tmp);
		EC_KEY_free(key);
		return (NULL);
	} /* release temporary key; 'key' now holds both private + public parts */
	EC_KEY_free(tmp);
	return (key);
}
