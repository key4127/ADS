#pragma once

#include "base.h"
#include "linear_model.h"

class DecisionTreeModel : public Model {
  class TreeNode {
   public:
    virtual ~TreeNode() = default;
    virtual std::optional<ValueType> predict(KeyType key) const = 0;
  };

  class LeafNode : public TreeNode {};

  class InternalNode : public TreeNode {};

 private:
  double compute_split_loss(const std::vector<DataPoint>& data,
                            size_t split_idx);  // In README Others.
  double compute_variance(const std::vector<DataPoint>& data, size_t start,
                          size_t end);  // In README Others.

  std::unique_ptr<TreeNode> build_tree(const std::vector<DataPoint>& data);

 public:
  DecisionTreeModel() {};

  void train(const std::vector<DataPoint>& data) override;

  std::optional<ValueType> predict(KeyType key) const override;
};