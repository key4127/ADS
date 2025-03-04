#include "linear_model.h"

#include <unordered_map>

// debug
#include <cstdio> 

void LinearModel::train(const std::vector<DataPoint>& data) {
  base_key = data[0].key;

  double n = data.size();
  double sum_xy = 0;
  double sum_x = 0;
  double sum_y = 0;
  double sum_pow_x = 0;

  for (int i = 0; i < n; i++) {
    double xi = data[i].key - base_key;
    double yi = i;

    sum_xy += (xi * yi);
    sum_x += xi;
    sum_y += yi;
    sum_pow_x += (xi * xi);
  }

  this->slope = (n * sum_xy - sum_x * sum_y) / (n * sum_pow_x - sum_x * sum_x);
  this->intercept = (sum_y - slope * sum_x) / n;

  /*
  printf("slope: %lf\n", slope);
  printf("intercept: %lf\n", intercept);
  printf("\n");
  */
}

std::unordered_map<KeyType, int32_t> grade_key_to_predict_position{};

std::optional<ValueType> LinearModel::predict(KeyType key) const {
  int32_t predicted_position = 0;

  grade_key_to_predict_position[key] = predicted_position;

  return predicted_position;
}
