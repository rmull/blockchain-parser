#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include "format.h"
#include "parse.h"

int
main(int argc, char *argv[])
{
    DIR *datadir = NULL;
    struct dirent *dp = NULL;
    struct stat sb;
    int fd;
    int ret = 0;

    if (argc == 2) {

        /* Open the blockchain data directory */
        datadir = opendir(argv[1]);
        if (datadir == NULL) {
            perror("opendir");
            ret = 1;

        } else {

            chdir(argv[1]);

            do {
                errno = 0;
                
                dp = readdir(datadir);
                if (dp != NULL) {
                    if (memcmp(dp->d_name, "blk", 3) == 0) {
                        
                        /* 
                         * NOTE: We are not bothering to sort by filename because we
                         * assume the blockchain files have ascending inode numbers due
                         * to the way they were created. Other sources may require more
                         * sorting.
                         */
                        /* printf("%d\n", dp->d_ino); */

                        fd = open(dp->d_name, O_RDONLY);
                        if (fd < 0) {
                            perror("open");
                        } else {
                            ret = fstat(fd, &sb);
                            if (ret < 0) {
                                perror("stat");
                            } else {
                                /* printf("%s\n", dp->d_name); */
                                parse(fd, sb.st_size);
                            }
                            close(fd);
                        }
                    }
                } else {
                    if (errno != 0) {
                        perror("readdir");
                    }
                }
            } while (dp != NULL && datadir != NULL);

        }
        
        closedir(datadir);

    } else {
        ret = 1;
    }

    if (ret != 0) {
        printf("Usage: %s /path/to/blockchain/datadir\n", argv[0]);
    }

    return ret; 
}
