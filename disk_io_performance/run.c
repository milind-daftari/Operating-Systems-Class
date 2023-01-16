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

    return result;
}

double now() 
{
  struct timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void file_read(const char *fileName, unsigned int blockSize, unsigned int blockCount)
{
	int fd = open(fileName, O_RDONLY);
	int i,j,bytesRead;
	long int totalBytesRead = 0;
	unsigned int xorValue =0;

	if(fd == -1)
	{
		printf("Error: File '%s' couldn't be opened\n", fileName);
		close(fd);
		exit(-1);
	}
	
	else
	{
		struct stat st;
		stat(fileName, &st);
		off_t fileSize = st.st_size;
		unsigned int readSize = blockCount * blockSize;

		if(fileSize < readSize){
			printf("File size: %ld Bytes, insufficient to read\n", fileSize);
		}
		else{
			unsigned int *buffer = malloc(blockSize);
			if(buffer == NULL)
			{
				printf("Error: Malloc failed while allocating for buffer");
				exit(-1);
			}

			double startTime = now();

			for (i = 0; i < blockCount; i++) 
			{
	    			int bytesRead = read(fd, buffer, blockSize);
		    		if (bytesRead == -1)
		    		{
	      				printf("Error: Unable to read");
	      				break;
		    		}
				else{
					xorValue ^= xorbuf(buffer, (bytesRead/4));
					totalBytesRead += bytesRead;
				}
	  		}

			double endTime = now();

			double timeDelta = (endTime - startTime);

	    	double speed = totalBytesRead / ((1024.0 * 1024.0) * timeDelta);

			printf("XOR Value: %08x, Performance: %.4f MiB/s\n", xorValue, speed);

			free(buffer);	
			close(fd);
		}
	}
}

void file_write(const char *fileName, unsigned int blockSize, unsigned int blockCount)
{
	int fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	long int i, fileSize =blockSize*blockCount;

	if(fd == -1)
	{
		printf("Error: File '%s' couldn't be opened\n", fileName);
		return;
	}
	else
	{
		char *buffer = malloc(blockSize);
		if(buffer == NULL){
			printf("Error: Malloc failed while allocating for buffer");
			return;
		}

		long int totalBytesWritten = 0;
		double startTime =0;

		for(i = 0; i < blockSize; i++){
			buffer[i] = 'A';
		}
			
		startTime = now();
		
		for(i = 0; i < blockCount; i++){
		    int bytesWritten = write(fd, buffer, blockSize);
		    if (bytesWritten == -1){
		      	printf("Error: Unable to write");
		      	return;
		    }
		    totalBytesWritten += bytesWritten;
	    }
    		
	    double endTime = now();

		float timeDelta = (endTime - startTime);

	    double speed = totalBytesWritten / ((1024.0 * 1024.0) * timeDelta);
	  
	    printf("Performance: %.5f MiB/s\n", speed);

	    free(buffer);
	}

    close(fd);
}

int main(int argc, char **argv) 
{
	if (argc != 5) 
	{
		printf("Usage: ./run <file name> [-r|-w] <block size> <block count>\n");
		return -1;
	}
	else
	{
		const char *fileName = argv[1];
		int blockSize = atoi(argv[3]);
		int blockCount = atoi(argv[4]);

		if(blockSize <=0 || blockCount <=0){
			printf("block size and block count should be positive\n");
			return -1;
		}
		
		if(argv[2][0]!='-' || strlen(argv[2])!=2){
			printf("Usage: ./run <file name> [-r|-w] <block size> <block count>\n");
			return -1;
		}

		switch(argv[2][1])
		{
			case 'r': file_read(fileName, blockSize, blockCount);
				break;
			case 'w': file_write(fileName, blockSize, blockCount);
				break;
			default: printf("Enter a correct operation: -r to read or -w to write");
				break;
		}

		return 0;
	}
}
