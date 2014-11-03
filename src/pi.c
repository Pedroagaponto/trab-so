#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCALE 10000
#define ARRINIT 2000

void pi_digits(int );

int main (){

   clock_t start, end;
   double time;
   
   start = clock();
   pi_digits(200000);
   end = clock();

   time = ((double) (end - start)) / CLOCKS_PER_SEC;
   printf("time = %lf\n", time);

   return 0;

}

void pi_digits(int digits) {
    int carry = 0;
    int arr[digits + 1];
	int i, j;
    for (i = 0; i <= digits; ++i)
        arr[i] = ARRINIT;
    for (i = digits; i > 0; i-= 14) {
        int sum = 0;
        for (j = i; j > 0; --j) {
            sum = sum * j + SCALE * arr[j];
            arr[j] = sum % (j * 2 - 1);
            sum /= j * 2 - 1;
        }
        printf("%04d", carry + sum / SCALE);
        carry = sum % SCALE;
    }
   printf("\n");
}
