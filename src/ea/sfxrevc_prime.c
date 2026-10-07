/* A fragment of the reverb set-up file (sfxrevc.c, 0x80168300): findprime
   returns the first prime at or above a delay length (rate * milliseconds /
   1000, at least 3), testing divisors up to the root found by a search. */
int findprime(int rate, int ms) {
    int n = ms * rate / 1000;
    int root;
    int i;
    int limit;
    int product;

    if (n < 3)
        n = 3;
restart:
    i = 0;
    do {
        root = i;
        i++;
        product = i * root;
        if (product == n) {
            i--;
            goto found;
        }
    } while (product <= n);
    i -= 2;
found:
    limit = i + 1;
    for (i = 2; i <= limit; i++) {
        if (n % i == 0) {
            n++;
            goto restart;
        }
        if (i == limit)
            return n;
    }
    goto restart;
}
