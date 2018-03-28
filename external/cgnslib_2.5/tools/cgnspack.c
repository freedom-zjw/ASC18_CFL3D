#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef _WIN32
# include <io.h>
#else
# include <unistd.h>
#endif
#include "ADF.h"

extern void adf_cond(double, double);

int main (int argc, char **argv)
{
    char *inpfile, *outfile;
    char errmsg[80];
    int ierr;
    double inpid, outid;
    struct stat inpst, outst;
    time_t ts, te;

    if (argc < 2 || argc > 3) {
        fprintf (stderr, "usage: cgnspack CGNSfile [newCGNSfile]\n");
        exit (1);
    }
    inpfile = argv[1];
    if (stat (inpfile, &inpst)) {
        fprintf (stderr, "can't stat %s\n", inpfile);
        exit (1);
    }

    if (argc == 3)
        outfile = argv[2];
    else {
        outfile = (char *) malloc (strlen(inpfile) + 6);
        if (NULL == outfile) {
            fprintf (stderr, "malloc failed for temporary filename\n");
            exit (1);
        }
        sprintf (outfile, "%s.temp", inpfile);
    }
    unlink (outfile);

    ADF_Database_Open (inpfile, "READ_ONLY", "NATIVE", &inpid, &ierr);
    if (ierr > 0) {
        ADF_Error_Message (ierr, errmsg);
        fprintf (stderr, "ADF_Database_Open ERROR : %s\n", errmsg);
        exit (1);
    }
    ADF_Database_Open (outfile, "NEW", "NATIVE", &outid, &ierr);
    if (ierr > 0) {
        ADF_Error_Message (ierr, errmsg);
        fprintf (stderr, "ADF_Database_Open ERROR : %s\n", errmsg);
        exit (1);
    }

    ts = time (NULL);
    adf_cond (inpid, outid);
    te = time (NULL);

    ADF_Database_Close (inpid, &ierr);
    if (ierr > 0) {
        ADF_Error_Message (ierr, errmsg);
        fprintf (stderr, "ADF_Database_Close ERROR : %s\n", errmsg);
        exit (1);
    }
    
    /* output database closed in adf_cond() */

    if (argc < 3) {
        unlink (inpfile);
        if (rename (outfile, inpfile)) {
            fprintf (stderr, "rename %s -> %s failed", outfile, inpfile);
            exit (1);
        }
        outfile = inpfile;
    }

    if (stat (outfile, &outst)) {
        fprintf (stderr, "can't stat %s\n", outfile);
        exit (1);
    }

    printf ("initial size      = %ld bytes\n", (long)inpst.st_size);
    printf ("compressed size   = %ld bytes\n", (long)outst.st_size);
    printf ("compression ratio = %f\n",
        (float)(inpst.st_size - outst.st_size) / (float)outst.st_size);
    printf ("compression time  = %d secs\n", (int)(te - ts));
    return 0;
}

