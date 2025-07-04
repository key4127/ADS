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
    if (model == "Linear") {
      LinearModel* m = static_cast<LinearModel*>(root_model.get());
      for (auto point : data_loader.get_data()) {
        m->data.insert(m->data.end(), point);
      }
    }
    clock_t begin, end;
    begin = clock();
    train();
    end = clock();
    if (model == "Linear") {
      printf("linear model write: %lfms\n", (end - begin) * 1000.0 / CLOCKS_PER_SEC);
    } else {
      printf("decision model write: %lfms\n", (end - begin) * 1000.0 / CLOCKS_PER_SEC);
    }
  }

  std::vector<DataPoint>& data();

  std::optional<ValueType> operator[](KeyType key) const;
};
