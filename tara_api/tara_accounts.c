#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/file.h>
#include <unistd.h>
//#include "shmkey.h"
//#include "mutex.h"
//#include "tara_accounts_priv.h"
#include "tara_accounts.h"
#include "cfg_util.h"

#define DEF_ACCOUNT_CFG                               "/var/cfg/accounts.cfg"
#define DEF_LANG_VALUE                                "Auto"

#define CFG_SECTION_GLOBAL                            "global"
#define CFG_FIELD_LANG                                "language"
#define CFG_FIELD_USERNAME                            "username"
#define CFG_FIELD_PASSWORD                            "password"
#define CFG_FIELD_USERLEVEL                           "userlevel"

#define STR_USER_LEVEL_ADMIN                          "Administrator"
#define STR_USER_LEVEL_OPERATOR                       "Operator"
#define STR_USER_LEVEL_USER                           "User"

#define RESERVED_USER_RECOVERY                        "recovery"

enum {
	RESERVED_USER_1=0,
	RESERVED_USER_NUM
};

const char *strlevel[USER_LEVEL_NUM]={STR_USER_LEVEL_USER, STR_USER_LEVEL_OPERATOR, STR_USER_LEVEL_ADMIN};
const char *strruser[RESERVED_USER_NUM]={RESERVED_USER_RECOVERY};


static void call_system(const char* file, const int line, const char* cmd)
{
	int rc = system(cmd);
	if (WIFSIGNALED(rc)) {
		printf("%s:%d system call interrupted by signal %d (%s)\n", file, line, WTERMSIG(rc), strsignal(WTERMSIG(rc)));
	}
}
#define SYSTEM(cmd) call_system(__FILE__, __LINE__, cmd)


