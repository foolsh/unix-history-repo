/*
 * Copyright (c) 1992, Brian Berliner and Jeff Polk
 * Copyright (c) 1989-1992, Brian Berliner
 * 
 * You may distribute under the terms of the GNU General Public License as
 * specified in the README file that comes with the CVS 1.4 kit.
 * 
 * "import" checks in the vendor release located in the current directory into
 * the CVS source repository.  The CVS vendor branch support is utilized.
 * 
 * At least three arguments are expected to follow the options:
 *	repository	Where the source belongs relative to the CVSROOT
 *	VendorTag	Vendor's major tag
 *	VendorReleTag	Tag for this particular release
 *
 * Additional arguments specify more Vendor Release Tags.
 */

#include "cvs.h"
#include "save-cwd.h"

#ifndef lint
static const char rcsid[] = "$CVSid: @(#)import.c 1.63 94/09/30 $";
USE(rcsid);
#endif

#define	FILE_HOLDER	".#cvsxxx"

static char *get_comment PROTO((char *user));
static int add_rcs_file PROTO((char *message, char *rcs, char *user, char *vtag,
		         int targc, char *targv[]));
static int expand_at_signs PROTO((char *buf, off_t size, FILE *fp));
static int add_rev PROTO((char *message, char *rcs, char *vfile, char *vers));
static int add_tags PROTO((char *rcs, char *vfile, char *vtag, int targc,
		     char *targv[]));
static int import_descend PROTO((char *message, char *vtag, int targc, char *targv[]));
static int import_descend_dir PROTO((char *message, char *dir, char *vtag,
			       int targc, char *targv[]));
static int process_import_file PROTO((char *message, char *vfile, char *vtag,
				int targc, char *targv[]));
static int update_rcs_file PROTO((char *message, char *vfile, char *vtag, int targc,
			    char *targv[], int inattic));
static void add_log PROTO((int ch, char *fname));

static int repos_len;
static char vhead[50];
static char vbranch[50];
static FILE *logfp;
static char repository[PATH_MAX];
static int conflicts;
static int use_file_modtime;
static char *keyword_opt = NULL;

static const char *const import_usage[] =
{
    "Usage: %s %s [-d] [-k subst] [-I ign] [-m msg] [-b branch]\n",
    "    [-W spec] repository vendor-tag release-tags...\n",
    "\t-d\tUse the file's modification time as the time of import.\n",
    "\t-k sub\tSet default RCS keyword substitution mode.\n",
    "\t-I ign\tMore files to ignore (! to reset).\n",
    "\t-b bra\tVendor branch id.\n",
    "\t-m msg\tLog message.\n",
    "\t-W spec\tWrappers specification line.\n",
    NULL
};

