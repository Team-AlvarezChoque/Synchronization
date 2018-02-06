#ifndef UTILS_H
#define UTILS_H

int toString(char a[]) {
	int c, sign, offset, n;

	if (a[0] == '-') {  // Handle negative integers
	sign = -1;
	}

	if (sign == -1) {  // Set starting position to convert
	offset = 1;
	}
	else {
	offset = 0;
	}

	n = 0;

	for (c = offset; a[c] != '\0'; c++) {
	n = n * 10 + a[c] - '0';
	}

	if (sign == -1) {
	n = -n;
	}

	return n;
}

void getHour(char * output){

	char hour[2];
	char minute[2];
	char seconds[2];

	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	if(timeinfo->tm_hour < 10){
			sprintf(hour,"0%d",timeinfo->tm_hour);
	}
	else{
			sprintf(hour,"%d",timeinfo->tm_hour);
	}

	if(timeinfo->tm_min < 10){
			sprintf(minute,"0%d",timeinfo->tm_min);
	}
	else{
			sprintf(minute,"%d",timeinfo->tm_min);
	}


	if(timeinfo->tm_sec < 10){
			sprintf(seconds,"0%d",timeinfo->tm_sec);
	}
	else{
			sprintf(seconds,"%d",timeinfo->tm_sec);
	}
	
	sprintf(output,"%s:%s:%s",hour, minute, seconds);
	//printf("%s\n",output);
}

void cleanFile(char * file){
	FILE *log;
	log = fopen(file, "w");
	fprintf(log,"");
	fclose(log);
}

void writeFile(char * file, char * data){
	char * back;
	asprintf(&back, "%s",data);

	FILE *log;
	log = fopen(file, "a");
	fprintf(log,"%s", back);
	fclose(log);
}

void writeProccess(unsigned long PID, char * data){

	char * back;
	asprintf(&back, "%s",data);

	char * ffile;
	asprintf(&ffile, ".%u",PID);

	FILE *pFile;
	pFile = fopen(ffile, "w"); // Se debe sobre escribir.
	fprintf(pFile,"%s", back);
	fclose(pFile);
}

void removeFile(unsigned long PID){
	char * ffile;
	asprintf(&ffile, ".%u",PID);
	remove(ffile);
}

int file_exist (char *filename)
{
	struct stat   buffer;
	return (stat (filename, &buffer) == 0);
}

#endif // UTILS_H