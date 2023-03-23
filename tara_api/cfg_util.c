#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "cfg_util.h"

s8 EQU            = '=';
s8 L_BRACKET      = '[';
s8 R_BRACKET      = ']';
s8 TAB            = '\t';
s8 NL             = '\n';

s32 param_close(param_handle_t *handle)
{
	struct param_section *section, *section2;
	struct param_item *item, *item2;
	if (handle) {
		section = handle->section_head;
		if (section) {
			do {
				item = section->item_head;
				if (item) {
					do {
						item2 = item;
						item = item->next_item;
						if (item2->item_name) free(item2->item_name);
						if (item2->item_value) free(item2->item_value);

						free(item2);
					} while (item2 != section->item_tail);
				}

				section2 = section;
				section = section->next_section;
				if (section2->items) free(section2->items);
				if (section2->section_name) free(section2->section_name);

				free(section2);
			} while (section2 != handle->section_tail);
		}

		if (handle->sections) free(handle->sections);

		close(handle->fd);
		free(handle->filename);
		free(handle);
	}

	return 0;
}

void param_sort_names(void **s, s32 size)
{
	s32 i, j;
	s8 **name[2];
	void *p;
	for (i=0; i<(size-1); i++) {
		for (j=0; j<(size-1-i); j++) {
			name[0] = (s8 **)s[j];
			name[1] = (s8 **)s[j+1];
			if (strcmp(*name[1], *name[0]) < 0) {
				p = s[j];
				s[j] = s[j+1];
				s[j+1]=p;
			}
		}
	}
}

s32 param_search_name(void **s, s32 size, s8 *name)
{
	s32 m, err, l=0, u=size-1;
	s8 *the_name;
	while (l <= u) {
		m = (l+u) >> 1;
		the_name = *((s8 **)s[m]);
		err = strcmp(the_name, name);
		if (err < 0) l = m + 1;
		else if (!err) return m;
		else u = m - 1;

	}

	return -1;
}

s32 readline(s32 fd, s8 *buffer, s32 size)
{
	s32 count, left, i=0;
	s8 *ptr;
	if (size < 2) return 0;

	size -= 1;
	for (;;) {
		count = read(fd, &buffer[i], size);
		if (count == size) {
			i += count;
			break;
		}

		if (count > 0) {
			if (strchr(&buffer[i], '\n')) {
				i += count;
				break;
			} else {
				size -= count;
				i += count;
			}
		} else if (count == 0) break;
		else {
			buffer[0] ='\0';
			return 0;
		}
	}

	if (i == 0) return 0;

	ptr = strchr(buffer, '\n');
	if (ptr) {
		*ptr = '\0';
		left = (i - 1) - (s32)(ptr - buffer);
		lseek(fd, -left, SEEK_CUR);
	} else buffer[i] = '\0';

	return 1;
}

