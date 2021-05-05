cmake_minimum_required(VERSION 3.19)
project(db_lab4 C)

set(CMAKE_C_STANDARD 11)

add_executable(db_lab4 main.c Q1.h Q1.c info.h extmem.h extmem.c block.c block.h utils.c utils.h Q2.c Q2.h Q3.c Q3.h Q4.c Q4.h Q5.c Q5.h)