int
import (argc, argv)
    int argc;
    char **argv;
{
    char *message = NULL;
    char tmpfile[L_tmpnam+1];
    char *cp;
    int i, c, msglen, err;
    List *ulist;
    Node *p;

    if (argc == -1)
	usage (import_usage);

    ign_setup ();
    wrap_setup ();

    (void) strcpy (vbranch, CVSBRANCH);
    optind = 1;
    while ((c = getopt (argc, argv, "Qqdb:m:I:k:W:")) != -1)
    {
	switch (c)
	{
	    case 'Q':
	    case 'q':
#ifdef SERVER_SUPPORT
		/* The CVS 1.5 client sends these options (in addition to
		   Global_option requests), so we must ignore them.  */
		if (!server_active)
#endif
		    error (1, 0,
			   "-q or -Q must be specified before \"%s\"",
			   command_name);
		break;
	    case 'd':
		use_file_modtime = 1;
		break;
	    case 'b':
		(void) strcpy (vbranch, optarg);
		break;
	    case 'm':
#ifdef FORCE_USE_EDITOR
		use_editor = TRUE;
#else
		use_editor = FALSE;
#endif
		message = xstrdup(optarg);
		break;
	    case 'I':
		ign_add (optarg, 0);
		break;
            case 'k':
		/* RCS_check_kflag returns strings of the form -kxx.  We
		   only use it for validation, so we can free the value
		   as soon as it is returned. */
		free (RCS_check_kflag(optarg));	
		keyword_opt = optarg;
		break;
	    case 'W':
		wrap_add (optarg, 0);
		break;
	    case '?':
	    default:
		usage (import_usage);
		break;
	}
    }
    argc -= optind;
    argv += optind;
    if (argc < 3)
	usage (import_usage);

    for (i = 1; i < argc; i++)		/* check the tags for validity */
    {
	int j;

	RCS_check_tag (argv[i]);
	for (j = 1; j < i; j++)
	    if (strcmp (argv[j], argv[i]) == 0)
		error (1, 0, "tag `%s' was specified more than once", argv[i]);
    }

    /* XXX - this should be a module, not just a pathname */
    if (! isabsolute (argv[0]))
    {
	if (CVSroot == NULL)
	{
	    error (0, 0, "missing CVSROOT environment variable\n");
	    error (1, 0, "Set it or specify the '-d' option to %s.",
		   program_name);
	}
	(void) sprintf (repository, "%s/%s", CVSroot, argv[0]);
	repos_len = strlen (CVSroot);
    }
    else
    {
	(void) strcpy (repository, argv[0]);
	repos_len = 0;
    }

    /*
     * Consistency checks on the specified vendor branch.  It must be
     * composed of only numbers and dots ('.').  Also, for now we only
     * support branching to a single level, so the specified vendor branch
     * must only have two dots in it (like "1.1.1").
     */
    for (cp = vbranch; *cp != '\0'; cp++)
	if (!isdigit (*cp) && *cp != '.')
	    error (1, 0, "%s is not a numeric branch", vbranch);
    if (numdots (vbranch) != 2)
	error (1, 0, "Only branches with two dots are supported: %s", vbranch);
    (void) strcpy (vhead, vbranch);
    cp = strrchr (vhead, '.');
    *cp = '\0';

#ifdef CLIENT_SUPPORT
    if (client_active)
    {
	/* Do this now; don't ask for a log message if we can't talk to the
	   server.  But if there is a syntax error in the options, give
	   an error message without connecting.  */
	start_server ();
    }
#endif

    if (use_editor)
    {
	do_editor ((char *) NULL, &message, repository,
		   (List *) NULL); 
    }

    msglen = message == NULL ? 0 : strlen (message);
    if (msglen == 0 || message[msglen - 1] != '\n')
    {
	char *nm = xmalloc (msglen + 2);
	if (message != NULL)
	{
	    (void) strcpy (nm, message);
	    free (message);
	}
	(void) strcat (nm + msglen, "\n");
	message = nm;
    }

#ifdef CLIENT_SUPPORT
    if (client_active)
    {
	int err;

	ign_setup ();

	if (use_file_modtime)
	    send_arg("-d");

	if (vbranch[0] != '\0')
	    option_with_arg ("-b", vbranch);
	if (message)
	    option_with_arg ("-m", message);
	if (keyword_opt != NULL)
	    option_with_arg ("-k", keyword_opt);

	{
	    int i;
	    for (i = 0; i < argc; ++i)
		send_arg (argv[i]);
	}

	logfp = stdin;
	client_import_setup (repository);
	err = import_descend (message, argv[1], argc - 2, argv + 2);
	client_import_done ();
	if (fprintf (to_server, "import\n") < 0)
	    error (1, errno, "writing to server");
	err += get_responses_and_close ();
	return err;
    }
#endif

    /*
     * Make all newly created directories writable.  Should really use a more
     * sophisticated security mechanism here.
     */
    (void) umask (cvsumask);
    make_directories (repository);

    /* Create the logfile that will be logged upon completion */
    if ((logfp = fopen (tmpnam (tmpfile), "w+")) == NULL)
	error (1, errno, "cannot create temporary file `%s'", tmpfile);
    (void) unlink (tmpfile);		/* to be sure it goes away */
    (void) fprintf (logfp, "\nVendor Tag:\t%s\n", argv[1]);
    (void) fprintf (logfp, "Release Tags:\t");
    for (i = 2; i < argc; i++)
	(void) fprintf (logfp, "%s\n\t\t", argv[i]);
    (void) fprintf (logfp, "\n");

    /* Just Do It.  */
    err = import_descend (message, argv[1], argc - 2, argv + 2);
    if (conflicts)
    {
	if (!really_quiet)
	{
	    (void) printf ("\n%d conflicts created by this import.\n",
			   conflicts);
	    (void) printf ("Use the following command to help the merge:\n\n");
	    (void) printf ("\t%s checkout -j%s:yesterday -j%s %s\n\n",
			   program_name, argv[1], argv[1], argv[0]);
	}

	(void) fprintf (logfp, "\n%d conflicts created by this import.\n",
			conflicts);
	(void) fprintf (logfp,
			"Use the following command to help the merge:\n\n");
	(void) fprintf (logfp, "\t%s checkout -j%s:yesterday -j%s %s\n\n",
			program_name, argv[1], argv[1], argv[0]);
    }
    else
    {
	if (!really_quiet)
	    (void) printf ("\nNo conflicts created by this import\n\n");
	(void) fprintf (logfp, "\nNo conflicts created by this import\n\n");
    }

    /*
     * Write out the logfile and clean up.
     */
    ulist = getlist ();
    p = getnode ();
    p->type = UPDATE;
    p->delproc = update_delproc;
    p->key = xstrdup ("- Imported sources");
    p->data = (char *) T_TITLE;
    (void) addnode (ulist, p);
    Update_Logfile (repository, message, vbranch, logfp, ulist);
    dellist (&ulist);
    (void) fclose (logfp);

    /* Make sure the temporary file goes away, even on systems that don't let
       you delete a file that's in use.  */
    unlink (tmpfile);

    if (message)
	free (message);

    return (err);
}

