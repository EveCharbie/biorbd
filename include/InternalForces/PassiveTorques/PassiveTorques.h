#ifndef BIORBD_PASSIVE_TORQUES_PASSIVE_TORQUES_H
#define BIORBD_PASSIVE_TORQUES_PASSIVE_TORQUES_H

#include <vector>
#include <memory>
#include "biorbdConfig.h"
#include "Utils/Scalar.h"

namespace BIORBD_NAMESPACE
{
namespace utils
{
class Vector;
}

namespace rigidbody
{
class GeneralizedCoordinates;
class GeneralizedVelocity;
class GeneralizedTorque;
}

namespace internal_forces
{
namespace passive_torques
{
class PassiveTorque;
///
/// \brief Class holder for a set of passive torques
///
class BIORBD_API PassiveTorques
{
public:
    ///
    /// \brief Construct passive torques
    ///
    PassiveTorques();

    ///
    /// \brief Construct passive torques from another set of passive torques
    /// \param other The other passive torques
    ///
    PassiveTorques(
        const PassiveTorques& other);

    ///
    /// \brief Destroy passive torques class properly
    ///
    virtual ~PassiveTorques();

    ///
    /// \brief Deep copy of the passive torque holder from other passive torque holder
    /// \param other The other passive torques
    ///
    void DeepCopy(
        const PassiveTorques& other);

    ///
    /// \brief Add a passive torque to the set of passive torques
    /// \param other The passive torque to add
    ///
    void addPassiveTorque(
        const PassiveTorque &other);

    ///
    /// \brief Return the passiveJointTorques
    /// \param Q The generalized coordinates of the passive torques
    /// \param Qdot The generalized velocities of the passive torques
    /// \return model passiveJointTorques
    ///
    rigidbody::GeneralizedTorque passiveJointTorque(
        const rigidbody::GeneralizedCoordinates& Q,
        const rigidbody::GeneralizedVelocity &Qdot);


    // Get and set
    ///
    /// \brief Return the toal number of passive torques
    /// \return The total number of passive torques
    ///
    size_t nbPassiveTorques() const;

    ///
    /// \brief Return a specific passive torque
    /// \param dof Index of the DoF associated with passive torque
    /// \return The passive torque
    ///
    const std::shared_ptr<PassiveTorque>& getPassiveTorque(size_t dof);

protected:
    std::shared_ptr<std::vector<std::shared_ptr<internal_forces::passive_torques::PassiveTorque>>>  m_pas; ///< Passive torque to add
    std::shared_ptr<std::vector<bool>> m_isDofSet;///< If DoF all dof are set

};

}
}
}

#endif // BIORBD_PASSIVE_TORQUES_PASSIVE_TORQUES_H
