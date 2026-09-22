/*
2024017035 김재현
시스템프로그래밍 과제
*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<time.h>
#include<utmp.h>
#include<string.h>

int show_record(const struct utmp *utbufp, int output_fd)
{
    char linebuf[256];
    int len;
    time_t login_time;
    char *time_text;
    ssize_t written;

    if (utbufp->ut_type != USER_PROCESS)
        return 0;

    login_time = (time_t)utbufp->ut_tv.tv_sec;
    time_text = ctime(&login_time);
    if (time_text == NULL)
    {
        fprintf(stderr, "Cannot convert login time\n");
        return -1;
    }

    if (utbufp->ut_host[0] != '\0')
    {
        len = snprintf(
            linebuf,
            sizeof(linebuf),
            "%-8.8s %-8.8s %.12s (%.*s)\n",
            utbufp->ut_user,
            utbufp->ut_line,
            time_text + 4,
            UT_HOSTSIZE,
            utbufp->ut_host
        );
    }
    else
    {
        len = snprintf(
            linebuf,
            sizeof(linebuf),
            "%-8.8s %-8.8s %.12s\n",
            utbufp->ut_user,
            utbufp->ut_line,
            time_text + 4
        );
    }


    if (len < 0 || len >= (int)sizeof(linebuf))
    {
        fprintf(stderr, "Output line is too long\n");
        return -1;
    }

    if (write(STDOUT_FILENO, linebuf, (size_t)len) != len)
    {
        perror("Cannot write to stdout");
        return -1;
    }

    written = write(output_fd, linebuf, (size_t)len);
    if (written == -1)
    {
        perror("Cannot write output file");
        return -1;
    }
    if (written != len)
    {
        fprintf(stderr, "Write error: incomplete write\n");
        return -1;
    }

    return 0;
}


int main (int argc, char *argv[])
{
    int output_fd;
    struct utmp *current_record;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./who_to_file <output_filename>\n");
        return EXIT_FAILURE;
    }

    output_fd = creat(argv[1], 0644);
    if (output_fd == -1)
    {
        perror("Cannot create output file");
        return EXIT_FAILURE;
    }

    setutent();

    while((current_record = getutent()) != NULL)
    {
        if (show_record(current_record, output_fd) == -1)
        {
            endutent();
            close(output_fd);
            return EXIT_FAILURE;
        }
    }

    endutent();

    if (close(output_fd) == -1)
    {
        perror("Cannot close output file");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}