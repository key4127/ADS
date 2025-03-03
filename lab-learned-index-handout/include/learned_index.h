#pragma once

#include <string>
#include <iostream>

#include "decision_model.h"
#include "linear_model.h"
#include "tools.h"

class LearnedIndex {
 private:
  std::unique_ptr<Model> root_model;
  DataLoader data_loader;

  void load_data(const std::string& data_path);

  void train();

  std::optional<ValueType> predict(KeyType key) const;

 public:

  LearnedIndex(std::string model, const std::string& data_path) {
    if (model == "Linear") {
      root_model = std::make_unique<LinearModel>();
    } else if (model == "DecisionTree") {
      root_model = std::make_unique<DecisionTreeModel>();
    } else {
      std::cout << "Unsupported model: " << model << std::endl;
      std::cout << "Please choose in (Linear, DecisionTree)" << std::endl;
      root_model = nullptr;
    }
    load_data(data_path);
    train();
  }

  std::vector<DataPoint>& data();

  std::optional<ValueType> operator[](KeyType key) const;
};
