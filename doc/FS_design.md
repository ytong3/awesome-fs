# Design of Awesome File System (AFS)

## Block
The whole vritual disk is divided to a number of blocks. Each block has 4K byte. Blocks can be used to store user's data, file metadata, etc.
A block is the smallest unit AFS can manage. Each block is addressable. The defult virtual disk is 100MB, thus the highest a block address is `100M/4k = 25600`, which can be well represented by a unsigned 16-bit long number;

## Data Region
The data region holds the user's data. Starting at AFS::DRStart.

## Inode
Inode is the data strucutre to keep bookkeepping information about a file. 
Each inode maps upto 10 blocks with direct pointers to blocks. Otherwise, indirect pointers are used such that it inode can now link to up to 10+4K/2 = 2K+10 blocks or approximately 8MB.
An inode has following fields (tentative),
name (20)
mode (2)
ctime (4)
blocks (2) //number of blocks the file occupies
block (20) //direct pointer, maps up to 10 block. 2*10 bytes




## The Inode Table
Several continuous blocks reserved for holding inodes. Together with the inode, the size of inode table determines the maximum number of files supports by the file system. It is given by `# of inode blocks*block_size/inode_size`.

We can work out the size of inode table as follows.

As previously discussed, we have 25600 blocks in total. We use the first block as the superblock. Then we consider the data bitmap, we need a bit for each block of the 100MB virtual disk. So we must have 25600 bits or 3.2KB for data bitmap, which requires just 1 block. For the same reason, we know that the inode bitmap also takes less than a block. By thining of the worst case, i.e. all files occupy only a block in the data region,  say x files, so x blocks in the data region,
and x inodes. We have fe 1+1+1+x* 

## data bitmap
A bitmap used to indicate the availability of the data region, in terms of block.

## inode bitmap
A bitmap used to indicate the availability of the inode in terms of inode.

## superblock
The super block contains information about this particular file system, including, for example, the number of inodes and data blocks in the file system, wehere the inode table, data region begins, respectively, and so forth.


