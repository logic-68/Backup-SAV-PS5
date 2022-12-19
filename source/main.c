#include <utils.h>

#define VERSION "1.0.1"
#define DEBUG_SOCKET
#define DEBUG_ADDR IP(192, 168, 1, 155);
#define DEBUG_PORT 5655
#define PROSPERO "savedata_prospero"
#define HOME "/user/home"
#define OCT_TO_MO / 1024 / 1024

int sock;
int nthread_run, sav, isxfer = 0;
char *cDir;
size_t sizeCurrent, total_bytes_copied = 0;

void copy_file(char *src_path, char *dst_path)
{
	int src = f_open(src_path, O_RDONLY, 0);
	if (src != -1)
	{
		int out = f_open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0777);
		if (out != -1)
		{

			char *buffer = f_malloc(4194304);
			if (buffer != NULL)
			{
				size_t bytes, bytes_size, bytes_copied = 0;
				f_lseek(src, 0L, SEEK_END);
				bytes_size = f_lseek(src, 0, SEEK_CUR);
				f_lseek(src, 0L, SEEK_SET);
				while ((bytes = f_read(src, buffer, 4194304)) > 0)
				{
					f_write(out, buffer, bytes);
					bytes_copied += bytes;
					if (bytes_copied > bytes_size)
						bytes_copied = bytes_size;
					total_bytes_copied += bytes_copied;
				}
				f_free(buffer);
			}
		}
		f_close(out);
	}
	f_close(src);
}

void copy_dir(char *dir_current, char *out_dir)
{
	DIR *dir = f_opendir(dir_current);
	struct dirent *dp;
	struct stat info;
	char src_path[256], dst_path[256];
	if (!dir)
	{
		return;
	}

	f_mkdir(out_dir, 0777);

	if (dir_exists(out_dir))
	{
		while ((dp = f_readdir(dir)) != NULL)
		{
			if (!f_strcmp(dp->d_name, ".") || !f_strcmp(dp->d_name, ".."))
			{
				continue;
			}
			else
			{
				f_sprintf(src_path, "%s/%s", dir_current, dp->d_name);
				f_sprintf(dst_path, "%s/%s", out_dir, dp->d_name);

				if (!f_stat(src_path, &info))
				{
					if (S_ISDIR(info.st_mode))
					{
						cDir = src_path;
						copy_dir(src_path, dst_path);
					}
					else if (S_ISREG(info.st_mode))
					{
						copy_file(src_path, dst_path);
					}
				}
			}
		}
	}
	f_closedir(dir);
}

