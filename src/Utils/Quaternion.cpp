#define BIORBD_API_EXPORTS
#include "Utils/Quaternion.h"

#include "Utils/Error.h"
#include "Utils/Matrix3d.h"
#include "Utils/Rotation.h"
#include "Utils/RotoTrans.h"
#include "Utils/Vector.h"
#include "Utils/Vector3d.h"

#ifdef USE_SMOOTH_IF_ELSE
#include "Utils/CasadiExpand.h"
#endif

using namespace BIORBD_NAMESPACE;

utils::Quaternion::Quaternion(const utils::Scalar& kStabilizer)
    : RigidBodyDynamics::Math::Vector4d(1, 0, 0, 0), m_Kstab(kStabilizer) {}

utils::Quaternion::Quaternion(const utils::Quaternion& other)
    : RigidBodyDynamics::Math::Vector4d(other), m_Kstab(other.m_Kstab) {}

utils::Quaternion::Quaternion(
    const RigidBodyDynamics::Math::Vector4d& vec4,
    const utils::Scalar& kStabilizer)
    : RigidBodyDynamics::Math::Vector4d(vec4), m_Kstab(kStabilizer) {}

utils::Quaternion::Quaternion(
    const utils::Scalar& w,
    const utils::Scalar& x,
    const utils::Scalar& y,
    const utils::Scalar& z,
    const utils::Scalar& kStabilizer)
    : RigidBodyDynamics::Math::Vector4d(w, x, y, z), m_Kstab(kStabilizer) {}

utils::Quaternion::Quaternion(
    const utils::Scalar& w,
    const utils::Vector3d& vec3,
    const utils::Scalar& kStabilizer)
    : RigidBodyDynamics::Math::Vector4d(w, vec3[0], vec3[1], vec3[2]),
      m_Kstab(kStabilizer) {}

utils::Scalar utils::Quaternion::w() const { return (*this)(0); }
utils::Scalar utils::Quaternion::x() const { return (*this)(1); }
utils::Scalar utils::Quaternion::y() const { return (*this)(2); }
utils::Scalar utils::Quaternion::z() const { return (*this)(3); }

void utils::Quaternion::setKStab(const utils::Scalar& newKStab) {
  m_Kstab = newKStab;
}

utils::Scalar utils::Quaternion::kStab() const { return m_Kstab; }

utils::Quaternion utils::Quaternion::operator*(
    const utils::Quaternion& q) const {
  return utils::Quaternion(
      (*this)[0] * q[0] - (*this)[1] * q[1] - (*this)[2] * q[2] -
          (*this)[3] * q[3],
      (*this)[0] * q[1] + (*this)[1] * q[0] + (*this)[2] * q[3] -
          (*this)[3] * q[2],
      (*this)[0] * q[2] + (*this)[2] * q[0] + (*this)[3] * q[1] -
          (*this)[1] * q[3],
      (*this)[0] * q[3] + (*this)[3] * q[0] + (*this)[1] * q[2] -
          (*this)[2] * q[1],
      (this->m_Kstab + q.m_Kstab) / 2);
}

utils::Quaternion utils::Quaternion::operator*(
    const utils::Scalar& scalar) const {
  return utils::Quaternion(
      this->RigidBodyDynamics::Math::Vector4d::operator*(scalar),
      this->m_Kstab);
}

utils::Quaternion utils::Quaternion::operator*(float scalar) const {
  return utils::Quaternion(
      this->RigidBodyDynamics::Math::Vector4d::operator*(
          static_cast<double>(scalar)),
      this->m_Kstab);
}

#ifdef BIORBD_USE_CASADI_MATH
utils::Quaternion utils::Quaternion::operator*(double scalar) const {
  return utils::Quaternion(
      this->RigidBodyDynamics::Math::Vector4d::operator*(scalar),
      this->m_Kstab);
}
#endif

utils::Quaternion utils::Quaternion::operator+(
    const utils::Quaternion& other) const {
  return utils::Quaternion(
      this->RigidBodyDynamics::Math::Vector4d::operator+(other),
      (this->m_Kstab + other.m_Kstab) / 2);
}

utils::Quaternion utils::Quaternion::operator-(
    const utils::Quaternion& other) const {
  return utils::Quaternion(
      this->RigidBodyDynamics::Math::Vector4d::operator-(other),
      (this->m_Kstab + other.m_Kstab) / 2);
}

utils::Quaternion utils::Quaternion::fromGLRotate(
    const utils::Scalar& angle,
    const utils::Scalar& x,
    const utils::Scalar& y,
    const utils::Scalar& z,
    const utils::Scalar& kStab) {
  utils::Scalar angle_copy(angle);
  utils::Scalar st = std::sin(angle_copy * M_PI / 360.);
  return utils::Quaternion(
      std::cos(angle_copy * M_PI / 360.), st * x, st * y, st * z, kStab);
}