static s32 _accounts_default()
{
	s32 ret=ERROR_NONE;
	param_handle_t *param_h;
	param_h = param_open(DEF_ACCOUNT_CFG, "r+");
	if (param_h) {
		s8 sec_name[32];
		s8 item_name[32], item_value[64];
		s32 fd=param_h->fd;
		s32 locked=0;
		while (1) {
			if (flock(fd, LOCK_EX) == 0) {
				locked = 1;
				break;
			}

			usleep(10000);
		}

		// global section
		strcpy(sec_name, CFG_SECTION_GLOBAL);
		strcpy(item_name, CFG_FIELD_LANG);
		snprintf(item_value, sizeof(item_value), "%s", DEF_LANG_VALUE);
		ret = param_set_value(param_h, sec_name, item_name, item_value);
		if (ret == -ENOENT) {  /* section not found */
			ret = param_insert_section(param_h, NULL, sec_name);
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		} else if (ret == -ENODATA) {  /* item not found */
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		}

		// user0
		strcpy(sec_name, "user0");
		strcpy(item_name, CFG_FIELD_USERNAME);
		strcpy(item_value, "recovery");
		ret = param_set_value(param_h, sec_name, item_name, item_value);
		if (ret == -ENOENT) {  /* section not found */
			ret = param_insert_section(param_h, NULL, sec_name);
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		} else if (ret == -ENODATA) {  /* item not found */
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		}

		strcpy(item_name, CFG_FIELD_PASSWORD);
		strcpy(item_value, "worldpeace");
		ret = param_set_value(param_h, sec_name, item_name, item_value);
		if (ret == -ENODATA) {  /* item not found */
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		}

		strcpy(item_name, CFG_FIELD_USERLEVEL);
		strcpy(item_value, strlevel[USER_LEVEL_ADMIN]);
		ret = param_set_value(param_h, sec_name, item_name, item_value);
		if (ret == -ENODATA) {  /* item not found */
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		}

		// user1
		strcpy(sec_name, "user1");
		strcpy(item_name, CFG_FIELD_USERNAME);
		strcpy(item_value, "integrator");
		ret = param_set_value(param_h, sec_name, item_name, item_value);
		if (ret == -ENOENT) {  /* section not found */
			ret = param_insert_section(param_h, NULL, sec_name);
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		} else if (ret == -ENODATA) {  /* item not found */
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		}

		strcpy(item_name, CFG_FIELD_PASSWORD);
		strcpy(item_value, "integrator");
		ret = param_set_value(param_h, sec_name, item_name, item_value);
		if (ret == -ENODATA) {  /* item not found */
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		}

		strcpy(item_name, CFG_FIELD_USERLEVEL);
		strcpy(item_value, strlevel[USER_LEVEL_ADMIN]);
		ret = param_set_value(param_h, sec_name, item_name, item_value);
		if (ret == -ENODATA) {  /* item not found */
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		}

		// user2
		strcpy(sec_name, "user2");
		strcpy(item_name, CFG_FIELD_USERNAME);
		strcpy(item_value, "admin");
		ret = param_set_value(param_h, sec_name, item_name, item_value);
		if (ret == -ENOENT) {  /* section not found */
			ret = param_insert_section(param_h, NULL, sec_name);
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		} else if (ret == -ENODATA) {  /* item not found */
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		}

		strcpy(item_name, CFG_FIELD_PASSWORD);
		strcpy(item_value, "camera");
		ret = param_set_value(param_h, sec_name, item_name, item_value);
		if (ret == -ENODATA) {  /* item not found */
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		}

		strcpy(item_name, CFG_FIELD_USERLEVEL);
		strcpy(item_value, strlevel[USER_LEVEL_ADMIN]);
		ret = param_set_value(param_h, sec_name, item_name, item_value);
		if (ret == -ENODATA) {  /* item not found */
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		}

		// user3
		strcpy(sec_name, "user3");
		strcpy(item_name, CFG_FIELD_USERNAME);
		strcpy(item_value, "operator");
		ret = param_set_value(param_h, sec_name, item_name, item_value);
		if (ret == -ENOENT) {  /* section not found */
			ret = param_insert_section(param_h, NULL, sec_name);
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		} else if (ret == -ENODATA) {  /* item not found */
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		}

		strcpy(item_name, CFG_FIELD_PASSWORD);
		strcpy(item_value, "operator");
		ret = param_set_value(param_h, sec_name, item_name, item_value);
		if (ret == -ENODATA) {  /* item not found */
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		}

		strcpy(item_name, CFG_FIELD_USERLEVEL);
		strcpy(item_value, strlevel[USER_LEVEL_OPERATOR]);
		ret = param_set_value(param_h, sec_name, item_name, item_value);
		if (ret == -ENODATA) {  /* item not found */
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		}

		// user4
		strcpy(sec_name, "user4");
		strcpy(item_name, CFG_FIELD_USERNAME);
		strcpy(item_value, "user");
		ret = param_set_value(param_h, sec_name, item_name, item_value);
		if (ret == -ENOENT) {  /* section not found */
			ret = param_insert_section(param_h, NULL, sec_name);
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		} else if (ret == -ENODATA) {  /* item not found */
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		}

		strcpy(item_name, CFG_FIELD_PASSWORD);
		strcpy(item_value, "changeme");
		ret = param_set_value(param_h, sec_name, item_name, item_value);
		if (ret == -ENODATA) {  /* item not found */
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		}

		strcpy(item_name, CFG_FIELD_USERLEVEL);
		strcpy(item_value, strlevel[USER_LEVEL_USER]);
		ret = param_set_value(param_h, sec_name, item_name, item_value);
		if (ret == -ENODATA) {  /* item not found */
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		}

		param_sync_to_file(param_h);
		fsync(fd);
		if(locked) flock(fd, LOCK_UN);

		param_close(param_h);
	}

	return ret;
}