void check_size_folder_current(char *dir_current)
{
	DIR *dir = f_opendir(dir_current);
	struct dirent *dp;
	struct stat info;
	char src_file[1024];
	size_t size;
	if (!dir)
		return;
	while ((dp = f_readdir(dir)) != NULL)
	{
		if (!f_strcmp(dp->d_name, ".") || !f_strcmp(dp->d_name, ".."))
			continue;
		else
		{
			f_sprintf(src_file, "%s/%s", dir_current, dp->d_name);
			if (!f_stat(src_file, &info))
			{
				if (S_ISDIR(info.st_mode))
				{
					sav++;
					check_size_folder_current(src_file);
				}
				else if (S_ISREG(info.st_mode))
				{
					size = size_file(src_file);
					sizeCurrent += size;
				}
			}
		}
	}
	f_closedir(dir);

}
void *nthread_func(void *arg)
{
	UNUSED(arg);
	time_t t1, t2;
	t1 = 0;
	while (nthread_run)
	{
		if (isxfer)
		{
			t2 = f_time(NULL);
			if ((t2 - t1) >= 7)
			{
				t1 = t2;
				printf_notification("Copy in progress please wait...\n%i%%", total_bytes_copied * 100 / sizeCurrent);
			}
		}
		else
			t1 = 0;
		f_sceKernelSleep(5);
	}

	return NULL;
}
int payload_main(struct payload_args *args)
{
	dlsym_t *dlsym = args->dlsym;

	int libKernel = 0x2001;

	dlsym(libKernel, "sceKernelSleep", &f_sceKernelSleep);
	dlsym(libKernel, "sceKernelLoadStartModule", &f_sceKernelLoadStartModule);
	dlsym(libKernel, "sceKernelDebugOutText", &f_sceKernelDebugOutText);
	dlsym(libKernel, "sceKernelSendNotificationRequest", &f_sceKernelSendNotificationRequest);
	dlsym(libKernel, "sceKernelUsleep", &f_sceKernelUsleep);
	dlsym(libKernel, "scePthreadMutexLock", &f_scePthreadMutexLock);
	dlsym(libKernel, "scePthreadMutexUnlock", &f_scePthreadMutexUnlock);
	dlsym(libKernel, "scePthreadExit", &f_scePthreadExit);
	dlsym(libKernel, "scePthreadMutexInit", &f_scePthreadMutexInit);
	dlsym(libKernel, "scePthreadCreate", &f_scePthreadCreate);
	dlsym(libKernel, "scePthreadMutexDestroy", &f_scePthreadMutexDestroy);
	dlsym(libKernel, "scePthreadJoin", &f_scePthreadJoin);
	dlsym(libKernel, "socket", &f_socket);
	dlsym(libKernel, "bind", &f_bind);
	dlsym(libKernel, "listen", &f_listen);
	dlsym(libKernel, "accept", &f_accept);
	dlsym(libKernel, "open", &f_open);
	dlsym(libKernel, "read", &f_read);
	dlsym(libKernel, "write", &f_write);
	dlsym(libKernel, "close", &f_close);
	dlsym(libKernel, "stat", &f_stat);
	dlsym(libKernel, "fstat", &f_fstat);
	dlsym(libKernel, "rename", &f_rename);
	dlsym(libKernel, "rmdir", &f_rmdir);
	dlsym(libKernel, "mkdir", &f_mkdir);
	dlsym(libKernel, "getdents", &f_getdents);
	dlsym(libKernel, "unlink", &f_unlink);
	dlsym(libKernel, "readlink", &f_readlink);
	dlsym(libKernel, "lseek", &f_lseek);
	dlsym(libKernel, "puts", &f_puts);
	dlsym(libKernel, "mmap", &f_mmap);
	dlsym(libKernel, "munmap", &f_munmap);

	dlsym(libKernel, "sceKernelReboot", &f_sceKernelReboot);

	int libNet = f_sceKernelLoadStartModule("libSceNet.sprx", 0, 0, 0, 0, 0);
	dlsym(libNet, "sceNetSocket", &f_sceNetSocket);
	dlsym(libNet, "sceNetConnect", &f_sceNetConnect);
	dlsym(libNet, "sceNetHtons", &f_sceNetHtons);
	dlsym(libNet, "sceNetAccept", &f_sceNetAccept);
	dlsym(libNet, "sceNetSend", &f_sceNetSend);
	dlsym(libNet, "sceNetInetNtop", &f_sceNetInetNtop);
	dlsym(libNet, "sceNetSocketAbort", &f_sceNetSocketAbort);
	dlsym(libNet, "sceNetBind", &f_sceNetBind);
	dlsym(libNet, "sceNetListen", &f_sceNetListen);
	dlsym(libNet, "sceNetSocketClose", &f_sceNetSocketClose);
	dlsym(libNet, "sceNetHtonl", &f_sceNetHtonl);
	dlsym(libNet, "sceNetInetPton", &f_sceNetInetPton);
	dlsym(libNet, "sceNetGetsockname", &f_sceNetGetsockname);
	dlsym(libNet, "sceNetRecv", &f_sceNetRecv);
	dlsym(libNet, "sceNetErrnoLoc", &f_sceNetErrnoLoc);
	dlsym(libNet, "sceNetSetsockopt", &f_sceNetSetsockopt);

	int libC = f_sceKernelLoadStartModule("libSceLibcInternal.sprx", 0, 0, 0, 0, 0);
	dlsym(libC, "vsprintf", &f_vsprintf);
	dlsym(libC, "memset", &f_memset);
	dlsym(libC, "sprintf", &f_sprintf);
	dlsym(libC, "snprintf", &f_snprintf);
	dlsym(libC, "snprintf_s", &f_snprintf_s);
	dlsym(libC, "strcat", &f_strcat);
	dlsym(libC, "free", &f_free);
	dlsym(libC, "memcpy", &f_memcpy);
	dlsym(libC, "strcpy", &f_strcpy);
	dlsym(libC, "strncpy", &f_strncpy);
	dlsym(libC, "sscanf", &f_sscanf);
	dlsym(libC, "scanf", &f_scanf);
	dlsym(libC, "scanf", &f_scanf);
	dlsym(libC, "malloc", &f_malloc);
	dlsym(libC, "calloc", &f_calloc);
	dlsym(libC, "strlen", &f_strlen);
	dlsym(libC, "strcmp", &f_strcmp);
	dlsym(libC, "strchr", &f_strchr);
	dlsym(libC, "strrchr", &f_strrchr);
	dlsym(libC, "gmtime_s", &f_gmtime_s);
	dlsym(libC, "time", &f_time);
	dlsym(libC, "localtime", &f_localtime);
	dlsym(libC, "closedir", &f_closedir);
	dlsym(libC, "opendir", &f_opendir);
	dlsym(libC, "readdir", &f_readdir);
	dlsym(libC, "fclose", &f_fclose);
	dlsym(libC, "fopen", &f_fopen);
	dlsym(libC, "strstr", &f_strstr);
	dlsym(libC, "fseek", &f_fseek);
	dlsym(libC, "ftell", &f_ftell);
	dlsym(libC, "fread", &f_fread);
	dlsym(libC, "usleep", &f_usleep);
	dlsym(libC, "fputs", &f_fputs);
	dlsym(libC, "fgetc", &f_fgetc);
	dlsym(libC, "feof", &f_feof);
	dlsym(libC, "fprintf", &f_fprintf);
	dlsym(libC, "realloc", &f_realloc);
	dlsym(libC, "seekdir", &f_seekdir);

	int libNetCtl = f_sceKernelLoadStartModule("libSceNetCtl.sprx", 0, 0, 0, 0, 0);
	dlsym(libNetCtl, "sceNetCtlInit", &f_sceNetCtlInit);
	dlsym(libNetCtl, "sceNetCtlTerm", &f_sceNetCtlTerm);
	dlsym(libNetCtl, "sceNetCtlGetInfo", &f_sceNetCtlGetInfo);

	int libSysModule = f_sceKernelLoadStartModule("libSceSysmodule.sprx", 0, 0, 0, 0, 0);
	dlsym(libSysModule, "sceSysmoduleLoadModuleInternal", &f_sceSysmoduleLoadModuleInternal);
	dlsym(libSysModule, "sceSysmoduleUnloadModuleInternal", &f_sceSysmoduleUnloadModuleInternal);

	int sysModule = f_sceSysmoduleLoadModuleInternal(SCE_SYSMODULE_INTERNAL_USER_SERVICE);
	SceUserServiceLoginUserIdList userIdList;
	memset_s(&userIdList, sizeof(SceUserServiceLoginUserIdList), 0, sizeof(SceUserServiceLoginUserIdList));

	int libUserService = f_sceKernelLoadStartModule("libSceUserService.sprx", 0, 0, 0, 0, 0);
	dlsym(libUserService, "sceUserServiceInitialize", &f_sceUserServiceInitialize);
	dlsym(libUserService, "sceUserServiceGetInitialUser", &f_sceUserServiceGetInitialUser);
	dlsym(libUserService, "sceUserServiceGetLoginUserIdList", &f_sceUserServiceGetLoginUserIdList);
	dlsym(libUserService, "sceUserServiceGetUserName", &f_sceUserServiceGetUserName);
	dlsym(libUserService, "sceUserServiceTerminate", &f_sceUserServiceTerminate);

	struct sockaddr_in server;
	server.sin_len = sizeof(server);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = DEBUG_ADDR;
	server.sin_port = f_sceNetHtons(DEBUG_PORT);
	f_memset(server.sin_zero, 0, sizeof(server.sin_zero));
	sock = f_sceNetSocket("debug", AF_INET, SOCK_STREAM, 0);
	f_sceNetConnect(sock, (struct sockaddr *)&server, sizeof(server));

	char src_path[256], dst_path[256], usb_mount_path[64], userName[16];
	int32_t userId;

	nthread_run = 1;

	ScePthread nthread;
	f_scePthreadCreate(&nthread, NULL, nthread_func, NULL, "nthread");

	char *usb_mnt_path = getusbpath();
	if (usb_mnt_path == NULL)
	{
		do
		{
			printf_notification("Please insert USB media in exfat/fat32 format");
			f_sceKernelSleep(7);
			usb_mnt_path = getusbpath();
		} while (usb_mnt_path == NULL);
	}
	f_sprintf(usb_mount_path, "%s", usb_mnt_path);
	f_free(usb_mnt_path);

	if (sysModule == 0)
	{
		if (getUserIDList(&userIdList) == 0)
		{
			for (int i = 0; i < SCE_USER_SERVICE_MAX_LOGIN_USERS; i++)
			{
				if (userIdList.userId[i] != -1 && userIdList.userId[i] != 0)
				{
					userId = userIdList.userId[i];
					f_sprintf(userName, "%s", getUserName(userIdList.userId[i]));
					f_sprintf(dst_path, "%s/%s", usb_mount_path, userName);
					f_mkdir(dst_path, 0777);
					printf_notification("Welcome %s to\nBackup-SAV-PS5 V%s\n\nBy ★@Logic-68★", userName, VERSION);
					f_sceKernelSleep(7);
				}
			}
			f_sprintf(src_path, "%s/%x/%s", HOME, userId, PROSPERO);

			check_size_folder_current(src_path);
			size_t tmpCurrentSize = sizeCurrent;
			
			printfsocket("%lu", sizeCurrent);
			printf_notification("Size of: %s\n%.2fMo\nNumber of saves: %d\nCopy start.Please wait...", PROSPERO, (double)sizeCurrent OCT_TO_MO, sav);
			f_sceKernelSleep(7);
			f_sprintf(dst_path, "%s/%s/%s", usb_mount_path, userName, PROSPERO);

			if (dir_exists(src_path))
			{
				sizeCurrent = 0;
				isxfer = 1;
				copy_dir(src_path, dst_path);
				isxfer = 0;
				f_sceKernelSleep(7);
				
				check_size_folder_current(src_path);
				if(tmpCurrentSize == sizeCurrent)
				{
					printf_notification("Copy of:\n%s\n★Successfully★\n%.2f/%.2fMo", src_path, (double)sizeCurrent OCT_TO_MO, (double)tmpCurrentSize OCT_TO_MO);	
					f_sceKernelSleep(7);
				} else {
					printf_notification("An error is served try again");
				}
			}
			else
			{
				printf_notification("%s", "No backup for this account");
			}
		}
		else
		{
			printf_notification("Sorry, error loading user list...");
		}
	}
	else
	{
		printf_notification("Sory, error loading libSceSysmodule...");
	}
	nthread_run = 0;
	sysModule = f_sceSysmoduleUnloadModuleInternal(SCE_SYSMODULE_INTERNAL_USER_SERVICE);

	printf_notification("Thank you for using Backup-SAV-PS5\n\nGoodbye!");
	return 0;
}