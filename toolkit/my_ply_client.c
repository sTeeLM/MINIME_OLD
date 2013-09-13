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

#define __MY_PLY_CLIENT_VERSION__ "1.0.0.0"

#define PLY_SOCKET_PATH "/org/freedesktop/plymouthd"

#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_PING 'P'
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_UPDATE 'U'
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_CHANGE_MODE 'C'
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_SYSTEM_UPDATE 'u'
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_SYSTEM_INITIALIZED 'S'
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_DEACTIVATE 'D'
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_REACTIVATE 'r'
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_QUIT 'Q'
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_PASSWORD '*'
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_CACHED_PASSWORD 'c'
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_QUESTION 'W'
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_SHOW_MESSAGE 'M'
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_HIDE_MESSAGE 'm'
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_KEYSTROKE 'K'
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_KEYSTROKE_REMOVE 'L'
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_PROGRESS_PAUSE 'A'
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_PROGRESS_UNPAUSE 'a'
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_SHOW_SPLASH '$'
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_HIDE_SPLASH 'H'
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_NEWROOT 'R'
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_HAS_ACTIVE_VT 'V'
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_ERROR '!'

#define PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ACK '\x6'
#define PLY_BOOT_PROTOCOL_RESPONSE_TYPE_NAK '\x15'
#define PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ANSWER '\x2'
#define PLY_BOOT_PROTOCOL_RESPONSE_TYPE_MULTIPLE_ANSWERS '\t'
#define PLY_BOOT_PROTOCOL_RESPONSE_TYPE_NO_ANSWER '\x5'

struct _header_t
{
    unsigned char type;
    uint32_t length;
}  __attribute__ ((__packed__));

typedef struct _header_t header_t;

static int req_handler(char cmd, int arg_cnt, const char * arg, char * req, size_t *req_len)
{
    if(NULL == req_len || *req_len < 2 || NULL == req) return -1;
    if(arg_cnt != 0 && ( NULL == arg ||  *req_len < 3 + strlen(arg) || strlen(arg) > 255 )) return -1;
    if(arg_cnt == 0) {
        req[0] = cmd;
        req[1] = 0;
        *req_len = 2;
        return 0;
    } else {
        req[0] = cmd;
        req[1] = PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ANSWER;
        req[2] = strlen(arg);
        memcpy(req + 3, arg, strlen(arg));
        *req_len = 3 + strlen(arg);
        if(cmd == PLY_BOOT_PROTOCOL_REQUEST_TYPE_QUIT) {
            if(arg[0] == '0') {
                req[3] = 0;
            } else {
                req[3] = 1;
            }
        }
        return 0;
    }

    return -1;
}

static int res_handler(header_t * header, char ** res, size_t * res_cnt, int * ret)
{
    char * p = (char *) header;
    size_t i, len;

    if(NULL == header || NULL == res || NULL == res_cnt || NULL == ret) return -1;


    if(header->type == PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ACK) {
        *res_cnt = 0;
        *ret = 0;
        return 0;
    } else if(header->type == PLY_BOOT_PROTOCOL_RESPONSE_TYPE_NAK) {
        *res_cnt = 0;
        *ret = 1;
        return 0;
    } else if(header->type == PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ANSWER) {
        if(*res_cnt < 1) return -1;
        res[0] = p + sizeof(header_t);
        *res_cnt = 1;
        *ret = 0;
        return 0;
    } else if(header->type == PLY_BOOT_PROTOCOL_RESPONSE_TYPE_NO_ANSWER) {
        *res_cnt = 0;
        *ret = 0;
        return 0;        
    } else if(header->type == PLY_BOOT_PROTOCOL_RESPONSE_TYPE_MULTIPLE_ANSWERS) {
        p = p + sizeof(header_t);
        len = 0;
        for(i = 0 ; i < *res_cnt && len < header->length; i ++) {
            res[i] = p;
            len += strlen(p) + 1;
            p += len;
        }

        *res_cnt = i;
        *ret = 0;
        return 0;
    }
    return -1;
}

typedef struct _cmd_handler_t
{
    char * cmd;
    char cmd_char;
    int arg_cnt;
}cmd_handler_t;

