#ifndef WIN32_H_INCLUDED
#define WIN32_H_INCLUDED

#include <io.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned int winver_major;
extern unsigned int winver_minor;
int winver(void);
void winsock_init(void);
const char * wsock_strerror(int err);
extern int (*my_open_osfhandle)(intptr_t osfhandle, int flags);
extern int (*my_close)(int fd);

struct tm *localtime_r(const time_t *timep, struct tm *result);

#ifdef __cplusplus
}
#endif

#endif /* WIN32_H_INCLUDED */
