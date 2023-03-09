#ifndef _TARA_ACCOUNTS_H_
#define _TARA_ACCOUNTS_H_

#include <errno.h>
#include <time.h>
#include "myglobal.h"
#include "myerr.h"


#define USER_NAME_SIZE                      32
#define USER_PWD_SIZE                       28
#define USER_ACCOUNT_MAX                    16
#define LANG_SIZE                           16


enum {
	USER_LEVEL_USER=0,
	USER_LEVEL_OPERATOR=1,
	USER_LEVEL_ADMIN=2,
	USER_LEVEL_NUM
};

typedef struct {
	s8 name[USER_NAME_SIZE];
	s8 password[USER_PWD_SIZE];
	s32 level;
	s8 language[LANG_SIZE];
} TaraAccount_t;

typedef struct {
	TaraAccount_t user[USER_ACCOUNT_MAX];
} TaraAccounts;

#ifdef CPLUSPLUS
extern "C" {
#endif

/*
 * get user list
 * return: 0 on success, otherwise error code returned
 * ptr: on success, user array will be stored
 */
extern s32 Tara_accounts_list(TaraAccount_t **ptr);

/*
 * update user
 * return: 0 on success, otherwise error code returned
 * ptr: update user by value
 */
extern s32 Tara_accounts_updateUser(TaraAccount_t *ptr);

/*
 * delete user
 * return: 0 on success, otherwise error code returned
 * name: user to delete
 */
extern s32 Tara_accounts_deleteUser(s8 *name);

/*
 * set language
 * return: 0 on success, otherwise error code returned
 * lang: language name to set
 */
extern s32 Tara_accounts_setlang(s8 *lang);

/*
 * get language
 * return: 0 on success, otherwise error code returned
 * lang: on success, current language will be stored
 */
extern s32 Tara_accounts_getlang(s8 *lang);

#ifdef CPLUSPLUS
}
#endif
#endif

