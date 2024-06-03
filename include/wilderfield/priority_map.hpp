// Copyright (c) [2024] [Bryan Wilder Field Lozano]
// Licensed under the MIT License. See LICENSE file in the project root for full
// license information.

/**
 * @file priority_map.hpp
 * @brief Priority Map Template Class Definition
 *
 * Defines a templated priority map class that allows for efficient retrieval,
 * update, and tracking of keys by their associated priority values.
 */
#ifndef INCLUDE_WILDERFIELD_PRIORITY_MAP_HPP_
#define INCLUDE_WILDERFIELD_PRIORITY_MAP_HPP_

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iterator>
#include <list>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace wilderfield {

/**
 * @brief Priority map class
 *
 * Implements a priority map where each key is associated with a priority value.
 * The map maintains the keys in sorted order based on their priority, allowing
 * for efficient retrieval and modification of priorities.
 *
 * @tparam KeyType The type of the keys.
 * @tparam ValType The type of the values (priorities), must be numeric.
 * @tparam Compare Comparison class used to maintain the ordering of values.
 * @tparam Hash Hashing class used for keys.
 */
template <typename KeyType, typename ValType,
          typename Compare = std::greater<ValType>,
          typename Hash = std::hash<KeyType>>
class PriorityMap final {
  static_assert(std::is_arithmetic<ValType>::value,
                "ValType must be a numeric type.");

 private:
  Compare comp_;

  std::list<ValType> vals_;  ///< List to maintain sorted values.

  std::unordered_map<KeyType, typename std::list<ValType>::iterator, Hash>
      keys_;  ///< Map from keys to their corresponding list iterator in vals_.

  std::unordered_map<ValType, std::unordered_set<KeyType>>
      val_to_keys_;  ///< Map from vals to their corresponding keys

  // Private member functions

  // Insert new key
  void Insert(const KeyType& key, const ValType& new_val);

  // Update Key with Val
  // This function can be used for increment, decrement, or assigning a new val
  void Update(const KeyType& key, const ValType& new_val);

  // Get the value associated with a key.
  ValType val(const KeyType& key) const { return *(keys_.at(key)); }

 public:
  std::size_t Size() const {
    return keys_.size();
  }  ///< Returns the number of unique keys in the priority map.

  bool Empty() const {
    return keys_.empty();
  }  ///< Checks whether the priority map is empty.

  std::size_t Count(const KeyType& key) const {
    return keys_.count(key);
  }  ///< Returns the count of a particular key in the map.

  std::pair<KeyType, ValType> Top()
      const;  ///< Returns the top element (key-value pair) in the priority map.

  std::size_t Erase(
      const KeyType& key);  ///< Erases key from the priority map. Returns the
                            ///< number of elements removed (0 or 1).

  void Pop();  ///< Removes the top element from the priority map.

  class Proxy;
  Proxy operator[](const KeyType& key);

  // Proxy class to handle the increment operation.
  class Proxy {
   private:
    PriorityMap* pm;
    KeyType key;

   public:
    Proxy(PriorityMap* pm, const KeyType& key) : pm(pm), key(key) {}

    Proxy& operator++() {
      pm->Update(key, pm->val(key) + 1);
      return *this;
    }

    Proxy operator++(int) {
      Proxy temp = *this;
      ++(*this);
      return temp;
    }

    Proxy& operator--() {
      pm->Update(key, pm->val(key) - 1);
      return *this;
    }

    Proxy operator--(int) {
      Proxy temp = *this;
      --(*this);
      return temp;
    }

    void operator=(const ValType& val) { pm->Update(key, val); }

    operator ValType() const { return pm->val(key); }
  };
};

// Out-of-line implementation of PriorityMap methods

template <typename KeyType, typename ValType, typename Compare, typename Hash>
std::size_t PriorityMap<KeyType, ValType, Compare, Hash>::Erase(const KeyType& key) {
  if (keys_.find(key) != keys_.end()) {
    auto old_it = keys_[key];
    val_to_keys_[*old_it].erase(key);
    if (val_to_keys_[*old_it].empty()) {
      val_to_keys_.erase(*old_it);  // For now avoid memory bloat
      vals_.erase(old_it);
    }
  }
  return keys_.erase(key);
}

template <typename KeyType, typename ValType, typename Compare, typename Hash>
std::pair<KeyType, ValType> PriorityMap<KeyType, ValType, Compare, Hash>::Top()
    const {
  if (vals_.empty()) {
    throw std::out_of_range("Can't access top on an empty PriorityMap.");
  }
  const auto val = vals_.front();
  if (val_to_keys_.at(val).empty()) {
    throw std::logic_error("Inconsistent state: Val with no keys.");
  }
  // Return a pair consisting of one of the keys and the value.
  return {*(val_to_keys_.at(val).begin()), val};
}

