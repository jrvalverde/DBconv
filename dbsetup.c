/*
 *	init.c
 *		initialization routines for DBconv
 *
 *		J. R. Valverde
 *	Format of reference file:
-------------------------------------------------------------------------------
<BOF>

	Specifications are of the form "NAME". If a double quote must be
included, it must be scaped by a \ character. \ itself is scaped by
repeating it. \ may scape any character. NAME may be an empty string.
	First specification must be that of the record separator:
" \"Record\\separator\" "
Which would stand for the string [ "Record\separator" ]

	Field specifications may be followed by a recommended size:
 "Field1"	125 chars
 "Fld2: "	or not
 " Fld3:"	if not followed by a number, rest of line is ignored

	All of it may be interspersed by blank lines or comments (lines not
beginning -after any blanks- with a double quote.

	If a quote is not closed then the rest of the line is taken as the
field name:
	" Field #4: 125<EOL>
	is taken as [ Field #4: 125<EOL>] (<EOL> means here the end of line)

	As an example this file would be interpreted as
	[ "Record\separator" ]
	[Field1]
	[Fld2: ]
	[ Fld3:]
	[ Field #4: 125<EOL>]
<EOF>
-------------------------------------------------------------------------
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <memory.h>

#include "dbconv.h"

static char buff[MAXLINE];

static flag yesno_answer();
static bool add_field();
static get_rc_delim();

void setup_record()
{
    int i;
    extern record_t rc;

    if (!get_rc_delim())
	exit(NO_REC_DELIM);

    rc.fld_nmbr = 0;
    for (i = 0; i < MAXFIELDS; ++i)
	if (!add_field())
	    break;
	else
	    rc.fld_nmbr++;

    if (i == MAXFIELDS) {
	fprintf(stderr, "Error: Maximum number of fields exceeded...\n");
	fprintf(stderr, "Continue using first %d fields only (Y/[N])?",
		MAXFIELDS);
	if (yesno_answer() == NO)
	    exit(FIELD_OVERFLOW);
    }
    if (i == 0)
	exit(NO_FIELDS_GIVEN);
}

static flag yesno_answer()
{
    char c;

    fflush(stdin);
    c = getchar();		/* gets first letter of CR terminated string */
    if ((c == 'Y') || (c == 'y'))
	return YES;
    else
	return NO;
}

static bool add_field()
{
    char *p1, *p2, *nam;
    int size, fld_no;
    extern record_t rc;
    extern FILE *ref_file;

    /* test EOF */
    if (feof(ref_file))
	return FALSE;

    /* skip blank and comment lines */
    do {
	if (fgets(buff, MAXLINE, ref_file) == NULL)
	    return FALSE;
	p1 = buff;
	while (isspace(*p1))
	    p1++;
    } while ((*p1 == '\0') || (*p1 != '"'));

    /* This is a non-blank line. p1 points to first single quote */
    /* Get field name: (must be enclosed in double quotes) */
    {
	fld_no = rc.fld_nmbr;	/* just to save indirections */
	p1++;			/* advance past first quote */
	p2 = p1;
	size = 0;
	while ((*p2) && (*p2 != '"')) {
	    if (*p2 == '\\')	/* escape character, for \ and " */
		p2++;
	    size++;
	    p2++;
	}
	/* Note that size may be zero. It will be OK, but the effect will
	   be that all fields defined after this one will be ignored, and
	   only the last line of the "field" will be saved in the field */
	if (*p2 == '\0')
	    /* the effect of this will be that field
	       name will end with a '\n' character */
	    fprintf(stderr,
		    "Warning: unterminated field name in line\n%s\n",
		    buff);

	/* save field name */
	nam = malloc(size + 1);
	if (nam == NULL)
	    exit(MEMORY_OVERFLOW);
	rc.fld_name[fld_no] = nam;
	while (*p1 != *p2) {
	    if (*p1 == '\\')
		p1++;
	    *nam = *p1;
	    nam++;
	    p1++;
	}
	*nam = '\0';
    }				/* Name got */

    /* Get size of field. If none is given, then use default size
       At this point, p1 = p2 = &(last quote of field name or '\0') */
    rc.fld_size[fld_no] = FLD_DEF_SIZE;
    if (*p1 != '\0') {
	p1++;			/* move past last quote */
	while (isspace(*p1))
	    p1++;
	if (isdigit(*p1))	/* a size is given */
	    rc.fld_size[fld_no] = atoi(p1);
    }

    /* Now, rc.fld_size[fld_no] contains either the default size or the
       advised size. Allocate space for its contents. */
    if ((rc.fld_text[fld_no] = malloc(rc.fld_size[fld_no])) == NULL)
	exit(MEMORY_OVERFLOW);

    return TRUE;
}

static bool get_rc_delim()
{
    char *p1, *p2, *nam;
    int size;
    extern FILE *ref_file;
    extern record_t rc;

    /* test EOF */
    if (feof(ref_file))
	return FALSE;

    /* skip blank and comment lines */
    do {
	if (fgets(buff, MAXLINE, ref_file) == NULL)
	    return FALSE;
	p1 = buff;
	while (isspace(*p1))
	    p1++;
    } while ((*p1 == '\0') || (*p1 != '"'));

    /* This is a non-blank line. p1 points to first single quote */
    /* Get record separator: (must be enclosed in double quotes) */
    p1++;			/* advance past first quote */
    p2 = p1;
    size = 0;
    while ((*p2) && (*p2 != '"')) {
	if (*p2 == '\\')	/* escape character, for \ and " */
	    p2++;
	size++;
	p2++;
    }
    /* Note that size may be zero. It will be OK, meaning that
       an empty line separates different records */
    if (*p2 == '\0')
	/* The result of this is that rc_dlmt will have a '\n' as
	   its last character */
	fprintf(stderr,
		"Warning: unterminated record delimiter in line\n%s\n",
		buff);

    /* save record delimiter */
    if (size == 0) {
	/* empty line: insert a '\n' or else, EVERY line will
	   be taken as a new-record line. The '\n' is needed
	   beacuse we read lines with fgets which conserves
	   the trailing '\n' char */
	if ((rc.rec_dlmt = malloc(2)) == NULL)
	    exit(MEMORY_OVERFLOW);
	rc.rec_dlmt[0] = '\n';
	rc.rec_dlmt[1] = '\0';
	return TRUE;
    }

    /* non-empty line */
    nam = malloc(size + 1);
    if (nam == NULL)
	exit(MEMORY_OVERFLOW);
    rc.rec_dlmt = nam;
    while (*p1 != *p2) {
	if (*p1 == '\\')
	    p1++;
	*nam = *p1;
	nam++;
	p1++;
    }
    *nam = '\0';

    return TRUE;
}
