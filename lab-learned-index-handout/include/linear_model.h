#pragma once

#include "base.h"

class LinearModel : public Model {
private:
    double slope;
    double intercept;
    KeyType base_key;

public:
    std::vector<DataPoint> data;

    void train(const std::vector<DataPoint>& data) override;

    std::optional<ValueType> predict(KeyType key) const override;
};
