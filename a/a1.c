#include <stdio.h>
#include <stdint.h>
#include <math.h>

int compute_rms_fixed(const int *samples, int len) {
    int64_t sum_sq = 0;
    for (int i = 0; i < len; i++) {
        int64_t v = samples[i]*100;
        sum_sq += v * v;
    }

    // mean of squares
    int64_t mean_sq = sum_sq / len;

    // RMS
    int64_t rms = (int64_t)sqrt(mean_sq);

    return rms;
}

int main(void) {
    const int samples[50] = {
        1010, 980, 1024, 990, 1005, 1018, 975, 992, 1003, 998,
        1012, 984, 1021, 995, 1008, 1015, 982, 987, 1000, 997,
        1007, 978, 1030, 993, 1002, 1022, 979, 989, 1004, 996,
        1011, 985, 1025, 994, 1006, 1016, 980, 986, 1001, 995,
        1009, 977, 1028, 991, 1000, 1020, 981, 990, 1002, 998
    };

    int rms = compute_rms_fixed(samples, 50);
    printf("RMS: %d.%02d\n", rms / 100, rms % 100);
    return 0;
}
