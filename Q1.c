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
        // �Ӵ����ж�һ��block��buf��
        if ((blk = readBlockFromDisk(j, buf)) == NULL) {
            perror("Reading block failed.\n");
            return -1;
        }

        printf("�������ݿ�%d\n", j);

        // ÿ�����̿��СΪ64�ֽڣ���7�����ݺ�1���������̿��ַ
        for (int i = 0; i < data_num; i++) {
            X = StringToInt(blk + i * data_len, 4);
            if (X != S_C) {
                continue;
            }
            Y = StringToInt(blk + i * data_len + 4, 4);
            printf("(X = %d, Y = %d)\n", X, Y);
            // ��������Ƶ������������
            memcpy(write_blk + (count % data_num) * data_len, blk + i * data_len, data_len);
            count++;
            // ��������֮������д��
            if (count % data_num == 0) {
                writeBlockToDisk(write_blk, write_start_addr++, buf);
                freeBlockInBuffer(write_blk, buf);
                write_blk = getNewBlockInBuffer(buf);
            }
        }
        // �����ͷ�
        freeBlockInBuffer(blk, buf);
    }
    // ���һ�λ������ݵĻ�����Ҫ����д��
    if (count % data_num) {
        writeBlockToDisk(write_blk, write_start_addr, buf);
    }

    printf("\n");
    printf("�������%d��\n", count);
    printf("ע�����д�����\n");
    printf("I/O��д������%lu\n", buf->numIO);

    return 0;
}