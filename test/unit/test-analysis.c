/**
 ******************************************************************************
 * @file 	test_analysis.c
 * @author 	Can GULMEZ
 * @brief 	Unit tests for signal analysis of passive acoustic surveillance.
 * 
 ******************************************************************************
 * @attention
 * 
 * Copyright (c) 2026 Can GULMEZ.
 * All rights reserved.
 * 
 * This software is licensed under the MIT License.
 * 
 ******************************************************************************
 */

#include "../../src/main.h"

PayloadData testPayload;

START_TEST(test_prepare_samples)
{
	int i;

	/* Setup payload data with known values */
	for (i = 0; i < DATA_SIZE; i++)
	{
		testPayload.micNorth[i] = (int8_t) (i % 128);
		testPayload.micEast[i] = (int8_t) (-i % 128);
	}

	/* Copy to global (normally set by read_mic_node()) */
	memcpy(&payloadData, &testPayload, sizeof(payloadData));

	/* Convert payload to samples */
	prepare_samples();
	
	/* Verify conversion */
	ck_assert_int_eq(sigSamples[0].length, DATA_SIZE);
	ck_assert_double_eq(sigSamples[0].data[0], 0.0);
	ck_assert_double_eq(sigSamples[0].data[1], 1.0);
	ck_assert_double_eq(sigSamples[2].data[0], 0.0);
	ck_assert_double_eq(sigSamples[2].data[1], -1.0);
}
END_TEST

START_TEST(test_select_sector_north)
{
	int i, j, sector;

	/* Make North microphone strongest */
	for (i = 0; i < MIC_COUNT; i++)
	{
		sigSamples[i].length = DATA_SIZE;
		for (j = 0; j < DATA_SIZE; j++)
		{
			sigSamples[i].data[j] = (i == 0) ? 100.0 : 10.0;
		}
	}

	/* ACT */
	sector = select_sector();

	/* Should return sector 1 (North) */
	ck_assert_int_eq(sector, 1);
}
END_TEST

START_TEST(test_select_sector_east)
{
	int i, j, sector;

	/* Make North microphone strongest */
	for (i = 0; i < MIC_COUNT; i++)
	{
		sigSamples[i].length = DATA_SIZE;
		for (j = 0; j < DATA_SIZE; j++)
		{
			sigSamples[i].data[j] = (i == 2) ? 100.0 : 10.0;
		}
	}

	/* ACT */
	sector = select_sector();

	/* Should return sector 3 (East) */
	ck_assert_int_eq(sector, 3);
}
END_TEST

START_TEST(test_accel_direction_x_plus)
{
	NavAccel accel;

	/* X acceleration dominant positive */
	payloadData.imuAccelX = 15.0;
	payloadData.imuAccelY = 1.0;
	payloadData.imuAccelZ = 5.0;

	/* ACT */
	accel = accel_direction();
	ck_assert_int_eq(accel, NAV_ACCEL_X_PLUS);
}
END_TEST

START_TEST(test_accel_direction_z_minus)
{
	NavAccel accel;
	/* Z acceleration dominant minus */
	payloadData.imuAccelX = 1.0;
	payloadData.imuAccelY = 5.0;
	payloadData.imuAccelZ = -15.0;

	/* ACT */
	accel = accel_direction();
	ck_assert_int_eq(accel, NAV_ACCEL_Z_MINUS);
}
END_TEST

START_TEST(test_accel_direction_undef)
{
	NavAccel accel;

	/* Undefined acceleration dominant */
	payloadData.imuAccelX = 5.0;
	payloadData.imuAccelY = 5.0;
	payloadData.imuAccelZ = 5.0;

	/* ACT */
	accel = accel_direction();
	ck_assert_int_eq(accel, NAV_ACCEL_UNDEF);
}
END_TEST

START_TEST(test_gyro_rotation_y_plus)
{
	NavGyro gyro;

	/* Y rotation dominant positive */
	payloadData.imuGyroX = 1.0;
	payloadData.imuGyroY = 50.0;
	payloadData.imuGyroZ = 2.0;

	/* ACT */
	gyro = gyro_rotation();
	ck_assert_int_eq(gyro, NAV_GYRO_Y_PLUS);
}
END_TEST

/*****************************************************************************/
/*****************************************************************************/

Suite *analysis_suite(void)
{
	Suite *s;
	TCase *tc_core;
	TCase *tc_edge;

	s = suite_create("Analysis");

	/* Core functionality tests */
	tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_prepare_samples);
	tcase_add_test(tc_core, test_select_sector_north);
	tcase_add_test(tc_core, test_select_sector_east);
	tcase_add_test(tc_core, test_accel_direction_x_plus);
	tcase_add_test(tc_core, test_accel_direction_z_minus);
	tcase_add_test(tc_core, test_gyro_rotation_y_plus);
	suite_add_tcase(s, tc_core);

	/* Edge case tests */
	tc_edge = tcase_create("EdgeCases");
	tcase_add_test(tc_edge, test_accel_direction_undef);
	suite_add_tcase(s, tc_edge);

	return s;
}

int main(void)
{
	int numFailed;
	SRunner *sr;

	/* Create test suite */
	sr = srunner_create(analysis_suite());
	srunner_add_suite(sr, analysis_suite());

	/* Configure output */
	srunner_set_log(sr, "test-analysis.log");

	/* Run all tests with forking (isolation) */
	srunner_run_all(sr, CK_VERBOSE);
	numFailed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (numFailed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
 
