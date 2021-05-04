//
// Created by Administrator on 2021/5/4.
//

#include "Q3.h"

/* ����˼·��
 * 1������������������õĿ����ζ��룬Ϊÿһ��Ԫ�齨��һ�������д�������ļ��У����Ļ�д�����
 * 2. �������ң�����һ����ֵ���������ļ������Բ��ö��ַ��������Һ��иü�ֵ�����������ڵ�λ��
 * 3. Ԫ���ȡ��������������ʴ��̣�����Ԫ��
 */

void Q3(Buffer *buf, int target, char relationship) {
    if (relationship == 'R') {
        setupIndex(buf, sort_addr + 1, sort_addr + 16, 'R');
    } else {
        setupIndex(buf, sort_addr + 17, sort_addr + 48, 'S');
    }
    if(!search(buf, target, relationship)) {
        printf("��ϵ%c���Ҳ���������ֵΪ%d��¼\n",relationship, target);
    }
}

int setupIndex(Buffer *buf, int blk_start, int blk_end, char relationship) {
    int count = 0;
    block *blk;
    id_file* f = (id_file*) getNewBlockInBuffer(buf);
//    printf("%llu", sizeof(id_entry));
//    printf("%llu", sizeof(id_file));
//    printf("%llu", sizeof(block));
    int w_addr;
    if (relationship == 'R') {
        w_addr = index_addr_R ;
    } else {
        w_addr = index_addr_S;
    }
    for (int addr = blk_start; addr <= blk_end; addr++) {
        if ((blk = (block*) readBlockFromDisk(addr, buf)) == NULL) {
            perror("Reading block failed.\n");
            return -1;
        }
        for (int j = 0; j < data_num; j++) {
            f->entry[count].key = getDataInBlock(blk, j);
            f->entry[count].blk_num = addr;
            f->entry[count].id_num = j;
            // �����ǰ�����ļ�������д�����
            count++;
            // �ر�ע�⣺ctrl+z��ASCII����1AH
            // ��windows�¶�ȡ��ʱ�����Ϊ��EOF��������û��ȫ������
            if (count == index_per_block) {
                writeBlockToDisk((unsigned char*) f, w_addr++, buf);
                f = (id_file*) getNewBlockInBuffer(buf);
                count = 0;
            }
        }
        freeBlockInBuffer((unsigned char*) blk, buf);
    }
    // ��������ļ���ʹ����ҲҪд��
    if (count > 0) {
        writeBlockToDisk((unsigned char*) f, w_addr, buf);
    }

    return 0;
}

int searchIndex(Buffer *buf, id_file *f, int target, block *w_blk, int *count) {
    block *blk = NULL;     // �ݴ��ȡ�����ݿ�
    int last_blk = -1;
    id_entry entry;
    for (int i = 0; i < index_per_block; i++) {
        entry = f->entry[i];
        if (entry.key == target) {
            // ���ǰ�����һ�µĻ������ظ���ȡ���ݿ�
            // �������ͬ�Ļ���ȡ�����ݿ鲢�Ҹ���last_blk
            if (entry.blk_num != last_blk) {
                printf("�������ݿ�%d\n", entry.blk_num);
                // ֮ǰ��ȡ���ݿ�Ļ����ǵü�ʱ�ͷ�
                if (last_blk != -1) {
                    freeBlockInBuffer((unsigned char*) blk, buf);
                }
                if ((blk = (block*) readBlockFromDisk(entry.blk_num, buf)) == NULL) {
                    perror("Reading block failed.\n");
                    return -1;
                }
                last_blk = entry.blk_num;
            }
            writeDataToBlock(w_blk, (*count) % data_num, &blk->data[entry.id_num * data_len]);
            int X = StringToInt(&blk->data[entry.id_num * data_len], 4);
            int Y = StringToInt(&blk->data[entry.id_num * data_len] + 4, 4);
            printf("X = %d, Y = %d\n", X, Y);
            (*count)++;
            if (*count % data_num == 0) {
                writeBlockToDisk((unsigned char *) w_blk, 700, buf);
                w_blk = (block*) getNewBlockInBuffer(buf);
            }
        }
    }

    return 0;
}

int search(Buffer *buf, int target, char relationship) {
    int id_addr, id_addr_end;
    id_file *f;
    block *w_blk;
    int count = 0;

    freeAllBlockInBuffer(buf);

    if (relationship == 'R') {
        id_addr = index_addr_R;
        id_addr_end = index_addr_end_R;
    } else {
        id_addr = index_addr_S;
        id_addr_end = index_addr_end_S;
    }

    w_blk = (block*) getNewBlockInBuffer(buf);
    for (int addr = id_addr; addr <= id_addr_end; addr++) {
        if ((f = (id_file*) readBlockFromDisk_bin(addr, buf)) == NULL) {
            perror("Reading block failed.\n");
            return -1;
        }
        // ���ֵ��Ŀ��С����һ��������
        // ��Сֵ��Ŀ����˳�
        // ���ֵ����СֵС���ڵ�ǰ����������
        if (f->entry[index_per_block - 1].key < target) {
            freeBlockInBuffer((unsigned char*)f, buf);
            continue;
        } else if (f->entry[0].key > target) {
            freeBlockInBuffer((unsigned char*)f, buf);
            break;
        } else {
            printf("����������%d\n", addr);
            searchIndex(buf, f, target, w_blk, &count);
        }
    }

    if (count == 0) {
        return 0;
    }
    // ��ʣ��ļ�¼��ûд��
    if (count % data_num != 0) {
        writeBlockToDisk((unsigned char *) w_blk, 700, buf);
    }
    return 1;
}