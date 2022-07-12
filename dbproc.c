/*
 *	DBproc.c
 *		routines to process the database for DBconvert
 *
 *	Contains:
 *		void process_database()
 *		Process the database, identifying and assigning each field
 *		and saving every record converted to tab-separated format.
 *
 *	Author:
 *		J. R. Valverde
 *		Biomedical Research Institute, Madrid. SPAIN
 *
 *		JRamon@uamed.uam.es
 *		JRamon@ccuam1.uam.es
 *		JRamon@cnbvx1.uam.es
 *		JRamon@EMDUAM51.Bitnet
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <memory.h>

#include "dbconv.h"

extern record_t rc;
extern char out_us;
extern char out_rs;

static bool is_eor(char *);
static int lookup(char *, int *);
static void save_header();
static void save_record();
static void init_record();

void process_database()
{
    int pos, i, fid;
    bool empty;
    char *content, *line;
    char buff[MAXLINE];
    char tail;
    extern FILE *db_file;
    extern record_t rc;
    extern int print_header;

    fid = -1;
    init_record();
    empty = TRUE;
    if (print_header)
    	save_header();
    while (!feof(db_file)) {
	if (fgets(buff, MAXLINE, db_file) == NULL)
	    break;		/* EOF */

	if (is_eor(buff)) {	/* EOR */
	    if (!empty)
		save_record();
	    init_record();
	    empty = TRUE;
	    fid = -1;
	} else {
	    /* Determine if line must be appended to field:
	       If we have found a field id.
	       If we haven't but we already are working one.
	       Otherwise we have begun a new record and are browsing
	       through text not assigned to any field. */

	    i = lookup(buff, &pos);
	    if (i >= 0)		/* new file id encountered */
		fid = i;
	    if (fid >= 0) {
		empty = FALSE;
		line = &buff[pos];	/* set line to point to field contents */
		content = rc.fld_text[fid];
		/* append line to text */
		if ((strlen(line) + strlen(content))
		    >= rc.fld_size[fid]) {
		    /* if needed length == allocated length then
		       we have not space for the trailing '\0' */

		    content = realloc((void *) content,
				      strlen(line) + strlen(content) + 1);
		    if (content == NULL)
			exit(MEMORY_OVERFLOW);
		    rc.fld_text[fid] = content;
		}
		strcat(content, line);
	    }
	}
    }
    if (!empty)
	save_record();
}

static void init_record()
{
    int i;

    for (i = 0; i < rc.fld_nmbr; i++)
	*(rc.fld_text[i]) = '\0';
}

static void save_header()
{
    int i;
    char *p;
    extern FILE *out_file;
    extern record_t rc;
    extern int quote_output;
    extern char out_us;
    extern char out_rs;

    /* output all field names */
    for (i = 0; i < rc.fld_nmbr; i++) {
	p = rc.fld_name[i];
	if (quote_output)
	    if (fputc('"',  out_file) == EOF)
	    	exit(OUT_FILE_ERROR);
	while (*p) {
	    if (quote_output && (*p == '"'))
	    	if (fputc('"', out_file) == EOF)    /* escape " */
		    exit(OUT_FILE_ERROR);
	    if (fputc(*p, out_file) == EOF)
		exit(OUT_FILE_ERROR);
	    p++;
	}
	if (quote_output)
	    if (fputc('"',  out_file) == EOF)
	    	exit(OUT_FILE_ERROR);
	if (i < (rc.fld_nmbr - 1))
 	    /* if not last field of record insert unit/field separator */
	    if (fputc(out_us, out_file) == EOF)
		exit(OUT_FILE_ERROR);
   }
   /* output end of record */
   if (fputc(out_rs, out_file) == EOF) /* insert record separator */
	exit(OUT_FILE_ERROR);
}

static void save_record()
{
    int i;
    char *p;
    int needs_escaping;
    extern FILE *out_file;
    extern record_t rc;
    extern int quote_output;
    extern char out_us;
    extern char out_rs;

    for (i = 0; i < rc.fld_nmbr; i++) {
	p = rc.fld_text[i];
	/*
	 * we need to run over the field twice:
	 * first to detect if it needs escaping
	 * then to write it out
	 */
	needs_escaping = FALSE;
	while (*p) {
	    /* skip trailing \n (if) included by fgets */
	    if ((*p == '\n') && (*(p+1) == '\0')) {
	    	*p = '\0';
		break;
	    }
	    if ((*p == out_rs) || (*p == out_us)) {
	    	if (quote_output)
    	    	    needs_escaping = TRUE;
		else
		    *p = ' ';	/* filter out special characters */
	    }
	    p++;
	}
    	/* write it out:
	 * we might as well enclose all output fields in quotes
	 * and then we wouldn't need to escape special chars other
	 * than quotes themselves (when quote is TRUE).
	 */
	p = rc.fld_text[i];
	if (quote_output && needs_escaping) /* insert initial " */
	    if (fputc('"', out_file) == EOF)
	    	exit(OUT_FILE_ERROR);
	while (*p) {
	    if (quote_output && needs_escaping && (*p == '"'))
	    	if (fputc('"', out_file) == EOF)    /* escape " */
		    exit(OUT_FILE_ERROR);
	    if (fputc(*p, out_file) == EOF)
		exit(OUT_FILE_ERROR);
	    p++;
	}
	if (quote_output && needs_escaping) /* insert final " */
	    if (fputc('"', out_file) == EOF)
	    	exit(OUT_FILE_ERROR);
	
	if (i < (rc.fld_nmbr - 1))
	    /* if not last field of record insert unit/field separator */
	    if (fputc(out_us, out_file) == EOF)
		exit(OUT_FILE_ERROR);
    }
    if (fputc(out_rs, out_file) == EOF) /* insert record separator */
	exit(OUT_FILE_ERROR);
}

static bool is_eor(line)
char *line;
{
    char *eor;
    extern record_t rc;

    eor = rc.rec_dlmt;
    while (*eor) {
	if (*line != *eor)
	    return FALSE;
	eor++;
	line++;
    }
    return TRUE;
}

static bool lookup(line, pos)
char *line;
int *pos;
{
    int i, j;
    char *field;
    extern record_t rc;

    for (i = 0; i < rc.fld_nmbr; i++) {
	field = rc.fld_name[i];
	j = 0;
	while (*field) {	/* compare line with field indicator */
	    if (line[j] != *field)
		break;
	    field++;
	    j++;
	}
	if (*field == '\0') {	/* new field: set BOL and return # */
	    *pos = j;
	    return i;
	}
    }
    return -1;			/* line is not beginning of a new field */
}
