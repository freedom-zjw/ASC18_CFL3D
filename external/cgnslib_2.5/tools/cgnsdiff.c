#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "ADF.h"
#include "getargs.h"

static int nocase = 0;
static int nospace = 0;
static int quiet = 0;
static int follow_links = 0;
static int recurse = 0;
static int node_data = 0;
static double tol = 0.0;

static char options[] = "ciqrfdt:";
static char *usgmsg[] = {
    "usage  : cgnsdiff [options] CGNSfile1 [dataset1] CGNSfile2 [dataset2]",
    "options:",
    "   -c      : case insensitive names",
    "   -i      : ignore white space in names",
    "   -q      : print only if they differ",
    "   -f      : follow links",
    "   -r      : recurse (used only when dataset given)",
    "   -d      : compare node data also",
    "   -t<tol> : tolerance for comparing floats/doubles (default 0)",
    NULL
};

static void err_exit (char *msg, char *name, int errcode)
{
    char errmsg[ADF_MAX_ERROR_STR_LENGTH+1];

    fflush (stdout);
    if (errcode <= 0) {
        if (msg != NULL)
            fprintf (stderr, "%s", msg);
        if (name != NULL) {
            if (msg != NULL) putc (':', stderr);
            fprintf (stderr, "%s", name);
        }
        putc ('\n', stderr);
    }
    else {
        ADF_Error_Message (errcode, errmsg);
        if (msg != NULL)
            fprintf (stderr, "%s:", msg);
        if (name != NULL)
            fprintf (stderr, "%s:", name);
        fprintf (stderr, "%s\n", errmsg);
    }
    exit (1);
}

static size_t data_size (char *type, int ndim, int *dims, int *size)
{
    int n;
    size_t bytes;

    *size = 0;
    if (ndim < 1) return 0;
    if (0 == strcmp (type, "C1") ||
        0 == strcmp (type, "B1"))
        bytes = sizeof(char);
    else if (0 == strcmp (type, "I4") ||
             0 == strcmp (type, "U4"))
        bytes = sizeof(int);
    else if (0 == strcmp (type, "I8") ||
             0 == strcmp (type, "U8"))
        bytes = sizeof(long);
    else if (0 == strcmp (type, "R4")) {
        *size = 4;
        bytes = sizeof(float);
    }
    else if (0 == strcmp (type, "R8")) {
        *size = 8;
        bytes = sizeof(double);
    }
    else if (0 == strcmp (type, "X4")) {
        *size = 4;
        bytes = 2 * sizeof(float);
    }
    else if (0 == strcmp (type, "X8")) {
        *size = 8;
        bytes = 2 * sizeof(double);
    }
    else
        return 0;

    for (n = 0; n < ndim; n++)
        bytes *= (size_t)dims[n];
    return bytes;
}

static int compare_bytes (size_t cnt, unsigned char *d1, unsigned char *d2)
{
    size_t n;

    for (n = 0; n < cnt; n++) {
        if (d1[n] != d2[n]) return 1;
    }
    return 0;
}

static int compare_floats (size_t cnt, float *d1, float *d2)
{
    size_t n;

    for (n = 0; n < cnt; n++) {
        if (fabs(d1[n] - d2[n]) > tol) return 1;
    }
    return 0;
}

static int compare_doubles (size_t cnt, double *d1, double *d2)
{
    size_t n;

    for (n = 0; n < cnt; n++) {
        if (fabs(d1[n] - d2[n]) > tol) return 1;
    }
    return 0;
}

