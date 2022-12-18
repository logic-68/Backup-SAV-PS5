/*
#  ____   ____    ____         ___ ____   ____ _     _
# |    |  ____>   ____>  |    |        | <____  \   /
# |____| |    \   ____>  | ___|    ____| <____   \_/	ORBISDEV Open Source Project.
#------------------------------------------------------------------------------------
# Copyright 2010-2020, orbisdev - http://orbisdev.github.io
# Licenced under MIT License
# Review README & LICENSE files for further details.
*/

#ifndef _ORBIS_LIBSCEUSERSERVICE_H_
#define _ORBIS_LIBSCEUSERSERVICE_H_

#pragma once
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif
#define SCE_USER_SERVICE_MAX_LOGIN_USERS 4
#define SCE_USER_SERVICE_MAX_USER_NAME_LENGTH 16

typedef struct SceUserServiceLoginUserIdList{
    int32_t userId[SCE_USER_SERVICE_MAX_LOGIN_USERS];
} SceUserServiceLoginUserIdList;


typedef int SceUserServiceUserId;

int sceUserServiceInitialize(void *);
int sceUserServiceGetLoginUserIdList(SceUserServiceLoginUserIdList *);
int sceUserServiceGetUserName(int32_t userId, char *userName, const size_t size);
int sceUserServiceGetInitialUser(int32_t *userId);
int sceUserServiceTerminate();
int sceUserServiceGetInitialUser(SceUserServiceUserId *userId);

#ifdef __cplusplus
}
#endif

#endif
