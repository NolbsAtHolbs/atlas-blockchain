#include "hblk_crypto.h"

/**
 * ec_load - Loads an EC key pair from disk
 * @folder: Path to the folder containing the key files
 * Return: Pointer to the EC_KEY structure, or NULL on failure
 */
EC_KEY *ec_load(char const *folder)
{
	char path[1024];
	FILE *fp;
	EC_KEY *key = NULL;

	if (!folder)
		return (NULL); /* build the full path to <folder>/key.pem */
	snprintf(path, sizeof(path), "%s/%s", folder, "key.pem");
	fp = fopen(path, "r");
	if (!fp)
		return (NULL); /* read prvt key in PEM format; yields full key pair */
	key = PEM_read_ECPrivateKey(fp, NULL, NULL, NULL);
	fclose(fp);
	if (!key)
		return (NULL);
	return (key);
}
