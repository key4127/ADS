#pragma once

#include "base.h"
#include "linear_model.h"

#include <cstdio>

class DecisionTreeModel : public Model {
  class TreeNode {
   public:
    virtual ~TreeNode() = default;
    virtual std::optional<ValueType> predict(KeyType key) const = 0;
  };

  class LeafNode : public TreeNode {
  public: 
    LinearModel leafModel;

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
  std::unique_ptr<TreeNode> root;

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

      //printf("len: %d\n", end - begin);
      for (int i = begin; i < end; i++) {
        r->leafModel.data.insert(r->leafModel.data.end(), data[i]);
        //printf("key = %d, value = %d\n", data[i].key, data[i].value);
      }
      //printf("\n");

      return r;

    } else {
      std::unique_ptr<InternalNode> r = std::make_unique<InternalNode>();

      int len = (end - begin) / 2;
      r->split_key = data[begin + len].key;
      r->left = build_tree_helper(data, begin, begin + len);
      r->right = build_tree_helper(data, begin + len, end);

      return r;
    }
  }

  void trainInternal(InternalNode* r);
  void trainLeaf(LeafNode *r);

 public:
  DecisionTreeModel() {};

  void train(const std::vector<DataPoint>& data) override;

  std::optional<ValueType> predict(KeyType key) const override;
};