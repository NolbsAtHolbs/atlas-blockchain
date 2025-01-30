#include "hblk_crypto.h"

#define EC_PUB_LEN 65
#define EC_CURVE NID_secp256k1

/**
 * ec_from_pub - Creates EC_KEY structure from given public key
 * @pub: Pointer to uncompressed public key (65 bytes)
 * Return: Pointer to created EC_KEY structure or NULL on failure
 */
EC_KEY *ec_from_pub(uint8_t const pub[EC_PUB_LEN])
{
	EC_KEY *key = NULL;
	EC_POINT *point = NULL;
	const EC_GROUP *group;
	int ret;

	if (!pub)
		return (NULL);
	key = EC_KEY_new_by_curve_name(EC_CURVE);
	if (!key)
		return (NULL);
	group = EC_KEY_get0_group(key);
	if (group)
	{
		point = EC_POINT_new(group);
		if (point)
		{
			ret = EC_POINT_oct2point(group, point, pub, EC_PUB_LEN, NULL);
			if (ret == 1)
			{
				ret = EC_KEY_set_public_key(key, point);
				if (ret == 1)
				{
					EC_POINT_free(point);
					return (key);
				}
			}
			EC_POINT_free(point);
		}
	}
	EC_KEY_free(key);
	return (NULL);
}
