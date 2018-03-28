/*
 * cgnsversion - this changes the version number of a CGNS file.
 *
 * This only works for versions 1.2 and later, since changes
 * were made in the ADF routines between 1.1 and 1.2 that
 * prevent the 1.1 CGNS library being able to read a file
 * written using the later ADF routines (CGNS version 1.2+)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
# include <io.h>
#else
# include <unistd.h>
#endif
#include "getargs.h"
#include "cgnslib.h"
#include "cgns_header.h"
#include "ADF.h"

#if !defined(CGNS_VERSION) || CGNS_VERSION < 2300
# error You need at least CGNS Version 2.3
#endif

#ifndef CG_MODE_READ
# define CG_MODE_READ MODE_READ
#endif

static cgns_base *CurrentBase;
static cgns_zone *CurrentZone;

static char *inpfile = NULL;
static char *outfile = NULL;

static cgns_file *cgfile;

static double inproot = -1.0;
static double outroot = -1.0;

static float FloatVersion;
static int LibraryVersion = CGNS_VERSION;
static int FileVersion;
static int FromVersion;

static int VersionList[] = {
    1200, 1270, 2000, 2100, 2200, 2300, 2400, 2500, 2510
};
#define nVersions (sizeof(VersionList)/sizeof(int))

static char datatype[ADF_DATA_TYPE_LENGTH+1];
static char label[ADF_LABEL_LENGTH+1];
static char linkfile[ADF_FILENAME_LENGTH+1];
static char linkpath[ADF_MAX_LINK_DATA_SIZE+1];

static int numdims, dimvals[ADF_MAX_DIMENSIONS];

/* command line options */

static int verbose = 0;
static int keep_links = 1;
static int keep_nodes = 0;

static char options[] = "vrk";

static char *usgmsg[] = {
    "usage  : cgnsversion [options] Version CGNSfile [CGNSoutfile]",
    "options:",
    "   -v : verbose output",
    "   -r : remove links",
    "   -k : keep all nodes",
    NULL
};


/*--------------------------------------------------------------------*/

static void error_exit (char *msg, int errcode)
{
    int ierr;

    /* if errcode, then it's an ADF error */

    if (errcode > 0) {
        char errmsg[ADF_MAX_ERROR_STR_LENGTH+1];
        ADF_Error_Message (errcode, errmsg);
        fprintf (stderr, "%s ERROR : %s\n", msg, errmsg);
    }
    else {

        /* if no error msg, then it's a CGNS error */

        if (msg == NULL) msg = (char *)cg_get_error();
        fprintf (stderr, "%s\n", msg);
    }

    /* close files, delete temporary file, and exit */

    if (inproot >= 0.0) ADF_Database_Close (inproot, &ierr);
    if (outroot >= 0.0) ADF_Database_Close (outroot, &ierr);
    if (outfile != NULL) unlink (outfile);
    exit (1);
}

/*--------------------------------------------------------------------*/

static int get_size (char *type, int nd, int *dims)
{
    int n, size = 1;

    if (nd < 1) return 0;
    for (n = 0; n < nd; n++)
        size *= dims[n];
    switch (*type) {
        case 'B':
        case 'C':
            if (type[1] == '1') return size;
            break;
        case 'I':
        case 'U':
            if (type[1] == '4') return size * sizeof(int);
            if (type[1] == '8') return size * sizeof(long);
            break;
        case 'R':
            if (type[1] == '4') return size * sizeof(float);
            if (type[1] == '8') return size * sizeof(double);
            break;
        case 'X':
            if (type[1] == '4') return 2 * size * sizeof(float);
            if (type[1] == '8') return 2 * size * sizeof(double);
            break;
    }
    return 0;
}

/*--------------------------------------------------------------------*/

static void copy_1200 (double inpid, double outid)
{
    int ierr, size;
    void *data;

    ADF_Get_Label (inpid, label, &ierr);
    if (ierr > 0) error_exit ("ADF_Get_Label", ierr);
    ADF_Set_Label (outid, label, &ierr);
    if (ierr > 0) error_exit ("ADF_Set_Label", ierr);

    ADF_Get_Data_Type (inpid, datatype, &ierr);
    if (ierr > 0) error_exit ("ADF_Get_Data_Type", ierr);
    ADF_Get_Number_of_Dimensions (inpid, &numdims, &ierr);
    if (ierr > 0) error_exit ("ADF_Get_Number_of_Dimensions", ierr);
    ADF_Get_Dimension_Values (inpid, dimvals, &ierr);
    if (ierr > 0) error_exit ("ADF_Get_Dimension_Values", ierr);
    if (numdims == 0) return;
    if (FromVersion == 1200 && numdims == 1) {
        numdims = 2;
        dimvals[1] = dimvals[0];
        dimvals[0] = 1;
    }
    if (FileVersion == 1200 && numdims == 2) {
        numdims = 1;
        dimvals[0] = dimvals[1];
    }
    ADF_Put_Dimension_Information (outid, datatype, numdims, dimvals, &ierr);
    if (ierr > 0) error_exit ("ADF_Put_Dimension_Information", ierr);

    size = get_size (datatype, numdims, dimvals);
    if (size == 0) return;
    if ((data = malloc (size)) == NULL)
        error_exit ("malloc failed for data", 0);
    ADF_Read_All_Data (inpid, data, &ierr);
    if (ierr > 0) {
        if (ierr != NO_DATA) error_exit ("ADF_Read_All_Data", ierr);
    }
    else {
        ADF_Write_All_Data (outid, data, &ierr);
        if (ierr > 0) error_exit ("ADF_Write_All_Data", ierr);
    }
    free (data);
}

