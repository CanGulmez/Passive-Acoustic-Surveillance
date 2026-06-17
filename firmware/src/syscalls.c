/**
 ******************************************************************************
 * @file 	syscalls.c
 * @author 	Can Gulmez
 * @brief 	Dummy syscall implementations.
 * 
 ******************************************************************************
 * @attention
 * 
 * Copyright (c) 2025 Can Gulmez.
 * All rights reserved.
 * 
 * This software is licensed under the MIT License.
 * 
 ******************************************************************************
 */

#include "main.h"

int _read(int fd, char *ptr, int len)
{
	(void)fd;
	(void)ptr;
	(void)len;
	
	return 0;
}

int _write(int fd, char *ptr, int len)
{
	(void)fd;
	(void)ptr;
	(void)len;

	return 0;
}

int _lseek(int fd, int ptr, int dir)
{
	(void)fd;
	(void)ptr;
	(void)dir;

	return 0;
}

int _close(int fd)
{
	(void)fd;

	return 0;
}

int _getpid(void)
{
	return 0;
}

int _kill(int fd)
{
	(void)fd;

	return 0;
}

int _fstat(int fd, struct stat *st)
{
	(void)fd;
	(void)st;

	return 0;
}

int _isatty(int fd)
{
	(void)fd;
	
	return 0;
}
