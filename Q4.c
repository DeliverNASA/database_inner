//
// Created by Administrator on 2021/5/5.
//

#include "Q4.h"

static int count = 0;
static int w_addr = data_join_addr;
block_j *blk_j;
static block *blkR;
static block *blkS;


void writeDataToBlockJoin(Buffer *buf, int p_R, int p_S) {
    memcpy(blk_j->data[count % 4].R_data, &blkR->data[p_R *data_len], data_len);
    memcpy(blk_j->data[count % 4].S_data, &blkS->data[p_S *data_len], data_len);
    count++;
    if (count % data_join_per_block == 0) {
        printf("结果写入磁盘块：%d\n", w_addr);
        writeBlockToDisk((unsigned char*) blk_j->data, w_addr++, buf);
        blk_j = (block_j*) getNewBlockInBuffer(buf);
    }
}


int Q4(Buffer *buf) {
    int addr_R = sort_addr_R;
    int addr_S = sort_addr_S;

    printf("\n");
    printf("-----------------------\n");
    printf("  基于排序的连接算法\n");
    printf("-----------------------\n");
    printf("\n");

    // 1. 获取最小的R数据块和最小的S数据块
    if ((blkR = (block *) readBlockFromDisk(addr_R++, buf)) == NULL) {
        perror("Reading block failed.\n");
        return -1;
    }
    if ((blkS = (block *) readBlockFromDisk(addr_S++, buf)) == NULL) {
        perror("Reading block failed.\n");
        return -1;
    }

    // 2. 按顺序获取一个R元组和一个S元组
    int p_R = 0, p_S = 0;
    int back_R;
    int A, C, temp;
    block *back_blkR;
    int back_addr_R;

    blk_j = (block_j *) getNewBlockInBuffer(buf);

    // 3. 比较R.A和S.C的大小：
    //    如果A = C，将元组组合成新的元，写入输出缓冲区，S或R指针后面的值和当前一样的往后移动
    //    如果A > C，S指针往后移动；
    //    如果A < C，R指针往后移动；
    // 4. 重复上述过程，直到所有的元组都遍历完毕
    while ((addr_R <= sort_addr_end_R || p_R < data_num) && (addr_S <= sort_addr_end_S || p_S < data_num)) {
        // 到底的时候需要调入新R块
        if (p_R == data_num) {
            freeBlockInBuffer((unsigned char *) blkR, buf);
            if ((blkR = (block *) readBlockFromDisk(addr_R++, buf)) == NULL) {
                perror("Reading block failed.\n");
                return -1;
            }
            p_R = 0;
        }
        // 到底的时候需要调入新S块
        if (p_S == data_num) {
            freeBlockInBuffer((unsigned char *) blkS, buf);
            if ((blkS = (block *) readBlockFromDisk(addr_S++, buf)) == NULL) {
                perror("Reading block failed.\n");
                return -1;
            }
            p_S = 0;
        }

        A = getDataInBlock(blkR, p_R);
        C = getDataInBlock(blkS, p_S);
        if (A == C) {
            writeDataToBlockJoin(buf, p_R, p_S);
            // 保存原来的R块、R块内指针、下一块的编号
            back_blkR = blkR;
            back_R = p_R;
            back_addr_R = addr_R;
            // 再读取一块临时的块给blkR
            if ((blkR = (block *) readBlockFromDisk(addr_R - 1, buf)) == NULL) {
                perror("Reading block failed.\n");
                return -1;
            }
            while ((addr_R <= sort_addr_end_R || p_R < data_num)) {
                // 如果到块底的时候及时调入
                p_R++;
                if (p_R == data_num && addr_R <= sort_addr_end_R) {
                    freeBlockInBuffer((unsigned char *) blkR, buf);
                    if ((blkR = (block *) readBlockFromDisk(addr_R++, buf)) == NULL) {
                        perror("Reading block failed.\n");
                        return -1;
                    }
                    p_R = 0;
                }
                // 能够继续匹配到就写入
                if (getDataInBlock(blkR, p_R) == A) {
                    writeDataToBlockJoin(buf, p_R, p_S);
                } else {
                    break;
                }
            }
            // 复原
            freeBlockInBuffer((unsigned char*) blkR->data, buf);
            blkR = back_blkR;
            p_R = back_R;
            addr_R = back_addr_R;
            p_S++;
        } else if (A < C) {
            p_R++;
        } else {
            p_S++;
        }
    }

    if (count % data_join_per_block != 0) {
        printf("结果写入磁盘块：%d\n", w_addr);
        writeBlockToDisk((unsigned char *) blk_j->data, w_addr, buf);
    }

    freeAllBlockInBuffer(buf);

    printf("\n");
    printf("共有%d个元组\n", count);

    return 0;
}
