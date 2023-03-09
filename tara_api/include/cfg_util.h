#ifndef __PARAM_UTIL_H
#define __PARAM_UTIL_H

#include "myglobal.h"
#include "myerr.h"

#ifdef __cplusplus
extern "C" {
#endif

struct param_handle {
	s8 *filename;
	s32 fd;
	s32 writable;
	s32 dirty;
	s32 section_num;
	s32 update_flag;
	struct param_section *section_head;
	struct param_section *section_tail;
	struct param_section **sections;
};

typedef struct param_handle param_handle_t;

struct param_section {
	s8 *section_name;
	s32 item_num;
	struct param_item *item_head;
	struct param_item *item_tail;
	struct param_item **items;
	struct param_section *next_section;
	struct param_section *prev_section;
};

struct param_item {
	s8 *item_name;
	s8 *item_value;
	struct param_item *next_item;
	struct param_item *prev_item;
};

/*
 * filename : full path of config file
 * mode     : "r" or "r+"
 * return   : opened handle ( config file open sucessfully )
 *          : NULL ( config file open failed , errno is set )
 * errno    : [ -EINVAL ===> invalid mode  ]
 *            [ -ENOMEM ===> out of memory ]
 */
extern param_handle_t *param_open(s8 *filename, const s8 *mode);

/*
 * handle : opened handle
 */
extern s32 param_close(param_handle_t *handle);

/*
 * handle       :
 * section_name : 
 * item_name    :
 * return       : NON-NULL ( item_value )
 *              : NULL ( errno is set )
 * errno        : [ -ENOENT  ===> section_name not found ]
 *                [ -ENODATA ===> item_name not found    ]
 */
extern s8 *param_get_value(param_handle_t *handle, s8 *section_name, s8 *item_name);

/*
 * handle       :
 * section_name :
 * item_name    :
 * value        :
 * return       : 0 ( set item value successfully )
 *              : -EACCES  ===> file opened read only
 *              : -EINVAL  ===> item_name or value not provided
 *              : -ENOENT  ===> section_name not found
 *              : -ENODATA ===> item_name not found
 *              : -ENOMEM  ===> out of memory
 */
extern s32 param_set_value(param_handle_t *handle, s8 *section_name, s8 *item_name, s8 *value);

/*
 * handle           :
 * section_name     : 
 * new_section_name :
 * return           : 0 ( insert section successfully )
 *                  : -EACCES ===> file opened read only
 *                  : -EINVAL ===> new_section_name not provided
 *                  : -EEXIST ===> new_section_name already exists
 *                  : -ENOENT ===> section_name not found
 *                  : -ENOMEM ===> out of memory
 */
extern s32 param_insert_section(param_handle_t *handle, s8 *section_name, s8 *new_section_name);

/*
 * handle        :
 * section_name  :
 * item_name     :
 * new_item_name :
 * value         :
 * return        : 0 ( insert item sucessfully )
 *               : -EACCES ===> file opened read only
 *               : -EINVAL ===> section_name or new_item_name or value not provided
 *               : -ENOENT ===> section_name or item_name not found
 *               : -ENOMEM ===> out of memory
 */
extern s32 param_insert_item(param_handle_t *handle, s8 *section_name, s8 *item_name, s8 *new_item_name, s8 *value);

/*
 * handle       :
 * section_name :
 * return       : 0 ( delete section sucessfully )
 *              : -EACCES ===> file opened read only
 *              : -EINVAL ===> section_name not provided
 *              : -ENOENT ===> section_name not found
 *              : -ENOMEM ===> out of memory
 */
extern s32 param_del_section(param_handle_t *handle, s8 *section_name);

/*
 * handle       :
 * section_name :
 * item_name    :
 * return       : 0 ( delete item successfully )
 *              : -EACCES ===> file opened read only
 *              : -EINVAL ===> section_name not provided
 *              : -ENOENT ===> section_name not found
 *              : -ENOMEM ===> out of memory
 */
extern s32 param_del_item(param_handle_t *handle, s8 *section_name, s8 *item_name);

/*
 * handle :
 * return : 0 ( sync file successfully )
 *        : -1 ( sync file failed )
 *        : -EACCES ===> file opened read only
 *        : -ENOSPC ===> write failed
 */
extern s32 param_sync_to_file(param_handle_t *handle);

extern s32 cfg_create(s8 *cfg_name);

#ifdef __cplusplus
}
#endif
#endif
