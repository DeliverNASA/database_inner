//
// Created by Administrator on 2021/5/4.
//

#include "block.h"
int getDataInBlock(block *blk, int id) {
    return StringToInt(&blk->data[id], 4);
}