static s32 _accounts_list(TaraAccount_t **accounts, s8 *cfg)
{
	s32 ucnt=0;
	s32 sizeacc=sizeof(TaraAccount_t);
	TaraAccount_t *ptr=NULL;
	param_handle_t *param_h;
START:
	param_h = param_open(cfg, "r");
	if (param_h) {
		s8 sec_name[32], lang[32], level[32];
		s32 fd=param_h->fd;
		s32 locked=0, i, j;
		while (1) {
			if (flock(fd, LOCK_EX) == 0) {
				locked = 1;
				break;
			}

			usleep(10000);
		}

		strcpy(lang, DEF_LANG_VALUE);
		strcpy(sec_name, CFG_SECTION_GLOBAL);
		if (param_get_value(param_h, sec_name, CFG_FIELD_LANG)) {
			strcpy(lang, param_get_value(param_h, sec_name, CFG_FIELD_LANG));
		}

		for (i=0; i<USER_ACCOUNT_MAX; i++) {
			snprintf(sec_name, sizeof(sec_name), "user%d", i);
			if (param_get_value(param_h, sec_name, CFG_FIELD_USERNAME) &&
			    strlen(param_get_value(param_h, sec_name, CFG_FIELD_USERNAME))>0) {
				s8 skip=0;
				for (j=0; j<RESERVED_USER_NUM; j++)
					if (!strcmp(param_get_value(param_h, sec_name, CFG_FIELD_USERNAME), strruser[j])) {
						skip = 1;
						break;
					}

				if (skip == 1) continue;
				*accounts = (TaraAccount_t*)realloc(*accounts, sizeacc*(ucnt+1));
				ptr = &(*accounts)[ucnt];
				memset(ptr, 0, sizeacc);
				ptr->level = USER_LEVEL_USER;
				strncpy(ptr->name, param_get_value(param_h, sec_name, CFG_FIELD_USERNAME), USER_NAME_SIZE-1);
				if (param_get_value(param_h, sec_name, CFG_FIELD_PASSWORD)) {
					strncpy(ptr->password, param_get_value(param_h, sec_name, CFG_FIELD_PASSWORD), USER_PWD_SIZE-1);
				}

				if (param_get_value(param_h, sec_name, CFG_FIELD_USERLEVEL)) {
					strcpy(level, param_get_value(param_h, sec_name, CFG_FIELD_USERLEVEL));
					for (j=0; j<USER_LEVEL_NUM; j++) {
						if (!strcmp(level, strlevel[j])) {
							ptr->level = j;
							break;
						}
					}

				}

				strcpy(ptr->language, lang);
				ucnt++;
			}
		}

		if (locked) flock(fd, LOCK_UN);

		param_close(param_h);
	} else {
		if (errno != -ENOENT) return -errno;
		if (cfg_create(cfg)==0 && _accounts_default()==0) goto START;
	}

	return ucnt;
}

s32 Tara_accounts_list(TaraAccount_t **accounts)
{
	if (!accounts) return ERROR_INVALARG;
	return _accounts_list(accounts, DEF_ACCOUNT_CFG);
}

