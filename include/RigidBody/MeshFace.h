#ifndef BIORBD_RIGIDBODY_MESH_FACE_H
#define BIORBD_RIGIDBODY_MESH_FACE_H

#include "biorbdConfig.h"

#include <memory>
#include <vector>

namespace BIORBD_NAMESPACE {
namespace utils {
class Vector3d;
}

namespace rigidbody {

///
/// \brief The face of the mesh
///
class BIORBD_API MeshFace {
 public:
  ///
  /// \brief Contruct face MeshFace
  /// \param vertex The vertex to connect to form a face
  ///
  MeshFace(const std::vector<int>& vertex = std::vector<int>(3));

  ///
  /// \brief Deep copy of a MeshFace
  /// \return A deep copy of a MeshFace
  ///
  MeshFace DeepCopy() const;

  ///
  /// \brief Deep copy of a MeshFace into another one
  /// \param other The MeshFace to copy
  ///
  void DeepCopy(const MeshFace& other);

  ///
  /// \brief Allows to assign/get using ()
  /// \param idx The index in the vector
  ///
  int& operator()(size_t idx);

  ///
  /// \brief set the MeshFace from a new point
  /// \param pts The new point to copy
  ///
  void setFace(const std::vector<int>& pts);

  ///
  /// \brief Copy the face from another MeshFace
  /// \param other The other MeshFace
  ///
  void setFace(const MeshFace& other);

  ///
  /// \brief convert the integer nature of the face to a double
  /// \return The vertex index in double format
  ///
  utils::Vector3d faceAsDouble();

  ///
  /// \brief Returns the face
  /// \return The face
  ///
  std::vector<int> face();

 protected:
  std::shared_ptr<std::vector<int>> m_face;  ///< The face
};

}  // namespace rigidbody
}  // namespace BIORBD_NAMESPACE

#endif  // BIORBD_RIGIDBODY_MESH_FACE_H
