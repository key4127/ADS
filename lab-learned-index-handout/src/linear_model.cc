#include "linear_model.h"

#include <unordered_map>

// debug
#include <cstdio> 

#include <cmath>

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
  //int32_t predicted_position = (key - base_key) * slope + intercept;
  int32_t predicted_position = static_cast<int32_t>(std::round((key - base_key) * slope + intercept));
  if (predicted_position < 0) {
    predicted_position = 0;
  }

  grade_key_to_predict_position[key] = predicted_position;

  ValueType value = 0;
  bool find_value = false;
  for (int i = predicted_position - epsilon; i <= predicted_position + epsilon; i++) {
    if (i >= data.size()) {
      break;
    }
    if (data[i].key == key) {
      value = data[i].value;
      find_value = true;
      break;
    }
  }

  if (!find_value) {
    for (auto point : data) {
      if (point.key == key) {
        value = point.value;
        find_value = true;
        break;
      }
    }
  }

  if (!find_value) {
    return std::nullopt;
  } else {
    return value;
  }
}
