#include "blockchain.h"

#define HEADER_LENGTH 8 /* "HBLK0.1" + 1 endianness byte */

/**
 * free_blockchain_partial - frees all blocks, list, and blockchain
 * @bc: blockchain to free
 * @fp: file pointer to close
 */
static void free_blockchain_partial(blockchain_t *bc, FILE *fp)
{
	size_t sz;

	if (fp)
		fclose(fp);
	if (!bc)
		return;
	if (bc->chain)
	{
		sz = llist_size(bc->chain);
		while (sz--)
			block_destroy(llist_pop(bc->chain));
		llist_destroy(bc->chain, 0, NULL);
	}
	free(bc);
}

/**
 * read_one_block - reads a single block from file
 * @fp: file pointer
 * Return: pointer to new block, or NULL on error
 */
static block_t *read_one_block(FILE *fp)
{
	block_t *blk = calloc(1, sizeof(*blk));

	if (!blk)
		return (NULL);
	/* info (24 bytes), data.len (4), data.buffer, hash (32) */
	if (fread(&blk->info, sizeof(blk->info), 1, fp) != 1 ||
		fread(&blk->data.len, sizeof(uint32_t), 1, fp) != 1 ||
		fread(blk->data.buffer, blk->data.len, 1, fp) != 1 ||
		fread(blk->hash, SHA256_DIGEST_LENGTH, 1, fp) != 1)
	{
		free(blk);
		return (NULL);
	}
	return (blk);
}

/**
 * blockchain_deserialize - loads a blockchain from file
 * @path: file path
 * Return: pointer to new blockchain, or NULL on failure
 */
blockchain_t *blockchain_deserialize(char const *path)
{
	FILE *fp;
	blockchain_t *bc;
	char header[HEADER_LENGTH];
	uint32_t count, i;

	if (!path || !(fp = fopen(path, "rb")))
		return (NULL);
	/* reading the 8 bytes of header */
	if (fread(header, 1, HEADER_LENGTH, fp) != HEADER_LENGTH ||
		memcmp(header, "HBLK", 4) || memcmp(header + 4, "0.1", 3))
		return (fclose(fp), NULL);
	/* allocate blockchain structure and create linked list */
	bc = calloc(1, sizeof(*bc));
	if (!bc)
		return (fclose(fp), NULL);
	bc->chain = llist_create(MT_SUPPORT_FALSE);
	if (!bc->chain)
		return (free_blockchain_partial(bc, fp), NULL);
	if (fread(&count, sizeof(uint32_t), 1, fp) != 1)
		return (free_blockchain_partial(bc, fp), NULL);
	/* loop over count, read each block & add to chain */
	for (i = 0; i < count; i++)
	{
		block_t *blk = read_one_block(fp);

		if (!blk || llist_add_node(bc->chain, blk, ADD_NODE_REAR) == -1)
			return (block_destroy(blk),
				free_blockchain_partial(bc, fp),
				NULL);
	}
	fclose(fp);
	return (bc);
}
