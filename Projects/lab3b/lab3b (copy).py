#NAME: YUCI SHEN
#EMAIL: SHEN.YUCI11@GMAIL.COM
#ID: 604836772

import sys
import csv

# CLASSES TO REPLACE THE SUPERBLOCK, INODE, ETC STRUCTS IN C
class Superblock:
    def __init__(self, row):
        self.num_blocks = int(row[1])
        self.num_inodes = int(row[2])
        self.block_size = int(row[3])
        self.inode_size = int(row[4])
        self.blocks_per_group = int(row[5])
        self.inodes_per_group = int(row[6])
        self.first_unreserved_inode = int(row[7])

class Group:
    def __init__(self, row):
        self.group_num = int(row[1])
        self.num_blocks = int(row[2])
        self.num_inodes = int(row[3])
        self.num_free_blocks = int(row[4])
        self.num_free_inodes = int(row[5])
        self.free_block_bitmap = int(row[6])
        self.free_inode_bitmap = int(row[7])
        self.first_inode = int(row[8])

class Inode:
    def __init__(self, row):
        self.inode_num = int(row[1])
        self.file_type = row[2]
        self.mode = int(row[3])
        self.owner = int(row[4])
        self.group = int(row[5])
        self.link_count = int(row[6])
        self.last_change = row[7]
        self.last_modified = row[8]
        self.last_accessed = row[9]
        self.file_size = int(row[10])
        self.num_blocks = int(row[11])
        self.blocks = []
    def addBlock(node, blockNum):
        node.blocks.append(blockNum)

class Directory:
    def __init__(self, row):
        self.parent_inode = int(row[1])
        self.byte_offset = int(row[2])
        self.referenced_inode = int(row[3])
        self.entry_length = int(row[4])
        self.name_length = int(row[5])
        self.name = row[6]

class IndirectBlock:
    def __init__(self, row):
        self.parent_inode = int(row[1])
        self.level = int(row[2])
        self.logical_offset = int(row[3])
        self.block_num = int(row[4])
        self.referenced_block = int(row[5])

# CONTAINS CLASS OBJECTS THAT CONTAIN THE CSV DATA
groups = []
freeBlocks = []
freeInodes = []
inodes = []
directories = []
indirectBlocks = []

# CONTAINS BLOCK INFORMATION MAPPED TO BLOCK NUMBER
blocks = []

# TRACKS BLOCK NUMBERS
allocatedBlocks = set()
repeatedBlocks = set()

# TRACKS INODE NUMBERS
allocatedInodes = set()

# TRACKS DIRERCTORY MAPPINGS
parentDir = {}

# READ IN THE FILE INFORMATION
fileName = sys.argv[1]
try:
    fileInfo = open(fileName)
    fileCSV = csv.reader(fileInfo)
except:
    sys.stderr.write("ERROR reading csv file")
    sys.exit(1)

for row in fileCSV:
    entryType = row[0]
    if entryType == "SUPERBLOCK":
        superblock = Superblock(row)
    if entryType == "GROUP":
        groups.append(Group(row))
    if entryType == "BFREE":
        freeBlocks.append(int(row[1]))
    if entryType == "IFREE":
        freeInodes.append(int(row[1]))
    if entryType == "INODE":
        inode = Inode(row)
        inodes.append(inode)
        if inode.file_type == "f" or inode.file_type == "d" or (inode.file_type == "s" and inode.file_size > 60):
            for i in range(12, 27):
                inode.addBlock(int(row[i]))
    if entryType == "DIRENT":
        directories.append(Directory(row))
    if entryType == "INDIRECT":
        indirectBlocks.append(IndirectBlock(row))

# FIND ALLOCATION DATA FOR INODES
offsets = [0, 12, 268, 65804]
for inode in inodes:
    allocatedInodes.add(inode.inode_num)
    if len(inode.blocks) == 0:
        continue
    for i in range(0, 15):
        if inode.blocks[i] == 0:
            continue
        if i >= 12:
            offset = offsets[i - 11]
            indirect_level = i - 11
        else:
            offset = 0
            indirect_level = 0
        
        if inode.blocks[i] in allocatedBlocks:
            repeatedBlocks.add(inode.blocks[i])
        else:
            allocatedBlocks.add(inode.blocks[i])
            
        blocks.append({
            "block_num":inode.blocks[i],
            "indirect_level":indirect_level,
            "inode_num":inode.inode_num,
            "offset":offset
            })

# FIND ALLOCATION DATA FOR INDIRECT BLOCKS
for block in indirectBlocks:
        if block.referenced_block in allocatedBlocks:
            repeatedBlocks.add(block.referenced_block)
        else:
            allocatedBlocks.add(block.referenced_block)
            
        blocks.append({
            "block_num":block.block_num,
            "indirect_level":block.level,
            "inode_num":block.parent_inode,
            "offset":offsets[block.level]
            })
        
