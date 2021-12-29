/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file DiscreteKey.h
 * @brief specialized key for discrete variables
 * @author Frank Dellaert
 * @date Feb 28, 2011
 */

#pragma once

#include <gtsam/global_includes.h>
#include <gtsam/inference/Key.h>

#include <map>
#include <string>
#include <vector>

namespace gtsam {

  /**
   * Key type for discrete conditionals
   * Includes name and cardinality
   */
  using DiscreteKey = std::pair<Key,size_t>;

  /// DiscreteKeys is a set of keys that can be assembled using the & operator
  struct GTSAM_EXPORT DiscreteKeys: public std::vector<DiscreteKey> {

    // Forward all constructors.
    using std::vector<DiscreteKey>::vector;

    /// Constructor for serialization
    DiscreteKeys() : std::vector<DiscreteKey>::vector() {}

    /// Construct from a key
    explicit DiscreteKeys(const DiscreteKey& key) { push_back(key); }

    /// Construct from a vector of keys
    DiscreteKeys(const std::vector<DiscreteKey>& keys) :
      std::vector<DiscreteKey>(keys) {
    }

    /// Construct from cardinalities with default names
    DiscreteKeys(const std::vector<int>& cs);

    /// Return a vector of indices
    KeyVector indices() const;

    /// Return a map from index to cardinality
    std::map<Key,size_t> cardinalities() const;

    /// Add a key (non-const!)
    DiscreteKeys& operator&(const DiscreteKey& key) {
      push_back(key);
      return *this;
    }
  }; // DiscreteKeys

  /// Create a list from two keys
  DiscreteKeys operator&(const DiscreteKey& key1, const DiscreteKey& key2);
}