param_handle_t *param_open(s8 *filename, const s8 *mode)
{
	param_handle_t *handle;
	s32 fd;
	s8 buffer[512], buffer2[512];
	s8 *tmp_str, *tmp_str2;
	s32 i, j;
	struct param_section *section;
	struct param_item *item=NULL;
	errno = 0;
	if (strcmp(mode, "r") && strcmp(mode, "r+")) {
		errno = -EINVAL;
		return NULL;
	}

	handle = (param_handle_t *)malloc(sizeof(param_handle_t));
	if (!handle) {
		errno = -ENOMEM;
		return NULL;
	}

	fd = (mode[1]=='+') ? open(filename, O_RDWR) : open(filename, O_RDONLY);
	if (fd == -1) {
		free(handle);
		errno = -errno;
		return NULL;
	}

	memset(handle, 0, sizeof(param_handle_t));
	handle->filename = strdup(filename);
	handle->fd = fd;
	if (mode[1] == '+') handle->writable = 1;

	if (!handle->filename) {
		param_close(handle);
		errno = -ENOMEM;
		return NULL;
	}

	section = NULL;
	handle->section_head = NULL;
	handle->section_tail = NULL;
	while (readline(fd, buffer, 511)) {
		tmp_str = buffer;
		while (tmp_str[0] != '\n') {
			if (tmp_str[0] != ' ' && tmp_str[0] != '\t') break;
			tmp_str++;
		}

		if (!tmp_str) continue;

		i = strlen(tmp_str);
		if ((tmp_str[0]=='[') && (tmp_str[i-1]==']')) {
			if (section) {
				if (section->item_head) section->item_head->prev_item = section->item_tail;

				section->next_section = (struct param_section *) malloc(sizeof(struct param_section));
				if (!section->next_section) {
					section->next_section = handle->section_head;
					errno = -ENOMEM;
					break;
				}

				section->next_section->prev_section = section;
				section = section->next_section;
				section->next_section = handle->section_head;
				section->items = NULL;
			} else {
				section = (struct param_section *)malloc(sizeof(struct param_section));
				if (!section) {
					errno = -ENOMEM;
					break;
				}

				handle->section_head = section;
				section->next_section = section;
				section->items = NULL;
			}

			++handle->section_num;
			section->section_name = strdup(strtok(buffer, "\t\n []"));
			section->item_num = 0;
			section->item_head = NULL;
			section->item_tail = NULL;
			handle->section_tail = section;
			if (!section->section_name) {
				errno = -ENOMEM;
				break;
			}
		} else if ((tmp_str2=strchr(tmp_str, '='))) {
			if (!section) {
				section = (struct param_section *)malloc(sizeof(struct param_section));
				if (!section) {
					errno = -ENOMEM;
					break;
				}

				++handle->section_num;
				handle->section_head = section;
				section->items = NULL;
				section->section_name = strdup("");
				section->item_num = 0;
				section->item_head = NULL;
				section->item_tail = NULL;
				handle->section_tail = section;
				if (!section->section_name) {
					errno = -ENOMEM;
					break;
				}
			}

			strcpy(buffer2, tmp_str);
			buffer2[tmp_str2-tmp_str] = 0;
			tmp_str2 = &buffer2[tmp_str2-tmp_str+1];
			if (section->item_head) {
				item->next_item = (struct param_item *)malloc(sizeof(struct param_item));
				if (!item->next_item) {
					item->next_item = section->item_head;
					errno = -ENOMEM;
					break;
				}

				item->next_item->prev_item = item;
				item = item->next_item;
				item->next_item = section->item_head;
			} else {
				item = (struct param_item *)malloc(sizeof(struct param_item));
				if (!item) {
					errno = -ENOMEM;
					break;
				}

				section->item_head = item;
				item->next_item = item;
			}

			++section->item_num;
			item->item_name = strdup(buffer2);
			item->item_value= strdup(tmp_str2);
			section->item_tail = item;
			if ((!item->item_name) || (!item->item_value)) {
				errno = -ENOMEM;
				break;
			}
		}
	}

	if (section && (section->item_head)) section->item_head->prev_item = section->item_tail;	
	if (handle->section_head) handle->section_head->prev_section = handle->section_tail;
	if (errno < 0) {
		param_close(handle);
		return NULL;
	}

	handle->sections = (struct param_section **)malloc(sizeof(struct param_section *)*handle->section_num);
	if (!handle->sections) {
		param_close(handle);
		errno = -ENOMEM;
		return NULL;
	}

	section = handle->section_head;
	for (i=0; i<handle->section_num; i++) {
		handle->sections[i] = section;
		section->items = (struct param_item **)malloc(sizeof(struct param_item *)*section->item_num);
		item = section->item_head;
		for (j=0; j<section->item_num; j++) {
			section->items[j] = item;
			item = item->next_item;
		}

		section = section->next_section;
	}

	param_sort_names((void **)handle->sections, handle->section_num);
	for (i=0; i<handle->section_num; i++) {
		param_sort_names((void **)handle->sections[i]->items, handle->sections[i]->item_num);
	}

	return handle;
}

