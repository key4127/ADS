#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

#include "learned_index.h"

int test(std::string name) {
  extern std::unordered_map<KeyType, int32_t> grade_key_to_predict_position;
  grade_key_to_predict_position.clear();
  int ret = 0;

  extern std::string test_name;
  test_name = name;
  std::string data_path = "./data/" + test_name + ".csv";

  LearnedIndex index("DecisionTree", data_path);

  DataLoader data_loader;
  std::string data_path_sample = "./data/" + test_name + "_sample.csv";
  data_loader.load_data(data_path_sample);
  std::vector<DataPoint> data = data_loader.get_data();

  if (data.size() == 0) {
    return -1;
  }

  for (int32_t i = 0; i < data.size(); ++i) {
    ValueType predicted = index[data[i].key].value_or(0);
    ValueType expected = data[i].value;
    if (predicted != expected) {
      std::cerr << "line:" << __LINE__ << " key: " << data[i].key
                << " predicted value: " << predicted << " expected value: " << expected
                << std::endl;
      ret = -1;
    }
  }

  DataLoader data_loader_predict;
  std::string data_path_predict = "./data/" + test_name + "_predict.csv";
  data_loader_predict.load_data(data_path_predict);
  std::vector<DataPoint> data_predict = data_loader_predict.get_data();
  
  if (data.size() == 0) {
    return -1;
  }

  for (int32_t i = 0; i < data_predict.size(); ++i) {
    ValueType predicted = grade_key_to_predict_position[data_predict[i].key];
    ValueType expected = data_predict[i].value;
    if (predicted != expected) {
      std::cerr << "line:" << __LINE__ << " key: " << data_predict[i].key
                << " predicted position: " << predicted << " expected position: " << expected
                << std::endl;
      ret = -1;
    }
  }
  return ret;
}
