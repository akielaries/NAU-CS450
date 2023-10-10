// test program for profiling
// will not work on OSX!
// gcc -pg test_profile.c -o test_profile
// ./test_profile
// gprof test_profile > profile_output.txt

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

int fun1();
int fun2();
int fun3();
void output(int idx, int x);

int main() {
	srand(time(NULL));
	int res = fun1() + fun2() + fun3();
	output(0, res);
	return 0;
}

int fun1() {
	int res = rand() % 5 + 3;
	for (int i = 0; i < res * 1e6; i++);
	output(1, res);
	return res;
}

int fun2() {
	int res = rand() % 4 + fun1();
	for (int i = 0; i < res * 1e6; i++);
	output(2, res);
	return res;
}

int fun3() {
	int res = rand() % 3 + fun2();
	for (int i = 0; i < res * 1e6; i++);
	output(3, res);
	return res;
}

void output(int idx, int x) {
	printf("Value from %d: %d\n", idx, x);
}

