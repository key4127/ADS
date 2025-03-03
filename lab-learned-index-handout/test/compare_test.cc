#include <cassert>
#include <map>

#include "learned_index.h"

void test_std_map(std::string data_path) {
  DataLoader data_loader;

  data_loader.load_data(data_path);
  std::vector<DataPoint> data = data_loader.get_data();

  std::map<KeyType, ValueType> tree;

  for (const auto &[k, v] : data) {
    tree[k] = v;
  }

  ValueType v;
  for (const auto &item : data) {
    v = tree[item.key];
    assert(v == item.value);
  }
}

void test_linear_model(std::string data_path) {
  LearnedIndex index("Linear", data_path);

  std::optional<ValueType> v;
  for (const auto &item : index.data()) {
    v = index[item.key];
    assert(v == item.value);
  }
}

void test_decision_tree_model(std::string data_path) {
  LearnedIndex index("DecisionTree", data_path);

  std::optional<ValueType> v;
  for (const auto &item : index.data()) {
    v = index[item.key];
    assert(v == item.value);
  }
}

int main(int argc, char **argv) {
  std::string test_name = "normal_100000";
  std::string data_path = "./data/" + test_name + ".csv";

  test_std_map(data_path);
  test_linear_model(data_path);
  test_decision_tree_model(data_path);

  test_name = "uniform_100000";
  data_path = "./data/" + test_name + ".csv";

  test_std_map(data_path);
  test_linear_model(data_path);
  test_decision_tree_model(data_path);
}
