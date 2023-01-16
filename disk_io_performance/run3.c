#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <string.h>
#include <sys/stat.h>

unsigned int xorbuf(unsigned int *buffer, int size)
{
	unsigned int result = 0;
	for (int i = 0; i < size; i++)
	{
    	result ^= buffer[i];
	}
	// printf("%u\n",result);
	return result;
}

double now()
{
  struct timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int file_read(const char *fileName, int blockSize)
{
    	int fd = open(fileName, O_RDONLY); // open the file
    	long int i,j,bytesRead;
    	long int totalBytesRead = 0;
    	unsigned int xorValue;
    	long int blockCount = 0;
    	double speed = 0;

    	if(fd == -1)
    	{
            	printf("Error: File '%s' couldn't be opened\n", fileName);
            	close(fd);
            	return -1;
    	}
    	else
    	{
            	struct stat st;
            	stat(fileName, &st);
            	off_t f_size = st.st_size;
            	blockCount = f_size / blockSize;
                if((f_size % blockSize) !=0)
                {
                    blockCount +=1;
                }
            	unsigned int *buffer = malloc(blockSize);
            	if(buffer == NULL)
            	{
                    	printf("Error: Malloc failed while allocating for buffer\n");
                    	exit(-1);
            	}
            	xorValue = 0;
            	double startTime = now();
            	for(i=0;i<blockCount;i++)
            	{
                    	bytesRead = read(fd, buffer, blockSize);
                    	xorValue ^= xorbuf(buffer, (bytesRead/4));
                    	totalBytesRead += bytesRead;
            	}
            	double endTime = now();
            	double timeDelta = (endTime - startTime);
            	close(fd);
            	double FileSize = totalBytesRead/1048576.0;
            	speed = FileSize/timeDelta;
            	printf("File size:\t%.3f\tMB\tXOR Value:\t%08x\tSpeed:\t%.5f\tMiB/s\nBlock Count:\t%ld\tBlock Size:\t%d\n", FileSize, xorValue, speed, blockCount, blockSize);
            	free(buffer);
            	return blockCount;
    	}
}

int main(int argc, char **argv)
{
    	if (argc != 3)
    	{
            	printf("Usage: ./run <fileName> <blockSize>\n");
            	return -1;
    	}
    	else
    	{
            	const char *fileName = argv[1];
            	int blockSize = atoi(argv[2]);

                if(blockSize <=0){
                    printf("block size should be positive\n");
                    return -1;
                }
            	int blockCount = file_read(fileName, blockSize);
            	return 0;
    	}
}
