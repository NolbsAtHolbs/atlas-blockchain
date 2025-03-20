#include "blockchain.h"

/**
 * hash_matches_difficulty - checks if a given hash meets a given difficulty
 * @hash: 32-byte SHA-256 hash
 * @difficulty: the desired number of leading 0 bits
 * Return: 1 if hash meets the difficulty, otherwise 0
 */
int hash_matches_difficulty(uint8_t const hash[SHA256_DIGEST_LENGTH],
							uint32_t difficulty)
{ /* POW; when difficulty = 20, theres 20 leading 0 bits in the 256-bit hash */
	uint32_t i;
	uint32_t bits = difficulty; /* tracks # of bits of 0 yet to be verified */
	uint8_t mask, byte;

	for (i = 0; i < SHA256_DIGEST_LENGTH && bits > 0; i++)
	{ /* loop through each byte in the hash as long as bits > 0 */
		byte = hash[i];

		if (bits >= 8)
		{ /* if >= 8 bits are needed, this whole byte must be 0 */
			if (byte != 0)
				return (0);
			bits -= 8;
		}
		else
		{ /* only bits (less than 8) must be zero in the top of this byte */
			mask = 1 << (8 - bits); /* 2^(8 - bits) */
			if (byte >= mask)
				return (0);
			bits = 0; /* always has leading bits */
		}
	}
	return (bits == 0);
}
