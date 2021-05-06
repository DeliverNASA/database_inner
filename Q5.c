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
    // ���Ļ�Ҫд�����
    if (count % data_num == 0) {
        printf("���д����̿飺%d\n", w_addr);
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
            printf("     S��R�Ľ���\n");
            break;
        case op_union:
            w_addr = union_addr;
            printf("     S��R�Ĳ���\n");
            break;
        case op_differ:
            w_addr = dif_addr;
            printf("  S��R�Ĳ��S-R)\n");
            break;
        default:
            break;
    }
    printf("-----------------------\n");
    printf("\n");

    // 1. ��ȡ��С��R���ݿ����С��S���ݿ�
    if ((blkR = (block *) readBlockFromDisk(addr_R++, buf)) == NULL) {
        perror("Reading block failed.\n");
        return -1;
    }
    if ((blkS = (block *) readBlockFromDisk(addr_S++, buf)) == NULL) {
        perror("Reading block failed.\n");
        return -1;
    }

    // 2. ��˳���ȡһ��RԪ���һ��SԪ��
    int p_R = 0, p_S = 0;
    int A, C;
    int B, D;

    w_blk = (block *) getNewBlockInBuffer(buf);

    // 3. �Ƚ�R.A��S.C�Ĵ�С��
    //    ���A = C��B = D������Ԫ��д�������������S��Rָ�붼�����ƶ�
    //    ���A > C��Sָ�������ƶ���
    //    ���A < C��Rָ�������ƶ���
    // 4. �ظ��������̣�ֱ�����е�Ԫ�鶼�������
    while ((addr_R <= sort_addr_end_R || p_R < data_num) && (addr_S <= sort_addr_end_S || p_S < data_num)) {
        // ���׵�ʱ����Ҫ������R��
        if (p_R == data_num) {
            freeBlockInBuffer((unsigned char *) blkR, buf);
            if ((blkR = (block *) readBlockFromDisk(addr_R++, buf)) == NULL) {
                perror("Reading block failed.\n");
                return -1;
            }
            p_R = 0;
        }
        // ���׵�ʱ����Ҫ������S��
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

    // �ǵý�ʣ�������д��
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
        printf("���д����̿飺%d\n", w_addr);
        writeBlockToDisk((unsigned char*) w_blk, w_addr, buf);
    }

    printf("\n");
    printf("�ܹ���%d��Ԫ��\n", count);


    freeAllBlockInBuffer(buf);

    return 0;
}