/*
 * process all the files in ".", then descend into other directories.
 */
static int
import_descend (message, vtag, targc, targv)
    char *message;
    char *vtag;
    int targc;
    char *targv[];
{
    DIR *dirp;
    struct dirent *dp;
    int err = 0;
    List *dirlist = NULL;

    /* first, load up any per-directory ignore lists */
    ign_add_file (CVSDOTIGNORE, 1);
    wrap_add_file (CVSDOTWRAPPER, 1);

    if ((dirp = opendir (".")) == NULL)
    {
	err++;
    }
    else
    {
	while ((dp = readdir (dirp)) != NULL)
	{
	    if (strcmp (dp->d_name, ".") == 0 || strcmp (dp->d_name, "..") == 0)
		continue;
	    if (ign_name (dp->d_name))
	    {
#ifdef SERVER_SUPPORT
		/* CVS directories are created by server.c because it doesn't
		   special-case import.  So don't print a message about them.
		   Do print a message about other ignored files (although
		   most of these will get ignored on the client side).  */
		if (server_active && strcmp (dp->d_name, CVSADM) == 0)
		    continue;
#endif
		add_log ('I', dp->d_name);
		continue;
	    }

	    if (
#ifdef DT_DIR
		(dp->d_type == DT_DIR
		 || (dp->d_type == DT_UNKNOWN && isdir (dp->d_name)))
#else
		isdir (dp->d_name)
#endif
		&& !wrap_name_has (dp->d_name, WRAP_TOCVS)
		)
            {	
		Node *n;

		if (dirlist == NULL)
		    dirlist = getlist();

		n = getnode();
		n->key = xstrdup (dp->d_name);
		addnode(dirlist, n);
	    }
	    else if (
#ifdef DT_DIR
		dp->d_type == DT_LNK || dp->d_type == DT_UNKNOWN && 
#endif
		islink (dp->d_name))
	    {
		add_log ('L', dp->d_name);
		err++;
	    }
	    else
	    {
#ifdef CLIENT_SUPPORT
		if (client_active)
		    err += client_process_import_file (message, dp->d_name,
							   vtag, targc, targv,
							   repository);
		else
#endif
		    err += process_import_file (message, dp->d_name,
						vtag, targc, targv);
	    }
	}
	(void) closedir (dirp);
    }

    if (dirlist != NULL) 
    {
	Node *head, *p;

	head = dirlist->list;
	for (p = head->next; p != head; p = p->next)
	{
	    err += import_descend_dir (message, p->key, vtag, targc, targv);
	}

	dellist(&dirlist);
    }

    return (err);
}

/*
 * Process the argument import file.
 */
static int
process_import_file (message, vfile, vtag, targc, targv)
    char *message;
    char *vfile;
    char *vtag;
    int targc;
    char *targv[];
{
    char attic_name[PATH_MAX];
    char rcs[PATH_MAX];
    int inattic = 0;

    (void) sprintf (rcs, "%s/%s%s", repository, vfile, RCSEXT);
    if (!isfile (rcs))
    {
	(void) sprintf (attic_name, "%s/%s/%s%s", repository, CVSATTIC,
			vfile, RCSEXT);
	if (!isfile (attic_name))
	{

	    /*
	     * A new import source file; it doesn't exist as a ,v within the
	     * repository nor in the Attic -- create it anew.
	     */
	    add_log ('N', vfile);
	    return (add_rcs_file (message, rcs, vfile, vtag, targc, targv));
	}
	inattic = 1;
    }

    /*
     * an rcs file exists. have to do things the official, slow, way.
     */
    return (update_rcs_file (message, vfile, vtag, targc, targv, inattic));
}

/*
 * The RCS file exists; update it by adding the new import file to the
 * (possibly already existing) vendor branch.
 */
