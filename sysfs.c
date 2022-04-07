#include <stdio.h>
#include <string.h>
#include <dirent.h>

#define SYSFS_BOARD_SERIAL      "/sys/class/dmi/id/board_serial"
#define SYSFS_PRODUCT_SERIAL    "/sys/class/dmi/id/product_serial"
#define SYSFS_PRODUCT_UUID      "/sys/class/dmi/id/product_uuid"
#define SYSFS_ETH_PATH          "/sys/class/net/"
#define MAC_LENGTH              18

/************************* need root!!! *****************************/
int get_board_serial(char *serial, int max_length)
{
    FILE *fSysFS = 0;
    int read_length = 0;
    if ((fSysFS = fopen(SYSFS_BOARD_SERIAL, "r")) != 0)
    {
        read_length = fread(serial, 1, max_length, fSysFS);
        if (0 < read_length)
            serial[read_length] = 0;
        fclose(fSysFS);
    }
    return read_length;
}

int get_product_serial(char *serial, int max_length)
{
    FILE *fSysFS = 0;
    int read_length = 0;
    if ((fSysFS = fopen(SYSFS_PRODUCT_SERIAL, "r")) != 0)
    {
        read_length = fread(serial, 1, max_length, fSysFS);
        if (0 < read_length)
            serial[read_length] = 0;
        fclose(fSysFS);
    }
    return read_length;
}

int get_product_uuid(char *uuid, int max_length)
{
    FILE *fSysFS = 0;
    int read_length = 0;
    if ((fSysFS = fopen(SYSFS_PRODUCT_UUID, "r")) != 0)
    {
        read_length = fread(uuid, 1, max_length, fSysFS);
        if (0 < read_length)
            uuid[read_length] = 0;
        fclose(fSysFS);
    }
    return read_length;
}
// TODO: for virtual machines may add support of chassis serial number
/********************************************************************/
int get_all_mac(char *macs, int max_length)
{
    struct dirent **namelist;
    FILE *fSysFS = 0;
    int macs_length = 0;
    int dir_count = 0;
    static char eth_path[PATH_MAX] = SYSFS_ETH_PATH;

    dir_count = scandir(SYSFS_ETH_PATH, &namelist, 0, 0);
    if (0 < dir_count)
    {
        while (0 != (dir_count--))
        {
            if (strncmp(".", namelist[dir_count]->d_name, 1) == 0 ||
                strncmp("lo", namelist[dir_count]->d_name, 2) == 0)
                continue;

            memcpy(eth_path + sizeof(SYSFS_ETH_PATH) - 1, namelist[dir_count]->d_name, strlen(namelist[dir_count]->d_name));
            eth_path[sizeof(SYSFS_ETH_PATH) + strlen(namelist[dir_count]->d_name)] = 0;
            strcat(eth_path, "/address");

            if ((fSysFS = fopen(eth_path, "r")) != 0)
            {
                macs_length += fread(macs + macs_length, 1, 18, fSysFS);
            }

            if ((macs_length - 18) < max_length)
                break;
        }
    }
    return macs_length;
}

