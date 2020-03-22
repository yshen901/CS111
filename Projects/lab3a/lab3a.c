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
struct ext2_group_desc group_descriptor;

/* Saves these values to make referencing them easier */
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
  
  fprintf(stdout, "SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n",
	  blocks_count, inodes_count, blocks_size, inodes_size,
	  blocks_per_group, inodes_per_group, first_free_inode);
}

/* PRINTS GROUP SUMMARRY */
void group_summary(uint32_t group_number) {
  uint32_t num_of_blocks, num_of_inodes;

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

/* PRINTS AN ENTRY FOR EACH FREE BLOCK */
void free_blocks_summary(uint32_t group_number) {
  char chunks[blocks_size];
  pread(img_fd, &chunks, blocks_size, 1024 + (group_descriptor.bg_block_bitmap-1)*blocks_size);
  uint32_t index = superblock.s_first_data_block + group_number * blocks_per_group;

  uint32_t i, j;
  char u_flags;
  for(i = 0; i < blocks_size; i++) {
    u_flags = chunks[i];
    for(j = 0; j < 8; j++) { //checks the first flag, then shifts the next flag so it's first
      if((u_flags & 1) == 0) 
	fprintf(stdout, "BFREE,%d\n", index);
      u_flags >>= 1;
      index++;
    }
  }
}

/* HELPER FUNCTION: CONVERTS A TIME_T OBJECT INTO A CSV OUTPUT */
void time_csv(time_t unformatted_time, char* formatted_time) {
  time_t temp_time = unformatted_time;
  struct tm temp_ts = *gmtime(&temp_time);
  strftime(formatted_time, 80, "%m/%d/%y %H:%M:%S", &temp_ts);
}

/* HELPER FUNCTION: PRINTS DIRECTORY ENTRY  */
void directory_summary(uint32_t block_number, uint32_t inode_number) {
  struct ext2_dir_entry directory;
  uint32_t offset = 1024 + (block_number - 1) * blocks_size;
  uint32_t i;

  for (i = 0; i < blocks_size; i += directory.rec_len) {
    pread(img_fd, &directory, sizeof(directory), i + offset);
    if (directory.inode == 0)
      continue;
    fprintf(stdout, "DIRENT,%d,%d,%d,%d,%d,'%s'\n",
	    inode_number,
	    i,
	    directory.inode,
	    directory.rec_len,
	    directory.name_len,
	    directory.name);
  }
}


/* HELPER FUNCTION TO RECURSIVELY OUTPUT INDIRECT BLOCKS */
void indirect_block_summary(uint32_t index, uint32_t inode_number, int type, int offset) {
  uint32_t num_blocks = blocks_size / sizeof(uint32_t);
  uint32_t blocks[num_blocks];
  uint32_t i;
  
  memset(blocks, 0, num_blocks);
  pread(img_fd, blocks, blocks_size, 1024 + (index - 1) * blocks_size);

  // Recursively generate the indirect blocks, making sure to keep track of offsets during recursion
  // and increment offset when necessary
  for (i = 0; i < num_blocks; i++) {
    if (blocks[i] == 0)
      continue;
    fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n",
	    inode_number,
	    type,
	    offset + i,
	    index,
	    blocks[i]);
    if (type == 2) {
      indirect_block_summary(blocks[i], inode_number, type-1, offset);
      offset += 256;
    }
    else if (type == 3) {
      indirect_block_summary(blocks[i], inode_number, type-1, offset);
      offset += 65536;
    }
  }
}

/* HELPER FUNCTION TO PRINT OUT ENTRIES FOR USED INODE BLOCKS */
void used_inode_summary(uint32_t index, uint32_t inode_number) {
  struct ext2_inode inode;
  char file_type;
  char ctime[18], mtime[18], atime[18];
  uint32_t i;
  
  pread(img_fd, &inode, sizeof(inode), group_descriptor.bg_inode_table * blocks_size + index * sizeof(inode));

  if (inode.i_links_count == 0 || inode.i_mode == 0)
    return;

  // Get file type and format times
  if ((inode.i_mode & 0xA000) == 0xA000)
    file_type = 's';
  else if ((inode.i_mode & 0x4000) == 0x4000)
    file_type = 'd';
  else if ((inode.i_mode & 0x8000) == 0x8000)
    file_type = 'f';
  else
    error_syscall("ERROR unsupported mode detected\n");
  
  time_csv(inode.i_ctime, ctime);
  time_csv(inode.i_mtime, mtime);
  time_csv(inode.i_atime, atime);

  fprintf(stdout, "INODE,%d,%c,%o,%d,%d,%d,%s,%s,%s,%d,%d",
	  inode_number,
	  file_type,
	  inode.i_mode & 0x0FFF,
	  inode.i_uid,
	  inode.i_gid,
	  inode.i_links_count,
	  ctime,
	  mtime,
	  atime,
	  inode.i_size,
	  inode.i_blocks);

  // Print the 15 block entries
  if (file_type == 'f' || file_type == 'd' || (file_type == 's' && inode.i_size > 60)) {
    for (i = 0; i < 15; i++)
      fprintf(stdout, ",%d", inode.i_block[i]);
  }
  fprintf(stdout, "\n");

  // Print directory entries
  for (i = 0; i < 12; i++) {
    if (inode.i_block[i] != 0 && file_type == 'd')
      directory_summary(inode.i_block[i], inode_number);
  }

  // Print indirect entries
  if (inode.i_block[12] != 0)
    indirect_block_summary(inode.i_block[12], inode_number, 1, 12);
  if (inode.i_block[13] != 0)
    indirect_block_summary(inode.i_block[13], inode_number, 2, 268);
  if (inode.i_block[14] != 0)
    indirect_block_summary(inode.i_block[14], inode_number, 3, 65804);
}

/* PRINTS ALL INODE ENTRIES (USED, UNUSED, DIRECTORY, INDIRECT)*/
void inodes_summary(uint32_t group_number) {
  uint32_t num_chunks = inodes_per_group / 8;
  char chunks[num_chunks];
  uint32_t index = group_number * inodes_per_group + 1;
  pread(img_fd, &chunks, num_chunks, 1024 + (group_descriptor.bg_inode_bitmap - 1)*blocks_size);

  uint32_t i, j;
  char u_flags; //contains flags, one bit for each block of data
  for (i = 0; i < num_chunks; i++) {
    u_flags = chunks[i];
    for (j = 0; j < 8; j++) {
      if((u_flags & 1) == 0)
	fprintf(stdout, "IFREE,%d\n", index);
      index++;
      u_flags >>= 1;
    }
  }

  index = group_number * inodes_per_group + 1;
  for (i = 0; i < num_chunks; i++) {
    u_flags = chunks[i];
    for (j = 0; j < 8; j++) {
      if((u_flags & 1) == 1)
        used_inode_summary(index, index - group_number * superblock.s_inodes_per_group + 1);
      index++;
      u_flags >>= 1;
    }
  }
}

/* RUN TO GET ALL OF THE ENTRIES FROM THE IMAGE */
void process_group(uint32_t group_number) {
  group_summary(group_number);
  free_blocks_summary(group_number);
  inodes_summary(group_number);
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
