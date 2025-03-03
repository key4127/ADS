#include "linear_model.h"

#include <unordered_map>

void LinearModel::train(const std::vector<DataPoint>& data) {
  base_key = data[0].key;
}

std::unordered_map<KeyType, int32_t> grade_key_to_predict_position{};

std::optional<ValueType> LinearModel::predict(KeyType key) const {
  int32_t predicted_position = 0;

  grade_key_to_predict_position[key] = predicted_position;

  return predicted_position;
}
