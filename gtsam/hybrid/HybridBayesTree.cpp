/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file    HybridBayesTree.cpp
 * @brief   Hybrid Bayes Tree, the result of eliminating a
 * HybridJunctionTree
 * @date Mar 11, 2022
 * @author  Fan Jiang
 */

#include <gtsam/base/treeTraversal-inst.h>
#include <gtsam/hybrid/HybridBayesNet.h>
#include <gtsam/hybrid/HybridBayesTree.h>
#include <gtsam/inference/BayesTree-inst.h>
#include <gtsam/inference/BayesTreeCliqueBase-inst.h>

namespace gtsam {

// Instantiate base class
template class BayesTreeCliqueBase<HybridBayesTreeClique,
                                   HybridGaussianFactorGraph>;
template class BayesTree<HybridBayesTreeClique>;

/* ************************************************************************* */
bool HybridBayesTree::equals(const This& other, double tol) const {
  return Base::equals(other, tol);
}

/* ************************************************************************* */
VectorValues HybridBayesTree::optimize(const DiscreteValues& assignment) const {
  GaussianBayesNet gbn;

  KeyVector added_keys;

  // Iterate over all the nodes in the BayesTree
  for (auto&& node : nodes()) {
    // Check if conditional being added is already in the Bayes net.
    if (std::find(added_keys.begin(), added_keys.end(), node.first) ==
        added_keys.end()) {
      // Access the clique and get the underlying hybrid conditional
      HybridBayesTreeClique::shared_ptr clique = node.second;
      HybridConditional::shared_ptr conditional = clique->conditional();

      KeyVector frontals(conditional->frontals().begin(),
                         conditional->frontals().end());

      // Record the key being added
      added_keys.insert(added_keys.end(), frontals.begin(), frontals.end());

      // If conditional is hybrid (and not discrete-only), we get the Gaussian
      // Conditional corresponding to the assignment and add it to the Gaussian
      // Bayes Net.
      if (conditional->isHybrid()) {
        auto gm = conditional->asMixture();
        GaussianConditional::shared_ptr gaussian_conditional =
            (*gm)(assignment);

        gbn.push_back(gaussian_conditional);
      }
    }
  }
  // Return the optimized bayes net.
  return gbn.optimize();
}

}  // namespace gtsam
