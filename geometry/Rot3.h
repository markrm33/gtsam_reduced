/**
 * @file    Rot3.h
 * @brief   Rotation
 * @author  Alireza Fathi
 * @author  Christian Potthast
 * @author  Frank Dellaert
 */

// \callgraph

#pragma once

#include <boost/math/constants/constants.hpp>
#include <gtsam/geometry/Point3.h>
#include <gtsam/base/Testable.h>
#include <gtsam/base/Lie.h>

namespace gtsam {

  /* 3D Rotation */
  class Rot3: Testable<Rot3>, public Lie<Rot3> {
  private:
    /** we store columns ! */
    Point3 r1_, r2_, r3_;  

  public:

    /** default constructor, unit rotation */
    Rot3() :
      r1_(Point3(1.0,0.0,0.0)),
      r2_(Point3(0.0,1.0,0.0)),
      r3_(Point3(0.0,0.0,1.0)) {}

    /** constructor from columns */
    Rot3(const Point3& r1, const Point3& r2, const Point3& r3) :
      r1_(r1), r2_(r2), r3_(r3) {}

    /**  constructor from vector */
    Rot3(const Vector &v) :
      r1_(Point3(v(0),v(1),v(2))),
      r2_(Point3(v(3),v(4),v(5))),
      r3_(Point3(v(6),v(7),v(8))) {}

    /** constructor from doubles in *row* order !!! */
    Rot3(double R11, double R12, double R13,
        double R21, double R22, double R23,
        double R31, double R32, double R33) :
          r1_(Point3(R11, R21, R31)),
          r2_(Point3(R12, R22, R32)),
          r3_(Point3(R13, R23, R33)) {}

    /** constructor from matrix */
    Rot3(const Matrix& R):
      r1_(Point3(R(0,0), R(1,0), R(2,0))),
      r2_(Point3(R(0,1), R(1,1), R(2,1))),
      r3_(Point3(R(0,2), R(1,2), R(2,2))) {}

    /** Static member function to generate some well known rotations */

    /**
     * Rotations around axes as in http://en.wikipedia.org/wiki/Rotation_matrix
     * Counterclockwise when looking from unchanging axis.
     */
    static Rot3 Rx(double t);
    static Rot3 Ry(double t);
    static Rot3 Rz(double t);
    static Rot3 RzRyRx(double x, double y, double z);

    /**
     * Tait-Bryan system from Spatial Reference Model (SRM) (x,y,z) = (roll,pitch,yaw)
     * as described in http://www.sedris.org/wg8home/Documents/WG80462.pdf
     * Assumes vehicle coordinate frame X forward, Y right, Z down
     */
    static Rot3 yaw  (double t) { return Rz(t);} // positive yaw is to right (as in aircraft heading)
    static Rot3 pitch(double t) { return Ry(t);} // positive pitch is up (increasing aircraft altitude)
    static Rot3 roll (double t) { return Rx(t);} // positive roll is to right (increasing yaw in aircraft)
    static Rot3 ypr  (double y, double p, double r) { return RzRyRx(r,p,y);}

    /** print */
    void print(const std::string& s="R") const { gtsam::print(matrix(), s);}

    /** equals with an tolerance */
    bool equals(const Rot3& p, double tol = 1e-9) const;

    /** return 3*3 rotation matrix */
    Matrix matrix() const;

    /** return 3*3 transpose (inverse) rotation matrix   */
    Matrix transpose() const;

    /** returns column vector specified by index */
    Point3 column(int index) const;
    Point3 r1() const { return r1_; }
    Point3 r2() const { return r2_; }
    Point3 r3() const { return r3_; }

    /**
     * Use RQ to calculate xyz angle representation
     * @return a vector containing x,y,z s.t. R = Rot3::RzRyRx(x,y,z)
     */
    Vector xyz() const;

    /**
     * Use RQ to calculate yaw-pitch-roll angle representation
     * @return a vector containing ypr s.t. R = Rot3::ypr(y,p,r)
     */
    Vector ypr() const;

    /** get the dimension by the type */
    static inline size_t dim() { return 3; };

    /* Find the inverse rotation R^T s.t. inverse(R)*R = I */
    inline Rot3 inverse() const {
      return Rot3(
          r1_.x(), r1_.y(), r1_.z(),
          r2_.x(), r2_.y(), r2_.z(),
          r3_.x(), r3_.y(), r3_.z());
    }

