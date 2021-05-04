//
// Created by Administrator on 2021/5/4.
//

#ifndef DB_LAB4_BLOCK_H
#define DB_LAB4_BLOCK_H

#include "utils.h"
#include "info.h"

typedef struct block {
    unsigned char data[56];
    unsigned char nextAddr[8];
} block;

// 获取块中第id个元素
int getDataInBlock(block *blk, int id);

// 将元素写入块中第id个位置
void writeDataToBlock(block *blk, int id, unsigned char* p_data);


#endif //DB_LAB4_BLOCK_H
