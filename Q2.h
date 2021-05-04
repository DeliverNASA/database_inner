//
// Created by Administrator on 2021/5/4.
//

#ifndef DB_LAB4_Q2_H
#define DB_LAB4_Q2_H

#include "utils.h"
#include "block.h"
#include "info.h"

/*
 * 实现两阶段多路归并排序算法（TPMMS）：
 * 利用内存缓冲区将关系R和S分别排序，
 * 并将排序后的结果存放在磁盘上。
 */

//static int sort_addr_start = 401;
//static int addr_start = 301;

typedef struct group {
    int blk_start;
    int blk_end;
    int cur_blk;
    int cur_data;
} Group;

static int addr;

// 总排序
int Q2(Buffer *buf, int blk_start, int blk_end);

// 对块内元素排序
void SortBlock(block *blk);

// 找到当前缓冲区中有效的最小值
unsigned char* findMin(block *p_block[], int cur[], int blk_num);

// 找到组中最小值（与findMin区别是需要调入同一组的其他块）
unsigned char* findMin_Group(Buffer *buf, block *p_block[], Group groups[], int blk_num);

// 对缓冲区内的块排序
void SortGroup(Buffer *buf, int blk_num);

// 对所有数据进行排序
int SortAll(Buffer *buf, Group groups[], int blk_num, int group_num);


#endif //DB_LAB4_Q2_H
