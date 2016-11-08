/*
  Reads two tables in the following format and merges them:

      ((<n:uint8> <ml:int8>){4} <value:float64>)*

  Both tables must contain the indices in the same ordering.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clh2of.h"

struct entry {
    struct clh2of_ix index;
    double value;
};

int main(int argc, char **argv)
{
    if (argc != 4) {
        fprintf(stderr, "Usage: <prog> <file1> <file2> <file3>\n");
        abort();
    }
    FILE *f1 = fopen(argv[1], "rb");
    FILE *f2 = fopen(argv[2], "rb");
    FILE *f3 = fopen(argv[3], "wb");

    unsigned long i;
    for (i = 0; ; ++i) {
        struct entry e1, e2;
        size_t n1 = fread(&e1, sizeof(struct entry), 1, f1);
        size_t n2 = fread(&e2, sizeof(struct entry), 1, f2);
        if (n1 != n2) {
            fprintf(stderr, "one file is shorter than other\n");
            abort();
        }
        if (memcmp(&e1.index, &e2.index, sizeof(struct clh2of_ix)) != 0) {
            fprintf(stderr,
                    "entry #%lu: indices do not match: "
                    "%u %i %u %i %u %i %u %i != "
                    "%u %i %u %i %u %i %u %i\n",
                    i,
                    e1.index.n1, e1.index.ml1,
                    e1.index.n2, e1.index.ml2,
                    e1.index.n3, e1.index.ml3,
                    e1.index.n4, e1.index.ml4,
                    e2.index.n1, e2.index.ml1,
                    e2.index.n2, e2.index.ml2,
                    e2.index.n3, e2.index.ml3,
                    e2.index.n4, e2.index.ml4);
            abort();
        }
        if (n1 != 1) {
            break;
        }
        struct entry e3 = {e1.index, e1.value + e2.value};
        fwrite(&e3, sizeof(struct entry), 1, f3);
    }

    return 0;
}
