#ifndef BIORBD_MUSCLES_WRAPPING_OBJECT_H
#define BIORBD_MUSCLES_WRAPPING_OBJECT_H

#include "biorbdConfig.h"

#include "Utils/Scalar.h"
#include "Utils/Vector3d.h"

namespace BIORBD_NAMESPACE {
namespace utils {
class String;
}

namespace rigidbody {
class Joints;
class GeneralizedCoordinates;
}  // namespace rigidbody

namespace internal_forces {
///
/// \brief Base class for the wrapping objects
///
class BIORBD_API WrappingObject : public utils::Vector3d {
 public:
  ///
  /// \brief Construct a wrapping object
  ///
  WrappingObject();

  ///
  /// \brief Construct a wrapping object
  /// \param x X-Component of the wrapping object
  /// \param y Y-Component of the wrapping object
  /// \param z Z-Component of the wrapping object
  ///
  WrappingObject(
      const utils::Scalar& x,
      const utils::Scalar& y,
      const utils::Scalar& z);
  ///
  /// \brief Construct a wrapping object
  /// \param x X-Component of the wrapping object
  /// \param y Y-Component of the wrapping object
  /// \param z Z-Component of the wrapping object
  /// \param name Name of the wrapping object
  /// \param parentName Name of the parent segment
  ///
  WrappingObject(
      const utils::Scalar& x,
      const utils::Scalar& y,
      const utils::Scalar& z,
      const utils::String& name,
      const utils::String& parentName);

  ///
  /// \brief Construct a wrapping object
  /// \param other Eigen vector
  ///
  WrappingObject(const utils::Vector3d& other);

  ///
  /// \brief Construct a wrapping object
  /// \param other Eigen vector
  /// \param name Name of the wrapping object
  /// \param parentName Name of the parent segment
  ///
  WrappingObject(
      const utils::Vector3d& other,
      const utils::String& name,
      const utils::String& parentName);

  ///
  /// \brief Deep copy of the wrapping ibject in another wrapping object
  /// \param other The wrapping object to copy
  ///
  void DeepCopy(const WrappingObject& other);

  ///
  /// \brief From the position of the wrapping object, return the 2 locations
  /// where the muscle leaves the wrapping object
  /// \param rt RotoTrans matrix of the wrapping object
  /// \param p1_bone 1st position of the muscle node
  /// \param p2_bone 2n position of the muscle node
  /// \param p1 The 1st position on the wrapping object the muscle leave
  /// \param p2 The 2nd position on the wrapping object the muscle leave
  /// \param muscleLength Length of the muscle (ignored if no value is provided)
  ///
  virtual void wrapPoints(
      const utils::RotoTrans& rt,
      const utils::Vector3d& p1_bone,
      const utils::Vector3d& p2_bone,
      utils::Vector3d& p1,
      utils::Vector3d& p2,
      utils::Scalar* muscleLength =
          nullptr) = 0;  // Premier et dernier points musculaire

  ///
  /// \brief From the position of the wrapping object, return the 2 locations
  /// where the muscle leaves the wrapping object
  /// \param model The joint model
  /// \param Q The generalized coordinates
  /// \param p1_bone 1st position of the muscle node
  /// \param p2_bone 2n position of the muscle node
  /// \param p1 The 1st position on the wrapping object the muscle leave
  /// \param p2 The 2nd position on the wrapping object the muscle leave
  /// \param muscleLength Length of the muscle (ignored if no value is provided)
  ///
  virtual void wrapPoints(
      rigidbody::Joints& model,
      const rigidbody::GeneralizedCoordinates& Q,
      const utils::Vector3d& p1_bone,
      const utils::Vector3d& p2_bone,
      utils::Vector3d& p1,
      utils::Vector3d& p2,
      utils::Scalar* muscleLength =
          nullptr) = 0;  // Premier et dernier points musculaire

  ///
  /// \brief Returns the previously computed 2 locations where the muscle leaves
  /// the wrapping object
  /// \param p1 The 1st position on the wrapping object the muscle leave
  /// \param p2 The 2nd position on the wrapping object the muscle leave
  /// \param muscleLength Length of the muscle (ignored if no value is provided)
  ///
  virtual void wrapPoints(
      utils::Vector3d& p1,
      utils::Vector3d& p2,
      utils::Scalar* muscleLength = nullptr) = 0;  // Assume un appel déja faits

  ///
  /// \brief Return the RotoTrans matrix of the wrapping object
  /// \param model The joint model
  /// \param Q The generalized coordinates
  /// \param updateKin If the kinematics should be computed
  /// \return The RotoTrans matrix of the wrapping object
  ///
  virtual const utils::RotoTrans& RT(
      rigidbody::Joints& model,
      const rigidbody::GeneralizedCoordinates& Q,
      bool updateKin = true) = 0;

  ///
  /// \brief Return the RotoTrans matrix of the wrapping object
  /// \return The RotoTrans matrix of the wrapping object
  ///
  const utils::RotoTrans& RT() const;

#ifndef SWIG
  ///
  /// \brief To be able to use the equal "=" operator to define wrapping object
  /// \param other The 3d node to define the wrapping object
  WrappingObject& operator=(const utils::Vector3d& other) {
    this->utils::Vector3d::operator=(other);
    return *this;
  }
#endif
 protected:
  std::shared_ptr<utils::RotoTrans>
      m_RT;  ///< RotoTrans matrix of the wrapping object
};

}  // namespace internal_forces
}  // namespace BIORBD_NAMESPACE

#endif  // BIORBD_MUSCLES_WRAPPING_OBJECT_H
