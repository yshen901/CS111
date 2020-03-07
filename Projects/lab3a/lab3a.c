/*
  NAME: YUCI SHEN
  EMAIL: SHEN.YUCI11@GMAIL.COM
  ID: 604836772
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <math.h>
#include "ext2_fs.h"

int img_fd;
struct ext2_super_block superblock;
//struct ext2_group_desc group_descriptor;

uint32_t groups_count;
uint32_t blocks_count;
uint32_t inodes_count;
uint32_t blocks_size;
uint32_t inodes_size;
uint32_t blocks_per_group;
uint32_t inodes_per_group;
uint32_t first_free_inode;

/* ERROR HELPER */
void error_syscall(char* message) {
  fprintf(stderr, "%s", message);
  exit(1);
}

/* READS AND PRINTS SUPERBLOCK SUMMARY */
void super_block_summary() {
  pread(img_fd, &superblock, sizeof(superblock), 1024); // 1024 offset given in spec

  blocks_count = superblock.s_blocks_count;
  inodes_count = superblock.s_inodes_count;
  blocks_size = EXT2_MIN_BLOCK_SIZE << superblock.s_log_block_size;
  inodes_size = superblock.s_inode_size;
  blocks_per_group = superblock.s_blocks_per_group;
  inodes_per_group = superblock.s_inodes_per_group;
  first_free_inode = superblock.s_first_ino;

  groups_count = blocks_count/blocks_per_group;
  if (blocks_count % blocks_per_group)
    groups_count++;
  fprintf(stdout, "%d", groups_count);
  
  fprintf(stdout, "SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n",
	  blocks_count, inodes_count, blocks_size, inodes_size,
	  blocks_per_group, inodes_per_group, first_free_inode);
}

void group_summary(uint32_t group_number) {
  uint32_t num_of_blocks, num_of_inodes;
  struct ext2_group_desc group_descriptor;

  int offset = 1024 + blocks_size + group_number * sizeof(group_descriptor);
  pread(img_fd, &group_descriptor, sizeof(group_descriptor), offset);

  // Some groups might not have the full number of blocks/inodes
  if (group_number == groups_count - 1) {
    num_of_blocks = blocks_count - blocks_per_group * group_number;
    num_of_inodes = inodes_count - inodes_per_group * group_number;
  }
  else {
    num_of_blocks = blocks_per_group;
    num_of_inodes = inodes_per_group;
  }
  
  fprintf(stdout, "GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n",
	  group_number,
	  num_of_blocks,
	  num_of_inodes,
	  group_descriptor.bg_free_blocks_count,
	  group_descriptor.bg_free_inodes_count,
	  group_descriptor.bg_block_bitmap,
	  group_descriptor.bg_inode_bitmap,
	  group_descriptor.bg_inode_table);
}

void free_blocks_summary(uint32_t group_number) {
  printf("%d", group_number);
}

void free_inodes_summary(uint32_t group_number) {
  printf("%d", group_number);
}

void process_group(uint32_t group_number) {
  group_summary(group_number);
  //free_blocks_summary(group_number);
  //free_inodes_summary(group_number);
}

int main(int argc, char** argv) {
  static struct option long_options[] = {
    {0, 0, 0, 0}
  };

  if (getopt_long(argc, argv, "", long_options, 0) != -1)
    error_syscall("ERROR arguments are unsupported\n");

  if (argc != 2)
    error_syscall("ERROR we only take one argument\n");

  if ((img_fd = open(argv[1], O_RDONLY)) == -1)
    error_syscall("ERROR file cannot be opened\n");

  super_block_summary();
  uint32_t i = 0;
  for (i = 0; i < groups_count; i++)
    process_group(i);

  exit(0);
}
