//
// Created by Administrator on 2021/5/4.
//

#ifndef DB_LAB4_UTILS_H
#define DB_LAB4_UTILS_H

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "info.h"
#include "extmem.h"

// 字符串转整形
int StringToInt(void* pStr, int size);

// 比较函数
bool less_than(void* A, void* B);

// 比较函数2
bool less_than_number(int X1, int Y1, int X2, int Y2);

// 交换函数
void swap(unsigned char* A, unsigned char* B);

#endif //DB_LAB4_UTILS_H