static s32 _accounts_update(TaraAccount_t *account, s8 *cfg)
{
	s32 ret=ERROR_NONE;
	param_handle_t *param_h;
	s8 cmd[128];
START:
	param_h = param_open(cfg, "r+");
	if (param_h) {
		s8 sec_name[32], uname[USER_NAME_SIZE];
		s8 item_name[32], item_value[64];
		s32 fd=param_h->fd;
		s32 locked=0, i, isnew=1, emptyidx=-1;
		while (1) {
			if (flock(fd, LOCK_EX) == 0) {
				locked = 1;
				break;
			}

			usleep(10000);
		}

		for (i=0; i<USER_ACCOUNT_MAX; i++) {
			snprintf(sec_name, sizeof(sec_name), "user%d", i);
			if (param_get_value(param_h, sec_name, CFG_FIELD_USERNAME) &&
			    strlen(param_get_value(param_h, sec_name, CFG_FIELD_USERNAME))>0)
				strcpy(uname, param_get_value(param_h, sec_name, CFG_FIELD_USERNAME));
			else {
				strcpy(uname, "");
				if (emptyidx == -1) emptyidx = i;
				continue;
			}

			if (!strcmp(account->name, uname)) {
				isnew = 0;
				strcpy(item_name, CFG_FIELD_PASSWORD);
				strcpy(item_value, account->password);
				param_set_value(param_h, sec_name, item_name, item_value);
				strcpy(item_name, CFG_FIELD_USERLEVEL);
				if (account->level<USER_LEVEL_USER || account->level>=USER_LEVEL_NUM) account->level = USER_LEVEL_USER;
				snprintf(item_value, sizeof(item_value), "%s", strlevel[account->level]);
				param_set_value(param_h, sec_name, item_name, item_value);

				param_sync_to_file(param_h);
				fsync(fd);
				if(locked) flock(fd, LOCK_UN);

				param_close(param_h);
				switch (account->level) {
					case USER_LEVEL_ADMIN:
						snprintf(cmd, sizeof(cmd), "/srv/sbin/pwd_generator.sh %s %s /srv/http/conf/htdigest_Administrator", account->name, account->password);
						SYSTEM(cmd);
					case USER_LEVEL_OPERATOR:
						if (account->level == USER_LEVEL_OPERATOR) {
							snprintf(cmd, sizeof(cmd), "/srv/sbin/del_account.sh %s %s /srv/http/conf/htdigest_Administrator", account->name, account->password);
							SYSTEM(cmd);
						}
						snprintf(cmd, sizeof(cmd), "/srv/sbin/pwd_generator.sh %s %s /srv/http/conf/htdigest_Operator+", account->name, account->password);
						SYSTEM(cmd);
					case USER_LEVEL_USER:
						if (account->level == USER_LEVEL_USER) {
							snprintf(cmd, sizeof(cmd), "/srv/sbin/del_account.sh %s %s /srv/http/conf/htdigest_Administrator", account->name, account->password);
							SYSTEM(cmd);
							snprintf(cmd, sizeof(cmd), "/srv/sbin/del_account.sh %s %s /srv/http/conf/htdigest_Operator+", account->name, account->password);
							SYSTEM(cmd);
						}
						snprintf(cmd, sizeof(cmd), "/srv/sbin/pwd_generator.sh %s %s /srv/http/conf/htdigest_User+", account->name, account->password);
						SYSTEM(cmd);
						snprintf(cmd, sizeof(cmd), "/srv/bin/nginx -s reload");
						SYSTEM(cmd);
					default: break;
				}
				break;
			}
		}

		// new user
		if (isnew==1 && emptyidx!=-1) {
			snprintf(sec_name, sizeof(sec_name), "user%d", emptyidx);
			strcpy(item_name, CFG_FIELD_USERNAME);
			snprintf(item_value, sizeof(item_value), "%s", account->name);
			ret = param_set_value(param_h, sec_name, item_name, item_value);
			if (ret == -ENOENT) {  /* section not found */
				ret = param_insert_section(param_h, NULL, sec_name);
				ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
			} else if (ret == -ENODATA) {  /* item not found */
				ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
			}

			strcpy(item_name, CFG_FIELD_PASSWORD);
			snprintf(item_value, sizeof(item_value), "%s", account->password);
			ret = param_set_value(param_h, sec_name, item_name, item_value);
			if (ret == -ENODATA) {  /* item not found */
				ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
			}

			strcpy(item_name, CFG_FIELD_USERLEVEL);
			snprintf(item_value, sizeof(item_value), "%s", strlevel[account->level]);
			ret = param_set_value(param_h, sec_name, item_name, item_value);
			if (ret == -ENODATA) {  /* item not found */
				ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
			}

			param_sync_to_file(param_h);
			fsync(fd);
			if(locked) flock(fd, LOCK_UN);

			param_close(param_h);
			switch (account->level) {
				case USER_LEVEL_ADMIN:
					snprintf(cmd, sizeof(cmd), "/srv/sbin/pwd_generator.sh %s %s /srv/http/conf/htdigest_Administrator", account->name, account->password);
					SYSTEM(cmd);
				case USER_LEVEL_OPERATOR:
					snprintf(cmd, sizeof(cmd), "/srv/sbin/pwd_generator.sh %s %s /srv/http/conf/htdigest_Operator+", account->name, account->password);
					SYSTEM(cmd);
				case USER_LEVEL_USER:
					snprintf(cmd, sizeof(cmd), "/srv/sbin/pwd_generator.sh %s %s /srv/http/conf/htdigest_User+", account->name, account->password);
					SYSTEM(cmd);
					snprintf(cmd, sizeof(cmd), "/srv/bin/nginx -s reload");
					SYSTEM(cmd);
				default: break;
			}
			ret = ERROR_NONE;
		}
	} else {
		if (errno != -ENOENT) return errno;
		if (cfg_create(cfg)==0 && _accounts_default()==0) goto START;
	}

	return ret;
}

s32 Tara_accounts_updateUser(TaraAccount_t *account)
{
	if (!account) return ERROR_INVALARG;
	s32 k=0;
	for (k=0; k<RESERVED_USER_NUM; k++) if (!strcmp(account->name, strruser[k])) return ERROR_UNSUPPORTED;
	return _accounts_update(account, DEF_ACCOUNT_CFG);
}