/*--------------------------------------------------------------------*/

static void copy_node (double inpid, double outid)
{
    int ierr, size;
    void *data;

    ADF_Get_Label (inpid, label, &ierr);
    if (ierr > 0) error_exit ("ADF_Get_Label", ierr);
    ADF_Set_Label (outid, label, &ierr);
    if (ierr > 0) error_exit ("ADF_Set_Label", ierr);

    ADF_Get_Data_Type (inpid, datatype, &ierr);
    if (ierr > 0) error_exit ("ADF_Get_Data_Type", ierr);
    ADF_Get_Number_of_Dimensions (inpid, &numdims, &ierr);
    if (ierr > 0) error_exit ("ADF_Get_Number_of_Dimensions", ierr);
    if (numdims == 0) return;
    ADF_Get_Dimension_Values (inpid, dimvals, &ierr);
    if (ierr > 0) error_exit ("ADF_Get_Dimension_Values", ierr);
    ADF_Put_Dimension_Information (outid, datatype, numdims, dimvals, &ierr);
    if (ierr > 0) error_exit ("ADF_Put_Dimension_Information", ierr);

    size = get_size (datatype, numdims, dimvals);
    if (size == 0) return;
    if ((data = malloc (size)) == NULL)
        error_exit ("malloc failed for data", 0);
    ADF_Read_All_Data (inpid, data, &ierr);
    if (ierr > 0) {
        if (ierr != NO_DATA) error_exit ("ADF_Read_All_Data", ierr);
    }
    else {
        ADF_Write_All_Data (outid, data, &ierr);
        if (ierr > 0) error_exit ("ADF_Write_All_Data", ierr);
    }
    free (data);
}

/*--------------------------------------------------------------------*/

static double get_child_id (double parid, int nchild)
{
    int cnt, ierr;
    double childid;

    ADF_Children_IDs (parid, nchild, 1, &cnt, &childid, &ierr);
    if (ierr > 0) error_exit ("ADF_Children_IDs", ierr);
    return childid;
}

/*--------------------------------------------------------------------*/

