/**
 * @file    ISAM-inl.h
 * @brief   Incremental update functionality (iSAM) for BayesTree.
 * @author  Michael Kaess
 */

#include <boost/foreach.hpp>
#include <boost/assign/std/list.hpp> // for operator +=
using namespace boost::assign;

#include <gtsam/inference/Conditional.h>
#include <gtsam/inference/ISAM.h>
#include <gtsam/inference/BayesTree-inl.h>

namespace gtsam {

	using namespace std;

	/** Create an empty Bayes Tree */
	template<class Conditional>
	ISAM<Conditional>::ISAM() : BayesTree<Conditional>() {}

	/** Create a Bayes Tree from a Bayes Net */
	template<class Conditional>
	ISAM<Conditional>::ISAM(const BayesNet<Conditional>& bayesNet) :
	  BayesTree<Conditional>(bayesNet) {}

	/* ************************************************************************* */
	template<class Conditional>
	template<class Factor>
	void ISAM<Conditional>::update_internal(const FactorGraph<Factor>& newFactors, Cliques& orphans) {

		// Remove the contaminated part of the Bayes tree
		BayesNet<Conditional> bn;
		removeTop(newFactors.keys(), bn, orphans);
		FactorGraph<Factor> factors(bn);

		// add the factors themselves
		factors.push_back(newFactors);

		// create an ordering for the new and contaminated factors
		Ordering ordering;
#ifndef SORT_KEYS
			ordering = factors.getOrdering();
#else
			list<Symbol> keys = factors.keys();
			keys.sort(); // todo: correct sorting order?
			ordering = keys;
#endif

		// Create Index from ordering
		IndexTable<Symbol> index(ordering);

		// eliminate into a Bayes net
		BayesNet<Conditional> bayesNet = eliminate<Factor, Conditional>(factors,ordering);

		// insert conditionals back in, straight into the topless bayesTree
		typename BayesNet<Conditional>::const_reverse_iterator rit;
		for ( rit=bayesNet.rbegin(); rit != bayesNet.rend(); ++rit )
			this->insert(*rit, index);

		// add orphans to the bottom of the new tree
		BOOST_FOREACH(sharedClique orphan, orphans) {

			Symbol parentRepresentative = findParentClique(orphan->separator_, index);
			sharedClique parent = (*this)[parentRepresentative];

			parent->children_ += orphan;
			orphan->parent_ = parent; // set new parent!
		}

	}

	template<class Conditional>
	template<class Factor>
	void ISAM<Conditional>::update(const FactorGraph<Factor>& newFactors) {
		Cliques orphans;
		this->update_internal<Factor>(newFactors, orphans);
	}

}
/// namespace gtsam