s8 *param_get_value(param_handle_t *handle, s8 *section_name, s8 *item_name)
{
	s32 i, j;
	i = param_search_name((void **)handle->sections, handle->section_num, section_name);

	if (i < 0) {
		errno = -ENOENT;
		return NULL;
	}

	j = param_search_name((void **)handle->sections[i]->items, handle->sections[i]->item_num, item_name);
	if (j < 0) {
		errno = -ENODATA;
		return NULL;
	}

	return handle->sections[i]->items[j]->item_value;
}

s32 param_insert_section(param_handle_t *handle, s8 *section_name, s8 *new_section_name)
{
	s32 i;
	struct param_section *new_section, *section;
	struct param_section **sections;
	if (!handle->writable) {
		errno = -EACCES;
		return errno;
	}

	if (!new_section_name) {
		errno = -EINVAL;
		return errno;
	}

	i = param_search_name((void **)handle->sections, handle->section_num, new_section_name);
	if (i >= 0) {
		errno = -EEXIST;
		return errno;
	}

	if (section_name) {
		i = param_search_name((void **)handle->sections, handle->section_num, section_name);
		if (i < 0) {
			errno = -ENOENT;
			return errno;
		}

		section = handle->sections[i];
	} else section = NULL;

	new_section = (struct param_section *)malloc(sizeof(struct param_section));
	if (!new_section) {
		errno = -ENOMEM;
		return errno;
	}

	new_section->section_name = strdup(new_section_name);
	if (!new_section->section_name) {
		free(new_section);
		errno = -ENOMEM;
		return errno;
	}

	sections = (struct param_section **)malloc(sizeof(struct param_section *)*(handle->section_num+1));
	if (!sections) {
		free(new_section->section_name);
		free(new_section);
		errno = -ENOMEM;
		return errno;
	}

	new_section->item_num = 0;
	new_section->item_head = NULL;
	new_section->item_tail = NULL;
	new_section->items = NULL;
	if (section) {
		new_section->prev_section = section->prev_section;
		new_section->next_section = section;
		section->prev_section->next_section = new_section;
		section->prev_section = new_section;
		if (section == handle->section_head) handle->section_head = new_section;
	} else {
		if (handle->section_head) {
			handle->section_tail->next_section = new_section;
			new_section->prev_section = handle->section_tail;
			new_section->next_section = handle->section_head;
			handle->section_head->prev_section = new_section;
			handle->section_tail = new_section;
		} else {
			new_section->prev_section = new_section;
			new_section->next_section = new_section;
			handle->section_head = new_section;
			handle->section_tail = new_section;
		}
	}

	if (handle->sections) free(handle->sections);

	handle->sections = sections;
	handle->section_num++;
	section = handle->section_head;
	for (i=0; i<handle->section_num; i++) {
		handle->sections[i] = section;
		section = section->next_section;
	}

	param_sort_names((void **)handle->sections, handle->section_num);
	handle->dirty = 1;
	return 0;
}

