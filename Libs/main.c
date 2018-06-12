#include <stdio.h>
#include <dlfcn.h>

int sum(int a, int b);
int dif(int a, int b);

int main() {
    void *handle = dlopen("./libmul.so", RTLD_LAZY);
    if (!handle) {
        perror("Error open lib");
    }
    int (*mul)(int, int);
    mul = dlsym(handle, "mul");
    if (dlerror() != NULL) {
        perror("Error find function");
    }
    printf("%d %d %d\n", sum(1, 2), dif(1 ,2), mul(1, 2));
    dlclose(handle);
    return 0;
}