#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/stat.h>

#define INODES_PATH         {"/home", "/etc"}
#define INODES_PATH_COUNT   2

int get_nodename(char *nodename, int max_length)
{
    struct utsname uname_info;
    int nodename_length = 0;

    if (uname(&uname_info) != -1)
    {
        nodename_length = (strlen(uname_info.nodename) < max_length) ? strlen(uname_info.nodename) : (max_length - 1);
        memcpy(nodename, uname_info.nodename, nodename_length);
        nodename[nodename_length] = 0;
    }
    return nodename_length;
}

int get_hostid_text(char *hostid_txt)
{
    long hostid = 0;
    hostid = gethostid();
    sprintf(hostid_txt, "%016lX", hostid);
    return strlen(hostid_txt);
}

int get_inodes(char *inodes, int max_length)
{
    const char inodes_path[INODES_PATH_COUNT][6] = INODES_PATH;
    struct stat dir_info;
    int i = 0;
    char buffer[12];
    
    memset(inodes, 0, max_length);
    for (i = 0; i < INODES_PATH_COUNT; i++)
    {
        if (stat(inodes_path[i], &dir_info) != -1)
        {
            sprintf(buffer, "%08lX ", dir_info.st_ino);
            strcat(inodes, buffer);
            
            if (max_length <= strlen(inodes))
                break;
        }
    }
    return strlen(inodes);
}
