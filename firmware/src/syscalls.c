/**
 ******************************************************************************
 * @file 	syscalls.c
 * @author 	Can GULMEZ
 * @brief 	Dummy syscall implementations.
 * 
 ******************************************************************************
 * @attention
 * 
 * Copyright (c) 2025 Can GULMEZ.
 * All rights reserved.
 * 
 * This software is licensed under the MIT License.
 * 
 ******************************************************************************
 */

#include "main.h"

int _read(int fd, char *ptr, int len)
{
	return 0;
}

int _write(int fd, char *ptr, int len)
{
	return 0;
}

int _lseek(int fd, int ptr, int dir)
{
	return 0;
}

int _close(int fd)
{
	return 0;
}

int _getpid(void)
{
	return 0;
}

int _kill(int fd)
{
	return 0;
}

int _fstat(int fd, struct stat *st)
{
	return 0;
}

int _isatty(int fd)
{
	return 0;
}