utils::Quaternion utils::Quaternion::fromAxisAngle(
    const utils::Scalar& angle,
    const utils::Vector3d& axis,
    const utils::Scalar& kStab) {
  utils::Scalar angle_copy(angle);
  utils::Scalar d = axis.norm();
  utils::Scalar s2 = std::sin(angle_copy * 0.5) / d;
  return utils::Quaternion(
      std::cos(angle_copy * 0.5),
      axis[0] * s2,
      axis[1] * s2,
      axis[2] * s2,
      kStab);
}

utils::Quaternion utils::Quaternion::fromMatrix(
    const utils::RotoTrans& rt,
    const utils::Scalar& kStab) {
  return fromMatrix(rt.rot(), kStab);
}

utils::Quaternion utils::Quaternion::fromMatrix(
    const utils::Rotation& mat,
    const utils::Scalar& kStab) {
  utils::Scalar w = std::sqrt(1. + mat(0, 0) + mat(1, 1) + mat(2, 2)) * 0.5;
  return Quaternion(
      w,
      (mat(2, 1) - mat(1, 2)) / (w * 4.),
      (mat(0, 2) - mat(2, 0)) / (w * 4.),
      (mat(1, 0) - mat(0, 1)) / (w * 4.),
      kStab);
}

utils::Quaternion utils::Quaternion::fromZYXAngles(
    const utils::Vector3d& zyx_angles,
    const utils::Scalar& kStab) {
  return fromAxisAngle(zyx_angles[2], utils::Vector3d(0., 0., 1.), kStab) *
         fromAxisAngle(zyx_angles[1], utils::Vector3d(0., 1., 0.), kStab) *
         fromAxisAngle(zyx_angles[0], utils::Vector3d(1., 0., 0.), kStab);
}

utils::Quaternion utils::Quaternion::fromYXZAngles(
    const utils::Vector3d& yxz_angles,
    const utils::Scalar& kStab) {
  return fromAxisAngle(yxz_angles[1], utils::Vector3d(0., 1., 0.), kStab) *
         fromAxisAngle(yxz_angles[0], utils::Vector3d(1., 0., 0.), kStab) *
         fromAxisAngle(yxz_angles[2], utils::Vector3d(0., 0., 1.), kStab);
}

utils::Quaternion utils::Quaternion::fromXYZAngles(
    const utils::Vector3d& xyz_angles,
    const utils::Scalar& kStab) {
  return fromAxisAngle(xyz_angles[0], utils::Vector3d(1., 0., 0.), kStab) *
         fromAxisAngle(xyz_angles[1], utils::Vector3d(0., 1., 0.), kStab) *
         fromAxisAngle(xyz_angles[2], utils::Vector3d(0., 0., 1.), kStab);
}

utils::Rotation utils::Quaternion::toMatrix(bool skipAsserts) const {
#ifndef BIORBD_USE_CASADI_MATH
  if (!skipAsserts) {
    utils::Error::check(
        fabs(this->squaredNorm() - 1.) < 1e-10,
        "The Quaternion norm is not equal to one");
  }
#endif

  utils::Scalar w = (*this)[0];
  utils::Scalar x = (*this)[1];
  utils::Scalar y = (*this)[2];
  utils::Scalar z = (*this)[3];
  utils::Rotation out = utils::Rotation(
      1 - 2 * y * y - 2 * z * z,
      2 * x * y - 2 * w * z,
      2 * x * z + 2 * w * y,
      2 * x * y + 2 * w * z,
      1 - 2 * x * x - 2 * z * z,
      2 * y * z - 2 * w * x,
      2 * x * z - 2 * w * y,
      2 * y * z + 2 * w * x,
      1 - 2 * x * x - 2 * y * y);
  return out;
}

