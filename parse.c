#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "format.h"
#include "parse.h"

void
parse(int blkfd)
{
    struct stat sb;
    int ret;

    ret = fstat(blkfd, &sb);
    if (ret < 0) {
        perror("stat");
    } else {
        printf("%d\n", sb.st_ino);
    }

}
