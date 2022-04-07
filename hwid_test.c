#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "diskid.h"
#include "sysfs.h"
#include "nonroot.h"
#include "sha256.h"

int main (int argc, char **argv)
{
    char *result_hw_string;
    int result_hw_length;
    static char disk_serials[64 * 5];
    static char board_serial[64];
    static char product_serial[64];
    static char product_uuid[64];
    static char macs[18 * 5];
    static char nodename[64];
    static char hostid[17];
    static char inodes[32];
    int disk_serials_length = 0;
    int board_serial_length = 0;
    int product_serial_length = 0;
    int product_uuid_length = 0;
    int macs_length = 0;
    int nodename_length = 0;
    int hostid_length = 0;
    int inodes_length = 0;
    int i = 0;
    SHA256_CTX ctx;
    unsigned char hash[32];
    char hash_text[65];

    unsigned char non_root_mode = 0;
    unsigned char print_info = 0;

    for (i = 1; i < argc; i++)
    {
        if (strncmp(argv[i], "-h", 2) == 0)
        {
            printf("Parameters of hwid_test:\n\n\t-p - print all got information about system\n\t-n - non root mode\n\t-h - print this message\n\n");
            return 0;
        }
        else if (strncmp(argv[i], "-n", 2) == 0)
            non_root_mode = 1;
        else if (strncmp(argv[i], "-p", 2) == 0)
            print_info = 1;
    }

    if (0 != non_root_mode)
    {
        // we can try get disks`s serials because CD/DVD-drive`s serials (for example) we can getting without root
        disk_serials_length = get_disk_serials(disk_serials, sizeof(disk_serials));
        if (0 != print_info)
            printf("Disk serials: %s Length: %d\n", disk_serials, disk_serials_length);

        macs_length = get_all_mac(macs, sizeof(macs));
        if (0 != print_info)
            printf("MAC-addresses: %s Length: %d\n", macs, macs_length);

        hostid_length = get_hostid_text(hostid);
        if (0 != print_info)
            printf("HostID: %s Length: %d\n", hostid, hostid_length);

        inodes_length = get_inodes(inodes, sizeof(inodes));
        if (0 != print_info)
            printf("Inodes: %s Length: %d\n", inodes, inodes_length);

        result_hw_length = disk_serials_length + macs_length + hostid_length + inodes_length + 1;
        if (10 > result_hw_length)
        {
            printf("\n\nNOT enough data for check hardware!!!\n\n");
            return 0;
        }
        result_hw_string = malloc(result_hw_length);
        if (0 != result_hw_string)
        {
            memset(result_hw_string, 0, result_hw_length);

            if (0 < disk_serials_length)
                strcat(result_hw_string, disk_serials);
            if (0 < macs_length)
                strcat(result_hw_string, macs);
            if (0 < hostid_length)
                strcat(result_hw_string, hostid);
            if (0 < inodes_length)
                strcat(result_hw_string, inodes);

            sha256_init(&ctx);
            sha256_update(&ctx, result_hw_string, result_hw_length - 1);
            sha256_final(&ctx, hash);
            memset(hash_text, 0, sizeof(hash_text));
            for (i = 0; i < sizeof(hash); i++)
            {
                char buf[3];
                sprintf(buf, "%02x", hash[i]);
                strcat(hash_text, buf);
            }

            printf("HW HASH: %s\n", hash_text);
            free(result_hw_string);
        }
        else
            printf("\n\nMEMORY error!!!\n\n");
    }
    else
    {
        disk_serials_length = get_disk_serials(disk_serials, sizeof(disk_serials));
        if (0 != print_info)
            printf("Disk serials: %s Length: %d\n", disk_serials, disk_serials_length);

        board_serial_length = get_board_serial(board_serial, sizeof(board_serial));
        if (0 != print_info)
            printf("Board serial: %s Length: %d\n", board_serial, board_serial_length);

        product_serial_length = get_product_serial(product_serial, sizeof(product_serial));
        if (0 != print_info)
            printf("Product serial: %s Length: %d\n", product_serial, product_serial_length);

        product_uuid_length = get_product_uuid(product_uuid, sizeof(product_uuid));
        if (0 != print_info)
            printf("Product uuid: %s Length: %d\n", product_uuid, product_uuid_length);

        result_hw_length = product_uuid_length + product_serial_length + board_serial_length + disk_serials_length + 1;
        //deliberately do not take disk_serials_length
        if (10 > result_hw_length || (0 == board_serial_length && 0 == product_serial_length && 0 == product_uuid_length))
        {
            printf("\n\nNOT enough data for check hardware!!!\n\n");
            return 0;
        }
        result_hw_string = malloc(result_hw_length);
        if (0 != result_hw_string)
        {
            memset(result_hw_string, 0, result_hw_length);

            if (0 < disk_serials_length)
                strcat(result_hw_string, disk_serials);
            if (0 < board_serial_length)
                strcat(result_hw_string, board_serial);
            if (0 < product_serial_length)
                strcat(result_hw_string, product_serial);
            if (0 < product_uuid_length)
                strcat(result_hw_string, product_uuid);

            sha256_init(&ctx);
            sha256_update(&ctx, result_hw_string, result_hw_length - 1);
            sha256_final(&ctx, hash);
            memset(hash_text, 0, sizeof(hash_text));
            for (i = 0; i < sizeof(hash); i++)
            {
                char buf[3];
                sprintf(buf, "%02x", hash[i]);
                strcat(hash_text, buf);
            }

            printf("HW HASH: %s\n", hash_text);
            free(result_hw_string);
        }
        else
            printf("\n\nMEMORY error!!!\n\n");
    }
    return 0;
}