static int
update_rcs_file (message, vfile, vtag, targc, targv, inattic)
    char *message;
    char *vfile;
    char *vtag;
    int targc;
    char *targv[];
    int inattic;
{
    Vers_TS *vers;
    int letter;
    int ierrno;
    char *tmpdir;
    char *tocvsPath;

    vers = Version_TS (repository, (char *) NULL, vbranch, (char *) NULL, vfile,
		       1, 0, (List *) NULL, (List *) NULL);
#ifdef DEATH_SUPPORT
    if (vers->vn_rcs != NULL
	&& !RCS_isdead(vers->srcfile, vers->vn_rcs))
#else
    if (vers->vn_rcs != NULL)
#endif
    {
	char xtmpfile[PATH_MAX];
	int different;
	int retcode = 0;

	tmpdir = getenv ("TMPDIR");
	if (tmpdir == NULL || tmpdir[0] == '\0') 
	  tmpdir = "/tmp";

	(void) sprintf (xtmpfile, "%s/cvs-imp%d", tmpdir, getpid());

	/*
	 * The rcs file does have a revision on the vendor branch. Compare
	 * this revision with the import file; if they match exactly, there
	 * is no need to install the new import file as a new revision to the
	 * branch.  Just tag the revision with the new import tags.
	 * 
	 * This is to try to cut down the number of "C" conflict messages for
	 * locally modified import source files.
	 */
#ifdef HAVE_RCS5
	run_setup ("%s%s -q -f -r%s -p -ko", Rcsbin, RCS_CO, vers->vn_rcs);
#else
	run_setup ("%s%s -q -f -r%s -p", Rcsbin, RCS_CO, vers->vn_rcs);
#endif
	run_arg (vers->srcfile->path);
	if ((retcode = run_exec (RUN_TTY, xtmpfile, RUN_TTY,
				 RUN_NORMAL|RUN_REALLY)) != 0)
	{
	    ierrno = errno;
	    fperror (logfp, 0, retcode == -1 ? ierrno : 0,
		     "ERROR: cannot co revision %s of file %s", vers->vn_rcs,
		     vers->srcfile->path);
	    error (0, retcode == -1 ? ierrno : 0,
		   "ERROR: cannot co revision %s of file %s", vers->vn_rcs,
		   vers->srcfile->path);
	    (void) unlink_file (xtmpfile);
	    return (1);
	}

	tocvsPath = wrap_tocvs_process_file (vfile);
	different = xcmp (xtmpfile, vfile);
	if (tocvsPath)
	    if (unlink_file_dir (tocvsPath) < 0)
		error (0, errno, "cannot remove %s", tocvsPath);

	(void) unlink_file (xtmpfile);
	if (!different)
	{
	    int retval = 0;

	    /*
	     * The two files are identical.  Just update the tags, print the
	     * "U", signifying that the file has changed, but needs no
	     * attention, and we're done.
	     */
	    if (add_tags (vers->srcfile->path, vfile, vtag, targc, targv))
		retval = 1;
	    add_log ('U', vfile);
	    freevers_ts (&vers);
	    return (retval);
	}
    }

    /* We may have failed to parse the RCS file; check just in case */
    if (vers->srcfile == NULL ||
	add_rev (message, vers->srcfile->path, vfile, vers->vn_rcs) ||
	add_tags (vers->srcfile->path, vfile, vtag, targc, targv))
    {
	freevers_ts (&vers);
	return (1);
    }

    if (vers->srcfile->branch == NULL || inattic ||
	strcmp (vers->srcfile->branch, vbranch) != 0)
    {
	conflicts++;
	letter = 'C';
    }
    else
	letter = 'U';
    add_log (letter, vfile);

    freevers_ts (&vers);
    return (0);
}

/*
 * Add the revision to the vendor branch
 */
static int
add_rev (message, rcs, vfile, vers)
    char *message;
    char *rcs;
    char *vfile;
    char *vers;
{
    int locked, status, ierrno;
    char *tocvsPath;
    struct stat vfile_stat;

    if (noexec)
	return (0);

    locked = 0;
    if (vers != NULL)
    {
	/* Before RCS_lock existed, we were directing stdout, as well as
	   stderr, from the RCS command, to DEVNULL.  I wouldn't guess that
	   was necessary, but I don't know for sure.  */
        if (RCS_lock (rcs, vbranch, 1) != 0)
	{
	    error (0, errno, "fork failed");
	    return (1);
	}
	locked = 1;
    }
    tocvsPath = wrap_tocvs_process_file (vfile);

    /* We used to deposit the revision with -r; RCS would delete the
       working file, but we'd keep a hard link to it, and rename it
       back after running RCS (ooh, atomicity).  However, that
       strategy doesn't work on operating systems without hard links
       (like Windows NT).  Instead, let's deposit it using -u, and
       restore its permission bits afterwards.  This also means the
       file always exists under its own name.  */
    if (! tocvsPath)
        stat (vfile, &vfile_stat);

    run_setup ("%s%s -q -f %s%s", Rcsbin, RCS_CI, 
	       (tocvsPath ? "-r" : "-u"),
	       vbranch);
    run_args ("-m%s", make_message_rcslegal (message));
    if (use_file_modtime)
	run_arg ("-d");
    run_arg (tocvsPath == NULL ? vfile : tocvsPath);
    run_arg (rcs);
    status = run_exec (RUN_TTY, RUN_TTY, RUN_TTY, RUN_NORMAL);
    ierrno = errno;

    /* Restore the permissions on vfile.  */
    if (! tocvsPath)
        chmod (vfile, vfile_stat.st_mode);

    if (status)
    {
	if (!noexec)
	{
	    fperror (logfp, 0, status == -1 ? ierrno : 0, "ERROR: Check-in of %s failed", rcs);
	    error (0, status == -1 ? ierrno : 0, "ERROR: Check-in of %s failed", rcs);
	}
	if (locked)
	{
	    (void) RCS_unlock(rcs, vbranch, 0);
	}
	return (1);
    }
    return (0);
}

