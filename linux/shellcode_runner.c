#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

// Replace this
unsigned char buf[] = "\x90";

int main(int argc, char **argv)
{
    size_t len = sizeof(buf);
    void *exec = mmap(NULL, len, PROT_READ | PROT_WRITE | PROT_EXEC,
                       MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (exec == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    memcpy(exec, buf, len);
    ((void(*)())exec)();
    return 0;
}
