# Lab1: Learned Index

**强烈建议**：在开始实验之前，先**认真**阅读本文档，确保了解了实验的目的、内容和要求后再开始动手编写代码。

## Introduction

Learned Index（学习索引）是一种利用机器学习模型替代传统数据结构（如B树、哈希表等）进行数据索引的技术。对于有序数据集，模型可以学习数据的分布规律，直接预测某个键值的位置。

## Background

传统的索引结构，例如B树，Hash-Index，BitMap，抽象上可以看成其实是一个函数映射/模型。B树其实是从记录到位置的有序映射；Hash-Index其实是记录到位置的无序映射；BitMap其实是一种存在性映射。基于该抽象，如果将中间的映射模型替换其他模型，例如深度学习模型，也是能够工作的。Learned Index（学习索引）就由此诞生.

传统索引（如B树）作为广义数据结构，未利用数据本身的分布特征。例如，当键（Key）为连续递增整数时，B树的O(logn)复杂度远高于理论最优O(1)。学习索引将索引抽象为回归问题：建立映射函数f(Key)→Position，其中Position对应键在有序数据集中的位置。这本质上是对数据累积分布函数（CDF）的建模。例如，若数据符合均匀分布，线性回归即可精准预测位置；复杂分布则需神经网络等非线性模型。

所以，Learned Index的关键思想是，一个模型可以学习查找键的排序顺序或结构，并使用这个信息来有效地预测记录的位置或存在性。Learned Index在真实数据集（如2亿条网站日志）中，学习索引查询速度比B树快1.5-3倍，内存占用减少99%。

## Design

在本Lab中，需要实现一个树状+线性模型的学习索引。

你可以先完成下文`Task`部分的第一步，阅读论文，了解学习索引的原理。这样可以更好的理解本lab实现的Learned Index的原理。

训练过程：

0. 从指定文件名的csv文件中读取`key` `value`数据并存储
1. 通过`二叉树`做数据划分。把所有数据从树根开始不断递归向下划分，划分的依据是数据`key`值的大小（类似`二叉查找树`），如此直到数据集足够小。
2. 当到某个节点时数据集足够小，就停止递归，在这个叶子节点上根据划分到上面的数据集训练预测模型（本Lab为了简化问题，采取线性模型），用于预测`key`对应的`value`的位置。

查询过程：

1. 查询的时候，按照本次查询`key`值的大小沿着二叉树往下走（此过程同二叉查找树的查询）
2. 走到叶子节点之后，用训练好的模型预测其位置。在预测位置的左右 `epsilon` 区间里寻找是否有需要的`key-value`对，如果都没找到，就扫描叶子中所有 `key-value`对，如若还找不到则为不存在。

## Implementation

在`learned_index.h`中，我们提供了类 `LearnedIndex` 的定义，这是我们的主类。其中的`data_loader`是用以加载、存储数据集的类，`root_model`是树状模型的根节点。`train()` 函数用于训练树状模型，`predict()` 函数用于预测键值key对应的value的位置的接口。

`root_model`默认应为指向一个`DecisionTreeModel`实例的指针，但你也可以尝试使用`LinearModel`来替代（见`Others`部分）。

在`DecisionTreeModel`类中，我们提供了 `LeafNode` 和 `InternalNode` 两个子类。
- `LeafNode` 表示叶子节点，存储了数据集的键值对
- `InternalNode` 表示内部节点，用于划分数据集、指向子节点

在 `LeafNode` 中，除了存储数据集合外，还存储了一个线性模型，用于预测键值的位置，根据预测的位置，我们可以缩小查询范围，从而找到目标键值的位置（如果预测值在误差范围内，则返回预测值，否则遍历节点中的其他键值对，找到目标键值）

**注意**：以上的诸多类的功能都可能没有完全实现，你需要阅读代码、参考注释，根据提示和自己的理解来完成这些类的实现。

