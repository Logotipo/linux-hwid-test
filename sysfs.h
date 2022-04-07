#ifndef SYSFS_H
#define SYSFS_H

int get_board_serial(char *serial, int max_length);
int get_product_serial(char *serial, int max_length);
int get_product_uuid(char *uuid, int max_length);
int get_all_mac(char *macs, int max_length);

#endif