template <typename KeyType, typename ValType, typename Compare, typename Hash>
void PriorityMap<KeyType, ValType, Compare, Hash>::Pop() {
  if (vals_.empty()) {
    throw std::out_of_range("Can't pop from empty PriorityMap.");
  }

  auto old_it = vals_.begin();
  if (val_to_keys_[*old_it].empty()) {
    throw std::logic_error("Inconsistent state: Val with no keys.");
  }
  KeyType key = *(val_to_keys_[*old_it].begin());

  val_to_keys_[*old_it].erase(key);
  keys_.erase(key);

  // Remove node if it's empty
  if (val_to_keys_[*old_it].empty()) {
    val_to_keys_.erase(*old_it);  // For now avoid memory bloat
    vals_.erase(old_it);
  }
}

template <typename KeyType, typename ValType, typename Compare, typename Hash>
void PriorityMap<KeyType, ValType, Compare, Hash>::Insert(
    const KeyType& key, const ValType& new_val) {
  if (keys_.find(key) == keys_.end()) {
    val_to_keys_[0].insert(key);

    // True if minHeap
    if (comp_(0, 1)) {
      // Linear search towards end
      auto insertion_point = std::find_if(
          vals_.begin(), vals_.end(), [&](const auto val) { return val >= 0; });

      if (insertion_point == vals_.end() || (*insertion_point) != 0) {
        insertion_point = vals_.insert(insertion_point, 0);
      }

      keys_[key] = insertion_point;
    } else {  // maxHeap
      // Linear search towards begin
      auto insertion_point =
          std::find_if(vals_.rbegin(), vals_.rend(),
                       [&](const auto val) { return val >= 0; });

      if (insertion_point == vals_.rend() || (*insertion_point) != 0) {
        auto newIt = vals_.insert(insertion_point.base(), 0);
        keys_[key] = newIt;
      } else {
        keys_[key] = std::next(insertion_point).base();
      }
    }
  }

  Update(key, new_val);
}

template <typename KeyType, typename ValType, typename Compare, typename Hash>
void PriorityMap<KeyType, ValType, Compare, Hash>::Update(
    const KeyType& key, const ValType& new_val) {
  auto old_it = keys_[key];

  // Save Old Value
  ValType oldVal = *old_it;

  if (oldVal == new_val) return;

  // Remove the key from the old association
  keys_.erase(key);
  val_to_keys_[oldVal].erase(key);

  // Make new association
  val_to_keys_[new_val].insert(key);

  // True if minHeap and oldVal < new_val or if maxHeap and oldVal > new_val
  if (comp_(oldVal, new_val)) {
    // Linear search towards end
    auto insertion_point =
        std::find_if(old_it, vals_.end(), [&](const auto val) {
          if (comp_(1, 0)) {
            return val <= new_val;
          }
          return val >= new_val;
        });

    if (insertion_point == vals_.end() || (*insertion_point) != new_val) {
      insertion_point = vals_.insert(insertion_point, new_val);
    }

    keys_[key] = insertion_point;
  } else {  // minHeap and oldVal > new_val or maxHeap and oldVal < new_val
    // Need to search in reverse
    typename std::list<ValType>::reverse_iterator old_rit(
        old_it);  // old_rit will point one element closer to begin

    // Linear search towards begin
    auto insertion_point =
        std::find_if(old_rit, vals_.rend(), [&](const auto val) {
          if (comp_(0, 1)) {
            return val <= new_val;
          }
          return val >= new_val;
        });

    if (insertion_point == vals_.rend() || (*insertion_point) != new_val) {
      auto newIt = vals_.insert(insertion_point.base(), new_val);
      keys_[key] = newIt;
      keys_[key] = std::next(insertion_point).base();
    } else {
      keys_[key] = std::next(insertion_point).base();
    }
  }

  // Remove the old node if it's empty
  if (val_to_keys_[oldVal].empty()) {
    val_to_keys_.erase(*old_it);  // For now avoid memory bloat
    vals_.erase(old_it);
  }
}

template <typename KeyType, typename ValType, typename Compare, typename Hash>
typename PriorityMap<KeyType, ValType, Compare, Hash>::Proxy
PriorityMap<KeyType, ValType, Compare, Hash>::operator[](const KeyType& key) {
  // If the key doesn't exist, create a new node with value 0
  if (keys_.find(key) == keys_.end()) {
    Insert(key, 0);
  }
  return Proxy(this, key);
}

}  // namespace wilderfield

#endif  // INCLUDE_WILDERFIELD_PRIORITY_MAP_HPP_
