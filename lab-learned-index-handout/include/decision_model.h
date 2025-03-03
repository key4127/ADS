#pragma once

#include "base.h"
#include "linear_model.h"

class DecisionTreeModel : public Model {
  class TreeNode {
   public:
    virtual ~TreeNode() = default;
    virtual std::optional<ValueType> predict(KeyType key) const = 0;
  };

  class LeafNode : public TreeNode {
  public: 
    std::vector<DataPoint> pairs;

    std::optional<ValueType> predict(KeyType key) const {
      
    }
  };

  class InternalNode : public TreeNode {
  public: 
    int split_key;
    std::unique_ptr<TreeNode> left;
    std::unique_ptr<TreeNode> right;

    std::optional<ValueType> predict(KeyType key) const {
      
    }
  };

 private:
  double compute_split_loss(const std::vector<DataPoint>& data,
                            size_t split_idx);  // In README Others.
  double compute_variance(const std::vector<DataPoint>& data, size_t start,
                          size_t end);  // In README Others.

  std::unique_ptr<TreeNode> build_tree(const std::vector<DataPoint>& data) {
    return build_tree_helper(data, 0, data.size());
  }

  // do not contain end
  std::unique_ptr<TreeNode> build_tree_helper(const std::vector<DataPoint>& data, int begin, int end) {
    if (end - begin < default_max_leaf_samples) {
      std::unique_ptr<LeafNode> r = std::make_unique<LeafNode>();

      for (int i = begin; i < end; i++) {
        r->pairs.insert(r->pairs.end(), data[i]);
      }

      return r;

    } else {
      std::unique_ptr<InternalNode> r = std::make_unique<InternalNode>();

      int len = (end - begin) / 2;
      r->split_key = 0;
      r->left = build_tree_helper(data, begin, begin + len);
      r->right = build_tree_helper(data, begin + len, end);

      return r;
    }
  }

 public:
  DecisionTreeModel() {};

  void train(const std::vector<DataPoint>& data) override;

  std::optional<ValueType> predict(KeyType key) const override;
};