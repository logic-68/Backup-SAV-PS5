#ifndef UTILS_H
#define UTILS_H
#include <resolve.h>
#include <debug.h>

int getUserIDList(SceUserServiceLoginUserIdList *userIdList);
int memset_s(void *s, rsize_t smax, int c, rsize_t n);
int dir_exists(char *dname);
int32_t getUserID();
char *getUserName(int32_t userId);
char *getusbpath();
void printf_notification(const char *fmt, ...);
size_t size_file(char *src_file);
#endif