s32 param_insert_item(param_handle_t *handle, s8 *section_name, s8 *item_name, s8 *new_item_name, s8 *value)
{
	s32 i, j;
	struct param_section *section;
	struct param_item	*item, *new_item;
	struct param_item	**items;
	s8 *str1, *str2;

	if (!handle->writable) {
		errno = -EACCES;
		return errno;
	}

	if (!section_name || !new_item_name || !value) {
		errno = -EINVAL;
		return errno;
	}

	i = param_search_name((void **)handle->sections, handle->section_num, section_name);
	if (i < 0) {
		errno = -ENOENT;
		return errno;
	}

	section = handle->sections[i];
	if (item_name) {
		j = param_search_name((void **)section->items, section->item_num, item_name);
		if (j < 0) {
			errno = -ENOENT;
			return errno;
		}

		item = section->items[j];
	} else item = NULL;

	new_item = (struct param_item *)malloc(sizeof(struct param_item));
	if (!new_item) {
		errno = -ENOMEM;
		return errno;
	}

	items = (struct param_item **)malloc((section->item_num+1)*sizeof(struct param_item *));
	if (!items) {
		free(new_item);
		errno = -ENOMEM;
		return errno;
	}

	str1 = strdup(new_item_name);
	if (!str1) {
		free(new_item);
		free(items);
		errno = -ENOMEM;
		return errno;
	}

	str2 = strdup(value);
	if (!str2) {
		free(str1);
		free(new_item);
		free(items);
		errno = -ENOMEM;
		return errno;
	}

	new_item->item_name = str1;
	new_item->item_value= str2;
	if (item) {
		new_item->next_item = item;
		new_item->prev_item = item->prev_item;
		item->prev_item->next_item = new_item;
		item->prev_item = new_item;
		if (section->item_head == item) section->item_head = new_item;
	} else {
		if (section->item_head) {
			new_item->next_item = section->item_head;
			new_item->prev_item = section->item_tail;
			section->item_head->prev_item = new_item;
			section->item_tail->next_item = new_item;
			section->item_tail = new_item;
		} else {
			new_item->next_item = new_item;
			new_item->prev_item = new_item;
			section->item_head  = new_item;
			section->item_tail  = new_item;
		}
	}

	if (section->items) free(section->items);

	section->items = items;
	section->item_num++;
	item = section->item_head;
	for (i=0; i<section->item_num; i++) {
		section->items[i] = item;
		item = item->next_item;
	}

	param_sort_names((void **)section->items, section->item_num);
	handle->dirty = 1;
	return 0;
}

s32 param_del_section(param_handle_t *handle, s8 *section_name)
{
	s32 i, j;
	struct param_section *section;
	struct param_item *item, *item2;
	struct param_section **sections;
	if (!handle->writable) {
		errno = -EACCES;
		return errno;
	}

	if (!section_name) {
		errno = -EINVAL;
		return errno;
	}

	i = param_search_name((void **)handle->sections, handle->section_num, section_name);
	if (i < 0) {
		errno = -ENOENT;
		return errno;
	}

	sections = NULL;
	if (handle->section_num > 1) {
		sections = (struct param_section **)malloc(sizeof(struct param_section *) * (handle->section_num - 1));
		if (!sections) {
			errno = -ENOMEM;
			return errno;
		}
	}

	section = handle->sections[i];
	if (section != section->next_section) {
		section->prev_section->next_section = section->next_section;
		section->next_section->prev_section = section->prev_section;
		if (section == handle->section_head) handle->section_head = section->next_section;
		else if (section == handle->section_tail) handle->section_tail = section->prev_section;
	} else {
		handle->section_head = NULL;
		handle->section_tail = NULL;
	}

	item = section->item_head;
	for (j=0; j<section->item_num; j++) {
		item2 = item;
		item = item->next_item;
		if (item2->item_name) free(item2->item_name);
		if (item2->item_value) free(item2->item_value);

		free(item2);
	}

	if (section->items) free(section->items);
	if (section->section_name) free(section->section_name);

	free(section);
	free(handle->sections);
	handle->sections = sections;
	handle->section_num--;
	section = handle->section_head;
	for (i=0; i<handle->section_num; i++) {
		handle->sections[i] = section;
		section = section->next_section;
	}

	param_sort_names((void **)handle->sections, handle->section_num);
	handle->dirty = 1;
	return 0;
}