static double create_node (double inpid, double parid, int recurse)
{
    int ierr, nchild, nc, cnt;
    char name[ADF_NAME_LENGTH+1],type[ADF_LABEL_LENGTH+1];
    double outid, inpchild;

    ADF_Get_Name (inpid, name, &ierr);
    if (ierr > 0) error_exit ("ADF_Get_Name", ierr);
    ADF_Get_Label (inpid, type, &ierr);
    if (ierr > 0) error_exit ("ADF_Get_Label", ierr);

    if (!keep_nodes) {
        if (FileVersion < 2100 &&
            0 == strcmp (type, "UserDefinedData_t"))
            return 0.0;
        if (FileVersion < 2400 &&
           (0 == strcmp (type, "AdditionalUnits_t") ||
            0 == strcmp (type, "AdditionalExponents_t")))
            return 0.0;
    }

    ADF_Is_Link (inpid, &cnt, &ierr);
    if (ierr > 0) error_exit ("ADF_Is_Link", ierr);

    /* create link */

    if (cnt > 0 && keep_links) {
        ADF_Get_Link_Path (inpid, linkfile, linkpath, &ierr);
        if (ierr > 0) error_exit ("ADF_Get_Link_Path", ierr);
        ADF_Link (parid, name, linkfile, linkpath, &outid, &ierr);
        if (ierr > 0) error_exit ("ADF_Link", ierr);
        return 0.0;
    }

    /* create node */

    ADF_Create (parid, name, &outid, &ierr);
    if (ierr > 0) error_exit ("ADF_Create", ierr);
    copy_node (inpid, outid);

    if (!recurse) return outid;

    /* recurse on children */

    ADF_Number_of_Children (inpid, &nchild, &ierr);
    if (ierr > 0) error_exit ("ADF_Number_of_Children", ierr);
    if (nchild < 1) return outid;

    if (FileVersion < 2100 &&
        0 == strcmp (type, "FlowEquationSet_t")) {
        for (nc = 1; nc <= nchild; nc++) {
            inpchild = get_child_id (inpid, nc);
            ADF_Get_Label (inpchild, label, &ierr);
            if (ierr > 0) error_exit ("ADF_Get_Label", ierr);
            if (strcmp (label, "ThermalRelaxationModel_t") &&
                strcmp (label, "ChemicalKineticsModel_t") &&
                strcmp (label, "EMElectricFieldModel_t") &&
                strcmp (label, "EMMagneticFieldModel_t") &&
                strcmp (label, "EMConductivityModel_t"))
                create_node (inpchild, outid, 1);
        }
    }
    else if (FileVersion < 2400 &&
        0 == strcmp (type, "FlowEquationSet_t")) {
        for (nc = 1; nc <= nchild; nc++) {
            inpchild = get_child_id (inpid, nc);
            ADF_Get_Label (inpchild, label, &ierr);
            if (ierr > 0) error_exit ("ADF_Get_Label", ierr);
            if (strcmp (label, "EMElectricFieldModel_t") &&
                strcmp (label, "EMMagneticFieldModel_t") &&
                strcmp (label, "EMConductivityModel_t"))
                create_node (inpchild, outid, 1);
        }
    }
    else if (FileVersion < 2400 &&
        0 == strcmp (type, "UserDefinedData_t")) {
        for (nc = 1; nc <= nchild; nc++) {
            inpchild = get_child_id (inpid, nc);
            ADF_Get_Label (inpchild, label, &ierr);
            if (ierr > 0) error_exit ("ADF_Get_Label", ierr);
            if (strcmp (label, "UserDefinedData_t") &&
                strcmp (label, "FamilyName_t") &&
                strcmp (label, "Ordinal_t") &&
                strcmp (label, "GridLocation_t") &&
                strcmp (label, "IndexArray_t") &&
                strcmp (label, "IndexRange_t"))
                create_node (inpchild, outid, 1);
        }
    }
    else if (FileVersion < 2400 &&
        0 == strcmp (type, "GridConnectivity1to1_t")) {
        for (nc = 1; nc <= nchild; nc++) {
            inpchild = get_child_id (inpid, nc);
            ADF_Get_Label (inpchild, label, &ierr);
            if (ierr > 0) error_exit ("ADF_Get_Label", ierr);
            if (strcmp (label, "GridConnectivityProperty_t"))
                create_node (inpchild, outid, 1);
        }
    }
    else if (FileVersion < 2400 &&
        0 == strcmp (type, "Family_t")) {
        for (nc = 1; nc <= nchild; nc++) {
            inpchild = get_child_id (inpid, nc);
            ADF_Get_Label (inpchild, label, &ierr);
            if (ierr > 0) error_exit ("ADF_Get_Label", ierr);
            if (strcmp (label, "RotatingCoordinates_t"))
                create_node (inpchild, outid, 1);
        }
    }
    else if (FileVersion < 2400 &&
        0 == strcmp (type, "FamilyBC_t")) {
        for (nc = 1; nc <= nchild; nc++) {
            inpchild = get_child_id (inpid, nc);
            ADF_Get_Label (inpchild, label, &ierr);
            if (ierr > 0) error_exit ("ADF_Get_Label", ierr);
            if (strcmp (label, "BCDataSet_t"))
                create_node (inpchild, outid, 1);
        }
    }
    else {
        for (nc = 1; nc <= nchild; nc++) {
            inpchild = get_child_id (inpid, nc);
            create_node (inpchild, outid, 1);
        }
    }
    return outid;
}

/*--------------------------------------------------------------------*/

void fix_name (double pid, double id, char *newname)
{
    int ierr;
    char oldname[ADF_NAME_LENGTH+1];

    ADF_Get_Name (id, oldname, &ierr);
    if (ierr > 0) error_exit ("ADF_Get_Name", ierr);
    if (strcmp (oldname, newname)) {
        ADF_Put_Name (pid, id, newname, &ierr);
        if (ierr > 0) error_exit ("ADF_Put_Name", ierr);
    }
}

/*--------------------------------------------------------------------*/

void CGI_write_dataset(double parent_id, cgns_dataset *dataset,
    GridLocation_t location) {
    int dim_vals, n;
    const char *type_name;

    if (dataset->link && keep_links) {
        create_node (dataset->id, parent_id, 0);
        return;
    }

    /* BCDataSet_t */
#if CGNS_VERSION >= 2500
    type_name = cg_BCTypeName(dataset->type);
#else
    type_name = BCTypeName[dataset->type];
#endif
    dim_vals= strlen(type_name);
    if (cgi_new_node(parent_id, dataset->name, "BCDataSet_t", &dataset->id,
        "C1", 1, &dim_vals, (void *)type_name))
        error_exit (NULL, 0);

    /* GridLocation */
#if CGNS_VERSION >= 2400
    if (FileVersion >= 2400) location = dataset->location;
#endif
    if (location != Vertex && (FileVersion == 1200 || FileVersion >= 2400)) {
        double dummy_id;
#if CGNS_VERSION >= 2500
        type_name = cg_GridLocationName(location);
#else
        type_name = GridLocationName[location];
#endif
        dim_vals = strlen(type_name);
        if (cgi_new_node(dataset->id, "GridLocation", "GridLocation_t",
            &dummy_id, "C1", 1, &dim_vals, (void *)type_name))
            error_exit (NULL, 0);
    }

    /* DirichletData */
    if (dataset->dirichlet)
        create_node (dataset->dirichlet->id, dataset->id, 1);

    /* NeumannData */
    if (dataset->neumann)
        create_node (dataset->neumann->id, dataset->id, 1);

     /* Descriptor_t */
    for (n=0; n<dataset->ndescr; n++)
        create_node (dataset->descr[n].id, dataset->id, 1);

    /* ReferenceState_t */
    if (dataset->state) create_node (dataset->state->id, dataset->id, 1);

    /* DataClass_t */
    if (dataset->data_class &&
        cgi_write_dataclass (dataset->id, dataset->data_class))
        error_exit (NULL, 0);

    /* DimensionalUnits_t */
    if (dataset->units) create_node (dataset->units->id, dataset->id, 1);

    if (keep_nodes || FileVersion >= 2100) {
        /* UserDefinedData_t */
        for (n=0; n<dataset->nuser_data; n++)
            create_node (dataset->user_data[n].id, dataset->id, 1);
    }
#if CGNS_VERSION >= 2400
    if (dataset->ptset && (keep_nodes || FileVersion >= 2400))
        create_node (dataset->ptset->id, dataset->id, 1);
#endif
}