static void compare_data (char *name1, double id1, char *name2, double id2)
{
    int n, err;
    size_t bytes;
    char label1[ADF_LABEL_LENGTH+1];
    char type1[ADF_DATA_TYPE_LENGTH+1];
    int ndim1, dims1[ADF_MAX_DIMENSIONS];
    char label2[ADF_LABEL_LENGTH+1];
    char type2[ADF_DATA_TYPE_LENGTH+1];
    int ndim2, dims2[ADF_MAX_DIMENSIONS];
    void *data1, *data2;

    /* compare labels */

    ADF_Get_Label (id1, label1, &err);
    if (err > 0)
        err_exit (name1, "ADF_Get_Label", err);
    ADF_Get_Label (id2, label2, &err);
    if (err > 0)
        err_exit (name2, "ADF_Get_Label", err);
    if (strcmp (label1, label2)) {
        printf ("%s <> %s : labels differ\n", name1, name2);
        return;
    }

    /* compare data types */

    ADF_Get_Data_Type (id1, type1, &err);
    if (err > 0)
        err_exit (name1, "ADF_Get_Data_Type", err);
    ADF_Get_Data_Type (id2, type2, &err);
    if (err > 0)
        err_exit (name2, "ADF_Get_Data_Type", err);
    if (strcmp (type1, type2)) {
        printf ("%s <> %s : data types differ\n", name1, name2);
        return;
    }

    /* compare number of dimensions */

    ADF_Get_Number_of_Dimensions (id1, &ndim1, &err);
    if (err > 0)
        err_exit (name1, "ADF_Get_Number_of_Dimensions", err);
    ADF_Get_Number_of_Dimensions (id2, &ndim2, &err);
    if (err > 0)
        err_exit (name2, "ADF_Get_Number_of_Dimensions", err);
    if (ndim1 != ndim2) {
        printf ("%s <> %s : number of dimensions differ\n", name1, name2);
        return;
    }

    /* compare dimensions */

    if (ndim1 > 0) {
        ADF_Get_Dimension_Values (id1, dims1, &err);
        if (err > 0)
            err_exit (name1, "ADF_Get_Dimension_Values", err);
        ADF_Get_Dimension_Values (id2, dims2, &err);
        if (err > 0)
            err_exit (name2, "ADF_Get_Dimension_Values", err);
        for (n = 0; n < ndim1; n++) {
            if (dims1[n] != dims2[n]) {
                printf ("%s <> %s : dimensions differ\n", name1, name2);
                return;
            }
        }
    }

    if (!node_data || ndim1 <= 0) return;

    /* compare data */

    bytes = data_size (type1, ndim1, dims1, &n);
    if (bytes > 0) {
        data1 = malloc (bytes);
        if (data1 == NULL) {
            fprintf (stderr, "%s:malloc failed for node data\n", name1);
            exit (1);
        }
        data2 = malloc (bytes);
        if (data2 == NULL) {
            fprintf (stderr, "%s:malloc failed for node data\n", name2);
            exit (1);
        }
        ADF_Read_All_Data (id1, data1, &err);
        if (err > 0)
            err_exit (name1, "ADF_Read_All_Data", err);
        ADF_Read_All_Data (id2, data2, &err);
        if (err > 0)
            err_exit (name2, "ADF_Read_All_Data", err);
        if (tol > 0.0 && n) {
            if (n == 4)
                err = compare_floats (bytes >> 2, data1, data2);
            else
                err = compare_doubles (bytes >> 3, data1, data2);
        }
        else {
            err = compare_bytes (bytes, (unsigned char *)data1,
                  (unsigned char *)data2);
        }
        free (data1);
        free (data2);
        if (err)
            printf ("%s <> %s : data values differ\n", name1, name2);
    }
}

static void copy_name (char *name, char *newname)
{
    int n1, n2;

    if (nospace) {
        if (nocase) {
            for (n1 = 0, n2 = 0; name[n1]; n1++) {
                if (!isspace (name[n1]))
                    newname[n2++] = tolower (name[n1]);
            }
        }
        else {
            for (n1 = 0, n2 = 0; name[n1]; n1++) {
                if (!isspace (name[n1]))
                    newname[n2++] = name[n1];
            }
        }
        newname[n2] = 0;
    }
    else if (nocase) {
        for (n1 = 0; name[n1]; n1++)
            newname[n1] = tolower (name[n1]);
        newname[n1] = 0;
    }
    else
        strcpy (newname, name);
}

static int sort_children (const void *v1, const void *v2)
{
    char p1[33], p2[33];

    copy_name ((char *)v1, p1);
    copy_name ((char *)v2, p2);
    return strcmp (p1, p2);
}

static int find_name (char *name, int nlist, char *namelist)
{
    int cmp, mid, lo = 0, hi = nlist - 1;
    char p1[33], p2[33];

    copy_name (name, p1);
    copy_name (namelist, p2);
    if (0 == strcmp (p1, p2)) return 0;
    copy_name (&namelist[33*hi], p2);
    if (0 == strcmp (p1, p2)) return hi;

    while (lo <= hi) {
        mid = (lo + hi) >> 1;
        copy_name (&namelist[33*mid], p2);
        cmp = strcmp (p1, p2);
        if (0 == cmp) return mid;
        if (cmp > 0)
            lo = mid + 1;
        else
            hi = mid - 1;
    }
    return -1;
}

