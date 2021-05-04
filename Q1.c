//
// Created by Administrator on 2021/5/4.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "info.h"
#include "Q1.h"
#include "utils.h"

int Q1(Buffer *buf, int S_C) {
    unsigned char *blk;
    unsigned char *write_blk;
    int count = 0, write_start_addr = 100;
    int X, Y;
    char str[5];
    write_blk = getNewBlockInBuffer(buf);

    for (int j = S_start; j <= S_end; j++) {
        // 从磁盘中读一个block到buf中
        if ((blk = readBlockFromDisk(j, buf)) == NULL) {
            perror("Reading block failed.\n");
            return -1;
        }

        printf("读入数据块%d\n", j);

        // 每个磁盘块大小为64字节，有7个数据和1个后续磁盘块地址
        for (int i = 0; i < data_num; i++) {
            X = StringToInt(blk + i * data_len, 4);
            if (X != S_C) {
                continue;
            }
            Y = StringToInt(blk + i * data_len + 4, 4);
            printf("(X = %d, Y = %d)\n", X, Y);
            // 将结果复制到输出缓冲区中
            memcpy(write_blk + (count % data_num) * data_len, blk + i * data_len, data_len);
            count++;
            // 缓冲区满之后将数据写入
            if (count % data_num == 0) {
                writeBlockToDisk(write_blk, write_start_addr++, buf);
                freeBlockInBuffer(write_blk, buf);
                write_blk = getNewBlockInBuffer(buf);
            }
        }
        // 读完释放
        freeBlockInBuffer(blk, buf);
    }
    // 最后一次还有数据的话，还要将其写入
    if (count % data_num) {
        writeBlockToDisk(write_blk, write_start_addr, buf);
    }

    printf("\n");
    printf("结果共有%d条\n", count);
    printf("注：结果写入磁盘\n");
    printf("I/O读写次数：%lu\n", buf->numIO);

    return 0;
}