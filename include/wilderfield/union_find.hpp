// Copyright (c) [2024] [Bryan Wilder Field Lozano]
// Licensed under the MIT License. See LICENSE file in the project root for full
// license information.

/**
 * @file union_find.hpp
 * @brief Union Find Template Class Definition
 *
 * Defines a templated union find class that allows for connectivity tracking,
 */
#ifndef INCLUDE_WILDERFIELD_UNION_FIND_HPP_
#define INCLUDE_WILDERFIELD_UNION_FIND_HPP_

#include <algorithm>
#include <cstdlib>
#include <unordered_map>

namespace wilderfield {

/**
 * @brief Union Find class
 *
 * Implements the union find data structure.
 *
 * @tparam T The type used to label graph nodes.
 */
template <typename T>
class UnionFind {
 protected:
  std::unordered_map<T, T> parent_of_;
  std::unordered_map<T, size_t> rank_of_;

 public:
  /**
   * @brief Insert a new node
   *
   * @param u The node to insert
   */
  void InsertNode(T u) {
    if (!parent_of_.count(u)) {
      parent_of_[u] = u;
      rank_of_[u] = 1;
    }
  }

  /**
   * @brief Perform union operation on two nodes
   *
   * @param u First node
   * @param v Second node
   */
  void Union(T u, T v) {
    // Both nodes should exist
    if (parent_of_.count(u) && parent_of_.count(v)) {
      auto repr_of_u = Find(u);
      auto repr_of_v = Find(v);
      if (repr_of_u != repr_of_v) {
        if (rank_of_[repr_of_u] >= rank_of_[repr_of_v]) {
          parent_of_[repr_of_v] = repr_of_u;
          rank_of_[repr_of_u] += rank_of_[repr_of_v];
        } else {
          parent_of_[repr_of_u] = repr_of_v;
          rank_of_[repr_of_v] += rank_of_[repr_of_u];
        }
      }
    }
  }

  /**
   * @brief Find the representative of a node
   *
   * @param u The node to find
   * @return The representative of the node
   */
  T Find(T u) {
    while (parent_of_[u] != u) {
      parent_of_[u] =
          parent_of_[parent_of_[u]];  // Path compression with grandparent
      u = parent_of_[u];
    }
    return u;
  }

  /**
   * @brief Get the maximum rank in the union find structure
   *
   * @return The maximum rank
   */
  std::size_t GetMaxRank() {
    size_t result = 0;
    for (auto& entry : rank_of_) {
      result = std::max(result, entry.second);
    }
    return result;
  }
};

}  // namespace wilderfield

#endif  // INCLUDE_WILDERFIELD_UNION_FIND_HPP_
