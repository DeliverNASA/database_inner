//
// Created by Administrator on 2021/5/4.
//

#ifndef DB_LAB4_BLOCK_H
#define DB_LAB4_BLOCK_H

#include "utils.h"
#include "info.h"

typedef struct block {
    unsigned char data[56];
    unsigned char nextAddr;
} block;

// 获取块中第id个元素
int getDataInBlock(block *blk, int id);



#endif //DB_LAB4_BLOCK_H
