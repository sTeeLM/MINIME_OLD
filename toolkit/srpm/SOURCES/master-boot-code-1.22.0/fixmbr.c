/*
 * Fixmbr - write generic code into the master boot record.
 *
 * Copyright (C) 2010 Suse Linux Products GmbH.
 * All rights reserved.
 *
 * Written by Torsten Duwe for Suse Linux 11.3.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by Suse Linux
 *        Products GmbH and its contributors.
 *
 * THIS SOFTWARE IS PROVIDED BY THE SUSE LINUX PRODUCTS GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <err.h>

/* The MBR */
unsigned char buf[512];

const char * progname;
const char * mbrcodeName = "/usr/lib/boot/master-boot-code";

#define CODE_SIZE 440
#define SIGNATURE_OFFSET 440

void usage(void)
{
  fprintf(stderr, "usage: %s [-s] [-i <mbr-file>] <device>\n", progname);
  fprintf(stderr, "\twrite %d bytes to <device> from %s\n", CODE_SIZE, mbrcodeName);
  fprintf(stderr, "\tor use code supplied in <mbr-file>.\n");
  fprintf(stderr, "\t-s will generate a new random signature for the MBR.\n");
  exit(2);
}

int
main(int argc, char ** argv)
{
  int fd, mbrcodeFD, opt;
  int do_sig = 0;

  progname = argv[0];
  
  if (argc < 2)
    usage();

  while ((opt = getopt(argc, argv, "si:")) != -1)
    {
      switch (opt)
	{
	case 's':
	  do_sig = 1;
	  break;
	case 'i':
	  mbrcodeName = optarg;
	  break;
	default:
	  usage();
	}
    }
  
  mbrcodeFD = open(mbrcodeName, O_RDONLY, 0);
  if (mbrcodeFD < 0)
    err(1, "Cannot open MBR code %s for reading", mbrcodeName);
  
  if (chdir("/dev") != 0)
    err(1, "change directory to /dev");
      
  fd = open(argv[optind], O_RDWR, 0);
  if (fd < 0)
    err(1, "Cannot open %s", argv[optind]);

  if (read(fd, buf, 512) < 512)
    err(1, "Read from %s", argv[optind]);

  if (lseek(fd, 0, SEEK_SET) != 0)
    err(1, "%s not seekable", argv[optind]);

  if (read(mbrcodeFD, buf, CODE_SIZE) < CODE_SIZE)
    err(1, "Read new MBR code from %s", mbrcodeName);

  if (do_sig)
    {
      srandom(time(0));
      buf[SIGNATURE_OFFSET+0] = random();
      buf[SIGNATURE_OFFSET+1] = random();
      buf[SIGNATURE_OFFSET+2] = random();
      buf[SIGNATURE_OFFSET+3] = random();
    }

  buf[510] = 0x55;
  buf[511] = 0xAA;
  if (write(fd, buf, 512) < 512)
    err(1, "Write back %s", argv[optind]);
  
  return 0;
}
