/*
 *  DBconv.c
 *
 *	Convert a text database file structured as
 *
 *  FIELD1  field contents
 *  FIELD2  2nd field contents
 *  ...	    etc...
 *  EOR
 *
 *	to tab-delimited format so as to make it processable by
 *  standard database management systems.
 *
 *  Author:
 *	J. R. Valverde	(April, 19th, 1991)
 *	Biomedical Research Institute, Madrid. SPAIN
 *
 *	JRamon@uamed.uam.es
 *	JRamon@ccuam3.uam.es
 *	JRamon@cnbvx1.uam.es
 *
 *	JRamon@EMDUAM51.Bitnet
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "dbconv.h"

flag ref, db;
record_t rc;
char *ref_filnam = NULL;
char *db_filnam = NULL;
char *out_filnam = NULL;
char ref_default[] = "DBConv.cf";
char db_default[MAXPATHLEN] = "-";
char out_default[MAXPATHLEN] = "-";
char out_us = '\t'; 	/* unit/field separator (named after ASCII US)*/
char out_rs = '\n'; 	/* record separator (named after ASCII RS) */
int quote_output;
int print_header;
FILE *ref_file;
FILE *db_file;
FILE *out_file;

static void parse_arguments(int, char *[]);
static void open_files();
static void usage();

int main(argc, argv)
int argc;
char *argv[];
{
    extern char *ref_filnam, *db_filnam, *out_filnam;
    extern FILE *ref_file, *db_file, *out_file;

    if (isatty(0)) {
	fprintf(stderr,
	    "DBconv: a program to convert database reports to TAB format\n");
	fprintf(stderr, "\tBy J. R. Valverde (%s)\n", "Aug, 14 1991");
	fprintf(stderr, "\t   Biomedical Research Institute, Madrid. SPAIN\n");
	fprintf(stderr, "\t   J. R. Valverde (%s)\n", "Aug, 01 2005");
	fprintf(stderr, "\t   <jrvalverde@acm.org>\n");
	fprintf(stderr,
	    "With special thanks to Dr. P. Penín for helpful inspiration\n\n");
    }
    parse_arguments(argc, argv);

    open_files();
/*  if ! ref ==> use default file: 1) test ENV var 2) use default name
    if ! db  ==> ask user? use stdin?
    if ! out ==> ask user? use stdout?
*/
    setup_record();
    fclose(ref_file);

    process_database();
    fclose(db_file);
    fclose(out_file);

    exit(EXIT_OK);
}

static void parse_arguments(argc, argv)
int argc;
char *argv[];
{
    int count;

    count = 1;
    while (count < argc) {
	if (argv[count][0] == '-') {
	    switch (argv[count][1]) {
	    case 'c':		/* reference file */
		count++;
		ref_filnam = argv[count];
		break;
	    case 'd':		/* database file */
		count++;
		db_filnam = argv[count];
		break;
	    case 'o':		/* output file */
		count++;
		out_filnam = argv[count];
		break;
	    case 'f':	    	/* output field delimiter */
	    	count++;
		out_us = argv[count][0];
		break;
	    case 'r':	    	/* output record delimiter */
	    	count++;
		out_rs = argv[count][0];
		break;
	    case 'q':	    	/* quote special chars (for MS compatibility) */
	    	quote_output = TRUE;
		break;
	    case '1':
	    	print_header = TRUE;
		break;
	    case 'h':	    	/* help */
	    	usage();
		break;
	    default:		/* unknown option */
		usage();
		break;
	    }
	} else
	    usage();
	count++;
    }
}

static void open_files()
{
    if (ref_filnam == NULL)
	ref_filnam = ref_default;

    if ((ref_file = fopen(ref_filnam, "r")) == NULL) {
	fprintf(stderr, "Error: cannot open reference file %s\n",
		ref_filnam);
    	usage();
	exit(FILE_NOT_FOUND);
    }

    if (db_filnam == NULL) {
	db_filnam = db_default;
    	db_file = stdin;
    }
    else if ((db_file = fopen(db_filnam, "r")) == NULL) {
	fprintf(stderr, "Error: cannot open database file %s\n",
		db_filnam);
	exit(FILE_NOT_FOUND);
    }

    if (out_filnam == NULL) {
	out_filnam = out_default;
	out_file = stdout;
    }
    else if ((out_file = fopen(out_filnam, "a")) == NULL) {
	fprintf(stderr, "Error: cannot open output file %s\n", out_filnam);
	exit(FILE_NOT_FOUND);
    }
}

static void usage()
{
    fprintf(stderr, "DBconv [-r ref_file] [-d database] [-o output] [-h]\n");
    fprintf(stderr, "    [-f delim] [-e delim] [-q] [-1]\n\n");
    fprintf(stderr, "\t-c conf_file\treference layout file (default 'DBConv.cf')\n");
    fprintf(stderr, "\t-d database\tdatabase to be converted (default stdin)\n");
    fprintf(stderr, "\t-o output\toutput file (default stdout)\n");
    fprintf(stderr, "\t-f delim\toutput end of field character (default TAB)\n");
    fprintf(stderr, "\t-r delim\toutput end of record character (default LF)\n");
    fprintf(stderr, "\t-q\t\tescape special characters using quotes\n");
    fprintf(stderr, "\t-1\t\tprepend a first record with field names\n");
    fprintf(stderr, "\t-h\t\tprint this help\n");
    exit(EXIT_OK);
}
