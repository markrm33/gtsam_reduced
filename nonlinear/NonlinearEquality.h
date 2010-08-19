/*
 * @file NonlinearEquality.h
 * @brief Factor to handle enforced equality between factors
 * @author Alex Cunningham
 */

#pragma once

#include <limits>
#include <iostream>

#include <gtsam/inference/Key.h>
#include <gtsam/nonlinear/NonlinearFactor.h>

namespace gtsam {

	/**
	 * Template default compare function that assumes a testable T
	 */
	template<class T>
	bool compare(const T& a, const T& b) { return a.equals(b); }

	/**
	 * An equality factor that forces either one variable to a constant,
	 * or a set of variables to be equal to each other.
	 *
	 * Depending on flag, throws an error at linearization if the constraints are not met.
	 *
	 * Switchable implementation:
	 *   - ALLLOW_ERROR : if we allow that there can be nonzero error, does not throw, and uses gain
	 *   - ONLY_EXACT   : throws error at linearization if not at exact feasible point, and infinite error
	 */
	template<class Config, class Key, class T>
	class NonlinearEquality: public NonlinearFactor1<Config, Key, T> {
	private:

		// feasible value
		T feasible_;

		// error handling flag
		bool allow_error_;

		// error gain in allow error case
		double error_gain_;

	public:

		/**
		 * Function that compares two values
		 */
		bool (*compare_)(const T& a, const T& b);

		typedef NonlinearFactor1<Config, Key, T> Base;

		/**
		 * Constructor - forces exact evaluation
		 */
		NonlinearEquality(const Key& j, const T& feasible, bool (*compare)(const T&, const T&) = compare<T>) :
			Base(noiseModel::Constrained::All(dim(feasible)), j), feasible_(feasible),
			allow_error_(false), error_gain_(std::numeric_limits<double>::infinity()),
			compare_(compare) {
		}

		/**
		 * Constructor - allows inexact evaluation
		 */
		NonlinearEquality(const Key& j, const T& feasible, double error_gain, bool (*compare)(const T&, const T&) = compare<T>) :
			Base(noiseModel::Constrained::All(dim(feasible)), j), feasible_(feasible),
			allow_error_(true), error_gain_(error_gain),
			compare_(compare) {
		}

		void print(const std::string& s = "") const {
			std::cout << "Constraint: " << s << " on [" << (std::string)(this->key_) << "]\n";
			gtsam::print(feasible_,"Feasible Point");
			std::cout << "Variable Dimension: " << dim(feasible_) << std::endl;
		}

		/** Check if two factors are equal */
		bool equals(const Factor<Config>& f, double tol = 1e-9) const {
			const NonlinearEquality<Config,Key,T>* p =
					dynamic_cast<const NonlinearEquality<Config,Key,T>*> (&f);
			if (p == NULL) return false;
			if (!Base::equals(*p)) return false;
			return compare_(feasible_, p->feasible_);
		}

		/** actual error function calculation */
		virtual double error(const Config& c) const {
			const T& xj = c[this->key_];
			Vector e = this->unwhitenedError(c);
			if (allow_error_ || !compare_(xj, feasible_)) {
				return error_gain_ * inner_prod(e,e);
			} else {
				return 0.0;
			}
		}

		/** error function */
		inline Vector evaluateError(const T& xj, boost::optional<Matrix&> H = boost::none) const {
			size_t nj = dim(feasible_);
			if (allow_error_) {
				if (H) *H = eye(nj); // FIXME: this is not the right linearization for nonlinear compare
				return logmap(xj, feasible_);
			} else if (compare_(feasible_,xj)) {
				if (H) *H = eye(nj);
				return zero(nj); // set error to zero if equal
			} else {
				if (H) throw std::invalid_argument(
						"Linearization point not feasible for " + (std::string)(this->key_) + "!");
				return repeat(nj, std::numeric_limits<double>::infinity()); // set error to infinity if not equal
			}
		}

		// Linearize is over-written, because base linearization tries to whiten
		virtual boost::shared_ptr<GaussianFactor> linearize(const Config& x) const {
			const T& xj = x[this->key_];
			Matrix A;
			Vector b = evaluateError(xj, A);
			// TODO pass unwhitened + noise model to Gaussian factor
			SharedDiagonal model = noiseModel::Constrained::All(b.size());
			return	GaussianFactor::shared_ptr(new GaussianFactor(this->key_, A, b, model));
		}

	}; // NonlinearEquality

} // namespace gtsam