static s32 _accounts_delete(s8 *name, s8*cfg)
{
	s32 ret=ERROR_NONE;
	param_handle_t *param_h;
	s8 cmd[128];
START:
	param_h = param_open(cfg, "r+");
	if (param_h) {
		s8 sec_name[32];
		s8 item_name[32], item_value[64];
		s32 fd=param_h->fd;
		s32 locked=0, i;
		while (1) {
			if (flock(fd, LOCK_EX) == 0) {
				locked = 1;
				break;
			}

			usleep(10000);
		}

		for (i=0; i<USER_ACCOUNT_MAX; i++) {
			snprintf(sec_name, sizeof(sec_name), "user%d", i);
			if (param_get_value(param_h, sec_name, CFG_FIELD_USERNAME) &&
			    !strcmp(name, param_get_value(param_h, sec_name, CFG_FIELD_USERNAME))) {
				strcpy(item_name, CFG_FIELD_USERNAME);
				strcpy(item_value, "");
				param_set_value(param_h, sec_name, item_name, item_value);

				strcpy(item_name, CFG_FIELD_PASSWORD);
				strcpy(item_value, "");
				param_set_value(param_h, sec_name, item_name, item_value);

				strcpy(item_name, CFG_FIELD_USERLEVEL);
				snprintf(item_value, sizeof(item_value), "%s", "");
				param_set_value(param_h, sec_name, item_name, item_value);

				param_sync_to_file(param_h);
				fsync(fd);
				if(locked) flock(fd, LOCK_UN);

				param_close(param_h);
				snprintf(cmd, sizeof(cmd), "/srv/sbin/del_account.sh %s \"\" /srv/http/conf/htdigest_Administrator", name);
				SYSTEM(cmd);
				snprintf(cmd, sizeof(cmd), "/srv/sbin/del_account.sh %s \"\" /srv/http/conf/htdigest_Operator+", name);
				SYSTEM(cmd);
				snprintf(cmd, sizeof(cmd), "/srv/sbin/del_account.sh %s \"\" /srv/http/conf/htdigest_User+", name);
				SYSTEM(cmd);
				snprintf(cmd, sizeof(cmd), "/srv/bin/nginx -s reload");
				SYSTEM(cmd);
				break;
			}
		}
	} else {
		if (errno != -ENOENT) return errno;
		if (cfg_create(cfg)==0 && _accounts_default()==0) goto START;
	}

	return ret;
}

s32 Tara_accounts_deleteUser(s8 *name)
{
	if (!name || strlen(name)<=0) return ERROR_INVALARG;
	s32 k=0;
	for (k=0; k<RESERVED_USER_NUM; k++) if (!strcmp(name, strruser[k])) return ERROR_UNSUPPORTED;
	return _accounts_delete(name, DEF_ACCOUNT_CFG);
}

s32 Tara_accounts_getlang(s8 *lang)
{
	s32 ret=ERROR_NONE;
	s8 sec_name[32];
	param_handle_t *param_h;
	strcpy(lang, DEF_LANG_VALUE);
START:
	param_h = param_open(DEF_ACCOUNT_CFG, "r");
	if (param_h) {
		s32 fd=param_h->fd;
		s32 locked=0;
		while (1) {
			if (flock(fd, LOCK_EX) == 0) {
				locked = 1;
				break;
			}

			usleep(10000);
		}

		strcpy(lang, DEF_LANG_VALUE);
		strcpy(sec_name, CFG_SECTION_GLOBAL);
		if (param_get_value(param_h, sec_name, CFG_FIELD_LANG)) {
			strcpy(lang, param_get_value(param_h, sec_name, CFG_FIELD_LANG));
		}

		if(locked) flock(fd, LOCK_UN);

		param_close(param_h);
	} else {
		if (errno != -ENOENT) return errno;
		if (cfg_create(DEF_ACCOUNT_CFG)==0 && _accounts_default()==0) goto START;
	}

	return ret;
}

s32 Tara_accounts_setlang(s8 *lang)
{
	if (!lang || strlen(lang)<=0) return ERROR_INVALARG;
	s32 ret=ERROR_NONE;
	param_handle_t *param_h;
START:
	param_h = param_open(DEF_ACCOUNT_CFG, "r+");
	if (param_h) {
		s8 sec_name[32];
		s8 item_name[32], item_value[64];
		s32 fd=param_h->fd;
		s32 locked=0;
		while (1) {
			if (flock(fd, LOCK_EX) == 0) {
				locked = 1;
				break;
			}

			usleep(10000);
		}

		strcpy(sec_name, CFG_SECTION_GLOBAL);
		strcpy(item_name, CFG_FIELD_LANG);
		snprintf(item_value, sizeof(item_value), "%s", lang);
		ret = param_set_value(param_h, sec_name, item_name, item_value);
		if (ret == -ENOENT) {  /* section not found */
			ret = param_insert_section(param_h, NULL, sec_name);
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		} else if (ret == -ENODATA) {  /* item not found */
			ret = param_insert_item(param_h, sec_name, NULL, item_name, item_value);
		}

		param_sync_to_file(param_h);
		fsync(fd);
		if(locked) flock(fd, LOCK_UN);

		param_close(param_h);
	} else {
		if (errno != -ENOENT) return errno;
		if (cfg_create(DEF_ACCOUNT_CFG)==0 && _accounts_default()==0) goto START;
	}

	return ERROR_NONE;
}
