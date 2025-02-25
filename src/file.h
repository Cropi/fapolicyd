/*
 * file.h - Header file for file.c
 * Copyright (c) 2016,2018 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This software may be freely redistributed and/or modified under the
 * terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor
 * Boston, MA 02110-1335, USA.
 *
 * Authors:
 *   Steve Grubb <sgrubb@redhat.com>
 */

#ifndef FILE_HEADER
#define FILE_HEADER

#include <sys/types.h>
#include <stdint.h>

// Information we will cache to identify the same executable
struct file_info
{
	dev_t    device;
	ino_t    inode;
	mode_t   mode;
	off_t    size;
	struct timespec time;
};

void file_init(void);
void file_close(void);
struct file_info *stat_file_entry(int fd);
int compare_file_infos(const struct file_info *p1, const struct file_info *p2);
char *get_program_cwd_from_pid(pid_t pid, size_t blen, char *buf);
char *get_file_from_fd(int fd, pid_t pid, size_t blen, char *buf);
char *get_device_from_stat(unsigned int device, size_t blen, char *buf);
char *get_file_type_from_fd(int fd, size_t blen, char *buf);
int  check_packaged_from_file(const char *filename);
char *get_hash_from_fd(int fd);
uint32_t gather_elf(int fd, off_t size);

#endif
