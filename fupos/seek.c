/* Copyright (C) 2012, 2018 James Craig Burley
   Source: https://github.com/jcburley/fupos
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define SEEK_VERSION "0.2"

int
main(int argc, char **argv)
{
  int rc = 0;
  off_t result = 0;
  char print_offset = 0;
  char print_fd = 0;
  char print_errno = 0;
  char exit_onfail = 1;

  if (!--argc) {
  usage:;
    fprintf(stderr,
            "Usage: seek [[[opts] fd posn]...] [[opts] fd [posn]]\n"
            "  lseek() is called on fd with posn for each pair\n"
            "  until it fails (-x); the final such failure's\n"
            "  errno is the exit status, otherwise 0 is returned.\n"
            "  The resulting offset is printed for each successful lseek.\n"
            "\n"
            "  <posn> is a long long int with optional leading + or -:\n"
            "  -N means SEEK_END plus N bytes (N s/b negative)\n"
            "  +N means SEEK_CUR plus N bytes (N can be + or -)\n"
            "  N means SEEK_SET to N bytes (N m/b non-negative)\n"
            "    posn (last pair only) defaults to +0\n"
            "\n"
            "<opts>:\n"
            "-e  print '<errno>: <strerror>' on stderr after failed seek(s)\n"
            "-E  do not print errno info (initial/default)\n"
            "-f  print '<fd> ' in front of offset/errno\n"
            "-F  do not print fd (initial default)\n"
            "-q  print offset after successful seeks\n"
            "-Q  do not print offset (initial/default)\n"
            "-x  exit on failed seek (initial/default)\n"
            "-X  do not exit on failed seek\n"
            "-v  same as '-e -f -q'\n"
            "-V  print version info and exit\n"
            "-h  print this help and exit\n"
            "-?  print this help and exit\n"
            "--help  print this help and exit\n"
            "--version  print version info and exit\n"
            "\n");
    return 1;
  version:;
    printf("seek Version %s\n"
           "  <http://www.burleyarch.com/>\n",
           SEEK_VERSION);
    return 0;
  }

  while (argc && !(rc && exit_onfail)) {
    char *fdp;
    char *posp;
    char *endptr;
    int fd;
    int whence;
    long long int pos;

    for (--argc, ++argv;
         argv[0][0] == '-';
         --argc, ++argv) {
      char *opt;
      if (!strcmp(argv[0], "--help")) {
        goto usage;
      }
      if (!strcmp(argv[0], "--version")) {
        goto version;
      }
      if (!strcmp(argv[0], "--verbose")) {
        print_offset = 1; print_fd = 1; print_errno = 1;
        continue;
      }
      for (opt = argv[0] + 1; *opt; ++opt) {
        switch (*opt) {
        case 'q': print_offset = 1; break;
        case 'Q': print_offset = 0; break;
        case 'f': print_fd = 1; break;
        case 'F': print_fd = 0; break;
        case 'e': print_errno = 1; break;
        case 'E': print_errno = 0; break;
        case 'x': exit_onfail = 1; break;
        case 'X': exit_onfail = 0; break;
        case 'v': print_offset = 1; print_fd = 1; print_errno = 1; break;
        case 'V': goto version;
        case 'h': goto usage;
        case '?': goto usage;
        default:  goto usage;
        }
      }
    }

    fdp = *argv;
    posp = argc ? --argc, *++argv : "+0";

    fd = strtol(fdp, &endptr, 0);
    if (*fdp && *endptr)
      goto usage;

    if (*posp == '+' || *posp == '-')
      whence = (*posp++ == '+') ? SEEK_CUR : SEEK_END;
    else
      whence = SEEK_SET;

    pos = strtoll(posp, &endptr, 0);
    if (*posp && *endptr)
      goto usage;

    result = lseek(fd, pos, whence);
    if ((off_t)-1 == result) {
      rc = errno;
      if (print_errno) {
      if (print_fd)
        fprintf(stderr, "%d ", fd);
      fprintf(stderr, "%d: %s\n", errno, strerror(errno));
      }
    }
    else if (print_offset) {
      if (print_fd)
        printf("%d ", fd);
      printf("%lld\n", (long long) result);
    }
  }

  return rc;
}