/*--------------------------------------------------------------------*/

void CGI_write_boco (double parent_id, cgns_boco *boco) {
    int dim_vals, n, ierr;
    double dummy_id;
    const char *type_name;
    BCType_t type;
    GridLocation_t location = boco->location;

    if (boco->link && keep_links) {
        create_node (boco->id, parent_id, 0);
        return;
    }

    /* BC_t */
    type = boco->type;
    if (type == FamilySpecified && FileVersion < 2000) {
        printf ("WARNING:BC type FamilySpecified changed to UserDefined\n");
        printf ("        for boundary condition \"%s\"\n", boco->name);
        type = BCTypeUserDefined;
    }
#if CGNS_VERSION >= 2500
    type_name = cg_BCTypeName(type);
#else
    type_name = BCTypeName[type];
#endif
    dim_vals = strlen(type_name);
    if (cgi_new_node(parent_id, boco->name, "BC_t", &boco->id, "C1",
        1, &dim_vals, (void *)type_name)) error_exit (NULL, 0);

    if (boco->ptset) {
        PointSetType_t ptype = boco->ptset->type;

        /* handle the various combinations of PointSetType and GridLocation */

        if (ptype == PointList || ptype == PointRange) {
            if (location != Vertex) {
                if (FileVersion == 1200 || FileVersion >= 2300) {
                    if (ptype == PointList)
                        ptype = ElementList;
                    else
                        ptype = ElementRange;
                    location = Vertex;
                }
            }
        }
        else if (ptype == ElementList || ptype == ElementRange) {
            if (FileVersion > 1200 && FileVersion < 2300) {
                if (ptype == ElementList)
                    ptype = PointList;
                else
                    ptype = PointRange;
                location = FaceCenter;
            }
            else
                location = Vertex;
        }
        else
            ptype = 0;

        if (ptype) {
#if CGNS_VERSION >= 2500
            type_name = cg_PointSetTypeName(ptype);
#else
            type_name = PointSetTypeName[ptype];
#endif
            ADF_Create (boco->id, type_name, &dummy_id, &ierr);
            if (ierr > 0) error_exit ("ADF_Create", ierr);
            if (CurrentZone->type == Unstructured &&
                ((FileVersion == 1200 &&
                  (ptype == ElementList ||
                   ptype == ElementRange)) ||
                 (FromVersion == 1200 &&
                  (boco->ptset->type == ElementList ||
                   boco->ptset->type == ElementRange))))
                copy_1200 (boco->ptset->id, dummy_id);
            else
                copy_node (boco->ptset->id, dummy_id);
        }
    }

    /* GridLocation */
    if (location != Vertex && FileVersion > 1200) {
#if CGNS_VERSION >= 2500
        type_name = cg_GridLocationName(location);
#else
        type_name = GridLocationName[location];
#endif
        dim_vals = strlen(type_name);
        if (cgi_new_node(boco->id, "GridLocation", "GridLocation_t",
            &dummy_id, "C1", 1, &dim_vals, (void *)type_name))
            error_exit (NULL, 0);
    }

     /* FamilyName_t */
    if (boco->family_name[0]!='\0') {
        if (FileVersion == 1200) {
            if (cgi_new_node(boco->id, boco->family_name, "FamilyName_t",
                &dummy_id, "MT",0, &dim_vals, NULL))
                error_exit (NULL, 0);
        }
        else {
            dim_vals = strlen(boco->family_name);
            if (cgi_new_node(boco->id, "FamilyName", "FamilyName_t",
                &dummy_id, "C1",1, &dim_vals, (void *)boco->family_name))
                error_exit (NULL, 0);
        }
    }

    /* BCDataSet_t */
    for (n=0; n<boco->ndataset; n++)
        CGI_write_dataset (boco->id, &boco->dataset[n], boco->location);

     /* InwardNormalIndex */
    if (boco->Nindex) {
        if (cgi_new_node(boco->id, "InwardNormalIndex",
            "\"int[IndexDimension]\"", &boco->index_id, "I4", 1,
            &(CurrentZone->index_dim), (void *)boco->Nindex))
            error_exit (NULL, 0);
    }

     /* InwardNormalList */
    if (boco->normal) create_node (boco->normal->id, boco->id, 0);

    /* Descriptor_t */
    for (n=0; n<boco->ndescr; n++)
        create_node (boco->descr[n].id, boco->id, 1);

    /* ReferenceState_t */
    if (boco->state) create_node (boco->state->id, boco->id, 1);

    /* DataClass_t */
    if (boco->data_class &&
        cgi_write_dataclass(boco->id, boco->data_class))
        error_exit (NULL, 0);

    /* DimensionalUnits_t */
    if (boco->units) create_node (boco->units->id, boco->id, 1);

     /* Ordinal_t */
    if (boco->ordinal &&
        cgi_write_ordinal(boco->id, boco->ordinal))
        error_exit (NULL, 0);

    if (keep_nodes || FileVersion >= 2100) {
        /* BCProperty_t */
        if (boco->bprop && (keep_nodes || FileVersion >= 2200))
            create_node (boco->bprop->id, boco->id, 1);
        /* UserDefinedData_t */
        for (n=0; n<boco->nuser_data; n++)
            create_node (boco->user_data[n].id, boco->id, 1);
    }
}

