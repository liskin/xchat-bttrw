#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <mswsock.h>
#include <errno.h>
#include <fcntl.h>
#include "win32.h"

unsigned int winver_major = 0;
unsigned int winver_minor = 0;
int (*my_open_osfhandle)(intptr_t osfhandle, int flags) = (int(*)(intptr_t, int)) &_open_osfhandle;
int (*my_close)(int fd) = 0;

static int w32hack_close(int fd);
static int w32hack_open_osfhandle(intptr_t osfhandle, int flags);

/*
 * Get windows version.
 */
int winver(void)
{
    OSVERSIONINFO osvi;
    
    if (winver_major != 0 || winver_minor != 0)
	return 0;

    memset(&osvi, 0, sizeof(osvi));
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    if (GetVersionEx(&osvi) == 0) {
	fputs("GetVersionEx failed", stderr);
	abort();
    }
    winver_major = osvi.dwMajorVersion;
    winver_minor = osvi.dwMinorVersion;

    return 1;
}

/*
 * Init winsock, call winver and enable hacks for older versions.
 */
void winsock_init(void) {
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(2, 0);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
	fputs("Too old winsock", stderr);
	abort();
    }

    int iSockOpt = SO_SYNCHRONOUS_NONALERT;
    setsockopt(INVALID_SOCKET, SOL_SOCKET, SO_OPENTYPE,
	    (char *)&iSockOpt, sizeof(iSockOpt));

    if (winver()) {
	if (winver_major <= 4) {
	    my_open_osfhandle = &w32hack_open_osfhandle;
	    my_close = &w32hack_close;
	}
    }
}

/*
 * strerror aware of winsock errors.
 */
const char * wsock_strerror(int err) {
    const char *error = 0;

    switch(err) {
	case 10004: error = "Interrupted system call"; break;
	case 10009: error = "Bad file number"; break;
	case 10013: error = "Permission denied"; break;
	case 10014: error = "Bad address"; break;
	case 10022: error = "Invalid argument (not bind)"; break;
	case 10024: error = "Too many open files"; break;
	case 10035: error = "Operation would block"; break;
	case 10036: error = "Operation now in progress"; break;
	case 10037: error = "Operation already in progress"; break;
	case 10038: error = "Socket operation on non-socket"; break;
	case 10039: error = "Destination address required"; break;
	case 10040: error = "Message too long"; break;
	case 10041: error = "Protocol wrong type for socket"; break;
	case 10042: error = "Bad protocol option"; break;
	case 10043: error = "Protocol not supported"; break;
	case 10044: error = "Socket type not supported"; break;
	case 10045: error = "Operation not supported on socket"; break;
	case 10046: error = "Protocol family not supported"; break;
	case 10047: error = "Address family not supported by protocol family"; break;
	case 10048: error = "Address already in use"; break;
	case 10049: error = "Can't assign requested address"; break;
	case 10050: error = "Network is down"; break;
	case 10051: error = "Network is unreachable"; break;
	case 10052: error = "Net dropped connection or reset"; break;
	case 10053: error = "Software caused connection abort"; break;
	case 10054: error = "Connection reset by peer"; break;
	case 10055: error = "No buffer space available"; break;
	case 10056: error = "Socket is already connected"; break;
	case 10057: error = "Socket is not connected"; break;
	case 10058: error = "Can't send after socket shutdown"; break;
	case 10059: error = "Too many references, can't splice"; break;
	case 10060: error = "Connection timed out"; break;
	case 10061: error = "Connection refused"; break;
	case 10062: error = "Too many levels of symbolic links"; break;
	case 10063: error = "File name too long"; break;
	case 10064: error = "Host is down"; break;
	case 10065: error = "No Route to Host"; break;
	case 10066: error = "Directory not empty"; break;
	case 10067: error = "Too many processes"; break;
	case 10068: error = "Too many users"; break;
	case 10069: error = "Disc Quota Exceeded"; break;
	case 10070: error = "Stale NFS file handle"; break;
	case 10091: error = "Network SubSystem is unavailable"; break;
	case 10092: error = "WINSOCK DLL Version out of range"; break;
	case 10093: error = "Successful WSASTARTUP not yet performed"; break;
	case 10071: error = "Too many levels of remote in path"; break;
	case 11001: error = "Host not found"; break;
	case 11002: error = "Non-Authoritative Host not found"; break;
	case 11003: error = "Non-Recoverable errors: FORMERR, REFUSED, NOTIMP"; break;
	case 11004: error = "Valid name, no data record of requested type"; break;
	default: error = strerror(err); break;
    }

    if (!error || !*error) {
	static char t[20];
	sprintf(t, "%i", err);
	error = t;
    }

    return error;
}


/*
 * Compatibility code to make open_osfhandle work on non-NT Windows versions.
 * Taken from Perl, many thanks.
 */
