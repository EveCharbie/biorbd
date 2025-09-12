#ifndef BIORBD_MUSCLES_PATH_MODIFIERS_H
#define BIORBD_MUSCLES_PATH_MODIFIERS_H

#include "biorbdConfig.h"

#include <memory>
#include <vector>

namespace BIORBD_NAMESPACE {
namespace utils {
class Vector3d;
}

namespace internal_forces {
///
/// \brief Holder of all the path modifiers of a muscle
///
class BIORBD_API PathModifiers {
 public:
  ///
  /// \brief Construct path changers
  ///
  PathModifiers();

  ///
  /// \brief Deep copy of path changers
  /// \return A deep copy of path changers
  ///
  PathModifiers DeepCopy() const;

  ///
  /// \brief Deep copy of path changers from another path changers
  /// \param other THe path changers to copy
  ///
  void DeepCopy(const PathModifiers& other);

  ///
  /// \brief Add a wrapping or a via point to the set of path modifiers
  /// \param object The wrapping or via point to add
  ///
  void addPathChanger(utils::Vector3d& object);

  // Set and get
  ///
  /// \brief Return the total number of wrapping objects in the set
  /// \return The total number of wrapping objects
  ///
  size_t nbWraps() const;

  ///
  /// \brief Return the total number of via points in the set
  /// \return The total number of via points
  ///
  size_t nbVia() const;

  ///
  /// \brief Return the total number of path modifier objects in the set
  /// \return The total number of path modifier objects
  ///
  size_t nbObjects() const;

  ///
  /// \brief Return the object at a specific index in the set
  /// \param idx Index of the object
  /// \return The object at a specific index
  ///
  utils::Vector3d& object(size_t idx);

  ///
  /// \brief Return the object at a specific index in the set
  /// \param idx Index of the object
  /// \return The object at a specific index
  ///
  const utils::Vector3d& object(size_t idx) const;

 protected:
  std::shared_ptr<std::vector<std::shared_ptr<utils::Vector3d>>>
      m_obj;                          ///< set of objects
  std::shared_ptr<size_t> m_nbWraps;  ///< Number of wrapping object in the set
  std::shared_ptr<size_t> m_nbVia;    ///< Number of via points in the set
  std::shared_ptr<size_t>
      m_totalObjects;  ///< Number of total objects in the set
};

}  // namespace internal_forces
}  // namespace BIORBD_NAMESPACE

#endif  // BIORBD_MUSCLES_PATH_MODIFIERS_H
