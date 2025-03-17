#include <iostream>
#include <array>
#include "MurmurHash3.h"

int main() {
    int n = 100;
    int m[4] = {200, 300, 400, 500};
    int false_num[5][4] = {0};

    // k, m, n
    bool hash[5][4][500] = {false};
    std::array<uint32_t, 4> hash_out;

    for (int k = 0; k < 5; k++) {
        for (int i = 0; i < n; i++) {
            for (int k1 = 0; k1 <= k; k1++) {
                MurmurHash3_x64_128(&i, sizeof(i), k1 + 1, hash_out.data());
                for (int j = 0; j < 4; j++) {
                    int index = hash_out[0] % m[j];
                    hash[k][j][index] = true;
                }
            }
        }
    }

    for (int k = 0; k < 5; k++) {
        for (int i = n; i < 2 * n; i++) {
            for (int j = 0; j < 4; j++) {
                bool false_element = true;
                for (int k1 = 0; k1 <= k; k1++) {
                    MurmurHash3_x64_128(&i, sizeof(i), k1 + 1, hash_out.data());
                    int index = hash_out[0] % m[j];
                    if (!hash[k][j][index]) {
                        false_element = false;
                        break;
                    }
                }
                if (false_element) {
                    false_num[k][j]++;
                }
            }
        }
    }

    for (int k = 0; k < 5; k++) {
        for (int j = 0; j < 4; j++) {
            printf("k = %d, m/n = %d, result = %.3lf\n", k + 1, j + 2, (double)false_num[k][j] / n);
        }
    }

    return 0;
}
