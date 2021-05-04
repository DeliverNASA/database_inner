//
// Created by Administrator on 2021/5/4.
//

#ifndef DB_LAB4_Q1_H
#define DB_LAB4_Q1_H

#include "extmem.h"

/*
 * 实现基于线性搜索的关系选择算法：
 * 基于ExtMem程序库，使用C语言实现线性搜索算法，
 * 选出S.C=50的元组，记录IO读写次数，
 * 并将选择结果存放在磁盘上。
 * （模拟实现 select S.C, S.D from S where S.C = 50）
 */

int Q1(Buffer *buf, int S_C);

#endif //DB_LAB4_Q1_H