corrupted = False
# UNREFERENCED BLOCK
for block in range(1, superblock.num_blocks):
    if block in [0, 1, 2, 3, 4, 5, 6, 7, 64]: # RESERVED LOCATIONS
        continue
    if block not in freeBlocks and block not in allocatedBlocks:
        corrupted = True
        print("UNREFERENCED BLOCK " + str(block))

# UNALLOCATED INODE NOT ON FREELIST
for inode in range(superblock.first_unreserved_inode, superblock.num_inodes):
    if inode not in freeInodes and inode not in allocatedInodes:
        corrupted = True
        print("UNALLOCATED INODE " + str(inode) + " NOT ON FREELIST")

# ALLOCATED BLOCK IN FREELIST
for block in range(1, superblock.num_blocks):
    if block in [0, 1, 2, 3, 4, 5, 6, 7, 64]:
        continue
    if block in freeBlocks and block in allocatedBlocks:
        corrupted = True
        print("ALLOCATED BLOCK " + str(block) + " ON FREELIST")

# ALLOCATED INODE ON FREELIST
for inode in range(1, superblock.num_inodes):
    if inode in freeInodes and inode in allocatedInodes:
        corrupted = True
        print("ALLOCATED INODE " + str(inode) + " ON FREELIST")

# DUPLICATE BLOCK IN INODE
for repeatedBlock in repeatedBlocks:
    for block in blocks:
        if block["block_num"] == repeatedBlock:
            corrupted = True
            if block["indirect_level"] == 1:
                print("DUPLICATE INDIRECT BLOCK " + str(block["block_num"]) + " IN INODE " + str(block["inode_num"]) + " AT OFFSET " + str(block["offset"]))
            if block["indirect_level"] == 2:
                print("DUPLICATE DOUBLE INDIRECT BLOCK " + str(block["block_num"]) + " IN INODE " + str(block["inode_num"]) + " AT OFFSET " + str(block["offset"]))
            if block["indirect_level"] == 3:
                print("DUPLICATE TRIPLE INDIRECT BLOCK " + str(block["block_num"]) + " IN INODE " + str(block["inode_num"]) + " AT OFFSET " + str(block["offset"]))
            
# INVALID/RESERVED BLOCK IN INODE
for inode in inodes:
    for i in range(0, len(inode.blocks)):
        if inode.blocks[i] == 0:
            continue

        offset = 0
        type = ""
        if i >= 12:
            offset = offsets[i - 11]
        if i == 12:
            type = "INDIRECT"
        if i == 13:
            type = "DOUBLE INDIRECT"
        if i == 14:
            type = "TRIPLE INDIRECT"

        if inode.blocks[i] < 0 or inode.blocks[i] > superblock.num_blocks:
            print("INVALID " + type + " BLOCK " + str(indirectBlock.block_num) + " IN INODE " + str(inode) + " AT OFFSET " + str(offset))
        if inode.blocks[i] in [1, 2, 3, 4, 5, 6, 7, 64]:
            print("INVALID " + type + " BLOCK " + str(indirectBlock.block_num) + " IN INODE " + str(inode) + " AT OFFSET " + str(offset))

# INVALID/RESERVED INDIRECT BLOCKS
for indirectBlock in indirectBlocks:
    inode = indirectBlock.parent_inode
    offset = offsets[indirectBlock.level]
    if indirectBlock.level == 1:
        type = "INDIRECT"
    if indirectBlock.level == 2:
        type = "DOUBLE INDIRECT"
    if indirectBlock.level == 3:
        type = "TRIPLE INDIRECT"
    if indirectBlock.block_num < 0 or indirectBlock.block_num > superblock.num_blocks:
        print("INVALID " + type + " BLOCK " + str(indirectBlock.block_num) + " IN INODE " + str(inode) + " AT OFFSET " + str(offset))
    if indirectBlock.block_num in [0, 1, 2, 3, 4, 5, 6, 7, 64]:
        print("INVALID " + type + " BLOCK " + str(indirectBlock.block_num) + " IN INODE " + str(inode) + " AT OFFSET " + str(offset))

# CHECK FOR INVALID/UNALLOCATED DIRECTORY
for directory in directories:
    name = directory.name
    inode = directory.referenced_inode
    parent = directory.parent_inode
    if inode < 1 or inode > superblock.num_inodes:
        corrupted = True
        print("DIRECTORY INODE " + str(parent) + " NAME " + name  + " INVALID INODE " + str(inode))
    elif inode not in allocatedInodes:
        corrupted = True
        print("DIRECTORY INODE " + str(parent) + " NAME " + name  + " UNALLOCATED INODE " + str(inode))
    elif name == "'.'" or name == "'..'":
        continue
    else:
        parentDir[inode] = parent
        
# CHECK ALL LINKS
for inode in inodes:
    link_count = 0
    for directory in directories:
        if directory.referenced_inode == inode.inode_num:
            link_count += 1
    if link_count != inode.link_count:
        corrupted = True
        print("INODE " + str(inode.inode_num) + " HAS " + str(link_count) + " LINKS BUT LINKCOUNT IS " + str(inode.link_count))
            