#if defined(__MINGW32__) && (__MINGW32_MAJOR_VERSION>=3)
#undef _CRTIMP
#endif
#ifndef _CRTIMP
#define _CRTIMP __declspec(dllimport)
#endif

/*
 * Control structure for lowio file handles
 */
typedef struct {
    intptr_t osfhnd;/* underlying OS file HANDLE */
    char osfile;    /* attributes of file (e.g., open in text mode?) */
    char pipech;    /* one char buffer for handles opened on pipes */
    int lockinitflag;
    CRITICAL_SECTION lock;
} ioinfo;


/*
 * Array of arrays of control structures for lowio files.
 */
EXTERN_C _CRTIMP ioinfo* __pioinfo[];

/*
 * Definition of IOINFO_L2E, the log base 2 of the number of elements in each
 * array of ioinfo structs.
 */
#define IOINFO_L2E	    5

/*
 * Definition of IOINFO_ARRAY_ELTS, the number of elements in ioinfo array
 */
#define IOINFO_ARRAY_ELTS   (1 << IOINFO_L2E)

/*
 * Access macros for getting at an ioinfo struct and its fields from a
 * file handle
 */
#define _pioinfo(i) (__pioinfo[(i) >> IOINFO_L2E] + ((i) & (IOINFO_ARRAY_ELTS - 1)))
#define _osfhnd(i)  (_pioinfo(i)->osfhnd)
#define _osfile(i)  (_pioinfo(i)->osfile)
#define _pipech(i)  (_pioinfo(i)->pipech)

/* since we are not doing a dup2(), this works fine */
#define _set_osfhnd(fh, osfh) (void)(_osfhnd(fh) = (intptr_t)osfh)

#define FOPEN			0x01	/* file handle open */
#define FNOINHERIT		0x10	/* file handle opened O_NOINHERIT */
#define FAPPEND			0x20	/* file handle opened O_APPEND */
#define FDEV			0x40	/* file handle refers to device */
#define FTEXT			0x80	/* file handle is in text mode */

/***
*int w32hack_open_osfhandle(intptr_t osfhandle, int flags) - open C Runtime file handle
*
*Purpose:
*       This function allocates a free C Runtime file handle and associates
*       it with the Win32 HANDLE specified by the first parameter. This is a
*	temperary fix for WIN95's brain damage GetFileType() error on socket
*	we just bypass that call for socket
*
*	This works with MSVC++ 4.0+ or GCC/Mingw32
*
*Entry:
*       intptr_t osfhandle - Win32 HANDLE to associate with C Runtime file handle.
*       int flags      - flags to associate with C Runtime file handle.
*
*Exit:
*       returns index of entry in fh, if successful
*       return -1, if no free entry is found
*
*Exceptions:
*
*******************************************************************************/

/*
 * we fake up some parts of the CRT that aren't exported by MSVCRT.dll
 * this lets sockets work on Win9X with GCC and should fix the problems
 * with perl95.exe
 *	-- BKS, 1-23-2000
*/

/* create an ioinfo entry, kill its handle, and steal the entry */

static int
_alloc_osfhnd(void)
{
    HANDLE hF = CreateFile("NUL", 0, 0, NULL, OPEN_ALWAYS, 0, NULL);
    int fh = _open_osfhandle((intptr_t)hF, 0);
    CloseHandle(hF);
    if (fh == -1)
        return fh;
    EnterCriticalSection(&(_pioinfo(fh)->lock));
    return fh;
}

static int
w32hack_open_osfhandle(intptr_t osfhandle, int flags)
{
    int fh;
    char fileflags;		/* _osfile flags */

    /* copy relevant flags from second parameter */
    fileflags = FDEV;

    if (flags & O_APPEND)
	fileflags |= FAPPEND;

    if (flags & O_TEXT)
	fileflags |= FTEXT;

    if (flags & O_NOINHERIT)
	fileflags |= FNOINHERIT;

    /* attempt to allocate a C Runtime file handle */
    if ((fh = _alloc_osfhnd()) == -1) {
	errno = EMFILE;		/* too many open files */
	_doserrno = 0L;		/* not an OS error */
	return -1;		/* return error to caller */
    }

    /* the file is open. now, set the info in _osfhnd array */
    _set_osfhnd(fh, osfhandle);

    fileflags |= FOPEN;		/* mark as open */

    _osfile(fh) = fileflags;	/* set osfile entry */
    LeaveCriticalSection(&_pioinfo(fh)->lock);

    return fh;			/* return handle */
}

static int
w32hack_close(int fd)
{
    _set_osfhnd(fd, INVALID_HANDLE_VALUE);
    return 0;
}

/*
 * localtime_r implementation. Win32 uses thread-local storage for localtime
 * result, so it's ok.
 */
struct tm *localtime_r(const time_t *timep, struct tm *result)
{
    struct tm *res = localtime(timep);
    if (res)
	*result = *res;

    return res;
}