/*
 * Add the vendor branch tag and all the specified import release tags to the
 * RCS file.  The vendor branch tag goes on the branch root (1.1.1) while the
 * vendor release tags go on the newly added leaf of the branch (1.1.1.1,
 * 1.1.1.2, ...).
 */
static int
add_tags (rcs, vfile, vtag, targc, targv)
    char *rcs;
    char *vfile;
    char *vtag;
    int targc;
    char *targv[];
{
    int i, ierrno;
    Vers_TS *vers;
    int retcode = 0;

    if (noexec)
	return (0);

    if ((retcode = RCS_settag(rcs, vtag, vbranch)) != 0)
    {
	ierrno = errno;
	fperror (logfp, 0, retcode == -1 ? ierrno : 0, 
		 "ERROR: Failed to set tag %s in %s", vtag, rcs);
	error (0, retcode == -1 ? ierrno : 0,
	       "ERROR: Failed to set tag %s in %s", vtag, rcs);
	return (1);
    }
    vers = Version_TS (repository, (char *) NULL, vtag, (char *) NULL, vfile,
		       1, 0, (List *) NULL, (List *) NULL);
    for (i = 0; i < targc; i++)
    {
	if ((retcode = RCS_settag (rcs, targv[i], vers->vn_rcs)) != 0)
	{
	    ierrno = errno;
	    fperror (logfp, 0, retcode == -1 ? ierrno : 0, 
		     "WARNING: Couldn't add tag %s to %s", targv[i], rcs);
	    error (0, retcode == -1 ? ierrno : 0,
		   "WARNING: Couldn't add tag %s to %s", targv[i], rcs);
	}
    }
    freevers_ts (&vers);
    return (0);
}

/*
 * Stolen from rcs/src/rcsfnms.c, and adapted/extended.
 */
struct compair
{
    char *suffix, *comlead;
};

static const struct compair comtable[] =
{

/*
 * comtable pairs each filename suffix with a comment leader. The comment
 * leader is placed before each line generated by the $Log keyword. This
 * table is used to guess the proper comment leader from the working file's
 * suffix during initial ci (see InitAdmin()). Comment leaders are needed for
 * languages without multiline comments; for others they are optional.
 */
    {"a", "-- "},			/* Ada		 */
    {"ada", "-- "},
    {"adb", "-- "},
    {"asm", ";; "},			/* assembler (MS-DOS) */
    {"ads", "-- "},			/* Ada		 */
    {"bat", ":: "},			/* batch (MS-DOS) */
    {"body", "-- "},			/* Ada		 */
    {"c", " * "},			/* C		 */
    {"c++", "// "},			/* C++ in all its infinite guises */
    {"cc", "// "},
    {"cpp", "// "},
    {"cxx", "// "},
    {"m", "// "},			/* Objective-C */
    {"cl", ";;; "},			/* Common Lisp	 */
    {"cmd", ":: "},			/* command (OS/2) */
    {"cmf", "c "},			/* CM Fortran	 */
    {"cs", " * "},			/* C*		 */
    {"csh", "# "},			/* shell	 */
    {"e", "# "},			/* efl		 */
    {"epsf", "% "},			/* encapsulated postscript */
    {"epsi", "% "},			/* encapsulated postscript */
    {"el", "; "},			/* Emacs Lisp	 */
    {"f", "c "},			/* Fortran	 */
    {"for", "c "},
    {"h", " * "},			/* C-header	 */
    {"hh", "// "},			/* C++ header	 */
    {"hpp", "// "},
    {"hxx", "// "},
    {"in", "# "},			/* for Makefile.in */
    {"l", " * "},			/* lex (conflict between lex and
					 * franzlisp) */
    {"mac", ";; "},			/* macro (DEC-10, MS-DOS, PDP-11,
					 * VMS, etc) */
    {"me", ".\\\" "},			/* me-macros	t/nroff	 */
    {"ml", "; "},			/* mocklisp	 */
    {"mm", ".\\\" "},			/* mm-macros	t/nroff	 */
    {"ms", ".\\\" "},			/* ms-macros	t/nroff	 */
    {"man", ".\\\" "},			/* man-macros	t/nroff	 */
    {"1", ".\\\" "},			/* feeble attempt at man pages... */
    {"2", ".\\\" "},
    {"3", ".\\\" "},
    {"4", ".\\\" "},
    {"5", ".\\\" "},
    {"6", ".\\\" "},
    {"7", ".\\\" "},
    {"8", ".\\\" "},
    {"9", ".\\\" "},
    {"p", " * "},			/* pascal	 */
    {"pas", " * "},
    {"pl", "# "},			/* perl	(conflict with Prolog) */
    {"ps", "% "},			/* postscript	 */
    {"psw", "% "},			/* postscript wrap */
    {"pswm", "% "},			/* postscript wrap */
    {"r", "# "},			/* ratfor	 */
    {"red", "% "},			/* psl/rlisp	 */
#ifdef sparc
    {"s", "! "},			/* assembler	 */
#endif
#ifdef mc68000
    {"s", "| "},			/* assembler	 */
#endif
#ifdef pdp11
    {"s", "/ "},			/* assembler	 */
#endif
#ifdef vax
    {"s", "# "},			/* assembler	 */
#endif
#ifdef __ksr__
    {"s", "# "},			/* assembler	 */
    {"S", "# "},			/* Macro assembler */
#endif
    {"sh", "# "},			/* shell	 */
    {"sl", "% "},			/* psl		 */
    {"spec", "-- "},			/* Ada		 */
    {"tex", "% "},			/* tex		 */
    {"y", " * "},			/* yacc		 */
    {"ye", " * "},			/* yacc-efl	 */
    {"yr", " * "},			/* yacc-ratfor	 */
#ifdef SYSTEM_COMMENT_TABLE
    SYSTEM_COMMENT_TABLE
#endif
    {"", "# "},				/* default for empty suffix	 */
    {NULL, "# "}			/* default for unknown suffix;	 */
/* must always be last		 */
};