/*--------------------------------------------------------------------*/

void CGI_write_zboco(double parent_id, cgns_zboco *zboco) {
    int n;

    if (zboco->link && keep_links) {
        create_node (zboco->id, parent_id, 0);
        return;
    }

    /* ZoneBC_t */
    if (cgi_new_node(parent_id, "ZoneBC", "ZoneBC_t", &zboco->id,
        "MT", 0, 0, 0)) error_exit (NULL, 0);

    /* BC_t */
    for (n=0; n<zboco->nbocos; n++)
        CGI_write_boco (zboco->id, &zboco->boco[n]);

    /* Descriptor_t */
    for (n=0; n<zboco->ndescr; n++)
        create_node (zboco->descr[n].id, zboco->id, 1);

    /* ReferenceState_t */
    if (zboco->state) create_node (zboco->state->id, zboco->id, 1);

    /* DataClass_t */
    if (zboco->data_class &&
        cgi_write_dataclass (zboco->id, zboco->data_class))
        error_exit (NULL, 0);

    /* DimensionalUnits_t */
    if (zboco->units) create_node (zboco->units->id, zboco->id, 1);

    if (keep_nodes || FileVersion >= 2100) {
        /* UserDefinedData_t */
        for (n=0; n<zboco->nuser_data; n++)
            create_node (zboco->user_data[n].id, zboco->id, 1);
    }
}

/*--------------------------------------------------------------------*/

void CGI_write_conns (double parent_id, cgns_conn *conn) {
    int n, dim_vals;
    double dummy_id;
    const char *type_name;
    cgns_ptset *ptset;
    GridLocation_t location = conn->location;

    if (conn->link && keep_links) {
        create_node (conn->id, parent_id, 0);
        return;
    }

    dim_vals = strlen(conn->donor);
    if (cgi_new_node(parent_id, conn->name, "GridConnectivity_t",
        &conn->id, "C1", 1, &dim_vals, conn->donor))
        error_exit (NULL, 0);

     /* GridConnectivityType_t */
#if CGNS_VERSION >= 2500
    type_name = cg_GridConnectivityTypeName(conn->type);
#else
    type_name = GridConnectivityTypeName[conn->type];
#endif
    dim_vals = strlen(type_name);
    if (cgi_new_node(conn->id, "GridConnectivityType",
        "GridConnectivityType_t", &dummy_id, "C1", 1, &dim_vals,
        (void *)type_name))
        error_exit (NULL, 0);

     /* write GridLocation */
    if (location != Vertex) {
        if (location != CellCenter && FileVersion < 2300) {
            if (FileVersion == 2200)
                location = FaceCenter;
            else
                location = CellCenter;
        }
#if CGNS_VERSION >= 2500
        type_name = cg_GridLocationName(location);
#else
        type_name = GridLocationName[location];
#endif
        dim_vals = strlen(type_name);
        if (cgi_new_node(conn->id, "GridLocation", "GridLocation_t",
            &dummy_id, "C1", 1, &dim_vals, (void *)type_name))
            error_exit (NULL, 0);
    }
    if (location != conn->location) {
        printf ("WARNING:GridLocation changed from %s to %s\n",
#if CGNS_VERSION >= 2500
            cg_GridLocationName(conn->location), type_name);
#else
            GridLocationName[conn->location], type_name);
#endif
        printf ("        for connectivity \"%s\"\n", conn->name);
    }

    /* PointRange or PointList */

    ptset = &(conn->ptset);
    create_node (ptset->id, conn->id, 0);

    /* Cell or Point ListDonor */
    ptset = &(conn->dptset);
    if (FileVersion > 1200) {
        create_node (ptset->id, conn->id, 0);
        if (conn->interpolants)
            create_node (conn->interpolants->id, conn->id, 1);
    }
    else {
        double donorid;
        ZoneType_t dtype = ZoneTypeNull;
        PointSetType_t ptype = ptset->type;
        for (n = 0; n < CurrentBase->nzones; n++) {
            if (0 == strcmp (conn->donor, CurrentBase->zone[n].name)) {
                dtype = CurrentBase->zone[n].type;
                break;
            }
        }
        if (dtype == Structured) {
            if (cgi_new_node(conn->id, "StructuredDonor",
                "StructuredDonor_t", &dummy_id, "MT", 0, &dim_vals, NULL))
                error_exit (NULL, 0);
            donorid = create_node (ptset->id, dummy_id, 0);
            ptype = PointListDonor;
        }
        else if (dtype == Unstructured) {
            if (cgi_new_node(conn->id, "UnstructuredDonor",
                "UnstructuredDonor_t", &dummy_id, "MT", 0, &dim_vals, NULL))
                error_exit (NULL, 0);
            donorid = create_node (ptset->id, dummy_id, 0);
            if (ptype != PointListDonor && ptype != CellListDonor)
                ptype = (location == Vertex || conn->interpolants == NULL) ?
                    PointListDonor : CellListDonor;
            if (ptype == CellListDonor && conn->interpolants)
                create_node (conn->interpolants->id, dummy_id, 0);
        }
        else {
            printf ("ERROR:couldn't find donor zone \"%s\" for \"%s\"\n",
                conn->donor, conn->name);
            printf ("     therefore donor data was not written\n");
            donorid = 0.0;
        }
        if (ptype != ptset->type && donorid > 0.0) {
#if CGNS_VERSION >= 2500
            type_name = cg_PointSetTypeName(ptype);
#else
            type_name = PointSetTypeName[ptype];
#endif
            fix_name (dummy_id, donorid, (char *)type_name);
            printf ("WARNING:%s changed to %s\n",
#if CGNS_VERSION >= 2500
                cg_PointSetTypeName(ptset->type), type_name);
#else
                PointSetTypeName[ptset->type], type_name);
#endif
            printf ("        for connectivity \"%s\"\n", conn->name);
        }
    }

    /* Descriptor_t */
    for (n=0; n<conn->ndescr; n++)
        create_node (conn->descr[n].id, conn->id, 1);

     /* Ordinal_t */
    if (conn->ordinal &&
        cgi_write_ordinal(conn->id, conn->ordinal))
        error_exit (NULL, 0);

    if (keep_nodes || FileVersion >= 2100) {
        /* GridConnectivityProperty_t */
        if (conn->cprop && (keep_nodes || FileVersion >= 2200))
            create_node (conn->cprop->id, conn->id, 1);
        /* UserDefinedData_t */
        for (n=0; n<conn->nuser_data; n++)
            create_node (conn->user_data[n].id, conn->id, 1);
    }
}

