//
// Created by Administrator on 2021/5/4.
//

/* 基本思路：
 * 分组，1个组有7个block，总共有多个组
 * 先将每一组中7个block（预留1个后面归并）读入缓冲区，依次排序
 * 然后用归并排序的方式将7个block按照从小到大进行排序，最后得到的是有序的组（占用7个数据块）
 * 然后将所有组再次进行排序，排序的方式是每次从各个组取最小的块，然后对这些最小块进行排序处理
 * 元组遍历完就再次取新的最小块
 * 最后得到的就是有序的block
 */

#include "Q2.h"


int Q2(Buffer *buf, int blk_start, int blk_end) {
    // 预留一块出来以供写入使用
    int blk_num = blk_end - blk_start + 1;
    int GroupSize = block_per_buffer - 1;
    int GroupNum = ceil((double)blk_num / (double)GroupSize);
    Group groups[GroupNum];
    int count = 0;
    block *blk;

    // 第一步：对所有块进行排序
    // 第二步，如果缓冲区满，对缓冲区排序后写入
    addr = temp_addr + blk_start;
    for (int i = blk_start; i <= blk_end; i++) {
        if ((blk = (block*) readBlockFromDisk(i, buf)) == NULL) {
            perror("Reading block failed!!!!\n");
            return -1;
        }
//        printf("read %d ok.\n", i);
        SortBlock(blk);
        count++;
        if (count % GroupSize == 0) {
            SortGroup(buf, GroupSize);
            freeAllBlockInBuffer(buf);
        }
    }
    // 如果缓冲区不满但仍有元组，组排序后写入
    SortGroup(buf, count % GroupSize);
    freeAllBlockInBuffer(buf);

    // 第三步，对所有的组再进行归并排序
    // 将所有块进行分组
    addr = sort_addr + blk_start;
    for (int i = 0; i < GroupNum; i++) {
        groups[i].blk_start = temp_addr + i * GroupSize + blk_start;
        if ((i + 1) * GroupSize < blk_num) {
            groups[i].blk_end = temp_addr + blk_start + (i + 1) * GroupSize - 1;
        } else {
            groups[i].blk_end = temp_addr + blk_end;
        }
        groups[i].cur_blk = groups[i].blk_start;
        groups[i].cur_data = 0;
    }

    SortAll(buf, groups, blk_num, GroupNum);
    freeAllBlockInBuffer(buf);

    return 0;
}

void SortBlock(block *blk) {
    int min_id;
    // 选择排序
    for (int i = 0; i < data_num; i++) {
        min_id = i;
        for (int j = i + 1; j < data_num; j++) {
            if (less_than(&blk->data[j * data_len], &blk->data[min_id * data_len])) {
                min_id = j;
            }
        }
        swap(&blk->data[min_id * data_len], &blk->data[i * data_len]);
    }
}

unsigned char* findMin(block *p_block[], int cur[], int blk_num) {
    int minX = 10000;
    int minY = 10000;
    int min_id = -1;
    for (int i = 0; i < blk_num; i++) {
        if (cur[i] < data_num) {
            int X = StringToInt(&p_block[i]->data[cur[i] * data_len], 4);
            int Y = StringToInt(&p_block[i]->data[cur[i] * data_len + 4], 4);
            if (less_than_number(X, Y, minX, minY)) {
                minX = X;
                minY = Y;
                min_id = i;
            }
        }
    }
    // 修改最小值所在的块并返回最小值的位置
//    printf("min = %d, id = %d\n", min, min_id);

    cur[min_id]++;
    return &p_block[min_id]->data[(cur[min_id]-1) * data_len];
}

void SortGroup(Buffer *buf, int blk_num) {
    // 指针指向每个块的开端
    block *p_block[block_per_buffer];  // p指向当前选中的块
    int cur[block_per_buffer] = {0};   // cur指向当前选中的块内元组
    int count = 0;                     // count记录当前写入块中的元组个数
    int all_data = blk_num * data_num; // all_data记录总共需要写入的元组个数，用于判断程序结束
    unsigned char *p_minData;          // p_minData指向当前的最小值
    for (int i = 0; i < blk_num; i++) {
        // 注意加1消除有效位
        p_block[i] = (block*) (buf->data + i * (buf->blkSize + 1) + 1);
    }
    block* w_blk = (block*) getNewBlockInBuffer(buf);
    while (count < all_data) {
        p_minData = findMin(p_block, cur, blk_num);
        memcpy(w_blk->data + (count % data_num) * data_len, p_minData, data_len);
        count++;
        // 缓冲区满之后将数据写入
        if (count % data_num == 0) {
            writeBlockToDisk((unsigned char*) w_blk, addr++, buf);
            w_blk = (block*) getNewBlockInBuffer(buf);
        }
    }
}

unsigned char* findMin_Group(Buffer *buf, block *p_block[], Group groups[], int group_num) {
    int minX = 10000;
    int minY = 10000;
    int min_id = -1;
    for(int i = 0; i < group_num; i++) {
        // 如果还有可以的数据要及时调入
        if (groups[i].cur_data == data_num && groups[i].cur_blk < groups[i].blk_end) {
            freeBlockInBuffer((unsigned char*) p_block[i], buf);
            if ((p_block[i]  = (block*) readBlockFromDisk(++groups[i].cur_blk, buf)) == NULL) {
                perror("Reading block failed.\n");
                return NULL;
            }
            groups[i].cur_data = 0;
        }
    }
    for (int i = 0; i < group_num; i++) {
        if (groups[i].cur_data < data_num) {
            int X = StringToInt(&p_block[i]->data[groups[i].cur_data * data_len], 4);
            int Y = StringToInt(&p_block[i]->data[groups[i].cur_data * data_len + 4], 4);
            if (less_than_number(X, Y, minX, minY)) {
                minX = X;
                minY = Y;
                min_id = i;
            }
        }
    }
    // 修改最小值所在的块并返回最小值的位置
//    printf("minX = %d, Y = %d, id = %d\n", minX, minY, min_id);

    if (min_id != -1) {
        groups[min_id].cur_data++;
        return &p_block[min_id]->data[(groups[min_id].cur_data-1) * data_len];
    } else {
        return NULL;
    }
}

int SortAll(Buffer *buf, Group groups[], int blk_num, int group_num) {
    block *blk;
    block *p_block[block_per_buffer];
    int count = 0;
    unsigned char *p_minData;
    int all_data = blk_num * data_num;

    // 将每一个组的最小块读入
    for (int i = 0; i < group_num; i++) {
        if ((p_block[i] = (block*) readBlockFromDisk(groups[i].cur_blk, buf)) == NULL) {
            perror("Reading block failed......\n");
            return -1;
        }
    }

    block* w_blk = (block*) getNewBlockInBuffer(buf);
    // 这里需要注意每一组的最小元块用完后要及时调入
    while (count < all_data) {
        p_minData = findMin_Group(buf, p_block, groups, group_num);
        memcpy(w_blk->data + (count % data_num) * data_len, p_minData, data_len);
        count++;
        if (count % data_num == 0) {
            writeBlockToDisk((unsigned char *) w_blk, addr++, buf);
            w_blk = (block *) getNewBlockInBuffer(buf);
        }
    }
    return 0;
}