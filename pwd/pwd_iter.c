/*
2024017035 김재현
시스템프로그래밍 과제
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

ino_t get_inode(char *);
void print_path_to(ino_t);
void inode_to_name(ino_t, char *, int);

int main(void)
{
    print_path_to(get_inode("."));
    putchar('\n');

    return 0;
}

void print_path_to(ino_t inode)
{
    ino_t current_inode;
    char its_name[256];
    char fullpath[20][256];
    int depth = 0;
    int i;

    current_inode = inode;


    while (get_inode("..") != current_inode)
    {
        if (depth >= 20)
        {
            fprintf(stderr, "path is too deep\n");
            exit(1);
        }

        if (chdir("..") == -1)
        {
            perror("chdir");
            exit(1);
        }

        inode_to_name(current_inode, its_name, 256);

        strcpy(fullpath[depth], its_name);
        depth++;

        current_inode = get_inode(".");
    }


    for (i = 0; i < depth; i++)
    {
        printf("[%s] -> ", fullpath[i]);
    }
    printf("/\n\n");

    printf("-------------------------\n");
    printf("Print working directory\n");
    printf("-------------------------\n");


    if (depth == 0)
    {
        printf("/");
    }
    else
    {
        for (i = depth - 1; i >= 0; i--)
        {
            printf("/%s", fullpath[i]);
        }
    }
}

void inode_to_name(ino_t inode_to_find, char *namebuf, int buflen)
{
    DIR *dir_ptr;
    struct dirent *dirent_ptr;

    if ((dir_ptr = opendir(".")) == NULL)
    {
        perror(".");
        exit(1);
    }

    while ((dirent_ptr = readdir(dir_ptr)) != NULL)
    {
        if (dirent_ptr -> d_ino == inode_to_find)
        {
            strncpy(namebuf, dirent_ptr -> d_name, buflen);
            namebuf[buflen -1] = '\0';

            closedir(dir_ptr);
            return;
        }
    }

    closedir(dir_ptr);

    fprintf(stderr, "error looking for inode %ld\n", (long)inode_to_find);
    exit(1);
}


ino_t get_inode(char *filename)
{
    struct stat info;

    if (stat(filename, &info) == -1)
    {
        fprintf(stderr, "cannot stat ");
        perror(filename);
        exit(1);
    }

    return info.st_ino;
}
