//
// Created by Administrator on 2021/5/4.
//

#include "Q3.h"


/* 基本思路：
 * 1，建立索引：将排序好的块依次读入，为每一个元组建立一个索引项，写入索引文件中，满的话写入磁盘
 * 2. 索引查找：输入一个键值，读索引文件（可以采用二分法），查找含有该键值的索引项所在的位置
 * 3. 元组获取：根据索引项访问磁盘，读出元组
 */

static int w_addr;

void Q3(Buffer *buf, int target, char relationship) {
    w_addr = index_search_addr;
    if (relationship == 'R') {
        setupIndex(buf, sort_addr + 1, sort_addr + 16, 'R');
    } else {
        setupIndex(buf, sort_addr + 17, sort_addr + 48, 'S');
    }
    // 清零
    printf("\n");
    printf("-----------------------\n");
    if (relationship == 'R') {
        printf("  查询R_A = %d的数据\n", target);
    } else {
        printf("  查询S_C = %d的数据\n", target);
    }
    printf("-----------------------\n");
    printf("\n");
    buf->numIO = 0;
    if(!search(buf, target, relationship)) {
        printf("关系%c中找不到属性值为%d记录\n", relationship, target);
    }
}

int setupIndex(Buffer *buf, int blk_start, int blk_end, char relationship) {
    int count = 0;
    block *blk;
    id_file* f = (id_file*) getNewBlockInBuffer(buf);
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
            count++;
            if (count == index_per_block) {
                writeBlockToDisk((unsigned char*) f, w_addr++, buf);
                f = (id_file*) getNewBlockInBuffer(buf);
                count = 0;
            }
        }
        freeBlockInBuffer((unsigned char*) blk, buf);
    }
    // 最后索引文件即使不满也要写入
    if (count > 0) {
        writeBlockToDisk((unsigned char*) f, w_addr, buf);
    }
    return 0;
}

int searchIndex(Buffer *buf, id_file *f, int target, block *w_blk, int *count) {
    block *blk = NULL;     // 暂存获取的数据块
    int last_blk = -1;
    id_entry entry;
    for (int i = 0; i < index_per_block; i++) {
        entry = f->entry[i];
        if (entry.key == target) {
            // 如果前后块是一致的话无需重复获取数据块
            // 如果不相同的话获取新数据块并且更新last_blk
            if (entry.blk_num != last_blk) {
                printf("搜索数据块%d\n", entry.blk_num);
                // 之前获取数据块的话，记得及时释放
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
                printf("结果写入磁盘块：%d\n\n", w_addr);
                writeBlockToDisk((unsigned char *) w_blk, w_addr++, buf);
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
        printf("搜索索引块%d\n", addr);
        // 特别注意：ctrl+z的ASCII码是1AH
        // 在windows下读取的时候会认为是EOF导致数据没法全部读出
        if ((f = (id_file*) readBlockFromDisk_bin(addr, buf)) == NULL) {
            perror("Reading block failed.\n");
            return -1;
        }
        // 最大值比目标小：下一个索引块
        // 最小值比目标大：退出
        // 最大值大，最小值小：在当前索引块搜索
        if (f->entry[index_per_block - 1].key < target) {
            freeBlockInBuffer((unsigned char*)f, buf);
            continue;
        } else if (f->entry[0].key > target) {
            freeBlockInBuffer((unsigned char*)f, buf);
            break;
        } else {
            searchIndex(buf, f, target, w_blk, &count);
            // 最大值大，直接退出，少读一个块
            if (f->entry[index_per_block - 1].key > target) {
                break;
            }
        }
    }

    if (count == 0) {
        return 0;
    }
    // 将剩余的记录还没写入
    if (count % data_num != 0) {
        printf("结果写入磁盘块：%d\n\n", w_addr);
        writeBlockToDisk((unsigned char *) w_blk, w_addr, buf);
    }

    printf("\n");
    printf("结果共有%d条\n", count);
    printf("I/O读写次数：%lu\n", buf->numIO);

    return 1;
}