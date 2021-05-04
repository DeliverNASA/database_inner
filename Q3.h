//
// Created by Administrator on 2021/5/4.
//

#ifndef DB_LAB4_Q3_H
#define DB_LAB4_Q3_H

#include <stdint.h>
#include "utils.h"
#include "block.h"

/*
 * 实现基于索引的关系选择算法：
 * 利用（2）中的排序结果为关系S建立索引文件，
 * 利用索引文件选出S.C=50的元组，
 * 并将选择结果存放在磁盘上。
 * 记录IO读写次数，与（1）中的结果对比。
 * （模拟实现 select S.C, S.D from S where S.C = 50 ）
 */

// 为节省空间，按字节对齐
#pragma pack (1)

// 索引项，占4个字节
typedef struct index_entry {
    uint8_t key;        // 键值
    uint16_t blk_num;   // 块号
    uint8_t id_num;     // 块内偏移
} id_entry;

// 索引文件，一个文件包含16个索引项（64 = 16 * 4）
typedef struct index_file {
    id_entry entry[index_per_block];
} id_file;


// 任务三
void Q3(Buffer *buf, int target, char relationship);

// 建立索引
int setupIndex(Buffer *buf, int blk_start, int blk_end, char relationship);

// 在索引块中搜索数据位置
int searchIndex(Buffer *buf, id_file *f, int target, block *w_blk, int *count);

// 在所有索引文件中找目标数据
int search(Buffer *buf, int target, char relationship);



#endif //DB_LAB4_Q3_H
