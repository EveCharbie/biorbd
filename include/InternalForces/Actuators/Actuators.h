#ifndef BIORBD_ACTUATORS_ACTUATORS_H
#define BIORBD_ACTUATORS_ACTUATORS_H

#include "biorbdConfig.h"

#include <memory>
#include <vector>

#include "Utils/Scalar.h"

namespace BIORBD_NAMESPACE {
namespace utils {
class Vector;
}

namespace rigidbody {
class GeneralizedCoordinates;
class GeneralizedVelocity;
class GeneralizedTorque;
}  // namespace rigidbody

namespace internal_forces {
namespace actuator {
class Actuator;
///
/// \brief Class holder for a set of actuators
///
class BIORBD_API Actuators {
 public:
  ///
  /// \brief Construct actuators
  ///
  Actuators();

  ///
  /// \brief Construct actuators from another set of actuators
  /// \param other The other actuators
  ///
  Actuators(const Actuators& other);

  ///
  /// \brief Destroy actuators class properly
  ///
  virtual ~Actuators();

  ///
  /// \brief Deep copy of the actuator holder from other actuator holder
  /// \param other The other actuators
  ///
  void DeepCopy(const Actuators& other);

  ///
  /// \brief Add an actuator to the set of actuators
  /// \param a The actuator to add
  ///
  void addActuator(const Actuator& a);

  ///
  /// \brief Indicate to biorbd to are done adding actuators, sanity checks are
  /// performed
  ///
  void closeActuator();

  ///
  /// \brief Return two vectors of max torque (it is impossible to know if
  /// eccentric or concentric is required, therefore both are returned)
  /// \param Q The generalized coordinates of the actuators
  /// \param Qdot The generalized velocities of the actuators
  /// \return Two vectors of maximal torque
  ///
  std::pair<rigidbody::GeneralizedTorque, rigidbody::GeneralizedTorque>
  torqueMax(
      const rigidbody::GeneralizedCoordinates& Q,
      const rigidbody::GeneralizedVelocity& Qdot);

  ///
  /// \brief Return the maximal generalized torque
  /// \param activation The level of activation of the torque. A positive value
  /// is interpreted as concentric contraction and negative as eccentric
  /// contraction
  /// \param Q The generalized coordinates of the actuators
  /// \param Qdot The generalized velocities of the actuators
  /// \return The maximal generalized torque
  ///
  rigidbody::GeneralizedTorque torqueMax(
      const utils::Vector& activation,
      const rigidbody::GeneralizedCoordinates& Q,
      const rigidbody::GeneralizedVelocity& Qdot);

  ///
  /// \brief Return the generalized torque
  /// \param activation The level of activation of the torque. A positive value
  /// is interpreted as concentric contraction and negative as eccentric
  /// contraction
  /// \param Q The generalized coordinates of the actuators
  /// \param Qdot The generalized velocities of the actuators
  /// \return The maximal generalized torque
  ///
  rigidbody::GeneralizedTorque torque(
      const utils::Vector& activation,
      const rigidbody::GeneralizedCoordinates& Q,
      const rigidbody::GeneralizedVelocity& Qdot);

  // Get and set
  ///
  /// \brief Return a specific concentric/eccentric actuator
  /// \param dof Index of the DoF associated with actuator
  /// \return The actuator
  ///
  const std::pair<std::shared_ptr<Actuator>, std::shared_ptr<Actuator>>&
  actuator(size_t dof);

  ///
  /// \brief Return a specific actuator
  /// \param dof Index of the DoF associated with actuator
  /// \param concentric If the return value is the concentric (true) or
  /// eccentric (false) value
  /// \return The actuator
  ///
  const Actuator& actuator(size_t dof, bool concentric);

  ///
  /// \brief Return the toal number of actuators
  /// \return The total number of actuators
  ///
  size_t nbActuators() const;

 protected:
  std::shared_ptr<std::vector<
      std::pair<std::shared_ptr<Actuator>, std::shared_ptr<Actuator>>>>
      m_all;  ///< All the actuators reunited /pair (+ or -)
  std::shared_ptr<std::vector<bool>> m_isDofSet;  ///< If DoF all dof are set
  std::shared_ptr<bool> m_isClose;                ///< If the set is ready

  ///
  /// \brief getTorqueMaxDirection Get the max torque of a specific actuator
  /// (interface necessary because of CasADi)
  /// \param actuator The actuator to gather from
  /// \param Q The Generalized coordinates
  /// \param Qdot The Generalized velocity
  /// \return The torque max
  ///
  utils::Scalar getTorqueMaxDirection(
      const std::shared_ptr<Actuator> actuator,
      const rigidbody::GeneralizedCoordinates& Q,
      const rigidbody::GeneralizedVelocity& Qdot) const;
};

}  // namespace actuator
}  // namespace internal_forces
}  // namespace BIORBD_NAMESPACE

#endif  // BIORBD_ACTUATORS_ACTUATORS_H
