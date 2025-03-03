#pragma once

#include <memory>
#include <optional>
#include <vector>

using KeyType = int32_t;
using ValueType = int32_t;

const size_t epsilon = 5;                     // 误差范围
const size_t default_max_leaf_samples = 100;  // 最大叶子节点数据量

struct DataPoint {
  KeyType key;
  ValueType value;
};

class Model {
 public:
  virtual void train(const std::vector<DataPoint>& data) = 0;
  virtual std::optional<ValueType> predict(KeyType key) const = 0;
  virtual ~Model() = default;
};