    /** compose two rotations */
    Rot3 operator*(const Rot3& R2) const {
      return Rot3(rotate(R2.r1_), rotate(R2.r2_), rotate(R2.r3_));
    }

    /**
     * rotate point from rotated coordinate frame to
     * world = R*p
     */
    Point3 rotate(const Point3& p) const
			{return r1_ * p.x() + r2_ * p.y() + r3_ * p.z();}
    inline Point3 operator*(const Point3& p) const { return rotate(p);}

  private:
    /** Serialization function */
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
      ar & BOOST_SERIALIZATION_NVP(r1_);
      ar & BOOST_SERIALIZATION_NVP(r2_);
      ar & BOOST_SERIALIZATION_NVP(r3_);
    }
  };

  /** Global print calls member function */
  inline void print(const Rot3& r, std::string& s) { r.print(s); }
  inline void print(const Rot3& r) { r.print(); }

  /**
   * Rodriguez' formula to compute an incremental rotation matrix
   * @param   w is the rotation axis, unit length
   * @param   theta rotation angle
   * @return incremental rotation matrix
   */
  Rot3 rodriguez(const Vector& w, double theta);

  /**
   * Rodriguez' formula to compute an incremental rotation matrix
   * @param v a vector of incremental roll,pitch,yaw
   * @return incremental rotation matrix
   */
  Rot3 rodriguez(const Vector& v);

  /**
   * Rodriguez' formula to compute an incremental rotation matrix
   * @param wx
   * @param wy
   * @param wz
   * @return incremental rotation matrix
   */
  inline Rot3 rodriguez(double wx, double wy, double wz)
		{ return rodriguez(Vector_(3,wx,wy,wz));}

  /** return DOF, dimensionality of tangent space */
  inline size_t dim(const Rot3&) { return 3; }

  // Exponential map at identity - create a rotation from canonical coordinates
  // using Rodriguez' formula
  template<> inline Rot3 expmap(const Vector& v) {
    if(zero(v)) return Rot3();
    else return rodriguez(v);
  }

  // Log map at identity - return the canonical coordinates of this rotation
  Vector logmap(const Rot3& R);

  // Compose two rotations
  inline Rot3 compose(const Rot3& R1, const Rot3& R2) { return R1*R2;}

  // Find the inverse rotation R^T s.t. inverse(R)*R = Rot3()
  inline Rot3 inverse(const Rot3& R) { return R.inverse();}

  // and its derivative
  inline Matrix Dinverse(Rot3 R) { return -R.matrix();}

  /**
   * rotate point from rotated coordinate frame to 
   * world = R*p
   */
  inline Point3 rotate(const Rot3& R, const Point3& p) { return R*p;}
  Matrix Drotate1(const Rot3& R, const Point3& p);
  Matrix Drotate2(const Rot3& R); // does not depend on p !

  /**
   * rotate point from world to rotated 
   * frame = R'*p
   */
  Point3 unrotate(const Rot3& R, const Point3& p);
  Point3 unrotate(const Rot3& R, const Point3& p,
  	boost::optional<Matrix&> H1, boost::optional<Matrix&> H2);

  /**
   * compose two rotations i.e., R=R1*R2
   */
  //Rot3    compose (const Rot3& R1, const Rot3& R2);
  Matrix Dcompose1(const Rot3& R1, const Rot3& R2);
  Matrix Dcompose2(const Rot3& R1, const Rot3& R2);

  /**
   * Return relative rotation D s.t. R2=D*R1, i.e. D=R2*R1'
   */
  //Rot3    between (const Rot3& R1, const Rot3& R2);
  Matrix Dbetween1(const Rot3& R1, const Rot3& R2);
  Matrix Dbetween2(const Rot3& R1, const Rot3& R2);

  /**
   * [RQ] receives a 3 by 3 matrix and returns an upper triangular matrix R
   * and 3 rotation angles corresponding to the rotation matrix Q=Qz'*Qy'*Qx'
   * such that A = R*Q = R*Qz'*Qy'*Qx'. When A is a rotation matrix, R will
   * be the identity and Q is a yaw-pitch-roll decomposition of A.
   * The implementation uses Givens rotations and is based on Hartley-Zisserman.
   * @param a 3 by 3 matrix A=RQ
   * @return an upper triangular matrix R
   * @return a vector [thetax, thetay, thetaz] in radians.
   */
  std::pair<Matrix,Vector> RQ(const Matrix& A);

}
