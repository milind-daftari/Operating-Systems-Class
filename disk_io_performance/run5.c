#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <string.h>

double now()
{
  struct timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int file_read(const char *fileName, int blockSize, int blockCount)
{
    	int fd = open(fileName, O_RDONLY); // open the file
    	int i,j,bytesRead;
    	long int totalBytesRead = 0;
    	double speed = 0;

    	if (fd == -1)
    	{
            	printf("Error: File '%s' couldn't be opened\n", fileName);
            	close(fd);
            	return -1;
    	}
    	else
    	{
            	unsigned int *buffer = malloc(blockSize);
            	if(buffer == NULL)
            	{
                    	printf("Error: Malloc failed while allocating for buffer\n");
                    	exit(-1);
            	}
            	double startTime = now();
            	for(i=0;i<blockCount;i++)
            	{
                    	bytesRead = read(fd, buffer, blockSize);
                    	totalBytesRead += bytesRead;
            	}
            	double endTime = now();
            	double timeDelta = (endTime - startTime);

            	double fileSize = totalBytesRead;
            	speed = fileSize/timeDelta;
                printf("Read Function Performance\n");
            	printf("File size:\t%.3f\tB\tSpeed:\t%.5f\tB/s\nBlock Count:\t%d\tBlock Size:\t%d\n", fileSize, speed, blockCount, blockSize);
              totalBytesRead = 0;
            	startTime = now();
            	for(i=0;i<blockCount;i++)
            	{
                    	lseek(fd, blockCount, SEEK_SET);
                    	bytesRead = read(fd, buffer, blockSize);
                    	totalBytesRead += bytesRead;
            	}
            	endTime = now();
            	timeDelta = (endTime - startTime);
            	close (fd);
            	fileSize = totalBytesRead;
            	speed = fileSize/timeDelta;
                printf("\n\nLseek Function Performance\n");
            	printf("File size:\t%.3f\tB\tSpeed:\t%.5f\tB/s\nBlock Count:\t%d\tBlock Size:\t%d\n", fileSize, speed, blockCount, blockSize);
            	free(buffer);
            	return 0;
    	}
}

int main(int argc, char **argv)
{
    	if (argc != 4)
    	{
            	printf("Usage: ./run5 <fileName> <blockSize> <blockCount>\n");
            	return -1;
    	}
    	else
    	{
            	const char *fileName = argv[1];
            	int blockSize = atoi(argv[2]);
            	int blockCount = atoi(argv[3]);
                if(blockSize <=0 || blockCount <=0){
                    printf("block size and block count should be positive\n");
                    return -1;
                }
            	blockCount = file_read(fileName, blockSize, blockCount);
            	return 0;
    	}
}