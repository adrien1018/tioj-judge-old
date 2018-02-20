#include <ext/rope>
#include <bits/stdc++.h>
#include "cpp_header.h"

using namespace std;

int main()
{
  using namespace std::chrono;
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  int a;
  cin >> a;
  cout << a << endl;
  if (a == 0) {
    srand(time(nullptr));
    scanf("%d", &a);
    printf("%d\n", a);
    cin >> a;
    cout << a << endl;
    clock_t start = clock();
    int* p1 = (int*)malloc(10 * sizeof(int));
    int* p2 = (int*)malloc(100000 * sizeof(int));
    int* p3 = new int[10];
    int* p4 = new int[100000];
    for (int i = 0; i < 100000; i++) p2[i] = rand(), p4[i] = rand();
    for (int i = 0; i < 10; i++) p1[i] = rand(), p3[i] = rand();
    cout << (int)(clock() - start) << ' ' << p3[4] << endl;
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    cout << time_span.count() << endl;
  }
  else if (a == 1) {
    try {
      testfunction();
    } catch (int) {}
    testfunction(); // uncatched exception
  }
  else if (a == 2) {
    a /= 0; // trigger a signal
  }
}
