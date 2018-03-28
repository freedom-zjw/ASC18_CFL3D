#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

void int_to_a (int num, char *s, int len)
{
    int i, j, k;

    for (i = 1; i * 10 <= num; i *= 10)
        ;
    k = 0;
    while (i > 0 && k < len - 1) {
        j = num / i;
        num %= i;
        i /= 10;
        s[k] = j + 48;
        k++;
    }
    s[k] = 0;
}

int main (int argc, char **argv)
{
    const char *dbname = "dbtest.cgns";
    char buf[30];
    int i, numzones, numvalues, isize[3][1];
    int index_file, index_base, index_zone, index_coord;
    float *values;
    double start_time, end_time;

    if (argc < 3 || argc > 4) {
        fprintf (stderr, "usage: dbtest numzones numvalues [CGNSfile]\n");
        exit (1);
    }
    numzones = atoi(argv[1]);
    numvalues = atoi(argv[2]);
    if (argc == 4)
        dbname = argv[3];

    values = (float *) malloc (numvalues * sizeof(float));
    if (values == NULL) {
        perror ("malloc");
        exit (-1);
    }
    for (i = 0; i < numvalues; i++)
        values[i] = (float)i;

    start_time = elapsed_time();

    cg_open (dbname, CG_MODE_WRITE, &index_file);
    cg_base_write (index_file, "Base", 1, 1, &index_base);

    isize[0][0] = numvalues;
    isize[1][0] = isize[0][0] - 1;
    isize[2][0] = 0;

    for (i = 0; i < numzones; i++) {
        int_to_a (i, buf, sizeof(buf));
        if (cg_zone_write (index_file, index_base, buf, *isize,
                Structured, &index_zone) ||
            cg_coord_write (index_file, index_base, index_zone,
                RealSingle, "CoordinateX", values, &index_coord))
            cg_error_exit();
    }

    cg_close (index_file);
    end_time = elapsed_time();

    printf ("numzones:%d numvalues:%d time:%f [s]\n",
        numzones, numvalues, end_time - start_time);

    return 0;
}