static void compare_nodes (char *name1, double id1, char *name2, double id2)
{
    int n1, n2, nc1, nc2, err, nret;
    char *p, *children1 = NULL, *children2 = NULL;
    char path1[1024], path2[1024];
    double cid1, cid2;

    compare_data (name1, id1, name2, id2);
    if (!recurse) return;
    if (!follow_links) {
        ADF_Is_Link (id1, &nret, &err);
        if (err > 0)
            err_exit (name1, "ADF_Is_Link", err);
        if (nret > 0) return;
        ADF_Is_Link (id2, &nret, &err);
        if (err > 0)
            err_exit (name2, "ADF_Is_Link", err);
        if (nret > 0) return;
    }

    ADF_Number_of_Children (id1, &nc1, &err);
    if (err > 0)
        err_exit (name1, "ADF_Number_of_Children", err);
    if (nc1) {
        children1 = (char *) malloc (33 * nc1);
        if (children1 == NULL) {
            fprintf (stderr, "%s:malloc failed for children names\n", name1);
            exit (1);
        }
        ADF_Children_Names (id1, 1, nc1, 32, &nret, children1, &err);
        if (err > 0)
            err_exit (name1, "ADF_Children_Names", err);
        if (nc1 > 1)
            qsort (children1, nc1, 33, sort_children);
    }

    ADF_Number_of_Children (id2, &nc2, &err);
    if (err > 0)
        err_exit (name2, "ADF_Number_of_Children", err);
    if (nc2) {
        children2 = (char *) malloc (33 * nc2);
        if (children2 == NULL) {
            fprintf (stderr, "%s:malloc failed for children names\n", name2);
            exit (1);
        }
        ADF_Children_Names (id2, 1, nc2, 32, &nret, children2, &err);
        if (err > 0)
            err_exit (name2, "ADF_Children_Names", err);
        if (nc2 > 1)
            qsort (children2, nc2, 33, sort_children);
    }

    if (0 == strcmp (name1, "/")) name1 = "";
    if (0 == strcmp (name2, "/")) name2 = "";
    if (nc1 == 0) {
        if (nc2 == 0) return;
        for (n2 = 0; n2 < nc2; n2++)
            printf ("> %s/%s\n", name2, &children2[33*n2]);
        free (children2);
        return;
    }
    if (nc2 == 0) {
        for (n1 = 0; n1 < nc1; n1++)
            printf ("< %s/%s\n", name1, &children1[33*n1]);
        free (children1);
        return;
    }

    for (n1 = 0, n2 = 0; n1 < nc1; n1++) {
        p = &children1[33*n1];
        nret = find_name (p, nc2, children2);
        if (nret < 0) {
            printf ("< %s/%s\n", name1, p);
            continue;
        }
        while (n2 < nret) {
            printf ("> %s/%s\n", name2, &children2[33*n2]);
            n2++;
        }
        ADF_Get_Node_ID (id1, p, &cid1, &err);
        if (err > 0)
            err_exit (name1, "ADF_Get_Node_ID", err);
        sprintf (path1, "%s/%s", name1, p);
        p = &children2[33*n2];
        ADF_Get_Node_ID (id2, p, &cid2, &err);
        if (err > 0)
            err_exit (name2, "ADF_Get_Node_ID", err);
        sprintf (path2, "%s/%s", name2, p);
        compare_nodes (path1, cid1, path2, cid2);
        n2++;
    }
    while (n2 < nc2-1) {
        printf ("> %s/%s\n", name2, &children2[33*n2]);
        n2++;
    }
    free (children1);
    free (children2);
}

int main (int argc, char *argv[])
{
    double root1, root2;
    double node1, node2;
    int n, err;
    char *file1, *file2;
    char *ds1 = NULL;
    char *ds2 = NULL;

    if (argc < 3)
        print_usage (usgmsg, NULL);
    while ((n = getargs (argc, argv, options)) > 0) {
        switch (n) {
            case 'c':
                nocase = 1;
                break;
            case 'i':
                nospace = 1;
                break;
            case 'q':
                quiet = 1;
                break;
            case 'f':
                follow_links = 1;
                break;
            case 'r':
                recurse = 1;
                break;
            case 'd':
                node_data = 1;
                break;
            case 't':
                tol = atof (argarg);
                break;
        }
    }

    if (argind > argc - 2)
        print_usage (usgmsg, "CGNSfile1 and/or CGNSfile2 not given");

    file1 = argv[argind++];
    ADF_Database_Open (file1, "read_only", "native", &root1, &err);
    if (err > 0)
        err_exit (file1, "ADF_Database_Open", err);
    if (argind < argc - 1)
        ds1 = argv[argind++];

    file2 = argv[argind++];
    ADF_Database_Open (file2, "read_only", "native", &root2, &err);
    if (err > 0)
        err_exit (file2, "ADF_Database_Open", err);
    if (argind < argc)
        ds2 = argv[argind++];

    if (ds1 == NULL) {
        recurse = 1;
        compare_nodes ("/", root1, "/", root2);
    }
    else {
        ADF_Get_Node_ID (root1, ds1, &node1, &err);
        if (err > 0) {
            if (err == CHILD_NOT_OF_GIVEN_PARENT)
                err_exit (ds1, "node not found", 0);
            err_exit (ds1, "ADF_Get_Node_ID", err);
        }
        if (ds2 == NULL) ds2 = ds1;
        ADF_Get_Node_ID (root2, ds2, &node2, &err);
        if (err > 0) {
            if (err == CHILD_NOT_OF_GIVEN_PARENT)
                err_exit (ds2, "node not found", 0);
            err_exit (ds2, "ADF_Get_Node_ID", err);
        }
        compare_nodes (ds1, node1, ds2, node2);
    }

    ADF_Database_Close (root1, &err);
    ADF_Database_Close (root2, &err);
    return 0;
}

