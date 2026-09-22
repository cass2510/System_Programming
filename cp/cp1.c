/*
2024017035 김재현
시스템프로그래밍 과제 2
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFERSIZE 1024
#define COPYMODE 0644

int main(int argc, char *argv[])
{
  int input_fd;
  int output_fd;
  ssize_t nread;
  ssize_t nwritten;
  long long total_bytes = 0;
  char buffer[BUFFERSIZE];
  
  if (argc != 3)
  {
    fprintf(stderr, "Usage: %s source destination\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  input_fd = open(argv[1], O_RDONLY);
  
  if (input_fd == -1)
  {
    perror("Cannot open source file");
    return EXIT_FAILURE;
  }
  
  output_fd = creat(argv[2], COPYMODE);
  
  if (output_fd == -1)
  {
    perror("Cannot create destination file");
    
    if (close(input_fd) == -1)
    {
      perror("Cannot close source file");
    }
    
    return EXIT_FAILURE;
  }
  
  while ((nread = read(input_fd, buffer, BUFFERSIZE)) > 0)
  {
    nwritten = write(output_fd, buffer, nread);
    
    if (nwritten == -1)
    {
      perror("Cannot write destination file");
      
      if (close(output_fd) == -1)
      {
        perror("Cannot close destination file");
      }
      
      if (close(input_fd) == -1)
      {
        perror("Cannot close source file");
      }
      
      return EXIT_FAILURE;
    }
    
    if (nwritten != nread)
    {
      fprintf(stderr, "Write error: incomplete write\n");
      
      if (close(output_fd) == -1)
      {
        perror("Cannot close destination file");
      }
      
      if (close(input_fd) == -1)
      {
        perror("Cannot close source file");
      }
      
      return EXIT_FAILURE;
    }
    
    total_bytes += nread;
  }
  
  if (nread == -1)
  {
    perror("Cannot read source file");
    
    if (close(output_fd) == -1)
    {
      perror("Cannot close destination file");
    }
    
    if (close(input_fd) == -1)
    {
      perror("Cannot close source file");
    }
    
    return EXIT_FAILURE;
  }
  
  if (close(input_fd) == -1)
  {
    perror("Cannot close source file");
    return EXIT_FAILURE;
  }
  
  printf("total read bytes: %lld\n", total_bytes);
  
  return EXIT_SUCCESS;
}
