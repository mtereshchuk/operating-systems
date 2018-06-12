#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <vector>
#include <algorithm>

using namespace std;

//Sum
vector<unsigned char> code {
    0x55, 0x48, 0x89, 0xe5, 0x89, 0x7d, 0xfc, 0x89, 0x75, 
    0xf8, 0x8b, 0x55, 0xfc, 0x8b, 0x45, 0xf8, 0x01, 0xd0, 0x5d, 0xc3
};

/*Mul
vector<unsigned char> code {
    0x55, 0x48, 0x89, 0xe5, 0x89, 0x7d, 0xfc, 0x89, 0x75,
    0xf8, 0x8b, 0x45, 0xfc, 0x0f, 0xaf, 0x45, 0xf8, 0x5d, 0xc3
};*/

void patch() {
    code[11] = 0x45;
    code[13] = 0x0f;
    code.insert(code.begin() + 14, 0xaf);
    code.erase(code.begin() + 17, code.begin() + 19);
}

void compile(int a, int b) {
    void *mem = mmap(NULL, code.size(), PROT_WRITE | PROT_READ, MAP_ANON | MAP_PRIVATE, -1, 0);
    if (mem == MAP_FAILED) {
        perror("Error mmap");
    }
    copy(code.begin(), code.end(), (unsigned char *) mem);
    if (mprotect(mem, code.size(), PROT_EXEC) == -1) {
        perror("Error mprotect");
    }
    int (*func)(int, int) = (int (*)(int, int)) mem;
    cout << func(a, b) << '\n';
    if (munmap(mem, code.size()) == -1) {
        perror("Erroe munmap");
    }
}

int main(int argc, char *argv[]) {
    if (stoi(argv[1])) {
        patch();    
    }
    compile(stoi(argv[2]), stoi(argv[3]));
    return 0;
}