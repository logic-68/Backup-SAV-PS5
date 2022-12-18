#ifndef UTILS_H
#define UTILS_H
#include <resolve.h>
#include <debug.h>

void printf_notification(const char *fmt, ...);
int memset_s(void *s, rsize_t smax, int c, rsize_t n);
int getUserIDList(SceUserServiceLoginUserIdList *userIdList);
char *getUserName(int32_t userId);
int32_t getUserID();
int dir_exists(char *dname);
char *getusbpath();
void copy_file(char *src_path, char *dst_path);
void copy_dir(char *dir_current, char *out_dir);
#endif