s32 param_del_item(param_handle_t *handle, s8 *section_name, s8 *item_name)
{
	s32 i, j;
	struct param_section *section;
	struct param_item *item;
	struct param_item **items;
	if (!handle->writable) {
		errno = -EACCES;
		return errno;
	}

	if (!section_name || !item_name) {
		errno = -EINVAL;
		return errno;
	}

	i = param_search_name((void **)handle->sections, handle->section_num, section_name);
	if (i < 0) {
		errno = -ENOENT;
		return errno;
	}

	section = handle->sections[i];
	j = param_search_name((void **)section->items, section->item_num, item_name);
	if (j < 0) {
		errno = -ENOENT;
		return errno;
	}

	item = section->items[j];
	items = NULL;
	if (section->item_num > 1) {
		items = (struct param_item **)malloc(sizeof(struct param_item *) * (section->item_num - 1));
		if (!items) {
			errno = -ENOMEM;
			return errno;
		}
	}

	if (item != item->next_item) {
		item->prev_item->next_item = item->next_item;
		item->next_item->prev_item = item->prev_item;
		if (item == section->item_head) section->item_head = item->next_item;
		else if (item == section->item_tail) section->item_tail = item->prev_item;
	} else {
		section->item_head = NULL;
		section->item_tail = NULL;
	}

	section->item_num--;
	free(item->item_name);
	free(item->item_value);
	free(item);
	free(section->items);
	section->items = items;
	section->item_num--;
	item = section->item_head;
	for (i=0; i<section->item_num; i++) {
		section->items[i] = item;
		item = item->next_item;
	}

	param_sort_names((void **)section->items, section->item_num);
	handle->dirty = 1;
	return 0;
}

s32 param_set_value(param_handle_t *handle, s8 *section_name, s8 *item_name, s8 *value)
{
	s32 i, j;
	struct param_section *section;
	struct param_item *item;
	s8 *str;
	if (!handle->writable) {
		errno = -EACCES;
		return errno;
	}

	if (!item_name || !value) {
		errno = -EINVAL;
		return errno;
	}

	i = param_search_name((void **)handle->sections, handle->section_num, section_name);
	if (i < 0) {
		errno = -ENOENT;
		return errno;
	}

	section = handle->sections[i];
	j = param_search_name((void **)section->items, section->item_num, item_name);
	if (j < 0) {
		errno = -ENODATA;
		return errno;
	}

	item = section->items[j];
	str = strdup(value);
	if (!str) {
		errno = -ENOMEM;
		return errno;
	}

	free(item->item_value);
	item->item_value = str;
	handle->dirty = 1;
	return 0;
}

s32 param_sync_to_file(param_handle_t *handle)
{
	s32 i, j, len1, len2, fd;
	struct param_section *section;
	struct param_item *item;
	s64 size;

	if (!handle->writable) {
		errno = -EACCES;
		return errno;
	}

	if (!handle->dirty) return 0;

	fd = handle->fd;
	if (lseek(fd, 0, SEEK_SET) == -1) return -errno;

	section = handle->section_head;
	for (i=0; i<handle->section_num; i++) {
		len1 = strlen(section->section_name);
		errno = (write(fd, &L_BRACKET, 1) != 1)                  ? -ENOSPC
		      : (write(fd, section->section_name, len1) != len1) ? -ENOSPC
		      : (write(fd, &R_BRACKET, 1) != 1)                  ? -ENOSPC
		      : (write(fd, &NL, 1) != 1)                         ? -ENOSPC
		      : 0;
		if (errno < 0) return errno;

		item = section->item_head;
		for (j=0; j<section->item_num; j++) {
			len1 = strlen(item->item_name);
			len2 = strlen(item->item_value);
			errno = (write(fd, &TAB, 1) != 1)                    ? -ENOSPC
			      : (write(fd, item->item_name, len1) != len1)   ? -ENOSPC
			      : (write(fd, &EQU, 1) != 1)                    ? -ENOSPC
			      : (write(fd, item->item_value, len2) != len2)  ? -ENOSPC
			      : (write(fd, &NL, 1) != 1)                     ? -ENOSPC
			      : 0;
			if (errno < 0) return errno;

			item = item->next_item;
		}

		if (write(fd, &NL, 1) != 1) return -ENOSPC;

		section = section->next_section;
	}

	errno = ((size = lseek(fd, 0, SEEK_CUR)) == -1) ? -errno
	      : (fsync(fd) == -1)                       ? -errno
	      : (ftruncate(fd, size) == -1)             ? -errno
	      : 0;
	handle->dirty = 0;
	return errno;
}

s32 cfg_create(s8 *cfg_name)
{
	s32 fd=open(cfg_name, O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH );
	if (fd == -1) return -errno;

	close(fd);
	return 0;
}