### 架构图
```
                  ┌───────────────────────┐
                  │     InternalNode      │
                  ├───────────────────────┤
                  │ - split_key: int      │
                  │ - left_child: Node    │
                  │ - right_child: Node   │
                  └───────────┬───────────┘
                              │
           ┌──────────────────┴───────────────────┐
           │                                      │
┌──────────▼───────────┐              ┌───────────▼─────────────┐
│     LeafNode         │              │     LeafNode            │
├──────────────────────┤              ├─────────────────────────┤
│ - Linear Model       │              │ - Linear Model          │
│(position = a*key + b)|              | (position = a*key + b)  │
├──────────────────────┤              ├─────────────────────────┤
│ Key-Value Pairs:     │              │ Key-Value Pairs:        │
│ [ (k1,v1),           │              │ [ (k5,v5),              │
│   (k2,v2),           │              │   (k6,v6),              │
│   ... (sorted) ]     │              │   ... (sorted) ]        │
└──────────────────────┘              └─────────────────────────┘
```
工作流程说明：
1. 树状结构通过 `InternalNode` 划分数据空间
2. 查询时从根 `InternalNode` 逐层路由到目标 `LeafNode`
3. 在 `LeafNode` 中：
```
   ┌───────────────┐
   │ 用线性模型预测  │
   └──────┬────────┘
          │
   ┌──────▼───────┐
   │  检查预测位置  │
   └──────┬───────┘
          │
   ┌──────▼───────┐       ┌───────────────┐
   │   命中预测区  │       │  遍历相邻区域   │
   │   返回对应值  ├──────►|  直到找到目标   │
   └──────────────┘       └───────────────┘
```

## Task

