#include "hblk_crypto.h"

/**
 * ec_save - saves an EC key pair to a folder
 * @key: key pair to save
 * @folder: folder where key files will be created
 * Return: 1 on success, or 0 on failure
 */
int ec_save(EC_KEY *key, char const *folder)
{
	struct stat st;
	char path_priv[1024], path_pub[1024];
	FILE *fp;

	if (!key || !folder)
		return (0);
	/* if folder doesn't exist, make it */
	if (stat(folder, &st) == -1 || !S_ISDIR(st.st_mode))
		if (mkdir(folder, 0755) == -1)
			return (0);
	/* build file paths for private and public keys */
	snprintf(path_priv, sizeof(path_priv), "%s/%s", folder, "key.pem");
	snprintf(path_pub, sizeof(path_pub), "%s/%s", folder, "key_pub.pem");
	/* write PRIVATE KEY -> key.pem */
	fp = fopen(path_priv, "w");
	if (!fp)
		return (0);
	if (!PEM_write_ECPrivateKey(fp, key, NULL, NULL, 0, NULL, NULL))
	{
		fclose(fp);
		return (0);
	}
	fclose(fp);
	/* write PUBLIC KEY -> key_pub.pem */
	fp = fopen(path_pub, "w");
	if (!fp)
		return (0);
	if (!PEM_write_EC_PUBKEY(fp, key))
	{
		fclose(fp);
		return (0);
	}
	fclose(fp);
	return (1);
}
