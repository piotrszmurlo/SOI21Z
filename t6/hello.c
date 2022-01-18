#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

#define MAX_FILENAME_LENGTH 32
#define BLOCK_SIZE 512
#define BLOCK_COUNT 128
#define MAX_FILE_COUNT 6

typedef struct inode {
    char fileName[MAX_FILENAME_LENGTH];
    unsigned int fileSize;
    unsigned int firstDataBlock;

} inode;

typedef struct superblock {
    unsigned int filesystemSize;
    unsigned int blockSize;
    unsigned int usedBlocksCount;
    unsigned int freeBlocksCount;
    unsigned int inodeCount;
    inode inodes[MAX_FILE_COUNT]; 
    int used_inodes[MAX_FILE_COUNT]; 
    unsigned int blockMap[BLOCK_COUNT]

} superblock;

typedef struct block {
    char data[BLOCK_SIZE-4];
    unsigned int nextBlock;
} block;

superblock* gsb;



// typedef struct node {
//     inode data;
//     node* next;

// } node;

int find_free_block() {
    // read_superblock(fildes);
    for (int i = 0; i < BLOCK_COUNT; i++){
        if (!gsb->blockMap[i]) return i;
    }
    return -1;
}

int rewrite_superblock(int fildes) {
    lseek(fildes, 0, SEEK_SET);
    write(fildes, gsb, sizeof(superblock));
    return 0;
}
int create_vfs(int fildes) {
    gsb->filesystemSize = sizeof(superblock);
    gsb->blockSize = BLOCK_SIZE;
    gsb->usedBlocksCount = 0;
    gsb->freeBlocksCount = BLOCK_COUNT;
    gsb->inodeCount = 0;
    for (int i = 0; i < MAX_FILE_COUNT; i++){
        gsb->used_inodes[i] = 0;
    }
    for (int i = 0; i < BLOCK_COUNT; i++){
        gsb->blockMap[i] = 0;
    }
    rewrite_superblock(fildes);
    char buffer[BLOCK_SIZE] = {0};                

    for (int i = 0; i < BLOCK_COUNT; i++) {
        write(fildes, buffer, BLOCK_SIZE);
    }
    return 0;
}


int read_superblock(int fildes) {
    inode inodes[MAX_FILE_COUNT]; 
    int used_inodes[MAX_FILE_COUNT]; 
    lseek(fildes, 0, SEEK_SET);
    if(read(fildes, &gsb->filesystemSize, 4) == -1){
        printf("read error\n");
    }
    lseek(fildes, 4, SEEK_SET);
    read(fildes, &gsb->blockSize, sizeof(unsigned int));
    lseek(fildes, 8, SEEK_SET);
    read(fildes, &gsb->usedBlocksCount, sizeof(unsigned int));
    lseek(fildes, 12, SEEK_SET);
    read(fildes, &gsb->freeBlocksCount, sizeof(unsigned int));
    lseek(fildes, 16, SEEK_SET);
    read(fildes, &gsb->inodeCount, sizeof(unsigned int));
    int offset = 5*sizeof(unsigned int);
    lseek(fildes, offset, SEEK_SET);
    
    for (int i = 0; i < MAX_FILE_COUNT; i++){
        read(fildes, &(&gsb->inodes[i])->fileName, MAX_FILENAME_LENGTH*sizeof(char));
        read(fildes, &(&gsb->inodes[i])->fileSize, sizeof(unsigned int));
        read(fildes, &(&gsb->inodes[i])->firstDataBlock, sizeof(unsigned int));
    }
    offset = offset + sizeof(gsb->inodes);
    lseek(fildes, offset, SEEK_SET);
    read(fildes, &(gsb->used_inodes), MAX_FILE_COUNT*sizeof(int));
    read(fildes, gsb->blockMap, sizeof(gsb->blockMap));

}

int check_space(int fildes, unsigned int fileSize) {
    if (gsb->inodeCount > MAX_FILE_COUNT - 1){
        printf("error copying file: max files num on vdisk\n");
        return -1;
    }
    if ((gsb->freeBlocksCount)*BLOCK_SIZE < fileSize){
        printf("error copying file: not enough space on vdisk\n");
        return -1;
    }
    return 0;

}

