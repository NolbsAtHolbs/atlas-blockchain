#include "blockchain.h"

/*
 * write_blocks - writes a single block to the file
 * @list: the block pointer (as a node of the linked list)
 * @index: unused here
 * @arg: the file pointer, passed in by llist_for_each
 * Return: 0 always (llist_for_each expects an int return)
 */
int write_blocks(llist_node_t list, unsigned int index, void *arg)
{
	block_t *tmp = NULL;
	char block_buf[1116]; /* 1024(data) + 56(info) + 4(data_len) + 32(hash) */
	uint32_t len = 0;

	(void)index; /* unused */
	tmp = (block_t *)list;

	/* if "END" = "\x02" (big-endian), swap it */
	if (!strcmp(END, "\x02"))
		SWAPENDIAN(list);

	/* copy fields into a local buffer - */
	/*  - block_info_t is 56 bytes */
	/*  - then write 4 bytes for 'data.len' */
	/*  - then 'data.len' bytes for data.buffer */
	/*  - then 32 bytes for the block hash */
	/* sum = 92 + data.len */
	len = tmp->data.len;
	memcpy(&block_buf[0], tmp, sizeof(block_info_t));
	memcpy(&block_buf[56], &tmp->data.len, 4);
	memcpy(&block_buf[60], tmp->data.buffer, len);
	memcpy(&block_buf[60 + len], tmp->hash, 32);

	fwrite(block_buf, 1, 92 + len, arg); /* write chunk to file pointr 'arg' */
	return (0);
}

/*
 * blockchain_serialize - writes the blockchain to file
 * @blockchain: the chain to serialize
 * @path: file path to write
 * Return: 1 on success, 0 on fail
 */
int blockchain_serialize(blockchain_t const *blockchain, char const *path)
{
	FILE *fptr = NULL;
	int blocknums = 0;
	char header[12] = {FILEHEADER};

	if (!blockchain || !path)
		return (0);
	/* blocknums is the total number of blocks in the chain. */
	/* stored into the header at offset 8 (4 bytes). */
	blocknums = llist_size(blockchain->chain);

	memcpy(&header[7], END, 1);  /* endianness byte */
	memcpy(&header[8], &blocknums, 4);

	fptr = fopen(path, "w"); /* consider wb instead of w */
	if (!fptr)
		return (0);
	fwrite(header, 1, 12, fptr);
	/* serialize each block by calling write_blocks on each node */
	llist_for_each(blockchain->chain, write_blocks, fptr);
	fclose(fptr);
	return (1);
}
