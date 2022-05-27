/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 *  @file HybridGaussianFactor.h
 *  @date Mar 11, 2022
 *  @author Fan Jiang
 */

#pragma once

#include <gtsam/hybrid/HybridFactor.h>
#include <gtsam/linear/GaussianFactor.h>
#include <gtsam/linear/JacobianFactor.h>

namespace gtsam {

/**
 * A HybridGaussianFactor is a layer over GaussianFactor so that we do not have
 * a diamond inheritance.
 */
class HybridGaussianFactor : public HybridFactor {
 private:
  GaussianFactor::shared_ptr inner_;

 public:
  using Base = HybridFactor;
  using This = HybridGaussianFactor;
  using shared_ptr = boost::shared_ptr<This>;

  // Explicit conversion from a shared ptr of GF
  explicit HybridGaussianFactor(GaussianFactor::shared_ptr other);

  // Forwarding constructor from concrete JacobianFactor
  explicit HybridGaussianFactor(JacobianFactor &&jf);

 public:
  virtual bool equals(const HybridFactor &lf, double tol) const override;

  void print(
      const std::string &s = "HybridFactor\n",
      const KeyFormatter &formatter = DefaultKeyFormatter) const override;

  GaussianFactor::shared_ptr inner() const { return inner_; }
};
}  // namespace gtsam