static char *
get_comment (user)
    char *user;
{
    char *cp, *suffix;
    char suffix_path[PATH_MAX];
    int i;

    cp = strrchr (user, '.');
    if (cp != NULL)
    {
	cp++;

	/*
	 * Convert to lower-case, since we are not concerned about the
	 * case-ness of the suffix.
	 */
	(void) strcpy (suffix_path, cp);
	for (cp = suffix_path; *cp; cp++)
	    if (isupper (*cp))
		*cp = tolower (*cp);
	suffix = suffix_path;
    }
    else
	suffix = "";			/* will use the default */
    for (i = 0;; i++)
    {
	if (comtable[i].suffix == NULL)	/* default */
	    return (comtable[i].comlead);
	if (strcmp (suffix, comtable[i].suffix) == 0)
	    return (comtable[i].comlead);
    }
}

static int
add_rcs_file (message, rcs, user, vtag, targc, targv)
    char *message;
    char *rcs;
    char *user;
    char *vtag;
    int targc;
    char *targv[];
{
    FILE *fprcs, *fpuser;
    struct stat sb;
    struct tm *ftm;
    time_t now;
    char altdate1[50];
#ifndef HAVE_RCS5
    char altdate2[50];
#endif
    char *author, *buf;
    int i, ierrno, err = 0;
    mode_t mode;
    char *tocvsPath;
    char *userfile;

    if (noexec)
	return (0);

#ifdef LINES_CRLF_TERMINATED
    /* There exits a port of RCS to such a system that stores files with
       straight newlines.  If we ever reach this point on such a system,
       we'll need to decide what to do with the open_file call below.  */
    abort ();
#endif
    tocvsPath = wrap_tocvs_process_file (user);
    userfile = (tocvsPath == NULL ? user : tocvsPath);
    fpuser = fopen (userfile, "r");
    if (fpuser == NULL) {
	/* not fatal, continue import */
	fperror (logfp, 0, errno, "ERROR: cannot read file %s", userfile);
	error (0, errno, "ERROR: cannot read file %s", userfile);
	goto read_error;
    }
    fprcs = fopen (rcs, "w+");
    if (fprcs == NULL) {
	ierrno = errno;
	goto write_error_noclose;
    }

    /*
     * putadmin()
     */
    if (fprintf (fprcs, "head     %s;\n", vhead) < 0 ||
	fprintf (fprcs, "branch   %s;\n", vbranch) < 0 ||
	fprintf (fprcs, "access   ;\n") < 0 ||
	fprintf (fprcs, "symbols  ") < 0)
    {
	goto write_error;
    }

    for (i = targc - 1; i >= 0; i--)	/* RCS writes the symbols backwards */
	if (fprintf (fprcs, "%s:%s.1 ", targv[i], vbranch) < 0)
	    goto write_error;

    if (fprintf (fprcs, "%s:%s;\n", vtag, vbranch) < 0 ||
	fprintf (fprcs, "locks    ; strict;\n") < 0 ||
	/* XXX - make sure @@ processing works in the RCS file */
	fprintf (fprcs, "comment  @%s@;\n", get_comment (user)) < 0)
    {
	goto write_error;
    }

    if (keyword_opt != NULL)
      if (fprintf (fprcs, "expand   @%s@;\n", keyword_opt) < 0)
	{
	  goto write_error;
	}

    if (fprintf (fprcs, "\n") < 0)
      goto write_error;

    /*
     * puttree()
     */
    if (fstat (fileno (fpuser), &sb) < 0)
	error (1, errno, "cannot fstat %s", user);
    if (use_file_modtime)
	now = sb.st_mtime;
    else
	(void) time (&now);
#ifdef HAVE_RCS5
    ftm = gmtime (&now);
#else
    ftm = localtime (&now);
#endif
    (void) sprintf (altdate1, DATEFORM,
		    ftm->tm_year + (ftm->tm_year < 100 ? 0 : 1900),
		    ftm->tm_mon + 1, ftm->tm_mday, ftm->tm_hour,
		    ftm->tm_min, ftm->tm_sec);
#ifdef HAVE_RCS5
#define	altdate2 altdate1
#else
    /*
     * If you don't have RCS V5 or later, you need to lie about the ci
     * time, since RCS V4 and earlier insist that the times differ.
     */
    now++;
    ftm = localtime (&now);
    (void) sprintf (altdate2, DATEFORM,
		    ftm->tm_year + (ftm->tm_year < 100 ? 0 : 1900),
		    ftm->tm_mon + 1, ftm->tm_mday, ftm->tm_hour,
		    ftm->tm_min, ftm->tm_sec);
#endif
    author = getcaller ();

    if (fprintf (fprcs, "\n%s\n", vhead) < 0 ||
	fprintf (fprcs, "date     %s;  author %s;  state Exp;\n",
		 altdate1, author) < 0 ||
	fprintf (fprcs, "branches %s.1;\n", vbranch) < 0 ||
	fprintf (fprcs, "next     ;\n") < 0 ||
	fprintf (fprcs, "\n%s.1\n", vbranch) < 0 ||
	fprintf (fprcs, "date     %s;  author %s;  state Exp;\n",
		 altdate2, author) < 0 ||
	fprintf (fprcs, "branches ;\n") < 0 ||
	fprintf (fprcs, "next     ;\n\n") < 0 ||
	/*
	 * putdesc()
	 */
	fprintf (fprcs, "\ndesc\n") < 0 ||
	fprintf (fprcs, "@@\n\n\n") < 0 ||
	/*
	 * putdelta()
	 */
	fprintf (fprcs, "\n%s\n", vhead) < 0 ||
	fprintf (fprcs, "log\n") < 0 ||
	fprintf (fprcs, "@Initial revision\n@\n") < 0 ||
	fprintf (fprcs, "text\n@") < 0)
    {
	goto write_error;
    }

    if (sb.st_size > 0)
    {
	off_t size;

	size = sb.st_size;
	buf = xmalloc ((int) size);
	if (fread (buf, (int) size, 1, fpuser) != 1)
	    error (1, errno, "cannot read file %s for copying", user);
	if (expand_at_signs (buf, size, fprcs) < 0)
	{
	    free (buf);
	    goto write_error;
	}
	free (buf);
    }
    if (fprintf (fprcs, "@\n\n") < 0 ||
	fprintf (fprcs, "\n%s.1\n", vbranch) < 0 ||
	fprintf (fprcs, "log\n@") < 0 ||
	expand_at_signs (message, (off_t) strlen (message), fprcs) < 0 ||
	fprintf (fprcs, "@\ntext\n") < 0 ||
	fprintf (fprcs, "@@\n") < 0)
    {
	goto write_error;
    }
    if (fclose (fprcs) == EOF)
    {
	ierrno = errno;
	goto write_error_noclose;
    }
    (void) fclose (fpuser);

    /*
     * Fix the modes on the RCS files.  The user modes of the original
     * user file are propagated to the group and other modes as allowed
     * by the repository umask, except that all write permissions are
     * turned off.
     */
    mode = (sb.st_mode |
	    (sb.st_mode & S_IRWXU) >> 3 |
	    (sb.st_mode & S_IRWXU) >> 6) &
	   ~cvsumask &
	   ~(S_IWRITE | S_IWGRP | S_IWOTH);
    if (chmod (rcs, mode) < 0)
    {
	ierrno = errno;
	fperror (logfp, 0, ierrno,
		 "WARNING: cannot change mode of file %s", rcs);
	error (0, ierrno, "WARNING: cannot change mode of file %s", rcs);
	err++;
    }
    if (tocvsPath)
	if (unlink_file_dir (tocvsPath) < 0)
		error (0, errno, "cannot remove %s", tocvsPath);
    return (err);

write_error:
    ierrno = errno;
    (void) fclose (fprcs);
write_error_noclose:
    (void) fclose (fpuser);
    fperror (logfp, 0, ierrno, "ERROR: cannot write file %s", rcs);
    error (0, ierrno, "ERROR: cannot write file %s", rcs);
    if (ierrno == ENOSPC)
    {
	(void) unlink (rcs);
	fperror (logfp, 0, 0, "ERROR: out of space - aborting");
	error (1, 0, "ERROR: out of space - aborting");
    }
read_error:
    if (tocvsPath)
	if (unlink_file_dir (tocvsPath) < 0)
	    error (0, errno, "cannot remove %s", tocvsPath);

    return (err + 1);
}

