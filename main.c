#include <stdio.h>

#include "Q1.h"
#include "Q2.h"

int main() {
    Buffer buf; /* A buffer */
    block *blk;

    /* Initialize the buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }


//    Q1(&buf, 50);

    // test Q2
//    if ((blk = (block*) readBlockFromDisk(1, &buf)) == NULL) {
//        perror("Reading block failed.\n");
//        return -1;
//    }
//    SortBlock(blk);
//    writeBlockToDisk((unsigned char*)blk, 201, &buf);
//
//    if ((blk = (block*) readBlockFromDisk(2, &buf)) == NULL) {
//        perror("Reading block failed.\n");
//        return -1;
//    }
//    SortBlock(blk);
//    writeBlockToDisk((unsigned char*)blk, 202, &buf);
//
//    SortGroup(&buf, 2);
//    writeBlockToDisk((unsigned char*)blk, 111, &buf);

//    Q2(&buf, 1, 16);
    Q2(&buf, 17, 48);

    return 0;
}
