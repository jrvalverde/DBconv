#include <stdio.h>

main()
{
    int ch;
    char fnam[128];
    FILE *fp;

    printf("Name of file: ");
    gets(fnam);
    if ((fp = fopen(fnam, "r")) == NULL)
	exit(4);

    while (!feof(fp)) {
	ch = fgetc(fp);
	switch (ch) {
	case '\t':
	    printf("<TAB>");
	    break;
	case '\n':
	    printf("<NL>\n");
	    break;
	case '\r':
	    printf("<CR>\n");
	    break;
	case EOF:
	    printf("<EOF>\n");
	    break;
	default:
	    printf("%c", ch);
	    break;
	}
    }

    fclose(fp);
}
