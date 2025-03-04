#include "decision_model.h"

#include "linear_model.h"

#include <queue>

void DecisionTreeModel::train(const std::vector<DataPoint>& data) {
  std::unique_ptr<TreeNode> root = build_tree(data);

  if (typeid(*root) == typeid(LeafNode)) {
    LeafNode* r = static_cast<LeafNode*>(root.get());
    trainLeaf(r);
  } else {
    InternalNode* r = static_cast<InternalNode*>(root.get());
    trainInternal(r);
  }
}

void DecisionTreeModel::trainInternal(InternalNode* r) {
  if (typeid(*(r->left)) == typeid(LeafNode)) {
    LeafNode* left = static_cast<LeafNode*>(r->left.get());
    trainLeaf(left);
  } else {
    InternalNode* left = static_cast<InternalNode*>(r->left.get());
    trainInternal(left);
  }

  if (typeid(*(r->right)) == typeid(LeafNode)) {
    LeafNode* right = static_cast<LeafNode*>(r->right.get());
    trainLeaf(right);
  } else {
    InternalNode* right = static_cast<InternalNode*>(r->right.get());
    trainInternal(right);
  }
}

void DecisionTreeModel::trainLeaf(LeafNode* r) {
  r->leafModel.train(r->leafModel.data);
}

std::optional<ValueType> DecisionTreeModel::predict(KeyType key) const {
  

  return std::nullopt;
}
