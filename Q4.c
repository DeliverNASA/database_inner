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
        printf("���д����̿飺%d\n", w_addr);
        writeBlockToDisk((unsigned char*) blk_j->data, w_addr++, buf);
        blk_j = (block_j*) getNewBlockInBuffer(buf);
    }
}


int Q4(Buffer *buf) {
    int addr_R = sort_addr_R;
    int addr_S = sort_addr_S;

    printf("\n");
    printf("-----------------------\n");
    printf("  ��������������㷨\n");
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
    int back_R;
    int A, C, temp;
    block *back_blkR;
    int back_addr_R;

    blk_j = (block_j *) getNewBlockInBuffer(buf);

    // 3. �Ƚ�R.A��S.C�Ĵ�С��
    //    ���A = C����Ԫ����ϳ��µ�Ԫ��д�������������S��Rָ������ֵ�͵�ǰһ���������ƶ�
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
        if (A == C) {
            writeDataToBlockJoin(buf, p_R, p_S);
            // ����ԭ����R�顢R����ָ�롢��һ��ı��
            back_blkR = blkR;
            back_R = p_R;
            back_addr_R = addr_R;
            // �ٶ�ȡһ����ʱ�Ŀ��blkR
            if ((blkR = (block *) readBlockFromDisk(addr_R - 1, buf)) == NULL) {
                perror("Reading block failed.\n");
                return -1;
            }
            while ((addr_R <= sort_addr_end_R || p_R < data_num)) {
                // �������׵�ʱ��ʱ����
                p_R++;
                if (p_R == data_num && addr_R <= sort_addr_end_R) {
                    freeBlockInBuffer((unsigned char *) blkR, buf);
                    if ((blkR = (block *) readBlockFromDisk(addr_R++, buf)) == NULL) {
                        perror("Reading block failed.\n");
                        return -1;
                    }
                    p_R = 0;
                }
                // �ܹ�����ƥ�䵽��д��
                if (getDataInBlock(blkR, p_R) == A) {
                    writeDataToBlockJoin(buf, p_R, p_S);
                } else {
                    break;
                }
            }
            // ��ԭ
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
        printf("���д����̿飺%d\n", w_addr);
        writeBlockToDisk((unsigned char *) blk_j->data, w_addr, buf);
    }

    freeAllBlockInBuffer(buf);

    printf("\n");
    printf("����%d��Ԫ��\n", count);

    return 0;
}
