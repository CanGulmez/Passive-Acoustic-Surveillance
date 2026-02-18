/**
 ******************************************************************************
 * @file 	utility.c
 * @author 	Ahmet Can GULMEZ
 * @brief 	Common utilities of AeroSONAR.
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
 * Write the system logs into required file.
 */
void logging(const char* buffer, size_t size)
{
	int fd;

	fd = open(SYSTEM_LOG_PATH, O_WRONLY | O_APPEND);
	if (fd == -1)
		syscallError();

	if (write(fd, buffer, size) == -1)	/* write the logs. */
		syscallError();
	
	/* Make sure that each log is explicitly written. */
	if (fsync(fd) == -1)
		syscallError();

	if (close(fd) == -1)
		syscallError();
}

/**
 * Get the current time to show with handlers.
 */
char *get_time(const char* format)
{
	static char buffer[64];
	time_t t;
	struct tm *tm;
	
	t = time(NULL);			/* get the time in seconds */
	tm = localtime(&t);		/* convert it into broken-down */
	if (tm == NULL)
		syscallError();

	/* Format the 'tm' struct into buffer. */
	strftime(buffer, 64, (format != NULL) ? format : "%c", tm);

	return buffer;
}

/**
 * Set the 'termios' structure according to selected attributes.
 */
void set_serial_attributes(int fd, struct termios *tty)
{
	if (tcgetattr(fd, tty) == -1)
		syscallError();
	
	cfsetispeed(tty, B115200);			/* Baud Rate - 115200 (most common) */
	cfsetospeed(tty, B115200);
	tty->c_cflag &= ~CSIZE;				/* Data Bits - 8 bits (most common) */
	tty->c_cflag |= CS8;
	tty->c_cflag &= ~PARENB;			/* Parity Bit - None (most common) */
	tty->c_cflag &= ~CSTOPB;			/* Stop bits - 1 bit (most common) */
   tty->c_cflag &= ~CRTSCTS;			/* Flow Control - None (most common) */
   tty->c_iflag &= ~(IXON | IXOFF | IXANY);
   tty->c_lflag &= ~(ICANON | ECHO | ECHOE | ECHONL | ISIG); 	/* Raw mode */
   tty->c_oflag &= ~OPOST;
   tty->c_cc[VTIME] = 10;  				
   tty->c_cc[VMIN] = 1;
   tty->c_cflag |= CREAD | CLOCAL;	/* Enable receiver */

   if (tcsetattr(fd, TCSANOW, tty) == -1)
		syscallError();
}

/**
 * Get the available microphone device nodes.
 */
int get_device_nodes(MicChannel channel)
{
	int index = 0;
	struct dirent *entry;
	const char *prefix;
	DIR *dir;

	/* Initialize with 0s. */
	memset(micDeviceNodes, 0, sizeof(micDeviceNodes));

	if (channel == MIC_CHANNEL_WIFI)
	{
		dir = opendir(MIC_WIFI_PATH);		/* open the Wi-Fi device folder */
	}
	else
	{
		dir = opendir(MIC_SERIAL_PATH);	/* open the serial device folder */
	}
	if ( dir == NULL )
		syscallError();

	if (channel == MIC_CHANNEL_UART)
	{
		prefix = MIC_UART_PREFIX;			/* get the UART prefix */
	}
	else if (channel == MIC_CHANNEL_USB)
	{
		prefix = MIC_USB_PREFIX;			/* get the USB prefix */
	}
	else
	{
		prefix = MIC_WIFI_PREFIX;			/* get the Wi-Fi prefix */
	}

	for (;;) 
	{
		entry = readdir(dir);				/* read each entity */
		if (entry == NULL)
		{
			break;
		}
		/* Continueing the look for ".", ".." */
		if (cmp(entry->d_name, ".") || cmp(entry->d_name, ".."))
		{
			continue;
		}
		if (strstr(entry->d_name, prefix) != NULL) 
		{
			micDeviceNodes[index] = strdup(entry->d_name);
			index++;
		}
	}
	if (closedir(dir) == -1)
		syscallError();
	
	return index;
}

