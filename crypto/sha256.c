#include "hblk_crypto.h"

/*
* sha256 - computes the hash of a sequence of bytes
* @s: sequence of bytes to be hashed
* @len: number of bytes to hash in s
* @digest: storage for resulting hash
* Return: pointer to digest
*/
uint8_t *sha256(int8_t const *s, size_t len,
				uint8_t digest[SHA256_DIGEST_LENGTH])
{
	if (!digest)
		return (NULL);
	if (s)
		SHA256((unsigned char const *)s, len, digest);
	return (digest);
}
