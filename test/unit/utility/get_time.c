/**
 ******************************************************************************
 * @file 	get_time.c
 * @author 	Ahmet Can GULMEZ
 * @brief 	Unit test for get_time() function.
 * 
 ******************************************************************************
 * @attention
 * 
 * Copyright (c) 2026 Ahmet Can GULMEZ.
 * All rights reserved.
 * 
 * This software is licensed under the MIT License.
 * 
 ******************************************************************************
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <check.h>

#define __file									__FILE__
#define __line									__LINE__
#define __func									__func__

#define syscall_error()																		\
{                                      												\
	fprintf(stderr, "\n*** %s (%s::%d in %s()) ***\n", strerror(errno),		\
			  __file, __line, __func);		   	 										\
	exit(EXIT_FAILURE);	/* exit with failure status */							\
} 

/**
 * Get the current time to show with signal handlers.
 */
char *get_time(const char* format)
{
	static char buffer[64];
	time_t t;
	size_t s;
	struct tm *tm;

	t = time(NULL);			/* get the time in seconds */
	tm = localtime(&t);		/* convert it into broken-down */
	if (tm == NULL)
		syscall_error();

	s = strftime(buffer, 64, (format != NULL) ? format : "%c", tm);

	return buffer;
}

START_TEST(get_time_default_format)
{
	printf("\n[TEST] Testing get_time() with default format...\n");

	char *res = get_time(NULL);

	ck_assert_ptr_nonnull(res);
	ck_assert_int_gt(strlen(res), 0);

	for (size_t i = 0; i < strlen(res); i++)
	{
		ck_assert(res[i] != '\0');
	}

	printf("Passed.\n");
}
END_TEST

START_TEST(get_time_custom_format)
{
	printf("\n[TEST] Testing get_time() with custom format...\n");	

	char *res = get_time("%Y-%m-%d");

	ck_assert_ptr_nonnull(res);
	ck_assert_int_gt(strlen(res), 10);

	for (size_t i = 0; i < strlen(res); i++)
	{
		ck_assert(res[i] != '\0');
	}

	printf("Passed.\n");
}

Suite *get_time_suite(void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("Time");
	tc_core = tcase_create("Core");

	tcase_add_test(tc_core, get_time_default_format);
	tcase_add_test(tc_core, get_time_custom_format);

	suite_add_tcase(s, tc_core);

	return s;
}

int main(int argc, char *argv[])
{
	int numFailed = 0;
	Suite *s;
	SRunner *sr;

	s = get_time_suite();
	sr = srunner_create(s);

	srunner_run_all(sr, CK_NORMAL);

	numFailed = srunner_ntests_failed(sr);

	srunner_free(sr);

	printf("numFailed = %d\n", numFailed);

	return EXIT_SUCCESS;
}
