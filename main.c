#include <stdio.h>

#include "format.h"
#include "parse.h"

#define BLOCKCHAIN_PATH "/raid/btc/blocks"

int
main(int argc, char *argv[])
{
    printf("Parsing from %s\n", BLOCKCHAIN_PATH);

    parse();

    return 0; 
}
