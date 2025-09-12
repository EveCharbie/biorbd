#ifndef BIORBD_UTILS_GENERALIZED_ACCELERATION_H
#define BIORBD_UTILS_GENERALIZED_ACCELERATION_H

#include "biorbdConfig.h"

#include "Utils/Vector.h"

namespace BIORBD_NAMESPACE {
namespace rigidbody {
class Joints;

///
/// \brief Class GeneralizedAcceleration
///
class BIORBD_API GeneralizedAcceleration : public utils::Vector {
 public:
  ///
  /// \brief Construct generalized acceleration vector
  ///
  GeneralizedAcceleration();

  ///
  /// \brief Create generalized acceleration vector of dimension nQddot
  /// \param nQddot number of degrees-of-freedom
  ///
  GeneralizedAcceleration(size_t nQddot);

  ///
  /// \brief Create generalized acceleration vector from a joint Model
  /// \param j The joint model
  ///
  GeneralizedAcceleration(const rigidbody::Joints& j);

  ///
  /// \brief Construct generalized acceleration vector
  /// \param Q State vector of the internal joints
  ///
  GeneralizedAcceleration(const GeneralizedAcceleration& Q);

  ///
  /// \brief Construct vector from Casadi vector
  /// \param v The vector to copy
  ///
  GeneralizedAcceleration(const RigidBodyDynamics::Math::VectorNd& v);

#ifdef BIORBD_USE_EIGEN3_MATH

  ///
  /// \brief Construct generalized acceleration vector from another vector
  /// \param other Eigen matrix
  ///
  template <typename OtherDerived>
  GeneralizedAcceleration(const Eigen::MatrixBase<OtherDerived>& other)
      : utils::Vector(other) {}

#endif

#ifdef BIORBD_USE_CASADI_MATH

  ///
  /// \brief Construct vector from Casadi vector
  /// \param v The vector to copy
  ///
  GeneralizedAcceleration(const casadi::MX& v);

#endif

  ///
  /// \brief Destroy the class properly
  ///
  virtual ~GeneralizedAcceleration();

#ifndef SWIG

#ifdef BIORBD_USE_EIGEN3_MATH

  ///
  /// \brief Allows for operator= to be used
  /// \param other
  /// \return The current Generalized Coordinate
  ///
  template <typename OtherDerived>
  GeneralizedAcceleration& operator=(
      const Eigen::MatrixBase<OtherDerived>& other) {
    this->utils::Vector::operator=(other);
    return *this;
  }

#endif

  ///
  /// \brief operator= For submatrices
  /// \param other The vector to copy
  ///
  void operator=(const utils::Vector& other);
#ifdef BIORBD_USE_CASADI_MATH

  ///
  /// \brief operator= For submatrices
  /// \param The vector to copy
  ///
  void operator=(const RBDLCasadiMath::MX_Xd_SubMatrix& other);

  ///
  /// \brief operator= For casadi
  /// \param The vector to copy
  ///
  void operator=(const casadi::MX& other);
#endif

#endif
};

}  // namespace rigidbody
}  // namespace BIORBD_NAMESPACE

#endif  // BIORBD_UTILS_GENERALIZED_ACCELERATION_H
