/*
 * log.cpp
 *
 *  Created on: Dec 29, 2011
 *      Author: james
 */

#include <cstdio>
#include <cstdarg>
#include <pthread.h>
#include "globals.h"

FILE *logfile;
pthread_mutex_t logfile_mutex;

void init_logfile() {
	logfile = fopen("logfile.txt","w");
}

void print(const char* format, ...) {
	pthread_mutex_lock(&logfile_mutex);

	va_list stdout_args;
	va_start(stdout_args,format);
	vprintf(format,stdout_args);
	fflush(stdout);
	va_end(stdout_args);

	va_list log_args;
	va_start(log_args,format);
	fprintf(logfile,"%d ",milli_timer());
	vfprintf(logfile,format,log_args);
	fflush(logfile);
	va_end(log_args);

	pthread_mutex_unlock(&logfile_mutex);
}

void error(const char* format, ...) {
	pthread_mutex_lock(&logfile_mutex);
	va_list args;
	va_start(args,format);
	vfprintf(stderr,format,args);
	va_end(args);
	pthread_mutex_unlock(&logfile_mutex);
}

