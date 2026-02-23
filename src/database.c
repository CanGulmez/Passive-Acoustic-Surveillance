/**
 ******************************************************************************
 * @file 	database.c
 * @author 	Ahmet Can GULMEZ
 * @brief 	SQLite3 database management of passive acoustic surveillance.
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

#include "main.h"

/**
 * Open the SQLite3 database.
 */
struct sqlite3 *db_open(const char *filepath)
{
	int rc;
	struct sqlite3 *db;

	rc = sqlite3_open(filepath, &db);
	if (rc != SQLITE_OK)
		dbError(db);

	return db;
}

/**
 * Create a new table into the open database.
 */
void db_create_table(struct sqlite3 *db, Database database)
{
	int i, rc;
	char data[32];
	char sql[SQL_SIZE];

	/* Create the database table for sensor data. */
	if (database == DATABASE_SENSOR_DATA)
	{
		sprintf(sql, 
			"CREATE TABLE IF NOT EXISTS %s (ID INTEGER "
			"PRIMARY KEY AUTOINCREMENT,", DB_SENSOR_DATA_TABLE
		);
		/* Append the data columns from 1 to DATA_SIZE. */
		for (i = 1; i <= DATA_SIZE; i++)
		{
			memset(data, 0, 32);		/* initialize 0s */
			sprintf(data, "Data%d INTEGER,", i);
			strcat(sql, data);
		}
		/* Latly, insert the date and time timestamp. */
		strcat(sql, "Timestamp TEXT NOT NULL);");
	}
	rc = sqlite3_exec(db, sql, 0, 0, 0);
	if (rc != SQLITE_OK)
		dbError(db);
}

/**
 * Bind the appropriate data into the open database.
 */
void db_bind_data(struct sqlite3 *db, Database database)
{
	int i, rc;
	char data[16];
	char sql[SQL_SIZE];
	sqlite3_stmt *stmt;
	
	/* Bind the "micSensorData" structure into open database. */
	if (database == DATABASE_SENSOR_DATA)
	{
		/* Build the sql sequence. */
		sprintf(sql, "INSERT INTO %s (", DB_SENSOR_DATA_TABLE);
		for (i = 1; i <= DATA_SIZE; i++)
		{
			memset(data, 0, 16);		/* initialize with 0s */
			sprintf(data, "Data%d, ", i);
			strcat(sql, data);
		}
		strcat(sql, "Timestamp) VALUES (");
		for (i = 1; i <= DATA_SIZE; i++)
		{
			strcat(sql, "?, ");
		}
		strcat(sql, "?);");

		/* Bind the sql sequence into the open database. */
		rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
		if (rc != SQLITE_OK)
			dbError(db);

		for (i = 1; i <= DATA_SIZE; i++)
		{
			sqlite3_bind_int(stmt, i, sigBeamformed.data[i - 1]);
		}
		sqlite3_bind_text(stmt, DATA_SIZE + 1, 
			get_time(TIME_FORMAT), -1, SQLITE_STATIC);
	}
	printLog("recorded the sensor data into '%s'", DB_SENSOR_DATA_PATH);

	/* Step and finalize the current insertion. */
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
		dbError(db);
	
	rc = sqlite3_finalize(stmt);
	if (rc != SQLITE_OK)
		dbError(db);
}

/**
 * Query the appropriate data into the open database.
 */
void db_query_data(struct sqlite3 *db, Database database)
{
	int i, rc;
	sqlite3_stmt *stmt;
	char sql[SQL_SIZE];

	/* Get the binded data of "micSensorData". */
	if (database == DATABASE_SENSOR_DATA)
	{
		sprintf(sql, "SELECT * FROM %s", DB_SENSOR_DATA_TABLE);

		rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
		if (rc != SQLITE_OK)
			dbError(db);
			
		while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
		{
			printf("ID: %d, ", sqlite3_column_int(stmt, 0));
			for (i = 1; i <= DATA_SIZE; i++)
			{
				printf("Data%d: %d, ", i, sqlite3_column_int(stmt, i));
			}
			printf("Timestamp: %s\n", sqlite3_column_text(stmt, 513));
		}
	}
	/* Finalize the reading operations. */
	rc = sqlite3_finalize(stmt);
	if (rc != SQLITE_OK)
		dbError(db);
}

/**
 * Close the open database.
 */
void db_close(struct sqlite3 *db)
{
	int rc;

	rc = sqlite3_close(db);
	if (rc != SQLITE_OK)
		dbError(db);
}
