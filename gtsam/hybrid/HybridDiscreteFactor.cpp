/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 *  @file HybridDiscreteFactor.cpp
 *  @brief Wrapper for a discrete factor
 *  @date Mar 11, 2022
 *  @author Fan Jiang
 */

#include <gtsam/hybrid/HybridDiscreteFactor.h>

#include <boost/make_shared.hpp>
#include "gtsam/discrete/DecisionTreeFactor.h"

namespace gtsam {

// TODO(fan): THIS IS VERY VERY DIRTY! We need to get DiscreteFactor right!
HybridDiscreteFactor::HybridDiscreteFactor(DiscreteFactor::shared_ptr other)
    : Base(boost::dynamic_pointer_cast<DecisionTreeFactor>(other)->discreteKeys()) {
  inner = other;

}

HybridDiscreteFactor::HybridDiscreteFactor(DecisionTreeFactor &&dtf)
    : Base(dtf.discreteKeys()),
      inner(boost::make_shared<DecisionTreeFactor>(std::move(dtf))) {}

bool HybridDiscreteFactor::equals(const HybridFactor &lf, double tol) const {
  return false;
}

void HybridDiscreteFactor::print(const std::string &s,
                                 const KeyFormatter &formatter) const {
  HybridFactor::print(s, formatter);
  inner->print("inner: ", formatter);
};

}  // namespace gtsam