#include <stdio.h>

FILE *infile;
FILE *outfile;

#define MAX_LINE_LEN	80

static char buffer[MAX_LINE_LEN+1];

main(argc,argv)
    int argc;
    char *argv[];
{
    long currentpos;
    char *s;

    if(argc != 2)
    {
	fprintf(stderr,"usage: %s filename\n",argv[0]);
	exit(1);
    }
    if((infile = fopen(argv[1],"r")) == NULL)
    {
	fprintf(stderr,"%s: can't open %s\n",argv[0],argv[1]);
	exit(2);
    }
    outfile = stdout;

    fprintf(outfile,"INDEX\n");
    while(!feof(infile))
    {
	currentpos = ftell(infile);
	if(fgets(buffer,MAX_LINE_LEN+1,infile) != buffer)
	    break;
	if(buffer[0] == '#')
	{
	    fprintf(outfile,">%ld ",currentpos);
	    s = buffer+1;
	    for(s = buffer+1 ; *s && *s != '\n' ; ++s)
		putc(*s,outfile);
	    putc('\n',outfile);
	}
    }
    fprintf(outfile,"TEXT\n");
    if(outfile != stdout)
	fclose(outfile);
    fclose(infile);
    exit(0);
}
