/*
 * util.c - Useful utility functions
 *
 * Copyright (C) 2009-2010 Mathias Lafeldt <misfire@debugon.org>
 *
 * This file is part of PS2rd, the PS2 remote debugger.
 *
 * PS2rd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PS2rd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PS2rd.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <tamtypes.h>
#include <kernel.h>
#include <loadfile.h>
#include <iopcontrol.h>
#include <iopheap.h>
#include <sifrpc.h>
#include "mycdvd.h"
#include "mystring.h"
#include "myutil.h"
#include "dbgprintf.h"

u32 kmem_read(void *addr, void *buf, u32 size)
{
	DI();
	ee_kmode_enter();
	memcpy(buf, addr, size);
	ee_kmode_exit();
	EI();

	return size;
}

u32 kmem_write(void *addr, const void *buf, u32 size)
{
	DI();
	ee_kmode_enter();
	memcpy(addr, buf, size);
	ee_kmode_exit();
	EI();

	return size;
}

void flush_caches(void)
{
	FlushCache(0); /* Writeback data cache */
	FlushCache(2); /* Instruction cache */
}

void install_debug_handler(const void *handler)
{
	u32 *p = (u32*)0x80000100;

	DI();
	ee_kmode_enter();
	p[0] = MAKE_J(handler);
	p[1] = 0;
	ee_kmode_exit();
	EI();
	FlushCache(0);
}

void reset_iop(const char *img)
{
	D_PRINTF("%s: IOP says goodbye...\n", __FUNCTION__);

	SifInitRpc(0);

	/* Exit services */
	fioExit();
	SifExitIopHeap();
	SifLoadFileExit();
	SifExitRpc();

	SifIopReset(img, 0);

	while (!SifIopSync())
		;

	/* Init services */
	SifInitRpc(0);
	SifLoadFileInit();
	SifInitIopHeap();
	fioInit();

	flush_caches();
}

int load_modules(const char **modv)
{
	int i = 0, ret;

	if (modv == NULL)
		return -1;

	while (modv[i] != NULL) {
		ret = SifLoadModule(modv[i], 0, NULL);
		if (ret < 0) {
			D_PRINTF("%s: Failed to load module: %s (%i)\n",
				__FUNCTION__, modv[i], ret);
			return -1;
		}
		i++;
	}

	D_PRINTF("%s: All modules loaded.\n", __FUNCTION__);
	return 0;
}

int set_dir_name(char *filename)
{
	int i;

	if (filename == NULL)
		return -1;

	i = last_chr_idx(filename, '/');
	if (i < 0) {
		i = last_chr_idx(filename, '\\');
		if (i < 0) {
			i = chr_idx(filename, ':');
			if (i < 0)
				return -2;
		}
	}

	filename[i+1] = '\0';
	return 0;
}

char *get_base_name(const char *full, char *base)
{
	const char *p;
	int i, len;

	if (full == NULL || base == NULL)
		return NULL;

	i = last_chr_idx(full, '/');
	if (i < 0) {
		i = last_chr_idx(full, '\\');
		if (i < 0)
			i = chr_idx(full, ':');
	}
	p = &full[++i];
	len = last_chr_idx(p, ';');
	if (len < 0)
		len = strlen(p);
	memmove(base, p, len);
	base[len] = '\0';

	return base;
}

enum dev get_dev(const char *path)
{
	const char *prefix[] = {
		"cdrom0:",
		"host:",
		"mass:",
		"mc0:",
		"mc1:",
		NULL
	};
	int i = 0;

	while (prefix[i] != NULL) {
		if (!strncmp(path, prefix[i], strlen(prefix[i])))
			return i;
		i++;
	}

	return DEV_UNKN;
}

int file_exists(const char *filename)
{
	int fd;

	if (filename == NULL)
		return 0;

	fd = open(filename, O_RDONLY);

	if (fd < 0) {
		return 0;
	} else {
		close(fd);
		return 1;
	}
}

char *read_text_file(const char *filename, int maxsize)
{
	char *buf = NULL;
	int fd, filesize;

	if (filename == NULL)
		return NULL;

	/* Open file */
	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		D_PRINTF("%s: Can't open text file %s\n",
			__FUNCTION__, filename);
		return NULL;
	}

	/* Process file size */
	filesize = lseek(fd, 0, SEEK_END);
	if (maxsize && filesize > maxsize) {
		D_PRINTF("%s: Text file too large: %i bytes, max: %i bytes\n",
			__FUNCTION__, filesize, maxsize);
		goto end;
	}

	/* Allocate memory for file contents */
	buf = (char*)malloc(filesize + 1);
	if (buf == NULL) {
		D_PRINTF("%s: Unable to allocate %i bytes\n",
			__FUNCTION__, filesize + 1);
		goto end;
	}

	/* Read from text file into buffer */
	if (filesize > 0) {
		lseek(fd, 0, SEEK_SET);
		if (read(fd, buf, filesize) != filesize) {
			D_PRINTF("%s: Can't read from text file %s\n",
				__FUNCTION__, filename);
			goto end;
		}
	}

	/* NUL-terminate buffer */
	buf[filesize] = '\0';
end:
	close(fd);
	return buf;
}

int upload_file(const char *filename, u32 addr, int *size)
{
	static u8 buf[32 * 1024] __attribute__((aligned(64)));
	u8 *p = (u8*)addr;
	int fd, bytes_read;

	if (filename == NULL)
		return -1;

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		D_PRINTF("%s: Can't open file %s\n", __FUNCTION__, filename);
		return -2;
	}

	while ((bytes_read = read(fd, buf, sizeof(buf))) > 0) {
		memcpy(p, buf, bytes_read); /* or kmem_write() ... */
		p += bytes_read;
	}

	close(fd);

	if (size != NULL)
		*size = bytes_read;

	return 0;
}