/*--------------------------------------------------------------------*/

void CGI_write_zconn(double parent_id, cgns_zconn *zconn) {
    int n;

    if (zconn->link && keep_links) {
        create_node (zconn->id, parent_id, 0);
        return;
    }

    /* ZoneGridConnectivity_t */
    if (cgi_new_node(parent_id, "ZoneGridConnectivity",
        "ZoneGridConnectivity_t", &zconn->id, "MT", 0, 0, 0))
        error_exit (NULL, 0);

    /* GridConnectivity1to1_t */
    for (n=0; n<zconn->n1to1; n++)
        create_node (zconn->one21[n].id, zconn->id, 1);

    /* GridConnectivity_t */
    for (n=0; n<zconn->nconns; n++)
        CGI_write_conns (zconn->id, &zconn->conn[n]);

    /* OversetHoles_t */
    for (n=0; n<zconn->nholes; n++)
        create_node (zconn->hole[n].id, zconn->id, 1);

    /* Descriptor_t */
    for (n=0; n<zconn->ndescr; n++)
        create_node (zconn->descr[n].id, zconn->id, 1);

    if (keep_nodes || FileVersion >= 2100) {
        /* UserDefinedData_t */
        for (n=0; n<zconn->nuser_data; n++)
            create_node (zconn->user_data[n].id, zconn->id, 1);
    }
}

/*--------------------------------------------------------------------*/

