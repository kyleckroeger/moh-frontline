// The first prime at or above a * b / 1000 (at least 3). Divisors are tried
// up to an integer square root found by stepping i * (i - 1).
int SNDI_findprime(int a, int b) {
    int n = b * a / 1000;
    if (n < 3)
        n = 3;
    for (;;) {
        int d;
        int root;
        int i = 0;
        for (;;) {
            int previous = i;
            i++;
            int product = i * previous;
            if (product == n) {
                root = i - 1;
                break;
            }
            if (product > n) {
                root = i - 2;
                break;
            }
        }
        int limit = root + 1;
        for (d = 2; d <= limit; d++) {
            if (n % d == 0) {
                n++;
                break;
            }
            if (d == limit)
                return n;
        }
    }
}
