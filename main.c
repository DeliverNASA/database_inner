#include <stdio.h>

#include "Q1.h"
#include "Q2.h"
#include "Q3.h"
#include "Q4.h"
#include "Q5.h"

int main() {
    Buffer buf; /* A buffer */

    /* Initialize the buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }


//    Q1(&buf, 50);

    // test Q2
//    Q2(&buf, 1, 16);
//    Q2(&buf, 17, 48);
//    Q3(&buf, 50, 'S');
//    printf("%llu\n", sizeof(block_j));
//    Q4(&buf);
    Q5(&buf, op_intersection);
    Q5(&buf, op_union);
    Q5(&buf, op_differ);

    return 0;
}
