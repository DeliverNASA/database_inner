//
// Created by Administrator on 2021/5/5.
//

#ifndef DB_LAB4_Q4_H
#define DB_LAB4_Q4_H

#include "block.h"

/* 实现基于排序的连接操作算法（Sort-Merge-Join）：
 * 对关系S和R计算S.C连接R.A ，
 * 并统计连接次数，将连接结果存放在磁盘上。
 * （模拟实现 select  S.C, S.D, R.A, R.B from S inner join R on S.C = R.A）
 */

/* 基本思路：
 * 1. 获取最小的R数据块和最小的S数据块
 * 2. 按顺序获取一个R元组和一个S元组
 * 3. 比较R.A和S.C的大小：
 *    如果A > C，S指针往后移动；
 *    如果A < C，R指针往后移动；
 *    如果A = C，将元组组合成新的元，写入输出缓冲区，S或R指针往后移动
 * 4. 重复上述过程，直到所有的元组都遍历完毕
 */

// 新数据结构体（链接后每一项有16字节，由于块大小为64字节，最多可存放4条数据
typedef struct data_join {
    unsigned char R_data[8];
    unsigned char S_data[8];
} data_J;

typedef struct block_join {
    data_J data[4];
} block_j;

// 将数据写入联合块
void writeDataToBlockJoin(Buffer *buf, int p_R, int p_S);

int Q4(Buffer *buf);
#endif //DB_LAB4_Q4_H
