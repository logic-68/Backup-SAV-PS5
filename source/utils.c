#include <utils.h>



// https://github.com/OSM-Made/PS4-Notify
void printf_notification(const char *fmt, ...)
{
	SceNotificationRequest noti_buffer;

	va_list args;
	va_start(args, fmt);
	f_vsprintf(noti_buffer.message, fmt, args);
	va_end(args);

	noti_buffer.type = 0;
	noti_buffer.unk3 = 0;
	noti_buffer.use_icon_image_uri = 1;
	noti_buffer.target_id = -1;
	f_strcpy(noti_buffer.uri, "cxml://psnotification/tex_icon_system");

	f_sceKernelSendNotificationRequest(0, (SceNotificationRequest *)&noti_buffer, sizeof(noti_buffer), 0);
}
void touch_file(char *destfile)
{
	int fd = f_open(destfile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (fd != -1)
		f_close(fd);
}
size_t size_file(char *src_file)
{
	FILE *file;
	size_t size = 0;
	file = f_fopen(src_file, "rb");
	if (file)
	{
		f_fseek(file, 0, SEEK_END);
		size = f_ftell(file);
	}
	f_fclose(file);
	return size;
}
int file_exists(char *fname)
{
	FILE *file = f_fopen(fname, "rb");
	if (file)
	{
		f_fclose(file);
		return 1;
	}
	return 0;
}
int dir_exists(char *dname)
{
	DIR *dir = f_opendir(dname);
	if (dir)
	{
		f_closedir(dir);
		return 1;
	}
	return 0;
}

char *getusbpath()
{
	char tmppath[64];
	char tmpusb[64];
	tmpusb[0] = '\0';
	char *retval = f_malloc(sizeof(char) * 10);

	for (int x = 0; x <= 7; x++)
	{
		f_sprintf(tmppath, "/mnt/usb%i/.probe", x);
		touch_file(tmppath);
		if (file_exists(tmppath))
		{
			f_unlink(tmppath);
			f_sprintf(tmpusb, "/mnt/usb%i", x);
			printfsocket("[USB MOUNT PATH]: %s\n", tmpusb);

			f_strcpy(retval, tmpusb);
			return retval;
		}
		tmpusb[0] = '\0';
	}
	printfsocket("[NO USB FOUND.Wait...]\n");
	return NULL;
}

int memset_s(void *s, rsize_t smax, int c, rsize_t n)
{
	int violation = (s == NULL) || (smax > SIZE_MAX) || (n > SIZE_MAX) || (n > smax);
	if (violation)
	{
		if ((s != NULL) && !(smax > SIZE_MAX))
		{
			for (rsize_t i = 0; i < smax; ++i)
			{
				((volatile unsigned char *)s)[i] = c;
			}
		}
		return 1;
	}
	else
	{
		for (rsize_t i = 0; i < n; ++i)
		{
			((volatile unsigned char *)s)[i] = c;
		}
		return 0;
	}
}

char *getUserName(int32_t userId)
{
	char *retval = f_malloc(SCE_USER_SERVICE_MAX_USER_NAME_LENGTH + 1);
	if (retval == NULL)
	{
		return NULL;
	}
	if (f_sceUserServiceInitialize(0) == 0)
	{
		char username[SCE_USER_SERVICE_MAX_USER_NAME_LENGTH + 1];
		if (f_sceUserServiceGetUserName(userId, username, sizeof(username)) == 0)
		{
			f_strcpy(retval, username);
			f_sceUserServiceTerminate();
			return retval;
		}
	}
	f_free(retval);
	return NULL;
}
int getUserIDList(SceUserServiceLoginUserIdList *userIdList)
{
	if (f_sceUserServiceInitialize(NULL) == 0)
	{
		if (f_sceUserServiceGetLoginUserIdList(userIdList) == 0)
		{
			f_sceUserServiceTerminate();
			return 0;
		}
	}
	return -1;
}