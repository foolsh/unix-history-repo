# include "../hdr/defines.h"

SCCSID(@(#)bdiff.c	4.2);

/*
	This program segments two files into pieces of <= seglim lines
	(which is passed as a third argument or defaulted to some number)
	and then executes diff upon the pieces. The output of
	'diff' is then processed to make it look as if 'diff' had
	processed the files whole. The reason for all this is that seglim
	is a reasonable upper limit on the size of files that diff can
	process.
	NOTE -- by segmenting the files in this manner, it cannot be
	guaranteed that the 'diffing' of the segments will generate
	a minimal set of differences.
	This process is most definitely not equivalent to 'diffing'
	the files whole, assuming 'diff' could handle such large files.

	'diff' is executed by a child process, generated by forking,
	and communicates with this program through pipes.
*/

int seglim;	/* limit of size of file segment to be generated */

char diff[]	"/bin/diff";
char tempskel[] "/tmp/bdXXXXX";		/* used to generate temp file names */
char tempfile[32];
char otmp[32], ntmp[32];
int linenum;

main(argc,argv)
int argc;
char *argv[];
{
	FILE *poldfile, *pnewfile, *tptr;
	char oline[BUFSIZ], nline[BUFSIZ], diffline[BUFSIZ];
	char *olp, *nlp, *dp;
	int i, otcnt, ntcnt;
	int pfd[2];
	FILE *poldtemp, *pnewtemp, *pipeinp;
	int status;

	/*
	Set flags for 'fatal' so that it will clean up,
	produce a message, and terminate.
	*/
	Fflags = FTLMSG | FTLCLN | FTLEXIT;

	setsig();

	if (argc < 3 || argc > 5)
		fatal("arg count (bd1)");

	if (equal(argv[1],"-") && equal(argv[2],"-"))
		fatal("both files standard input (bd2)");
	if (equal(argv[1],"-"))
		poldfile = stdin;
	else
		poldfile = xfopen(argv[1],0);
	if (equal(argv[2],"-"))
		pnewfile = stdin;
	else
		pnewfile = xfopen(argv[2],0);

	seglim = 3500;

	if (argc > 3) {
		if (argv[3][0] == '-' && argv[3][1] == 's')
			Fflags =& ~FTLMSG;
		else {
			if ((seglim = patoi(argv[3])) == -1)
				fatal("non-numeric limit (bd4)");
			if (argc == 5 && argv[4][0] == '-' &&
					argv[4][1] == 's')
				Fflags =& ~FTLMSG;
		}
	}

	linenum = 0;

	/*
	The following while-loop will prevent any lines
	common to the beginning of both files from being
	sent to 'diff'. Since the running time of 'diff' is
	non-linear, this will help improve performance.
	If, during this process, both files reach EOF, then
	the files are equal and the program will terminate.
	If either file reaches EOF before the other, the
	program will generate the appropriate 'diff' output
	itself, since this can be easily determined and will
	avoid executing 'diff' completely.
	*/
	while (1) {
		olp = fgets(oline,BUFSIZ,poldfile);
		nlp = fgets(nline,BUFSIZ,pnewfile);

		if (!olp && !nlp)	/* files are equal */
			exit(0);

		if (!olp) {
			/*
			The entire old file is a prefix of the
			new file. Generate the appropriate "append"
			'diff'-like output, which is of the form:
					nan,n
			where 'n' represents a line-number.
			*/
			addgen(nline,pnewfile);
		}

		if (!nlp) {
			/*
			The entire new file is a prefix of the
			old file. Generate the appropriate "delete"
			'diff'-like output, which is of the form:
					n,ndn
			where 'n' represents a line-number.
			*/
			delgen(oline,poldfile);
		}

		if (equal(olp,nlp))
			linenum++;
		else
			break;
	}

	/*
	Here, first 'linenum' lines are equal.
	The following while-loop segments both files into
	seglim segments, forks and executes 'diff' on the
	segments, and processes the resulting output of
	'diff', which is read from a pipe.
	*/
	while (1) {
		/*
		If both files are at EOF, everything is done.
		*/
		if (!olp && !nlp)	/* finished */
			exit(0);

		if (!olp) {
			/*
			Generate appropriate "append"
			output without executing 'diff'.
			*/
			addgen(nline,pnewfile);
		}

		if (!nlp) {
			/*
			Generate appropriate "delete"
			output without executing 'diff'.
			*/
			delgen(oline,poldfile);
		}

		/*
		Create a temporary file to hold a segment
		from the old file, and write it.
		*/
		poldtemp = maket(otmp);
		otcnt = 0;
		while(olp && otcnt < seglim) {
			fputs(oline,poldtemp);
			olp = fgets(oline,BUFSIZ,poldfile);
			otcnt++;
		}
		fclose(poldtemp);

		/*
		Create a temporary file to hold a segment
		from the new file, and write it.
		*/
		pnewtemp = maket(ntmp);
		ntcnt = 0;
		while(nlp && ntcnt < seglim) {
			fputs(nline,pnewtemp);
			nlp = fgets(nline,BUFSIZ,pnewfile);
			ntcnt++;
		}
		fclose(pnewtemp);

		/*
		Create pipes and fork.
		*/
		xpipe(pfd);
		if ((i = fork()) < 0) {
			close(pfd[0]);
			close(pfd[1]);
			fatal("cannot fork, try again (bd3)");
		}
		else if (i == 0) {	/* child process */
			close(pfd[0]);
			dup2(pfd[1], 1);
			if (pfd[1] != 1)
				close(pfd[1]);

			/*
			Execute 'diff' on the segment files.
			*/
			execl(diff,diff,otmp,ntmp,0);
			close(1);
			fatal(sprintf(Error,"cannot execute '%s' (bd5)",diff));
		}
		else {			/* parent process */
			close(pfd[1]);
			pipeinp = fdopen(pfd[0],"r");

			/*
			Process 'diff' output.
			*/
			while ((dp = fgets(diffline,BUFSIZ,pipeinp))) {
				if (numeric(*dp))
					fixnum(diffline);
				else
					printf("%s",diffline);
			}

			fclose(pipeinp);

			/*
			EOF on pipe.
			*/
			wait(&status);
			if (status&~0x100)
				fatal(sprintf(Error,"'%s' failed (bd6)",diff));
		}
		linenum =+ seglim;

		/*
		Remove temporary files.
		*/
		unlink(otmp);
		unlink(ntmp);
	}
}


/*
	Routine to save remainder of a file.
*/
saverest(line,iptr)
char *line;
FILE *iptr;
{
	register char *lp;
	FILE *temptr;

	temptr = maket(tempfile);

	lp = line;

	while (lp) {
		fputs(line,temptr);
		linenum++;
		lp = fgets(line,BUFSIZ,iptr);
	}
	fclose(temptr);
}


/*
	Routine to write out data saved by
	'saverest' routine and to remove the file.
*/
putsave(line,type)
char *line;
char type;
{
	FILE *temptr;

	temptr = xfopen(tempfile,0);

	while (fgets(line,BUFSIZ,temptr))
		printf("%c %s",type,line);

	fclose(temptr);

	xunlink(tempfile);
}


fixnum(lp)
char *lp;
{
	int num;

	while (*lp) {
		switch (*lp) {

		case 'a':
		case 'c':
		case 'd':
		case ',':
		case '\n':
			printf("%c",*lp);
			lp++;
			break;

		default:
			lp = satoi(lp,&num);
			num =+ linenum;
			printf("%d",num);
		}
	}
}


addgen(lp,fp)
char *lp;
FILE *fp;
{
	printf("%da%d,",linenum,linenum+1);

	/*
	Save lines of new file.
	*/
	saverest(lp,fp);

	printf("%d\n",linenum);

	/*
	Output saved lines, as 'diff' would.
	*/
	putsave(lp,'>');

	exit(0);
}


delgen(lp,fp)
char *lp;
FILE *fp;
{
	int savenum;

	printf("%d,",linenum+1);
	savenum = linenum;

	/*
	Save lines of old file.
	*/
	saverest(lp,fp);

	printf("%dd%d\n",linenum,savenum);

	/*
	Output saved lines, as 'diff' would.
	*/
	putsave(lp,'<');

	exit(0);
}


clean_up()
{
	unlink(tempfile);
	unlink(otmp);
	unlink(ntmp);
}


maket(file)
char *file;
{
	FILE *iop;

	copy(tempskel,file);
	iop = xfcreat(mktemp(file),0644);

	return(iop);
}