int copy_file_to_vfs(int fildes, char* fileName) {
    struct stat stbuf;
    stat(fileName, &stbuf);
    int newFile = open(fileName, O_RDONLY, 0666);
    if (newFile == -1) {
        printf("error opening external file");
        return -1;
    }
    if (check_space(fildes, stbuf.st_size) == -1) return -1;
    // write file
    int i;
    for (i = 0; i < MAX_FILE_COUNT; i++){
        if (!gsb->used_inodes[i]){
            break;
        }
    }
    gsb->inodeCount++;
    inode* newInode = malloc(sizeof(inode));
    strncpy(newInode->fileName, fileName, MAX_FILENAME_LENGTH-1);
    newInode->fileSize = stbuf.st_size;
    //if not -1
    newInode->firstDataBlock = (unsigned int)find_free_block();
    block* newBlock = malloc(sizeof(block));
    gsb->blockMap[newInode->firstDataBlock] = (unsigned int)1;
    gsb->filesystemSize = gsb->filesystemSize + stbuf.st_size;
    for (int i = 0; i < MAX_FILE_COUNT; i++){
        if (!gsb->used_inodes[i]){
            gsb->inodes[i] = *newInode;
            break;
        }
    }
    gsb->used_inodes[i] = 1;
    lseek(fildes, (newInode->firstDataBlock)*BLOCK_SIZE + sizeof(superblock), SEEK_SET);
    int rd = read(newFile, newBlock->data, BLOCK_SIZE-4);
    while (rd)
    {
        if (rd < BLOCK_SIZE-4) {
            newBlock->nextBlock = -1;
        }
        else {
            newBlock->nextBlock = find_free_block();
            gsb->blockMap[newBlock->nextBlock] = 1;
        }
        gsb->usedBlocksCount++;
        gsb->freeBlocksCount--;
        write(fildes, newBlock, BLOCK_SIZE);
        lseek(fildes, newBlock->nextBlock*BLOCK_SIZE + sizeof(superblock), SEEK_SET);
        rd = read(newFile, newBlock->data, BLOCK_SIZE-4);
    }
    
    
    close(newFile);
    rewrite_superblock(fildes);
}

int print_files(int fildes){
    inode inodes[MAX_FILE_COUNT]; 
    int used_inodes[MAX_FILE_COUNT]; 
    int offset = 5*sizeof(unsigned int);
    lseek(fildes, offset, SEEK_SET);

    for (int i = 0; i < MAX_FILE_COUNT; i++){
        read(fildes, &(&inodes[i])->fileName, MAX_FILENAME_LENGTH*sizeof(char));
        read(fildes, &(&inodes[i])->fileSize, sizeof(unsigned int));
        read(fildes, &(&inodes[i])->firstDataBlock, sizeof(unsigned int));
    }
    offset = offset + sizeof(gsb->inodes);
    lseek(fildes, offset, SEEK_SET);
    read(fildes, &used_inodes, MAX_FILE_COUNT*sizeof(int));
    for (int i = 0; i < MAX_FILE_COUNT; i++){
        if (used_inodes[i])
        printf("file: %s(%d)\n", (&inodes[i])->fileName, (int)(&inodes[i])->fileSize);
    }
}

int print_block_map(int fildes) {
    read_superblock(fildes);
    printf("Filesystem total size: %d\n", gsb->filesystemSize);
    printf("Blocks used/free: %d/%d\n", gsb->usedBlocksCount, gsb->freeBlocksCount);
    printf("Block size: %d\n", gsb->blockSize);
    printf("|Block index : State|\n");
    for (int i = 1; i < BLOCK_COUNT+1; i++) {
        if (gsb->blockMap[i - 1]) {
            printf("|%d : used", i);
        }
        else {
            printf("|%d : free", i);
        }
        if (i%5 == 0) printf("\n");
    }
    printf("\n");
    return 0;
}

