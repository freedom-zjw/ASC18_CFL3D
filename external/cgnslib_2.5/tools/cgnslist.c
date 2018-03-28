#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "ADF.h"
#include "getargs.h"

#define MAX_LEADER 1024
#define INDENT     2

static char leader[MAX_LEADER+1];
static int leader_len;
static int indent = INDENT;
static int follow_links = 0;
static int out_flags = 0;

static char options[] = "i:faltds";
static char *usgmsg[] = {
    "usage  : cgnslist [options] CGNSfile [node]",
    "options:",
    "   -i<cnt> = set indent level (default 2)",
    "   -f      = follow links",
    "   -l      = print node label",
    "   -t      = print node data type",
    "   -d      = print node dimensions",
    "   -s      = print node size in bytes",
    "   -a      = print all -ltds",
    NULL
};

static void err_exit (char *name, int errcode)
{
    char errmsg[ADF_MAX_ERROR_STR_LENGTH+1];

    fflush (stdout);
    ADF_Error_Message (errcode, errmsg);
    fprintf (stderr, "error in %s:%s\n", name, errmsg);
    exit (1);
}

static void print_node (double node_id)
{
    int n, err, bytes;
    char label[ADF_LABEL_LENGTH+1];
    char type[ADF_DATA_TYPE_LENGTH+1];
    int ndim, dims[ADF_MAX_DIMENSIONS];

    if ((out_flags & 1) != 0) {
        ADF_Get_Label (node_id, label, &err);
        if (err > 0)
            err_exit ("ADF_Get_Label", err);
        printf (" %s", label);
    }
    if ((out_flags & 10) != 0) {
        ADF_Get_Data_Type (node_id, type, &err);
        if (err > 0)
            err_exit ("ADF_Get_Data_Type", err);
        if ((out_flags & 2) != 0)
            printf (" %s", type);
    }
    if ((out_flags & 12) != 0) {
        ADF_Get_Number_of_Dimensions (node_id, &ndim, &err);
        if (err > 0)
            err_exit ("ADF_Get_Number_of_Dimensions", err);
        if (ndim > 0) {
            ADF_Get_Dimension_Values (node_id, dims, &err);
            if (err > 0)
                err_exit ("ADF_Get_Dimension_Values", err);
        }
        if ((out_flags & 4) != 0) {
            printf (" (");
            if (ndim > 0) {
                printf ("%d", dims[0]);
                for (n = 1; n < ndim; n++)
                    printf (",%d", dims[n]);
            }
            putchar (')');
        }
        if ((out_flags & 8) != 0) {
            if (ndim < 1 || NULL != strchr ("LlMm", type[0]))
                bytes = 0;
            else if (NULL != strchr ("CcBb", type[0]))
                bytes = 1;
            else if (type[0] == 'X' || type[0] == 'x')
                bytes = type[1] == '8' ? 16 : 8;
            else
                bytes = type[1] == '8' ? 8 : 4;
            for (n = 0; n < ndim; n++)
                bytes *= dims[n];
            printf (" %d", bytes);
        }
    }
}

static void print_children (double parent_id)
{
    int nc, err, nchildren, len_ret;
    char *p = leader + leader_len;
    char name[ADF_NAME_LENGTH+1];
    char name_in_file[ADF_MAX_LINK_DATA_SIZE+1];
    char file_name[ADF_FILENAME_LENGTH+1];
    double child_id;

    ADF_Number_of_Children (parent_id, &nchildren, &err);
    if (err > 0)
        err_exit ("ADF_Number_of_Children", err);
    if (!nchildren) return;

    if (leader_len + indent > MAX_LEADER) {
        fprintf (stderr, "nesting is too deep\n");
        exit (1);
    }
    leader_len += indent;
    for (nc = 0; nc < indent; nc++)
        p[nc] = ' ';
    p[indent] = 0;

    for (nc = 1; nc <= nchildren; nc++) {
#ifdef NULL_NODEID_POINTER
        ADF_Children_IDs (parent_id, nc, 1, &len_ret, &child_id, &err);
        if (err > 0)
            err_exit ("ADF_Children_IDs", err);
        ADF_Get_Name (child_id, name, &err);
        if (err > 0)
            err_exit ("ADF_Get_Name", err);
#else
        ADF_Children_Names (parent_id, nc, 1, ADF_NAME_LENGTH+1,
            &len_ret, name, &err);
        if (err > 0)
            err_exit ("ADF_Children_Names", err);
        ADF_Get_Node_ID (parent_id, name, &child_id, &err);
        if (err > 0)
            err_exit ("ADF_Get_Node_ID", err);
#endif
        ADF_Is_Link (child_id, &len_ret, &err);
        if (err > 0)
            err_exit ("ADF_Is_Link", err);

        *p = 0;
        if (len_ret > 0) {
            ADF_Get_Link_Path (child_id, file_name, name_in_file, &err);
            if (err > 0)
                err_exit ("ADF_Get_Link_Path", err);
            if (*file_name)
                printf ("%s+-%s  -> %s @ %s\n", leader, name,
                    name_in_file, file_name);
            else
                printf ("%s+-%s  -> %s\n", leader, name, name_in_file);
        }
        else if (out_flags) {
            printf ("%s+-%s  --", leader, name);
            print_node (child_id);
            putchar ('\n');
        }
        else
            printf ("%s+-%s\n", leader, name);

        if (follow_links || len_ret <= 0) {
            *p = (char)(nc < nchildren ? '|' : ' ');
            print_children (child_id);
        }
    }
    *p = 0;
    leader_len -= indent;
}

int main (int argc, char *argv[])
{
    double root_id, node_id;
    int n = 1, err;
    char *name, rootname[ADF_NAME_LENGTH+1];

    if (argc < 2)
        print_usage (usgmsg, NULL);
    while ((n = getargs (argc, argv, options)) > 0) {
        switch (n) {
            case 'i':
                indent = atoi (argarg);
                if (indent < 1) {
                    fprintf (stderr, "indent must be > 0\n");
                    exit (1);
                }
                break;
            case 'f':
                follow_links = 1;
                break;
            case 'l':
                out_flags |= 1;
                break;
            case 't':
                out_flags |= 2;
                break;
            case 'd':
                out_flags |= 4;
                break;
            case 's':
                out_flags |= 8;
                break;
            case 'a':
                out_flags |= 15;
                break;
        }
    }

    if (argind == argc)
        print_usage (usgmsg, "CGNSfile not given");

    ADF_Database_Open (argv[argind], "read_only", "native", &root_id, &err);
    if (err > 0)
        err_exit ("ADF_Database_Open", err);

    if (++argind < argc) {
        name = argv[argind];
        ADF_Get_Node_ID (root_id, name, &node_id, &err);
        if (err > 0)
            err_exit ("ADF_Get_Node_ID", err);
    }
    else {
        ADF_Get_Name (root_id, rootname, &err);
        if (err > 0)
            err_exit ("ADF_Get_Name", err);
        node_id = root_id;
        name = rootname;
    }

    for (n = 0; n < indent; n++)
        leader[n] = ' ';
    leader[indent] = 0;
    leader_len = indent;

    if (out_flags) {
        printf ("%s  --", name);
        print_node (node_id);
        putchar ('\n');
    }
    else
        printf ("%s\n", name);
    print_children (node_id);

    ADF_Database_Close (root_id, &err);
    if (err > 0)
        err_exit ("ADF_Database_Close", err);
    exit (0);
    return 0; /* prevent compiler warning */
}

