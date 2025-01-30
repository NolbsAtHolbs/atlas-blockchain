#include "hblk_crypto.h"

/**
 * ec_load - loads an EC key pair from disk
 * @folder: path to the folder containing the key files
 * Return: pointer to the EC_KEY structure, or NULL on failure
 */
EC_KEY *ec_load(char const *folder)
{
	FILE *fp = NULL;
	char path[1024];
	EC_KEY *key = NULL;
	int len;

	if (!folder)
		return (NULL);
	len = snprintf(path, sizeof(path), "%s/key.pem", folder);
	if (len < 0 || len >= (int)sizeof(path))
		return (NULL);
	fp = fopen(path, "r");
	if (!fp)
		return (NULL);
	key = PEM_read_ECPrivateKey(fp, NULL, NULL, NULL);
	fclose(fp);
	return (key);
}
