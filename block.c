//
// Created by Administrator on 2021/5/4.
//

#include "block.h"
int getDataInBlock(block *blk, int id) {
    return StringToInt(&blk->data[id * 8], 4);
}

void writeDataToBlock(block *blk, int id, unsigned char* p_data) {
    memcpy(blk->data + id * data_len, p_data, data_len);
}