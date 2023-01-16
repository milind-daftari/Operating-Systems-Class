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

int file_read(const char *fileName, unsigned int blockSize)
{
	int fd = open(fileName, O_RDONLY); // open the file
	int i,j,bytesRead;
	unsigned long totalBytesRead =0;
	unsigned int xorValue =0;
	unsigned int blockCount = 0;
	double speed = 0;
	long double fileSize =0;

	if(fd == -1)
	{
		printf("Error: File '%s' couldn't be opened\n", fileName);
		close(fd);
		return -1;
	}

	else
	{
		close(fd);
		unsigned int *buffer = malloc(blockSize);
		if(buffer == NULL)
		{
			printf("Error: Malloc failed while allocating for buffer");
			exit(-1);
		}
		else{
			struct stat st;
			stat(fileName, &st);
			off_t Filesize = st.st_size;
			unsigned int maxBlockCount = Filesize/blockSize;
			if((Filesize%blockSize)!=0){
				maxBlockCount += 1;
			}
			for(blockCount=1; blockCount<=maxBlockCount;)
			{
				xorValue = 0;
				totalBytesRead =0;
				fd = open(fileName, O_RDONLY);

				double startTime = now();
				for(i=0;i<blockCount;i++)
				{
					bytesRead = read(fd, buffer, blockSize);
					totalBytesRead += bytesRead;
					xorValue ^= xorbuf(buffer, (bytesRead/4));
				}
				
				double endTime = now();

				double timeDelta = (endTime - startTime);
				
				close(fd);
				//printf("test run time:%f s for block count:%d\n", timeDelta, blockCount);

				if((timeDelta > 5 && timeDelta < 15) || blockCount==maxBlockCount)
				{
					fileSize = totalBytesRead/(1024.0*1024.0);
					speed = fileSize / timeDelta;
					if(timeDelta < 5)
					{
						printf("Note: Need a bigger file to run read for more than 5s. Please update Block Size\n");
					}
					break;
				}
				
				if(timeDelta > 15)
				{
					blockCount = blockCount/6;
				}

				blockCount=blockCount*2;
				if(blockCount>maxBlockCount)
				{
					blockCount=maxBlockCount;
				}
			
			}
			printf("Block Count: %u \tXOR Value: %08x\tFile size: %.3Lf MB\tSpeed:\t%.5f\tMiB/s\n", blockCount, xorValue, fileSize, speed);
			free(buffer);
			return blockCount;
		}
	}
}

int main(int argc, char **argv) 
{
	if (argc != 3) 
	{
		printf("Usage: ./run <file name> <block size> \n");
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

		file_read(fileName, blockSize);

		return 0;
	}
}
