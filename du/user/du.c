#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"

int check_number(char *num_str){
    int i, num_str_size = strlen(num_str);
    for(i = 0; i < num_str_size; i++)
    {
        if((num_str[i]> '9') || (num_str[i] <'0'))
        {
            return 0;
        }
    }
    return 1;
}

int check_path(char *ck_path){
    
    int file_directory;
    static struct stat st;
    
    if ((file_directory = open(ck_path, 0)) < 0) 
    {
        return 0;
    }
    
    stat(ck_path, &st);
    close(file_directory);
    return 1;
}

int du(char *du_path, int *du_output_type, int *du_threshold, int *du_recursive, int *du_backslash)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    int s_corrector= 1;
    
    s_corrector = (!*du_output_type) ? 1 : BSIZE;

    if ((fd = open(du_path, 0)) < 0) 
    {
        return -1;
    }

    if (fstat(fd, &st) < 0) 
    {
        close(fd);
        return -1;
    }
    
    int total_size = 0;

    switch (st.type) {
    case T_FILE:{
    total_size = st.size/s_corrector;
    total_size = (st.size%s_corrector == 0) ? total_size : (total_size + 1);
    printf(2, "%d %s\n", total_size, du_path);
    printf(2, "%d %s\n", total_size, du_path);
    break;
    }

    case T_DIR:

    if (strlen(du_path) + 1 + DIRSIZ + 1 > sizeof buf) 
    {
      printf(2, "du: path too long\n");
      total_size = -1;
      break;
    }
    strcpy(buf, du_path);
    p = buf + strlen(buf);
    *p++ = '/';
    int count = 0;
    
    while (read(fd, &de, sizeof(de)) == sizeof(de)) 
    {
        // Skipping the first two file descriptors because
        // first two will always be "." and ".."
        // These are not needed in the current du program

        if(count<2){
            count++;
            continue;
        }
        
        if (de.inum == 0)
            continue;
            
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if (stat(buf, &st) < 0) {
            total_size = -1;
            continue;
        }
        
        if ((st.type == T_DIR) && *du_recursive){
            int temp = du(buf, du_output_type, du_threshold, du_recursive, du_backslash);
            if(temp == -1)
            {
                total_size = -1;
            }
            else {
                total_size += temp;
            }
        }

        if ((st.size > *du_threshold) && (st.type == T_FILE)){
            int dir_file_size = st.size/s_corrector;
            dir_file_size = (st.size % s_corrector == 0) ? dir_file_size : (dir_file_size + 1);
            total_size += dir_file_size;
            
            printf(2, "%d %s\n", dir_file_size, buf);
        }
    }
    if (*du_backslash == 0)
    {
        printf(2, "%d %s\n", total_size, du_path);
    }
    else if (*du_backslash == 1)
    {
        printf(2, "%d %s/\n", total_size, du_path);
    }
    break;
  }
  
  close(fd);
  return total_size;
}

int main(int argc, char *argv[])
{
    char default_du_path[2] = ".";
    char *du_path = default_du_path;
    // du_size_format is set 0 for Bytes format
    // du_size_format is set 1 for Blocks format
    // du_size_format is set to 0 intially
    int du_size_format = 0;
    // du_threshold is set to 0 intially
    // threshold is applied in bytes
    int du_threshold = 0;
    // du_recursive is set 0 for non-recursive
    // du_recursive is set 1 for recursive
    // du_recursive is set to 0 intially
    int du_recursive = 0;
    // du_backslash is set 0 for no terminating slash
    // du_backslash is set 1 for terminating slash
    // du_backslash is set to 0 intially
    int du_backslash = 0;

    if (argc < 2) 
    {

    }
    else{

        int i = 1;

        while( i < argc){
            if (!strcmp(argv[i], "-k") && du_size_format == 0)
            {
                du_size_format = 1;
            }

            else if (!strcmp(argv[i], "-t") && du_threshold == 0)  
            {
                i++;
                if((i < argc) && (check_number(argv[i]))){
                    du_threshold = atoi(argv[i]);
                }
                
                else
                {
                    printf(2, "check usage.\n");
                    exit();
                }
            }
            else if(!strcmp(argv[i], "-r") && du_recursive == 0) 
            {
                du_recursive = 1;
            }
            else if(check_path(argv[i]))  
            {
                int temp = strlen(argv[i]);
                char *temp2 = argv[i]+(temp-1);
                if(*temp2 == '/')
                {
                    *temp2 = '\0';
                    du_backslash = 1;
                }
                du_path = argv[i];
            }
            else
            {
                printf(2, "check usage.\n");
                exit();
            }

            i++;
        }

    }

    int final_check = du(du_path, &du_size_format, &du_threshold, &du_recursive, &du_backslash);
    if(final_check < 0)
    {
        printf(2, "check usage.\n");
    }
    exit();
}