void CGI_write_zone(double parent_id, cgns_zone *zone) {
    int n, dim_vals[2];
    double dummy_id;
    const char *type_name;

    CurrentZone = zone;
    if (zone->link && keep_links) {
        create_node (zone->id, parent_id, 0);
        return;
    }

     /* Create the Zone_t nodes */
    dim_vals[0]= zone->index_dim;
    dim_vals[1]= 3;
    if (cgi_new_node(parent_id, zone->name, "Zone_t", &zone->id,
        "I4", 2, dim_vals, (void *)zone->nijk))
        error_exit (NULL, 0);

     /* write ZoneType */
#if CGNS_VERSION >= 2500
    type_name = cg_ZoneTypeName(zone->type);
#else
    type_name = ZoneTypeName[zone->type];
#endif
    dim_vals[0] = strlen(type_name);
    if (cgi_new_node(zone->id, "ZoneType", "ZoneType_t", &dummy_id,
        "C1", 1, dim_vals, (void *)type_name))
        error_exit (NULL, 0);

    /* GridCoordinates_t */
    if (zone->nzcoor >= 1) {
        if (FileVersion >= 2000) {
            for (n=0; n<zone->nzcoor; n++)
                create_node (zone->zcoor[n].id, zone->id, 1);
        }
        else {
            dummy_id = create_node (zone->zcoor->id, zone->id, 1);
            fix_name (zone->id, dummy_id, "GridCoordinates");
        }
    }

     /* FamilyName_t */
    if (zone->family_name[0]!='\0') {
        if (FileVersion == 1200) {
            if (cgi_new_node(zone->id, zone->family_name, "FamilyName_t",
                &dummy_id, "MT",0, dim_vals, NULL))
                error_exit (NULL, 0);
        }
        else {
            dim_vals[0] = strlen(zone->family_name);
            if (cgi_new_node(zone->id, "FamilyName", "FamilyName_t",
                &dummy_id, "C1",1, dim_vals, (void *)zone->family_name))
                error_exit (NULL, 0);
        }
    }

    /* Elements_t */
    for (n=0; n<zone->nsections; n++)
        create_node (zone->section[n].id, zone->id, 1);

    /* FlowSolution_t */
    for (n=0; n<zone->nsols; n++)
        create_node (zone->sol[n].id, zone->id, 1);

    /* ZoneGridConnectivity_t */
    if (zone->zconn) CGI_write_zconn(zone->id, zone->zconn);

    /* ZoneBC_t */
    if (zone->zboco) CGI_write_zboco(zone->id, zone->zboco);

    /* DescreteData_t */
    for (n=0; n<zone->ndiscrete; n++)
        create_node (zone->discrete[n].id, zone->id, 1);

    /* Descriptor_t */
    for (n=0; n<zone->ndescr; n++)
        create_node (zone->descr[n].id, zone->id, 1);

    /* ReferenceState_t */
    if (zone->state) create_node (zone->state->id, zone->id, 1);

    /* DataClass_t */
    if (zone->data_class && cgi_write_dataclass (zone->id, zone->data_class))
        error_exit (NULL, 0);

    /* DimensionalUnits_t */
    if (zone->units) create_node (zone->units->id, zone->id, 1);

    /* ConvergenceHistory_t */
    if (zone->converg) create_node (zone->converg->id, zone->id, 1);

    /* FlowEquationSet_t */
    if (zone->equations) create_node (zone->equations->id, zone->id, 1);

    /* IntegralData_t */
    for (n=0; n<zone->nintegrals; n++)
        create_node (zone->integral[n].id, zone->id, 1);

     /* Ordinal_t */
    if (zone->ordinal && cgi_write_ordinal (zone->id, zone->ordinal))
        error_exit (NULL, 0);

    if (keep_nodes || FileVersion >= 2000) {
        /* RigidGridMotion_t */
        for (n=0; n<zone->nrmotions; n++)
            create_node (zone->rmotion[n].id, zone->id, 1);

        /* ArbitraryGridMotion_t */
        for (n=0; n<zone->namotions; n++)
            create_node (zone->amotion[n].id, zone->id, 1);

        /* ZoneIterativeData_t */
        if (zone->ziter) create_node (zone->ziter->id, zone->id, 1);

        if (keep_nodes || FileVersion >= 2100) {
            /* UserDefinedData_t */
            for (n=0; n<zone->nuser_data; n++)
                create_node (zone->user_data[n].id, zone->id, 1);
             /* RotatingCoordinates_t */
            if (zone->rotating && (keep_nodes || FileVersion >= 2200))
                create_node (zone->rotating->id, zone->id, 1);
        }
    }
}

/*--------------------------------------------------------------------*/

void CGI_write () {
    cgns_base *base;
    int n, b;
    int dim_vals, data[2];
    double dummy_id;

    /* write version number */
    dim_vals = 1;
    if (cgi_new_node(cgfile->rootid, "CGNSLibraryVersion", "CGNSLibraryVersion_t",
        &dummy_id, "R4", 1, &dim_vals, (void *)&FloatVersion))
        error_exit (NULL, 0);

    /* write all CGNSBase_t nodes in ADF file */
    for (b=0; b < cgfile->nbases; b++) {
        CurrentBase = base = &(cgfile->base[b]);

        data[0]=base->cell_dim;
        data[1]=base->phys_dim;

        /* Create the CGNSBase_t nodes */
        dim_vals=2;
        if (cgi_new_node(cgfile->rootid, base->name, "CGNSBase_t", &base->id,
            "I4", 1, &dim_vals, (void *)data))
            error_exit (NULL, 0);

        /* Descriptor_t */
        for (n=0; n<base->ndescr; n++)
            create_node (base->descr[n].id, base->id, 1);

        /* ReferenceState_t */
        if (base->state) create_node (base->state->id, base->id, 1);

        if (keep_nodes || FileVersion >= 2200) {
            /* Gravity_t */
            if (base->gravity) create_node (base->gravity->id, base->id, 1);

            /* Axisymmetry_t */
            if (base->axisym) create_node (base->axisym->id, base->id, 1);

            /* RotatingCoordinates_t */
            if (base->rotating) create_node (base->rotating->id, base->id, 1);
        }

        /* Zone_t */
        for (n=0; n<base->nzones; n++)
            CGI_write_zone(base->id, &base->zone[n]);

       /* Family_t */
        for (n=0; n<base->nfamilies; n++)
            create_node (base->family[n].id, base->id, 1);

       /* DataClass_t */
        if (base->data_class &&
            cgi_write_dataclass (base->id, base->data_class))
            error_exit (NULL, 0);

       /* DimensionalUnits_t */
        if (base->units) create_node (base->units->id, base->id, 1);

       /* ConvergenceHistory_t */
        if (base->converg) create_node (base->converg->id, base->id, 1);

       /* FlowEquationSet_t */
        if (base->equations) create_node (base->equations->id, base->id, 1);

       /* IntegralData_t */
        for (n=0; n<base->nintegrals; n++)
            create_node (base->integral[n].id, base->id, 1);

        if (keep_nodes || FileVersion >= 2000) {
            /* SimulationType_t */
            if (base->type) {
                const char *sim_name;
#if CGNS_VERSION >= 2500
                sim_name = cg_SimulationTypeName(base->type);
#else
                sim_name = SimulationTypeName[base->type];
#endif
                dim_vals = strlen(sim_name);
                if (cgi_new_node(base->id, "SimulationType", "SimulationType_t",
                    &base->type_id,"C1", 1, &dim_vals, (void *)sim_name))
                    error_exit (NULL, 0);
            }

            /* BaseIterativeData_t */
            if (base->biter) create_node (base->biter->id, base->id, 1);

            /* UserDefinedData_t */
            if (keep_nodes || FileVersion >= 2100) {
                for (n=0; n<base->nuser_data; n++)
                    create_node (base->user_data[n].id, base->id, 1);
            }
        }
    }
}

