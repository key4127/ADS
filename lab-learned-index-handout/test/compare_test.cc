#include <cassert>
#include <map>

#include <ctime>

#include "learned_index.h"

clock_t write_begin, write_end;
clock_t read_begin, read_end;

void test_std_map(std::string data_path) {
  DataLoader data_loader;

  data_loader.load_data(data_path);
  std::vector<DataPoint> data = data_loader.get_data();

  std::map<KeyType, ValueType> tree;

  write_begin = clock();
  for (const auto &[k, v] : data) {
    tree[k] = v;
  }
  write_end = clock();

  printf("map write: %lfms\n", (write_end - write_begin) * 1000.0 / CLOCKS_PER_SEC);

  ValueType v;
  read_begin = clock();
  for (const auto &item : data) {
    v = tree[item.key];
    if (v != item.value) {
      printf("%d\n", item.key);
    }
    assert(v == item.value);
  }
  read_end = clock();

  printf("map read:  %lfms\n\n", (read_end - read_begin) * 1000.0 / CLOCKS_PER_SEC);
}

void test_linear_model(std::string data_path) {
  LearnedIndex index("Linear", data_path);

  std::optional<ValueType> v;
  read_begin = clock();
  for (const auto &item : index.data()) {
    v = index[item.key];
    assert(v == item.value);
  }
  read_end = clock();

  printf("linear model read:  %lfms\n\n", (read_end - read_begin) * 1000.0 / CLOCKS_PER_SEC);
}

void test_decision_tree_model(std::string data_path) {
  LearnedIndex index("DecisionTree", data_path);

  std::optional<ValueType> v;
  read_begin = clock();
  for (const auto &item : index.data()) {
    v = index[item.key];
    assert(v == item.value);
  }
  read_end = clock();

  printf("decision model read:  %lfms\n\n", (read_end - read_begin) * 1000.0 / CLOCKS_PER_SEC);
}

int main(int argc, char **argv) {
  std::string test_name = "normal_10000";
  std::string data_path = "./data/" + test_name + ".csv";

  test_std_map(data_path);
  test_linear_model(data_path);
  test_decision_tree_model(data_path);

  test_name = "normal_100000";
  data_path = "./data/" + test_name + ".csv";

  test_std_map(data_path);
  test_linear_model(data_path);
  test_decision_tree_model(data_path);
}
