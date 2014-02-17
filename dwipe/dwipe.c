/* ----------------------------------------------------------------------- *
 *
 *   Copyright 2014 sTeeL<steel.mental@gmail.com> - All Rights Reserved
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *   Boston MA 02110-1301, USA; either version 2 of the License, or
 *   (at your option) any later version; incorporated herein by reference.
 *
 * ----------------------------------------------------------------------- */

#include <stdio.h>
#include <console.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getkey.h>
#include <disk/geom.h>
#include <disk/util.h>
#include <disk/errno_disk.h>
#include <disk/error.h>
#include <disk/write.h>
#include <syslinux/reboot.h>

#define __DWIPE_VERSION__ "1.0.0.0"

/*
 * 100%|>>>>>>>>    |
 *
 */
static void print_progress(int progress, int cols)
{
    int pc1 = progress * (cols - 14) / 100; /* how many >? */
    int pc2 = (cols - 14) - pc1; /* how many =? */
    int i;

    printf("\r");
    printf("WIPING %3d%%|", progress);
    for(i = 0 ; i < pc1; i ++) {
        printf(">");
    }
    for(i = 0 ; i < pc2; i ++) {
        printf(" ");
    }
    printf("|");
}

int main(int argc, char *argv[])
{
	struct driveinfo drive;
	struct driveinfo *d = &drive;
    char buffer[4096];
    unsigned int index = 0;
    int ret, progress, progress_old;
    int rows, cols;

	(void)argc;
	(void)argv;

	openconsole(&dev_stdcon_r, &dev_stdcon_w);

    if (getscreensize(1, &rows, &cols)) {
        /* Unknown screen size? */
        rows = 24;
        cols = 80;
    }

    printf("DWIPE version %s, by sTeeL <steel.mental@gmail.com>\n", __DWIPE_VERSION__);

    printf("remove USB DISK and hit ENTER key\n");

    get_key(stdin, 0);

	for (int disk = 0x80; disk < 0xff; disk++) {
		memset(d, 0, sizeof(struct driveinfo));
		d->disk = disk;
		get_drive_parameters(d);

		/* Do not print output when drive does not exists */
		if (errno_disk == -1 || !d->cbios)
			continue;

		if (errno_disk) {
			get_error("reading disk");
			continue;
		}

		printf("DISK 0x%X:\n", d->disk);
		printf("  C/H/S: %d heads, %d cylinders\n",
			d->legacy_max_head + 1, d->legacy_max_cylinder + 1);
		printf("         %d sectors/track, %d drives\n",
			d->legacy_sectors_per_track, d->legacy_max_drive);
		printf("  EDD:   ebios=%d, EDD version: %X\n",
			d->ebios, d->edd_version);
		printf("         %d heads, %d cylinders\n",
			(int) d->edd_params.heads, (int) d->edd_params.cylinders);
		printf("         %d sectors, %d bytes/sector, %d sectors/track\n",
			(int) d->edd_params.sectors, (int) d->edd_params.bytes_per_sector,
			(int) d->edd_params.sectors_per_track);
		printf("         Host bus: %s, Interface type: %s\n\n",
			d->edd_params.host_bus_type, d->edd_params.interface_type);

        progress_old = 0;

        for(index = 0; index < d->edd_params.sectors ; index += 8 ) {
            ret = write_sectors(d, index, buffer, 8);
            if(ret == -1) {
                continue;
            } else {
                progress = index * 100 / d->edd_params.sectors;
                if(progress != progress_old) {
                    progress_old = progress;
                    print_progress(progress, cols);
                }
            }
        }
        print_progress(100, cols);
        printf("\nDONE\n\n");

        
	}
    syslinux_reboot(0);
	return 0;
}