/*
 * Write SIZE bytes at BUF to FP, expanding @ signs into double @
 * signs.  If an error occurs, return a negative value and set errno
 * to indicate the error.  If not, return a nonnegative value.
 */
static int
expand_at_signs (buf, size, fp)
    char *buf;
    off_t size;
    FILE *fp;
{
    char *cp, *end;

    errno = 0;
    for (cp = buf, end = buf + size; cp < end; cp++)
    {
	if (*cp == '@')
	{
	    if (putc ('@', fp) == EOF && errno != 0)
		return EOF;
	}
	if (putc (*cp, fp) == EOF && errno != 0)
	    return (EOF);
    }
    return (1);
}

/*
 * Write an update message to (potentially) the screen and the log file.
 */
static void
add_log (ch, fname)
    int ch;
    char *fname;
{
    if (!really_quiet)			/* write to terminal */
    {
	if (repos_len)
	    (void) printf ("%c %s/%s\n", ch, repository + repos_len + 1, fname);
	else if (repository[0])
	    (void) printf ("%c %s/%s\n", ch, repository, fname);
	else
	    (void) printf ("%c %s\n", ch, fname);
    }

    if (repos_len)			/* write to logfile */
	(void) fprintf (logfp, "%c %s/%s\n", ch,
			repository + repos_len + 1, fname);
    else if (repository[0])
	(void) fprintf (logfp, "%c %s/%s\n", ch, repository, fname);
    else
	(void) fprintf (logfp, "%c %s\n", ch, fname);
}