1. 阅读论文[The Case for Learned Index Structures](https://arxiv.org/abs/1712.01208)，学习 Learned Index 的原理。
2. 参考`data/`文件夹中csv文件的数据格式，从文件中读取数据，以key升序排序后存储到 `DataLoader`中的`data`里。
3. 完成`DecisionTreeModel`中的`build_tree`函数。训练树状模型，如果目前的数据集已经小于 `default_max_leaf_samples`，则将其直接作为 `LeafNode` 的叶子节点；否则按照 `data.len() / 2` 的位置，将数据集划分为左右两个部分，作为 `InternalNode` 的子节点继续划分，直到划分结束。
4. 在 `LeafNode` 中，用数据的 `key` 作为 `x`，在 `LeafNode` 中 `data` 的下标作为 `y` 使用 `最小二乘法` 训练线性模型。把训练得到的斜率与截距保存下来，用于之后的预测（参考`linear_model.h`）
5. 实现预测函数
  - 在 `InternalNode` 中，根据键值，选择合适的子节点继续查询，直到找到 `LeafNode`
  - 在 `LeafNode` 中，根据预测的位置，在误差范围内寻找目标键值的位置（如果超出误差范围，则遍历节点中的其他键值对，找到目标键值）
6. 当完成以上内容后，你需要将 `key:predict_position(线性模型中预测的value的位置)` 这一对键值对写入到定义在`linear_model.cc`中的`std::unordered_map<KeyType, size_t> grade_key_to_predict_position` 中，我们会在test中据此检测你的predict是否正确

### Compare

请补齐`test/compare_test.cc`中缺失的代码。比较 `LinearModel` `DecisionTreeModel` `std::map` 之间，在**不同数据分布**下，写入和读取的性能差异。我们建议你使用测量时间的方法来评估性能。

请将最后的结果和你的发现写入到报告（详见下文`Report`部分）中，我们会根据你的报告评分。

### Tips

1. 由于`key`值范围较大，可能会导致线性回归的斜率过大，导致精度不足。为了避免这种情况，可以将`key`值做一个偏移，例如减去`base_key`，这样可以避免斜率过大的情况。

       例如，如果此叶节点 `key` 的值都是大于 `1000000` 的整数，可以将 `base_key` 设置为 `1000000`左右的数字，然后在训练时，将所有 `key` 减去 `base_key`再运算，这样可以避免精度问题。
       
       为了统一标准，我们的代码已经给大家固定了`base_key`的值，你可以直接使用`base_key`的值来做偏移，注意：无论是训练还是预测，都需要对`key`值进行偏移。

2. 线性回归模型的目标是找到一条直线 $y = \text{slope} \cdot x + \text{intercept}$，使得预测值与实际值的误差最小。通过最小二乘法，斜率和截距的计算公式如下

**斜率（`slope`）**：
$$
\text{slope} = \frac{n \cdot \sum{(x_i \cdot y_i)} - \sum{x_i} \cdot \sum{y_i}}{n \cdot \sum{(x_i^2)} - (\sum{x_i})^2}
$$
其中，$x_i$ 是偏移后的键值，$y_i$ 是位置

**截距（`intercept`）**：
$$
\text{intercept} = \frac{\sum{y_i} - \text{slope} \cdot \sum{x_i}}{n}
$$

## Build & Run

### Mac/Linux/Windows

请确保你的环境中已经安装了`cmake`和C++编译器。

```
cmake -B build (execute only when edit CMakeLists.txt and first build)
cmake --build build (execute to rebuild)
./build/test/basic_test1
./build/test/basic_test2
./build/test/large_test1
./build/test/large_test2
```

## Report

我们提供了一份latex模板，链接如下：[report_template](https://latex.sjtu.edu.cn/4184116212ttcjdwbsfbgc#2b04b7)

你可以基于这个模板来撰写你的报告，详情参见链接里的模板内容。

报告应为pdf格式，命名为`学号_姓名.pdf`，例如`523030912345_张三.pdf`。会由canvas上单独的报告提交链接进行提交。

## Submission

请将你的代码打包成`zip`格式，命名为`学号_姓名.zip`，例如`523030912345_张三.zip`

你的压缩包应包含以下内容：
- 包含你代码的src/
- 包含你代码的include/

一个建议的打包命令为：`zip -r 523030912345_张三.zip include/ src/`


## Grade

- basic_test1 (18%)
- basic_test2 (18%)
- large_test1 (18%)
- large_test2 (18%)
- hidden_test (18%) (上传后测试)
- lab report (10%)

## Others

此部分不计入成绩

1. 尝试在划分树状模型的数据集时，计算不同划分方法的 `loss` ，找到更好的划分方式，最brute force的方法是，从数据的开始，遍历每种划分方法，找到 `loss` 最小的方法，你也可以想想，有没有更好的算法？

### 分割损失计算（`compute_split_loss`函数）

将数据集 $\mathcal{D}$ 在索引 $k$ 处分割为两个子集：
$$
\mathcal{L} = \{x_1, x_2, \dots, x_k\}, \quad \mathcal{R} = \{x_{k+1}, x_{k+2}, \dots, x_n\}
$$

分割损失定义为原始方差与分割后加权方差的差值：
$$
\text{SplitLoss} = \text{Var}(\mathcal{D}) - \frac{|\mathcal{L}| \cdot \text{Var}(\mathcal{L}) + |\mathcal{R}| \cdot \text{Var}(\mathcal{R})}{|\mathcal{D}|}
$$

其中：
- $|\mathcal{D}| = n$ 是总样本数
- $|\mathcal{L}| = k$, $|\mathcal{R}| = n - k$
- $\text{Var}(\mathcal{L})$ 和 $\text{Var}(\mathcal{R})$ 是子集的方差

### 方差计算（`compute_variance`函数）

对于数据集 $\mathcal{D} = \{x_1, x_2, \dots, x_n\}$，其方差计算公式为：

$$
\text{Var}(\mathcal{D}) = \frac{1}{n} \sum_{i=1}^n x_i^2 - \left( \frac{1}{n} \sum_{i=1}^n x_i \right)^2
$$

或等价地：

$$
\text{Var}(\mathcal{D}) = \mathbb{E}[x^2] - (\mathbb{E}[x])^2
$$

其中：
- $\mathbb{E}[x] = \frac{1}{n} \sum_{i=1}^n x_i$ 是均值
- $\mathbb{E}[x^2] = \frac{1}{n} \sum_{i=1}^n x_i^2$ 是平方的均值


## 参考资料

1. [The Case for Learned Index Structures](https://arxiv.org/abs/1712.01208)
2. [论文 The Case for Learned Index Structures](https://zhuanlan.zhihu.com/p/336823036)