#include "blockchain.h"

/**
 * blockchain_deserialize - loads a blockchain from a file
 * @path: path to the file
 *
 * Return: pointer to a new blockchain on success, or NULL on failure
 */
blockchain_t *blockchain_deserialize(char const *path)
{
	FILE *fp;
	blockchain_t *bc;
	block_t *blk;
	char header[8];
	uint32_t count, i;
	size_t r;

	if (!path)
		return (NULL);
	fp = fopen(path, "rb"); /* open in binary mode */
	if (!fp)
		return (NULL);
	/* check "HBLK0.1" in 1st 7 bytes, + endianess byte(?) */
	r = fread(header, 1, 8, fp);
	if (r != 8)
	{
		fclose(fp);
		return (NULL);
	}
	if (memcmp(header, "HBLK", 4) != 0 || memcmp(header + 4, "0.1", 3) != 0)
	{
		fclose(fp);
		return (NULL);
	}
	bc = calloc(1, sizeof(*bc)); /* allocate blockchain */
	if (!bc)
	{
		fclose(fp);
		return (NULL);
	}
	bc->chain = llist_create(MT_SUPPORT_FALSE);
	if (!bc->chain)
	{
		fclose(fp);
		free(bc);
		return (NULL);
	}
	if (fread(&count, 4, 1, fp) != 1)
	{ /* if reading num of blocks fails or is partial, return NULL. */
		fclose(fp);
		free(bc);
		return (NULL);
	}
	/* for each block, read: block_info_t (24 bytes), data.len (4 bytes) */
	/* data.buffer (data.len bytes), hash (32 bytes) */
	for (i = 0; i < count; i++)
	{
		blk = calloc(1, sizeof(*blk));
		if (!blk)
		{
			fclose(fp);
			while (llist_size(bc->chain))
				block_destroy(llist_pop(bc->chain));
			llist_destroy(bc->chain, 0, NULL);
			free(bc);
			return (NULL);
		}
		if (fread(&blk->info, sizeof(blk->info), 1, fp) != 1 ||
			fread(&blk->data.len, 4, 1, fp) != 1 ||
			fread(blk->data.buffer, blk->data.len, 1, fp) != 1 ||
			fread(blk->hash, SHA256_DIGEST_LENGTH, 1, fp) != 1)
		{
			block_destroy(blk);
			fclose(fp);
			while (llist_size(bc->chain))
				block_destroy(llist_pop(bc->chain));
			llist_destroy(bc->chain, 0, NULL);
			free(bc);
			return (NULL);
		}
		llist_add_node(bc->chain, blk, ADD_NODE_REAR);
	}
	fclose(fp);
	return (bc);
}
