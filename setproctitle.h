#ifndef _SETPROCTITLE_H_INCLUDED
#define _SETPROCTITLE_H_INCLUDED

extern "C" {
void compat_init_setproctitle(int argc, char *argv[]);
void setproctitle(const char *fmt, ...);
}

#endif /* _SETPROCTITLE_H_INCLUDED */