/**
 * Open the microphone device node and then return the fd.
 */
int open_device_node(MicChannel channel, const char *node)
{
	int fd, pathSize = 0;
	char devicePath[32];
	struct termios tty;

	/* Create the device node path name correctly. */
	if (channel == MIC_CHANNEL_UART || channel == MIC_CHANNEL_USB) 
	{
		strcpy(devicePath, MIC_SERIAL_PATH);
		pathSize = strlen(MIC_SERIAL_PATH);
	} 
	else if (channel == MIC_CHANNEL_WIFI) 
	{
		strcpy(devicePath, MIC_WIFI_PATH);
		pathSize = strlen(MIC_WIFI_PATH);
	}
	strcat(devicePath, node);
	pathSize += strlen(node);
	devicePath[pathSize] = '\0';

	fd = open(devicePath, O_RDONLY);	/* read-only channel */	
	if (fd == -1)
	{
		syscallError();
	}
	else
	{
		printLog("opened the '%s' device node", devicePath);
	}
	/* Set serial terminal attributes. */
	set_serial_attributes(fd, &tty);

	return fd;
}

/**
 * Read the selected channel's device node.
 */
void read_device_node(int fd)
{
	ssize_t numRead, totalRead;
	uint8_t *payloadPtr;
	uint32_t magicNum;
	long payloadSize;

	/* Initialize with 0s. */
	memset(&payloadData, 0, sizeof(payloadData));

	/* Firstly, find the magic number to synchronize the stream. */
	while (TRUE)
	{
		numRead = read(fd, &magicNum, sizeof(magicNum));
		if (numRead == sizeof(magicNum))
		{
			if (magicNum == MAGIC_WORD)
			{
				break;
			}
		}
		else if (numRead == -1 && errno != EAGAIN)
			syscallError();
	}

	totalRead = 0;
	payloadPtr = (uint8_t *) &payloadData;
	payloadSize = sizeof(payloadData);
	/**
	 * Read the device node simulatenously. If the received data is 
	 * bigger than kernel buffer size (defaultly, 4096 bytes), loop 
	 * the read operations.
	 */
	while (totalRead < payloadSize)
	{
		numRead = read(fd, payloadPtr+totalRead, payloadSize-totalRead);
		if (numRead > 0) 
		{
			totalRead += numRead;
			printLog("read %ld bytes from the node", numRead);
		} 
		else if (numRead == -1) 
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK)
				syscallError(); 
				/* EAGAIN is normal (no data at this cycle) */
		}
	}
	printLog("read total %ld bytes from the associated node", totalRead);
}

/**
 * Read the appropriate model dataset.
 */
int get_model_datasets(void)
{
	int i, index = 0;
	struct dirent *entry;
	DIR *dir;
	char *filename;

	/* Initialize with 0s. */
	memset(modelDatasets, 0, sizeof(modelDatasets));

	dir = opendir(MODEL_DATASET_PATH);	/* open the directory */
	if (dir == NULL)
		syscallError();

	for (;;) 
	{
		entry = readdir(dir);				/* read its contents */
		if (entry == NULL)
		{
			break;
		}
		if (cmp(entry->d_name, ".") || cmp(entry->d_name, ".."))
		{
			continue;
		}
		/* Check the suffix of the datasets. */
		filename = entry->d_name;
		for (i = 0; i < strlen(entry->d_name)-4; i++)
		{
			filename++;
		}
		if (strstr(filename, MODEL_DATASET_SUFFIX)) 
		{
			modelDatasets[index] = strdup(entry->d_name);
			index++;
		}
	}
	if (closedir(dir) == -1)
		syscallError();

	return index;
}

/**
 * Run the Keras script and return the child process ID.
 */
