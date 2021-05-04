//
// Created by Administrator on 2021/5/4.
//

#include "utils.h"
#include <stdio.h>

int StringToInt(void* pStr, int size) {
    char* ptr = (char*) pStr;
    int res = 0;
    while(*ptr != 0 && size > 0) {
        res = res * 10 + *ptr - '0';
        ++ptr;
        --size;
    }
    return res;
}

// 比较函数
bool less_than(void* A, void* B) {
    int X1 = StringToInt(A, 4);
    int X2 = StringToInt(B, 4);
    if (X1 < X2) {
        return true;
    } else if (X1 == X2) {
        int Y1 = StringToInt(A + 4,4);
        int Y2 = StringToInt(B + 4, 4);
        if (Y1 <= Y2) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

// 比较函数
bool less_than_number(int X1, int Y1, int X2, int Y2) {
    if (X1 < X2) {
        return true;
    } else if (X1 == X2) {
        if (Y1 <= Y2) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

// 交换函数
void swap(unsigned char* A, unsigned char* B) {
    char temp[8];
    memcpy(&temp, A, data_len);
    memcpy(A, B, data_len);
    memcpy(B, &temp, data_len);
}