#ifndef BIORBD_USE_CASADI_MATH
utils::Quaternion utils::Quaternion::slerp(
    double alpha,
    const utils::Quaternion& quat) const {
  // check whether one of the two has 0 length
  utils::Scalar s = std::sqrt(squaredNorm() * quat.squaredNorm());

  // division by 0.f is unhealthy!
#ifndef BIORBD_USE_CASADI_MATH
  assert(s != 0.);
#endif

  utils::Scalar angle = acos(dot(quat) / s);
#ifdef BIORBD_USE_CASADI_MATH
  if (true) {
#else
  if (angle == 0. || std::isnan(angle)) {
#endif
    return *this;
  }

  utils::Scalar d = 1. / std::sin(angle);
  utils::Scalar p0 = std::sin((1. - alpha) * angle);
  utils::Scalar p1 = std::sin(alpha * angle);

#ifdef BIORBD_USE_CASADI_MATH
  return Quaternion(
      IF_ELSE_NAMESPACE::if_else(
          IF_ELSE_NAMESPACE::lt(dot(quat), 0.),
          RigidBodyDynamics::Math::Vector4d(((*this) * p0 - quat * p1) * d),
          RigidBodyDynamics::Math::Vector4d(((*this) * p0 + quat * p1) * d)),
      (this->m_Kstab + quat.m_Kstab) / 2);

#else
  if (dot(quat) < 0.) {
    return Quaternion(((*this) * p0 - quat * p1) * d, this->m_Kstab);
  }
  return Quaternion(
      ((*this) * p0 + quat * p1) * d, (this->m_Kstab + quat.m_Kstab) / 2);
#endif
}
#endif

utils::Quaternion utils::Quaternion::conjugate() const {
  return utils::Quaternion(
      (*this)[0], -(*this)[1], -(*this)[2], -(*this)[3], this->kStab());
}

utils::Quaternion utils::Quaternion::timeStep(
    const utils::Vector3d& omega,
    const utils::Scalar& dt) {
  utils::Scalar omega_norm = omega.norm();
  return fromAxisAngle(dt * omega_norm, omega / omega_norm, this->m_Kstab) *
         (*this);
}

utils::Vector3d utils::Quaternion::rotate(const utils::Vector3d& vec) const {
  utils::Quaternion vec_quat(0., vec);

  utils::Quaternion res_quat(vec_quat * (*this));
  res_quat = conjugate() * res_quat;

  return utils::Vector3d(res_quat[1], res_quat[2], res_quat[3]);
}

#include <iostream>
utils::Quaternion utils::Quaternion::omegaToQuatDot(
    const utils::Vector3d& omega) const {
  RigidBodyDynamics::Math::MatrixNd m(4, 3);
  m(0, 0) = -(*this)[1];
  m(0, 1) = -(*this)[2];
  m(0, 2) = -(*this)[3];
  m(1, 0) = (*this)[0];
  m(1, 1) = -(*this)[3];
  m(1, 2) = (*this)[2];
  m(2, 0) = (*this)[3];
  m(2, 1) = (*this)[0];
  m(2, 2) = -(*this)[1];
  m(3, 0) = -(*this)[2];
  m(3, 1) = (*this)[1];
  m(3, 2) = (*this)[0];

  return utils::Quaternion(0.5 * m * omega, this->m_Kstab);
}

utils::Matrix3d utils::Quaternion::velocityMatrix(
    const utils::Vector3d& euler,
    const utils::String& seq) {
  utils::Matrix3d baseMatrix = utils::Matrix3d::fromEulerSequence(seq);
  utils::Vector3d rot1(euler[0], 0, 0);
  utils::Vector3d rot2(euler[0], euler[1], 0);
  utils::Matrix3d RotMat1 = utils::Rotation::fromEulerAngles(rot1, seq);
  utils::Matrix3d RotMat2 = utils::Rotation::fromEulerAngles(rot2, seq);

  utils::Matrix3d result;
  result.block(0, 0, 3, 1) = baseMatrix.block(0, 0, 3, 1);
  result.block(0, 1, 3, 1) = RotMat1 * baseMatrix.block(0, 1, 3, 1);
  result.block(0, 2, 3, 1) = RotMat2 * baseMatrix.block(0, 2, 3, 1);
  return result;
}

utils::Vector3d utils::Quaternion::eulerDotToOmega(
    const utils::Vector3d& euler,
    const utils::Vector3d& eulerDot,
    const utils::String& seq) {
  utils::Matrix3d velocity_matrix = velocityMatrix(euler, seq);
  utils::Vector3d w = velocity_matrix * eulerDot;

  return w;
}

utils::Vector3d utils::Quaternion::omegaToEulerDot(
    const utils::Vector3d& euler,
    const utils::Vector3d& w,
    const utils::String& seq) {
  utils::Matrix3d velocity_matrix = velocityMatrix(euler, seq).inverse();
  utils::Vector3d eulerDot = velocity_matrix * w;
  return eulerDot;
}

void utils::Quaternion::derivate(const utils::Vector& w) {
  // Création du quaternion de "préproduit vectoriel"
#ifdef BIORBD_USE_CASADI_MATH
  utils::Scalar qw = (*this)(0);
  utils::Scalar qx = (*this)(1);
  utils::Scalar qy = (*this)(2);
  utils::Scalar qz = (*this)(3);
#else
  utils::Scalar& qw = (*this)(0);
  utils::Scalar& qx = (*this)(1);
  utils::Scalar& qy = (*this)(2);
  utils::Scalar& qz = (*this)(3);
#endif
  RigidBodyDynamics::Math::Matrix4d Q = RigidBodyDynamics::Math::Matrix4d(
      qw, -qx, -qy, -qz, qx, qw, -qz, qy, qy, qz, qw, -qx, qz, -qy, qx, qw);

  // Ajout du paramètre de stabilisation
  RigidBodyDynamics::Math::Vector4d w_tp(
      m_Kstab * w.norm() * (1 - this->norm()), w(0), w(1), w(2));
  RigidBodyDynamics::Math::Vector4d newQuat(0.5 * Q * w_tp);

  // Assigning is slightly faster than create a new Quaternion
  qw = newQuat[0];
  qx = newQuat[1];
  qy = newQuat[2];
  qz = newQuat[3];
#ifdef BIORBD_USE_CASADI_MATH
  *this = utils::Quaternion(qw, qx, qy, qz, m_Kstab);
#endif
}

void utils::Quaternion::normalize() { *this = *this / this->norm(); }