int run_keras_script(const char *script)
{
	int childPid, logFd;
	char scriptFile[BUFFER_SIZE];
	char logFile[BUFFER_SIZE];
	char datasetPath[BUFFER_SIZE];
	char layerNumStr[8];
	char unitsStr[8];
	char epochsStr[8];
	char dropoutStr[8];
	const char *layerTypeStr = NULL;
	const char *batchSizeStr = NULL;
	const char *earlyStopStr = NULL;

	/* Build the required file paths. */
	snprintf(scriptFile, sizeof(scriptFile), "%s", script);
	snprintf(logFile, sizeof(logFile), "%s", MODEL_LOG_PATH);
	snprintf(datasetPath, sizeof(datasetPath), "%s%s", MODEL_DATASET_PATH,
				modelDataset);

	/* Convert enum values to strings. */
	switch (modelLayerType)								/* layer type */
	{
		case MODEL_LAYER_TYPE_LSTM: layerTypeStr = "LSTM";	break;
		case MODEL_LAYER_TYPE_GRU:  layerTypeStr = "GRU";  break;
	}
	snprintf(layerNumStr, sizeof(layerNumStr), "%u", modelLayerNumber);
   snprintf(unitsStr, sizeof(unitsStr), "%u", modelUnits);
   snprintf(epochsStr, sizeof(epochsStr), "%u", modelEpochs);
	switch (modelBatchSize) 							/* batch size */
	{
      case MODEL_BATCH_SIZE_16:   batchSizeStr = "16"; break;
      case MODEL_BATCH_SIZE_32:   batchSizeStr = "32"; break;
      case MODEL_BATCH_SIZE_64:   batchSizeStr = "64"; break;
      case MODEL_BATCH_SIZE_128:  batchSizeStr = "128"; break;
      case MODEL_BATCH_SIZE_256:  batchSizeStr = "256"; break;
      case MODEL_BATCH_SIZE_512:  batchSizeStr = "512"; break;
   }
	switch (modelEarlyStop) 							/* early stopping */
	{
      case MODEL_EARLY_STOP_FALSE: earlyStopStr = "0"; break;
      case MODEL_EARLY_STOP_TRUE:  earlyStopStr = "1"; break;
   }
	snprintf(dropoutStr, sizeof(dropoutStr), "%.2f", modelDropout);
	
	/* Create a new child-process and then execute the deep learning script. */
	switch (childPid = fork()) 
	{
		case -1: 	/* child-process couldn't be forked */
			syscallError();
		
		case 0: 
			logFd = open(logFile, O_RDWR | O_TRUNC);
			if (logFd == -1)
				syscallError();

			if ((dup2(logFd, STDOUT_FILENO) == -1) ||
				 (dup2(logFd, STDERR_FILENO) == -1))	/* duplicate the stdout */
				syscallError();

			if (close(logFd) == -1)
				syscallError();
			
			execl(
				INTERPRETER, 						/* python 3 interpreter */
				INTERPRETER, scriptFile, datasetPath, modelOutputName, 
				layerTypeStr, layerNumStr, unitsStr, epochsStr, batchSizeStr,
				earlyStopStr, dropoutStr, NULL
			);	
			_exit(EXIT_SUCCESS);					/* NEVER should come here */
		
		default:		/* let the parent-process runs the main program */
			break;
	}
	return childPid;
}
 
/**
 * Abort the sub-process that runs Keras model.
 */
void abort_keras_script(int childPid)
{
	if (kill(childPid, SIGKILL) == -1) 
		syscallError();
}

/**
 * Check the whether Keras model fitting finished.
 */
int is_keras_script_running(int childPid)
{
	int status;
	pid_t res;

	res = waitpid(childPid, &status, WNOHANG);
	if (res == 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/**
 * Get the Keras script outputs from the its log file.
 */
char *get_keras_script_logs(const char *logFile)
{
	int fd;
	struct stat st;
	static char buffer[MAX_BUFFER_SIZE];

	fd = open(logFile, O_RDONLY);	/* open for read-only */
	if (fd == -1)
		syscallError();

	/* Bother the "stat" structure to get size of logs. */
	if (fstat(fd, &st) == -1)
		syscallError();

	if (read(fd, buffer, st.st_size) == -1)
		syscallError();
	/* Make sure that the log results terminate. */
	buffer[st.st_size] = '\0';

	if (close(fd) == -1)
		syscallError();

	return buffer;
}