int copy_file_from_vfs(int fildes, char* fileName) {
    read_superblock(fildes);
    int found = 0;
    inode* fileNode;
    for (int i = 0; i < MAX_FILE_COUNT; i++){
        if (strcmp(fileName, gsb->inodes[i].fileName) == 0 && gsb->used_inodes[i]) {
            found = 1;
            fileNode = &gsb->inodes[i];
        }
    }
    if (!found) {
        printf("File not found in vdisk");
        return -1;
    }
    int outFile = open(fileName, O_WRONLY | O_CREAT, 0666);
    block* buffer = malloc(sizeof(block));
    buffer->nextBlock = fileNode->firstDataBlock;
    do {
        lseek(fildes, buffer->nextBlock*BLOCK_SIZE + sizeof(superblock), SEEK_SET);
        read(fildes, buffer->data, BLOCK_SIZE-4);
        read(fildes, &buffer->nextBlock, sizeof(unsigned int));
        write(outFile, buffer->data, BLOCK_SIZE-4);
    } while (buffer->nextBlock != -1);
    close(outFile);
    free(buffer);
    return 0;
}

int delete_file(int fildes, char* fileName) {
    read_superblock(fildes);
    int found = 0;
    inode* fileNode;
    for (int i = 0; i < MAX_FILE_COUNT; i++){
        if (strcmp(fileName, gsb->inodes[i].fileName) == 0 && gsb->used_inodes[i] == 1) {
            found = 1;
            fileNode = &gsb->inodes[i];
            gsb->used_inodes[i] = 0;
        }
    }
    if (!found) {
        printf("File not found in vdisk\n");
        return -1;
    }
    block* buffer = malloc(sizeof(block));
    buffer->nextBlock = fileNode->firstDataBlock;
    for (int i = 0; i < BLOCK_SIZE-4; i++) {
        buffer->data[i] = '\0';
    }
    const unsigned int zero = 0x0000;
    do {
        gsb->freeBlocksCount++;
        gsb->usedBlocksCount--;
        gsb->blockMap[buffer->nextBlock] = 0;
        lseek(fildes, buffer->nextBlock*BLOCK_SIZE + sizeof(superblock), SEEK_SET);
        write(fildes, buffer->data, BLOCK_SIZE-4);
        read(fildes, &buffer->nextBlock, sizeof(unsigned int));
        lseek(fildes, (buffer->nextBlock+1)*BLOCK_SIZE + sizeof(superblock) - sizeof(unsigned int), SEEK_SET);
        write(fildes, &zero, sizeof(unsigned int));
    } while (buffer->nextBlock != -1);
    gsb->filesystemSize = gsb->filesystemSize - fileNode->fileSize;
    gsb->inodeCount--;
    free(buffer);
    rewrite_superblock(fildes);
}

int delete_vdisk(char* filename, int fildes){
    close(fildes);
    if (remove(filename) == 0)
      printf("vdisk eleted successfully");
   else
      printf("error deleting vdisk");
   return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 3 && argc != 4) {
        printf("usage: ./fs -[command] [vdisk_name] [file_name] \n");
        return -1;
    }
    gsb = malloc(sizeof(superblock));
    mode_t mask = 0000;
    umask(mask);
    int vdiskID = open(argv[2], O_RDWR | O_CREAT, 0666);

    if (strcasecmp(argv[1], "-create") == 0) {
        if(create_vfs(vdiskID)){
            printf("error creating vdisk\n");
            return -1;
        }
        printf("created vdisk\n");
    }
    else if (strcasecmp(argv[1], "-put") == 0) {
        read_superblock(vdiskID);
        if(copy_file_to_vfs(vdiskID, argv[3])){
            printf("error copying file\n");
            return -1;
        }    
        printf("file copied to vdisk\n");
    }
    else if (strcasecmp(argv[1], "-cut") == 0) {
        if(copy_file_from_vfs(vdiskID, argv[3])){
            return -1;
        }    
        printf("copied file");
    }
    else if (strcasecmp(argv[1], "-ls") == 0) {
        read_superblock(vdiskID);
        if(print_files(vdiskID)){
            return -1;
        }    
    }
    else if (strcasecmp(argv[1], "-rm") == 0){
        if(delete_file(vdiskID, argv[3])) {
            return -1;
        }    
    }
    else if (strcasecmp(argv[1], "-wipe") == 0){
        if(delete_vdisk(argv[2], vdiskID)) {
            return -1;
        }    
    }
    else if (strcasecmp(argv[1], "-map") == 0){
        read_superblock(vdiskID);
        if(print_block_map(vdiskID)) {
            return -1;
        }    
    }
    else {
        printf("unknown arguments\n");
    }
    close(vdiskID);


}

