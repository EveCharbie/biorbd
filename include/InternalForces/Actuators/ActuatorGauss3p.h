#ifndef BIORBD_ACTUATORS_ACTUATOR_GAUSS_3P_H
#define BIORBD_ACTUATORS_ACTUATOR_GAUSS_3P_H

#include "biorbdConfig.h"
#include "InternalForces/Actuators/Actuator.h"

namespace BIORBD_NAMESPACE
{
namespace rigidbody
{
class GeneralizedCoordinates;
class GeneralizedVelocity;
}

namespace internal_forces
{
namespace actuator
{

///
/// \brief Class ActuatorGauss3p is a joint actuator type which maximum
/// is 3 parameter gaussian (Gauss3p)
/// Please note that all parameters are given in degrees
///
class BIORBD_API ActuatorGauss3p : public Actuator
{
public:
    ///
    /// \brief Construct Gauss3p actuator
    ///
    ActuatorGauss3p();

    ///
    /// \brief Construct Gauss3p actuator from another Gauss3p actuator
    /// \param other The other Gauss3p actuator to copy
    ///
    ActuatorGauss3p(
        const ActuatorGauss3p& other);

    ///
    /// \brief Construct Gauss3p actuator
    /// \param direction The direction of the Gauss3p actuator (+1 or -1)
    /// \param Tmax The maximal torque in the eccentric phase
    /// \param T0 The maximal torque isometric
    /// \param wmax Maximum angular velocity above which torque cannot be produced
    /// \param wc Angluar velocity of the vertical asymptote of the concentric hyperbola
    /// \param amin Low plateau level
    /// \param wr 1/10 of the distance amax/amin
    /// \param w1 Mid point plateau
    /// \param r width of the gaussian curve
    /// \param qopt Optimal position
    /// \param dofIdx Index of the DoF associated with actuator
    ///
    ActuatorGauss3p(
        int direction,
        const utils::Scalar& Tmax,
        const utils::Scalar& T0,
        const utils::Scalar& wmax,
        const utils::Scalar& wc,
        const utils::Scalar& amin,
        const utils::Scalar& wr,
        const utils::Scalar& w1,
        const utils::Scalar& r,
        const utils::Scalar& qopt,
        size_t dofIdx);

    ///
    /// \brief Construct Gauss3p actuator
    /// \param direction The direction of the Gauss3p actuator
    /// \param Tmax The maximal torque in the eccentric phase
    /// \param T0 The maximal torque isometric
    /// \param wmax Maximum angular velocity above which torque cannot be produced
    /// \param wc Angluar velocity of the vertical asymptote of the concentric hyperbola
    /// \param amin Low plateau level
    /// \param wr 1/10 of the distance amax/amin
    /// \param w1 Mid point plateau
    /// \param r width of the gaussian curve
    /// \param qopt Optimal position
    /// \param dofIdx Index of the DoF associated with actuator
    /// \param jointName Name of the parent joint
    ///
    ActuatorGauss3p(
        int direction,
        const utils::Scalar& Tmax,
        const utils::Scalar& T0,
        const utils::Scalar& wmax,
        const utils::Scalar& wc,
        const utils::Scalar& amin,
        const utils::Scalar& wr,
        const utils::Scalar& w1,
        const utils::Scalar& r,
        const utils::Scalar& qopt,
        size_t dofIdx,
        const utils::String &jointName);

    ///
    /// \brief Destroy the class properly
    ///
    virtual ~ActuatorGauss3p();

    ///
    /// \brief Deep copy of the Gauss3p actuator
    /// \return A deep copy of the Gauss3p actuator
    ///
    ActuatorGauss3p DeepCopy() const;

    ///
    /// \brief Deep copy of the Gauss3p actuator from another Gauss3p actuator
    /// \param other The Gauss3p actuator to copy
    ///
    void DeepCopy(
        const ActuatorGauss3p& other);

    ///
    /// \brief Return the maximal torque (invalid)
    /// \return The maximal torque
    /// torqueMax for ActuatorGauss3p must be called with Q and Qdot
    ///
    virtual utils::Scalar torqueMax();

    ///
    /// \brief Return the maximal torque at a given Q and Qdot
    /// \param Q The generalized coordinates of the actuator
    /// \param Qdot The generalized velocities of the actuator
    /// \return The maximal torque
    ///
    virtual utils::Scalar torqueMax(
        const rigidbody::GeneralizedCoordinates &Q,
        const rigidbody::GeneralizedVelocity &Qdot);

protected:
    ///
    /// \brief Set the type of actuator
    ///
    virtual void setType();

    // For informations on these parameters, see Monique Iris Jackson's these from page 54
    // Angular/velocity relationship
    std::shared_ptr<utils::Scalar>
    m_k;         ///< Ratio of slope of the eccentric and concentric phases
    std::shared_ptr<utils::Scalar>
    m_Tmax;      ///< Maximum torque in the eccentric phase
    std::shared_ptr<utils::Scalar>
    m_T0;        ///< Maximum torque isometric
    std::shared_ptr<utils::Scalar>
    m_wmax;      ///< Maximum angular velocity above which torque cannot be produced
    std::shared_ptr<utils::Scalar>
    m_wc;        ///< Angular velocity of the vertical asymptote of the concentric hyperbola

    // Activation/velocity relationship
    std::shared_ptr<utils::Scalar>
    m_amax;      ///< Maximum activation level (set to 1)
    std::shared_ptr<utils::Scalar> m_amin;      ///< Low plateau level
    std::shared_ptr<utils::Scalar>
    m_wr;        ///< 1/10 de la distance amax/amin
    std::shared_ptr<utils::Scalar> m_w1;        ///< Mid point plateau

    // Torque/angle relationship
    std::shared_ptr<utils::Scalar>
    m_r;         ///< Width of the gaussian curve
    std::shared_ptr<utils::Scalar> m_qopt;      ///< Optimal position

};

}
}
}

#endif // BIORBD_ACTUATORS_ACTUATOR_GAUSS_3P_H