/*
 * This is the recursive function that walks the argument directory looking
 * for sub-directories that have CVS administration files in them and updates
 * them recursively.
 * 
 * Note that we do not follow symbolic links here, which is a feature!
 */
static int
import_descend_dir (message, dir, vtag, targc, targv)
    char *message;
    char *dir;
    char *vtag;
    int targc;
    char *targv[];
{
    struct saved_cwd cwd;
    char *cp;
    int ierrno, err;

    if (islink (dir))
	return (0);
    if (save_cwd (&cwd))
    {
	fperror (logfp, 0, 0, "ERROR: cannot get working directory");
	return (1);
    }
    if (repository[0] == '\0')
	(void) strcpy (repository, dir);
    else
    {
	(void) strcat (repository, "/");
	(void) strcat (repository, dir);
    }
#ifdef CLIENT_SUPPORT
    if (!quiet && !client_active)
#else
    if (!quiet)
#endif
#ifdef SERVER_SUPPORT
	/* Needs to go on stdout, not stderr, to avoid being interspersed
	   with the add_log messages.  */
	printf ("%s %s: Importing %s\n",
		program_name, command_name, repository);
#else
	error (0, 0, "Importing %s", repository);
#endif

    if (chdir (dir) < 0)
    {
	ierrno = errno;
	fperror (logfp, 0, ierrno, "ERROR: cannot chdir to %s", repository);
	error (0, ierrno, "ERROR: cannot chdir to %s", repository);
	err = 1;
	goto out;
    }
#ifdef CLIENT_SUPPORT
    if (!client_active && !isdir (repository))
#else
    if (!isdir (repository))
#endif
    {
	if (isfile (repository))
	{
	    fperror (logfp, 0, 0, "ERROR: %s is a file, should be a directory!",
		     repository);
	    error (0, 0, "ERROR: %s is a file, should be a directory!",
		   repository);
	    err = 1;
	    goto out;
	}
	if (noexec == 0 && CVS_MKDIR (repository, 0777) < 0)
	{
	    ierrno = errno;
	    fperror (logfp, 0, ierrno,
		     "ERROR: cannot mkdir %s -- not added", repository);
	    error (0, ierrno,
		   "ERROR: cannot mkdir %s -- not added", repository);
	    err = 1;
	    goto out;
	}
    }
    err = import_descend (message, vtag, targc, targv);
  out:
    if ((cp = strrchr (repository, '/')) != NULL)
	*cp = '\0';
    else
	repository[0] = '\0';
    if (restore_cwd (&cwd, NULL))
      exit (1);
    free_cwd (&cwd);
    return (err);
}
