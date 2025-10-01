#include <lksan.h>
#include <stdio.h>
#include <stdlib.h>

int main(void){
    int* leak = malloc(10 * sizeof(int)); // Intentional leak
    return 0;
}