#pragma once

#include <string>
#include <vector>

#include <fstream>
#include <sstream>
#include <algorithm>

class DataLoader {
 private:
  std::vector<DataPoint> data;

  bool static cmp(DataPoint a, DataPoint b) {
    if (a.key == b.key) {
      return a.value <= b.value;
    }
    return a.key <= b.key;
  }

 public:
  void load_data(const std::string& data_path) {
    std::ifstream file(data_path);
    std::string line;

    while (std::getline(file, line)) {
      std::istringstream ss(line);
      KeyType key;
      char comma;
      ValueType value;

      ss >> key >> comma >> value;

      DataPoint dataPoint;
      dataPoint.key = key;
      dataPoint.value = value;
      data.insert(data.begin(), dataPoint);
    }
    
    sort(data.begin(), data.end(), cmp);
  }

  std::vector<DataPoint>& get_data() { return data; }
};
