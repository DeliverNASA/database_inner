//
// Created by Administrator on 2021/5/5.
//

#include "Q5.h"

static int count;
static int w_addr;
block *w_blk;
static block *blkR;
static block *blkS;


void writeDataToBlock_auto(Buffer *buf, block *src_blk, int p_R) {
    writeDataToBlock(w_blk, count % data_num, &src_blk->data[p_R * data_len]);
    count++;
    // 满的话要写入磁盘
    if (count % data_num == 0) {
        printf("结果写入磁盘块：%d\n", w_addr);
        writeBlockToDisk((unsigned char*) w_blk, w_addr++, buf);
        w_blk = (block*) getNewBlockInBuffer(buf);
    }
}

int Q5(Buffer *buf, int op) {
    int addr_R = sort_addr_R;
    int addr_S = sort_addr_S;
    count = 0;

    printf("\n");
    printf("-----------------------\n");
    switch (op) {
        case op_intersection:
            w_addr = intersection_addr;
            printf("     S和R的交集\n");
            break;
        case op_union:
            w_addr = union_addr;
            printf("     S和R的并集\n");
            break;
        case op_differ:
            w_addr = dif_addr;
            printf("  S和R的差集（S-R)\n");
            break;
        default:
            break;
    }
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
    int A, C;
    int B, D;

    w_blk = (block *) getNewBlockInBuffer(buf);

    // 3. 比较R.A和S.C的大小：
    //    如果A = C且B = D，将该元组写入输出缓冲区，S或R指针都往后移动
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
        B = getSecondDataInBlock(blkR, p_R);
        D = getSecondDataInBlock(blkS, p_S);

        switch (op) {
            case op_intersection:
                if (A == C) {
                    if (B == D) {
                        writeDataToBlock_auto(buf, blkR, p_R);
                        p_R++;
                        p_S++;
                    } else if (B < D) {
                        p_R++;
                    } else {
                        p_S++;
                    }
                } else if (A < C) {
                    p_R++;
                } else {
                    p_S++;
                }
                break;

            case op_union:
                if (A == C) {
                    if (B == D) {
                        writeDataToBlock_auto(buf, blkR, p_R);
                        p_R++;
                        p_S++;
                    } else if (B < D) {
                        writeDataToBlock_auto(buf, blkR, p_R);
                        p_R++;
                    } else {
                        writeDataToBlock_auto(buf, blkS, p_S);
                        p_S++;
                    }
                } else if (A < C) {
                    writeDataToBlock_auto(buf, blkR, p_R);
                    p_R++;
                } else {
                    writeDataToBlock_auto(buf, blkS, p_S);
                    p_S++;
                }
                break;

            case op_differ:
                if (A == C) {
                    if (B == D) {
                        p_R++;
                        p_S++;
                    } else if (B < D) {
                        p_R++;
                    } else {
                        writeDataToBlock_auto(buf, blkS, p_S);
                        p_S++;
                    }
                } else if (A < C) {
                    p_R++;
                } else {
                    writeDataToBlock_auto(buf, blkS, p_S);
                    p_S++;
                }
                break;
            default:
                break;
        }
    }

    // 记得将剩余的数据写入
    if (op == op_union) {
        while (addr_R <= sort_addr_end_R || p_R < data_num) {
            if (p_R == data_num) {
                freeBlockInBuffer((unsigned char *) blkR, buf);
                if ((blkR = (block *) readBlockFromDisk(addr_R++, buf)) == NULL) {
                    perror("Reading block failed.\n");
                    return -1;
                }
                p_R = 0;
            }
            writeDataToBlock_auto(buf, blkR, p_R++);
        }
        while (addr_S <= sort_addr_end_S || p_S < data_num) {
            if (p_S == data_num) {
                freeBlockInBuffer((unsigned char *) blkS, buf);
                if ((blkR = (block *) readBlockFromDisk(addr_S++, buf)) == NULL) {
                    perror("Reading block failed.\n");
                    return -1;
                }
                p_S = 0;
            }
            writeDataToBlock_auto(buf, blkS, p_S++);
        }
    }else if (op == op_differ) {
        while (addr_S <= sort_addr_end_S || p_S < data_num) {
            if (p_S == data_num) {
                freeBlockInBuffer((unsigned char *) blkS, buf);
                if ((blkR = (block *) readBlockFromDisk(addr_S++, buf)) == NULL) {
                    perror("Reading block failed.\n");
                    return -1;
                }
                p_S = 0;
            }
            writeDataToBlock_auto(buf, blkS, p_S++);
        }
    }

    if (count % data_num != 0) {
        printf("结果写入磁盘块：%d\n", w_addr);
        writeBlockToDisk((unsigned char*) w_blk, w_addr, buf);
    }

    printf("\n");
    printf("总共有%d个元组\n", count);


    freeAllBlockInBuffer(buf);

    return 0;
}