static cmd_handler_t cmd_handlers[] =
{
    {"ping", PLY_BOOT_PROTOCOL_REQUEST_TYPE_PING, 0},
    {"update", PLY_BOOT_PROTOCOL_REQUEST_TYPE_UPDATE, 0},
    {"change-mode", PLY_BOOT_PROTOCOL_REQUEST_TYPE_CHANGE_MODE, 1},
    {"system-update", PLY_BOOT_PROTOCOL_REQUEST_TYPE_SYSTEM_UPDATE, 1},
    {"system-initialized", PLY_BOOT_PROTOCOL_REQUEST_TYPE_SYSTEM_INITIALIZED, 0},
    {"deactivate", PLY_BOOT_PROTOCOL_REQUEST_TYPE_DEACTIVATE, 0},
    {"reactivate", PLY_BOOT_PROTOCOL_REQUEST_TYPE_REACTIVATE, 0},
    {"quit", PLY_BOOT_PROTOCOL_REQUEST_TYPE_QUIT, 1},
    {"password", PLY_BOOT_PROTOCOL_REQUEST_TYPE_PASSWORD, 1},
    {"cached-password", PLY_BOOT_PROTOCOL_REQUEST_TYPE_CACHED_PASSWORD, 1},
    {"question", PLY_BOOT_PROTOCOL_REQUEST_TYPE_QUESTION, 1},
    {"show-message", PLY_BOOT_PROTOCOL_REQUEST_TYPE_SHOW_MESSAGE, 1},
    {"hide-message", PLY_BOOT_PROTOCOL_REQUEST_TYPE_HIDE_MESSAGE, 1},
    {"keystroke", PLY_BOOT_PROTOCOL_REQUEST_TYPE_KEYSTROKE, 1},
    {"keystroke-remove", PLY_BOOT_PROTOCOL_REQUEST_TYPE_KEYSTROKE_REMOVE, 1},
    {"progress-pause", PLY_BOOT_PROTOCOL_REQUEST_TYPE_PROGRESS_PAUSE, 0},
    {"progress-unpause", PLY_BOOT_PROTOCOL_REQUEST_TYPE_PROGRESS_UNPAUSE, 0},
    {"show-splash", PLY_BOOT_PROTOCOL_REQUEST_TYPE_SHOW_SPLASH, 0},
    {"hide-splash", PLY_BOOT_PROTOCOL_REQUEST_TYPE_HIDE_SPLASH, 0},
    {"new-root", PLY_BOOT_PROTOCOL_REQUEST_TYPE_NEWROOT, 1},
    {"has-active-vt", PLY_BOOT_PROTOCOL_REQUEST_TYPE_HAS_ACTIVE_VT, 0},
    {"error", PLY_BOOT_PROTOCOL_REQUEST_TYPE_ERROR, 0},
};

static int write_fd(int sock, char * buffer, size_t number_of_bytes)
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

static cmd_handler_t * lockup_cmd(const char * cmd)
{
    cmd_handler_t * ret = NULL;
    int i;
    for( i = 0 ; i < sizeof(cmd_handlers)/ sizeof(cmd_handler_t); i ++) {
        if(strcmp(cmd_handlers[i].cmd, cmd) == 0) {
            ret = &cmd_handlers[i];
            break;
        }
    }

    return ret;
}

static void usage()
{
    int i;
    fprintf(stderr, "my_ply_client version %s\n", __MY_PLY_CLIENT_VERSION__);
    fprintf(stderr, "command is\n", __MY_PLY_CLIENT_VERSION__);
    for( i = 0 ; i < sizeof(cmd_handlers) / sizeof(cmd_handler_t); i ++) {
        fprintf(stderr, "%s\n", cmd_handlers[i].cmd);
    }
}

static int read_header(int sock, header_t * header)
{
    char type = 0;
    uint32_t length = 0;

    if(read_fd(sock, (char *)&type, sizeof(type)) != sizeof(type)) {
        return -1;
    }

    header->type = type;
    header->length = 0;

    if(type == PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ANSWER || type == PLY_BOOT_PROTOCOL_RESPONSE_TYPE_MULTIPLE_ANSWERS) {
        if(read_fd(sock, (char *)&length, sizeof(length)) != sizeof(length)) {
            return -1;
        }
    }
    
    header->length = length;

    return 0;
}

int main(int argc, char *argv[])
{
    struct sockaddr_un addr;
    int sock = -1;
    int should_pass_credentials = 1;
    int addr_size = 0;
    char buffer[4096];
    header_t * pheader;
    cmd_handler_t * cmd = NULL;
    size_t len, i;
    int ret;
    char *res[255];

    if(argc < 2) {
        usage();
        exit(1);
    }

    if(( cmd = lockup_cmd(argv[1]) ) == NULL || (argc - cmd->arg_cnt) != 2) {
        usage();
        exit(1);
    }


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

    len = sizeof(buffer);
    if(req_handler(cmd->cmd_char, cmd->arg_cnt, argv[2], buffer, &len) < 0) {
        fprintf(stderr, "make request error: %s %s\n", argv[1], argv[2] == NULL ? "null" : argv[2]);
        goto err;
    }

    if(write_fd(sock, buffer, len) != len) {
        fprintf(stderr, "send request error: %s\n", argv[1]);
        goto err;
    }

    // read response
    // read header
    memset(buffer, 0, sizeof(buffer));
    pheader = (header_t *)buffer;

    if(read_header(sock, pheader) < 0) {
        fprintf(stderr, "read response header failed\n");
        goto err;
    }

    if(pheader->length > sizeof(buffer) - 1 - sizeof(header_t)) {
        fprintf(stderr, "header not expect: %x length=%u\n",  pheader->type, pheader->length);
        goto err;
    }


    if(pheader->length > 0) {
        if(read_fd(sock, buffer + sizeof(header_t), pheader->length) != pheader->length) {
            fprintf(stderr, "read response body failed\n");
            goto err;
        }
    }

    len = sizeof(res) / sizeof(char *);
    memset(res, 0, sizeof(res));
    if(res_handler(pheader, res, &len, &ret) < 0) {
	    fprintf(stderr, "parse response error\n");
        goto err;
    }

    for(i = 0 ; i < len; i ++) {
        fprintf(stdout, "%s\n", res[i]);
    }

    close(sock);

    sock = -1;

    exit(ret);
err:
    if(sock != -1) {
        close(sock);
    }
    exit(1);
}

