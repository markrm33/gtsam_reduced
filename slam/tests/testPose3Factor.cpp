/**
 *  @file  testPose3Pose.cpp
 *  @brief Unit tests for Pose3Factor Class
 *  @authors Frank Dellaert
 **/

#include <iostream>
#include <boost/assign/std/list.hpp>
using namespace boost::assign;

#include <gtsam/CppUnitLite/TestHarness.h>
#include <gtsam/slam/pose3SLAM.h>

using namespace std;
using namespace gtsam;

/* ************************************************************************* */
TEST( Pose3Factor, error )
{
	// Create example
	Pose3 t1; // origin
	Pose3 t2(rodriguez(0.1,0.2,0.3),Point3(0,1,0));
	Pose3 z(rodriguez(0.2,0.2,0.3),Point3(0,1.1,0));;

	// Create factor
	SharedGaussian I6(noiseModel::Unit::Create(6));
	Pose3Factor factor(1,2, z, I6);

	// Create config
	Pose3Config x;
	x.insert(1,t1);
	x.insert(2,t2);

	// Get error h(x)-z -> logmap(z,h(x)) = logmap(z,between(t1,t2))
	Vector actual = factor.unwhitenedError(x);
	Vector expected = logmap(z,between(t1,t2));
	CHECK(assert_equal(expected,actual));
}

/* ************************************************************************* */
int main() {
	TestResult tr;
	return TestRegistry::runAllTests(tr);
}
/* ************************************************************************* */
