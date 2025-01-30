#include "hblk_crypto.h"

/**
 * ec_save - saves an EC key pair to a folder
 * @key: key pair to save
 * @folder: folder where key files will be created
 *
 * Return: 1 on success, or 0 on failure
 */
int ec_save(EC_KEY *key, char const *folder)
{
	struct stat info;
	char pub_path[1024], priv_path[1024];
	FILE *fptr;

	if (!key || !folder)
		return (0); /* if folder doesn't exist, make it */
	if (stat(folder, &info) == -1)
	{ /* folder doesn't exist or can't stat */
		if (mkdir(folder, 0755) == -1)
			return (0);
	}
	else if (!S_ISDIR(info.st_mode))
	{ /* path exists but isn't a directory */
		return (0);
	} /* build file paths safely with snprintf */
	snprintf(pub_path, sizeof(pub_path), "%s/%s", folder, "key.pem");
	snprintf(priv_path, sizeof(priv_path), "%s/%s", folder, "key_pub.pem");
	fptr = fopen(pub_path, "w"); /* write public key to PEM */
	if (!fptr)
		return (0);
	if (!PEM_write_EC_PUBKEY(fptr, key))
	{
		fclose(fptr);
		return (0);
	}
	fclose(fptr);
	fptr = fopen(priv_path, "w"); /* write private key to PEM */
	if (!fptr)
		return (0);
	if (!PEM_write_ECPrivateKey(fptr, key, NULL, NULL, 0, NULL, NULL))
	{
		fclose(fptr);
		return (0);
	}
	fclose(fptr);
	return (1);
}
