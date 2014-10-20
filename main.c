#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include "format.h"
#include "parse.h"

#define BLOCKCHAIN_PATH "/raid/btc/blocks"

int
main(int argc, char *argv[])
{
    DIR *datadir = NULL;
    struct dirent *dp = NULL;
    int fd;
    int ret = 0;

    /* Open the blockchain data directory */
    datadir = opendir(BLOCKCHAIN_PATH);
    if (datadir == NULL) {
        perror("opendir");
        ret = 1;
        goto exit;
    }
    printf("Parsing from %s\n", BLOCKCHAIN_PATH);
    chdir(BLOCKCHAIN_PATH);

    do {
        errno = 0;
        
        dp = readdir(datadir);
        if (dp != NULL) {
            if (memcmp(dp->d_name, "blk", 3) == 0) {
                
                /* 
                 * NOTE: We are not bothering to sort by filename because we
                 * assume the blockchain files have ascending inode numbers
                 */
                /* printf("%d\n", dp->d_ino); */

                fd = open(dp->d_name, O_RDONLY);
                if (fd < 0) {
                    perror("open");
                } else {
                    parse(fd);
                    close(fd);
                }
            }
        } else {
            if (errno != 0) {
                perror("readdir");
            }
        }
    } while (dp != NULL && datadir != NULL);

    
exit:
    closedir(datadir);
    return ret; 
}