/*--------------------------------------------------------------------*/

int main (int argc, char **argv)
{
    char tmpfile[16];
    int n, ierr, inpfn;
    float file_version;

    if (argc < 3)
        print_usage (usgmsg, NULL);

    while ((n = getargs (argc, argv, options)) > 0) {
        switch (n) {
            case 'v':
                verbose = 1;
                break;
            case 'r':
                keep_links = 0;
                break;
            case 'k':
                keep_nodes = 1;
                break;
        }
    }

    if (argind >= argc - 1)
        print_usage (usgmsg, "version and/or CGNSfile not given");

    /* get the file version */

    FloatVersion = (float)atof(argv[argind++]);
    FileVersion = (int)(1000.0 * FloatVersion + 0.5);

    if (FileVersion < 1200) {
        fprintf (stderr,
            "ADF incompatibilities do not allow versions prior to 1.2\n");
        exit (1);
    }

    for (n = 0; n < nVersions; n++) {
        if (FileVersion == VersionList[n]) break;
    }
    if (n >= nVersions || VersionList[n] > LibraryVersion) {
        fprintf (stderr, "Version %g is not valid\n", FloatVersion);
        fprintf (stderr, "Version is one of 1.2");
        for (n = 1; n < nVersions; n++) {
            if (VersionList[n] < LibraryVersion)
                fprintf (stderr, ", %g", 0.001 * (float)VersionList[n]);
        }
        fprintf (stderr, " or %g\n", 0.001 * (float)LibraryVersion);
        exit (1);
    }

    inpfile = argv[argind++];
    if (access (inpfile, 0)) {
        fprintf (stderr, "input file \"%s\" not found\n", inpfile);
        exit (1);
    }

    /* read the input file using the CGNS routines in order to
       fill in the cgns_* structures. */

    if (cg_open (inpfile, CG_MODE_READ, &inpfn)) cg_error_exit();

    /* get version and check if valid */

    if (cg_version (inpfn, &file_version)) cg_error_exit();
    FromVersion = (int)(1000.0 * file_version + 0.5);
    if (LibraryVersion < FromVersion) {
        cg_close (inpfn);
        fprintf (stderr,
            "file version is more recent than then CGNS library version\n");
        exit (1);
    }
    if (FileVersion == FromVersion) {
        cg_close (inpfn);
        fprintf (stderr, "file version is already at version %g\n",
            FloatVersion);
        exit (1);
    }

    printf ("converting \"%s\" from version %g to %g\n",
        inpfile, file_version, FloatVersion);

    if (FileVersion < 2100) keep_links = 0;

    /* set the default cgns_file structure pointer */

    cgfile = cgi_get_file(inpfn);
    inproot = cgfile->rootid;

    /* write output to temporary file */

    strcpy (tmpfile, "cgnsXXXXXX");
    outfile = mktemp (tmpfile);
    if (outfile == NULL) {
        cg_close (inpfn);
        fprintf (stderr, "failed to create a temporary file\n");
        exit (1);
    }
    printf ("writing output to temporary file \"%s\"\n", outfile);

    /* use ADF routines to write the file */

    ADF_Database_Open (outfile, "NEW", "NATIVE", &outroot, &ierr);
    if (ierr > 0) error_exit ("ADF_Database_Open", ierr);

    cgfile->rootid = outroot;
    CGI_write ();

    /* close using ADF routines */

    ADF_Database_Close (inproot, &ierr);
    inproot = -1.0;
    ADF_Database_Close (outroot, &ierr);
    outroot = -1.0;
    if (ierr > 0) error_exit ("ADF_Database_Close", ierr);

    /* rename the temporary file to the output name */

    if (argind < argc)
        inpfile = argv[argind];
    printf ("renaming \"%s\" to \"%s\"\n", outfile, inpfile);

    unlink (inpfile);
    if (rename (outfile, inpfile)) {
        fprintf (stderr, "rename %s -> %s failed", outfile, inpfile);
        exit (1);
    }

    return 0;
}

