#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <linux/un.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#define PLY_SOCKET_PATH "/org/freedesktop/plymouthd"

struct header_t
{
    unsigned char type;
    uint32_t length;
} __attribute__ ((__packed__));

int write_fd(int sock, char * buffer, size_t number_of_bytes)
{
  size_t bytes_left_to_write;
  size_t total_bytes_written = 0;

  bytes_left_to_write = number_of_bytes;

  do
    {
      ssize_t bytes_written = 0;

      bytes_written = write (sock,
                             ((uint8_t *) buffer) + total_bytes_written,
                             bytes_left_to_write);

      if (bytes_written > 0)
        {
          total_bytes_written += bytes_written;
          bytes_left_to_write -= bytes_written;
        }
      else if ((errno != EINTR))
        break;
    }
  while (bytes_left_to_write > 0);

  return number_of_bytes - bytes_left_to_write;
}

int read_fd(int sock, char * buffer, size_t max_bytes)
{
  size_t bytes_left_to_read;
  size_t total_bytes_read = 0;

  bytes_left_to_read = max_bytes;

  do
    {
      ssize_t bytes_read = 0;

      bytes_read = read (sock,
                         ((uint8_t *) buffer) + total_bytes_read,
                         bytes_left_to_read);

      if (bytes_read > 0)
        {
          total_bytes_read += bytes_read;
          bytes_left_to_read -= bytes_read;
        }
      else if ((errno != EINTR))
        break;
    }
  while (bytes_left_to_read > 0);

  if ((bytes_left_to_read > 0) && (errno != EAGAIN))
    total_bytes_read = -1;

  return total_bytes_read;
}

int main(int argc, char *argv[])
{
    struct sockaddr_un addr;
    int sock = -1;
    int should_pass_credentials = 1;
    int addr_size = 0;
    char buffer[4096];
    int buf_len = 0;
    struct header_t header;

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy (addr.sun_path + 1, PLY_SOCKET_PATH, sizeof (addr.sun_path) - 1);
    addr_size = offsetof (struct sockaddr_un, sun_path)
                      + 1 /* NUL */ +
                      strlen (addr.sun_path + 1) /* path */;

    sock = socket (PF_UNIX, SOCK_STREAM, 0);
    if(sock < 0) {
        fprintf(stderr, "socket: %m");
        goto err;
    }

    if (setsockopt (sock, SOL_SOCKET, SO_PASSCRED,
                  &should_pass_credentials, sizeof (should_pass_credentials)) < 0) {
        fprintf(stderr, "setsockopt: %m");
        goto err;
    }

    if (connect (sock, (const struct sockaddr *)&addr, addr_size) < 0) {
        fprintf(stderr, "connect: %m");
        goto err;
    }

    // send command
    memset(buffer, 0, sizeof(buffer));
    if(!strcmp(argv[1], "question")) {
        buffer[0] = 'W'; // question
    } else {
        buffer[0] = '*'; // password
    }
    buffer[1] = '\002';
    buffer[2] = strlen(argv[2]);
    strncpy(&buffer[3], argv[2], sizeof(buffer) - 3); // prompt
    buf_len = 3 + strlen(argv[2]);

    if(write_fd(sock, buffer, buf_len) != buf_len) {
        goto err;
    }

    // read response
    // read header
    memset(buffer, 0, sizeof(buffer));
    if(read_fd(sock, (char *)&header, sizeof(header)) != sizeof(header)) {
        fprintf(stderr, "read header failed\n");
        goto err;
    }

    if(header.type != 0x2 || header.length > sizeof(buffer) - 1) {
        fprintf(stderr, "header not expect: %x length=%u\n",  header.type, header.length);
        goto err;
    }

    fprintf(stderr, "header is %x, length is %u\n", header.type, header.length);

    if(read_fd(sock, buffer, header.length) != header.length) {
	fprintf(stderr, "read body failed\n");
        goto err;
    }    

    fprintf(stdout, "%s", buffer);

    close(sock);

    sock = -1;

    exit(0);
err:
    if(sock != -1) {
        close(sock);
    }
    exit(1);
}

