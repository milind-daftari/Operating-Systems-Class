#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <sys/stat.h>

typedef struct {
	int fd;
  int blockSize;
	unsigned int offset;
  unsigned int *buffer;
  int maxBlockCount;
  unsigned int xor_val;
} readPara;

unsigned int XOR_VALUE = 0;

double now() {
  struct timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec + tv.tv_usec / 1000000.0;
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *read_file(void *arg) {
  
  readPara *input = (readPara *) arg;
  unsigned int num_read =0;
  unsigned int offset = input->offset;
  //printf("blocksize: %d, offset: %u, MBC: %d, xor: %08x\n", input->blockSize, input->offset, input->maxBlockCount, input->xor_val);
  for(int i =0; i<input->maxBlockCount; i++) {
    num_read = pread(input->fd, input->buffer, input->blockSize, offset);
    for (int i = 0; i < (num_read/4); i++) 
    {
        input->xor_val ^= input->buffer[i];
    }
    offset += input->blockSize;
  }

  //printf("blocksize: %d, offset: %u, MBC: %d, xor: %08x\n", input->blockSize, offset, input->maxBlockCount, input->xor_val);

  pthread_exit(NULL);
}

int main(int argc, char **argv) {
  if(argc < 2){
    printf("Need more variables of the format\n");
  }
  const char *fileName = argv[1];
  int threadCount = 16;
  int blockSize = 262144;

  struct stat st;
	stat(fileName, &st);
	off_t f_size = st.st_size;

  //printf("thread count: %d, file name: %s, block size: %d, file size: %u\n", threadCount, fileName, blockSize, f_size);
  pthread_t threads[threadCount];
  readPara threadInputs[threadCount];
  unsigned int offsetBlock = 0;
  
  int maxBlockCount = f_size/blockSize;
  if(f_size%blockSize!=0){
    maxBlockCount +=1;
  }
  int threadMBC = maxBlockCount/threadCount;
  int threadRBC = maxBlockCount%threadCount;
    int fd = open(fileName, O_RDONLY);
    XOR_VALUE =0;
    for (int i = 0; i < threadCount; i++){
      threadInputs[i].offset = i*threadMBC*blockSize;
      threadInputs[i].blockSize = blockSize;
      threadInputs[i].buffer = (unsigned int *)malloc(blockSize);
      threadInputs[i].fd = fd;
      threadInputs[i].xor_val = 0;
      if(i==(threadCount-1)){
        threadInputs[i].maxBlockCount = threadMBC + threadRBC;
      }
      else{
        threadInputs[i].maxBlockCount = threadMBC;
      }
    }

    double start_time = now();
    for (int i = 0; i < threadCount; i++) {
      pthread_create(&threads[i], NULL, read_file, (void *)&threadInputs[i]);
    }

    for (int i = 0; i < threadCount; i++) {
      pthread_join(threads[i], NULL);
    }
    
    double end_time = now();

    for (int i = 0; i < threadCount; i++) {
      XOR_VALUE ^= threadInputs[i].xor_val;
      free(threadInputs[i].buffer);
    }
    
    double speed = f_size / ((1024.0 * 1024.0) * (end_time - start_time));
    close(fd);
  printf("Performance: \t%.4f MiB/s \tBlock Size: \t%d\n", speed, blockSize);
  return 0;
}