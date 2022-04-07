#include <stdio.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/ioctl.h>
#include <linux/hdreg.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_SERIAL_NUMBERS      5

/// simpled version of disk-utils/fdisk-list.c code
char *get_next_disk (FILE **partFile)
{
    char line[128 + 1];
    static char buf[PATH_MAX];
    if (0 == *partFile)
    {
        *partFile = fopen("/proc/partitions", "r");
        if (0 == *partFile)
        {
            printf("Can`t open /proc/partitions!\n");
            return 0;
        }
    }

    while (fgets(line, sizeof(line), *partFile) != 0)
    {
        memset(buf, 0, sizeof(buf));
        if (sscanf(line, " %*d %*d %*d %128[^\n ]", buf) != 1)
		    continue;

        return buf;
    }
    memset(buf, 0, sizeof(buf));
    fclose(*partFile);
    return 0;
}

int get_disk_serials(char *serials_string, int max_length)
{
    FILE *partFile = 0;
    static struct hd_driveid hd;
    static char full_disk_path[PATH_MAX] = "/dev/";
    static char serials[MAX_SERIAL_NUMBERS][64];
    int disk_descriptor = 0;
    int i = 0;
    char *disk_name = 0;
    char repeate = 0;
    int serials_count = 0;
    while ((disk_name = get_next_disk(&partFile)) != 0)
    {
        repeate = 0;
        memcpy(full_disk_path + 5, disk_name, strlen(disk_name));
        full_disk_path[5 + strlen(disk_name)] = 0;
        if ((disk_descriptor = open(full_disk_path, O_RDONLY | O_NONBLOCK)) < 0) 
            continue;

        if (ioctl(disk_descriptor, HDIO_GET_IDENTITY, &hd) == 0)
        {
            for (i = 0; i < serials_count; i++)
            {
                if (strncmp(hd.serial_no, serials[i], strlen(hd.serial_no)) == 0)
                {
                    repeate = 1;
                    break;
                }
            }
            if (0 != repeate)
                continue;

            memcpy(serials[serials_count], hd.serial_no, strlen(hd.serial_no));
            serials_count++;

            if (MAX_SERIAL_NUMBERS <= serials_count)
                break;
        }
    }
    memset(serials_string, 0, max_length);
    for (i = 0; i < serials_count; i++)
    {
        if ((strlen(serials_string) + strlen(serials[i])) >= max_length)
            break;

        strcat(serials_string, serials[i]);
        strcat(serials_string, ":");
    }
    return strlen(serials_string);
}

