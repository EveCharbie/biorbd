#ifndef BIORBD_UTILS_ROTOTRANSNODES_H
#define BIORBD_UTILS_ROTOTRANSNODES_H

#include <vector>
#include <memory>
#include "biorbdConfig.h"

namespace BIORBD_NAMESPACE
{
namespace utils
{
class String;
class Matrix;
class RotoTransNode;
}

namespace rigidbody
{
class GeneralizedCoordinates;

///
/// \brief Hold a set of RotoTransNodes
///
class BIORBD_API RotoTransNodes
{
public:
    ///
    /// \brief Construct RT set
    ///
    RotoTransNodes();

    ///
    /// \brief Construct RTs set from another set
    /// \param other The other RTs set
    ///
    RotoTransNodes(
        const RotoTransNodes& other);

    ///
    /// \brief Destroy the class properly
    ///
    virtual ~RotoTransNodes();

    ///
    /// \brief Deep copy of the RTs data
    /// \return A copy of the RTs data
    ///
    RotoTransNodes DeepCopy() const;

    ///
    /// \brief Deep copy the RTs data
    /// \param other The RT data to copy
    ///
    void DeepCopy(
        const RotoTransNodes& other);

    ///
    /// \brief Add a new RT to the set
    ///
    void addRT();

    ///
    /// \brief Add a new RT to the set
    /// \param RotoTrans The RotaTrans of the RT
    ///
    virtual void addRT(
        const utils::RotoTransNode &RotoTrans);

    ///
    /// \brief Return the number of RTs in the set
    /// \return The number of RTs
    ///
    size_t nbRTs() const;

    ///
    /// \brief Return the number of RTs in the set
    /// \return The number of RTs
    ///
    size_t size() const;

    ///
    /// \brief Return the names of the RTs
    /// \return The names of the RTs
    ///
    std::vector<utils::String> RTsNames();

    ///
    /// \brief Return all the RTs in the local reference of the segment
    /// \return All the RTs in local reference frame
    ///
    const std::vector<utils::RotoTransNode>& RTs() const;

    ///
    /// \brief Return all the RTs of a segment
    /// \param segmentName The name of the segment to return the RTs
    /// \return All the RTs of attached to the segment
    ///
    std::vector<utils::RotoTransNode> RTs(
        const utils::String& segmentName);

    ///
    /// \brief Return the RTs of a specified index
    /// \param idx The index of the RT in the set
    /// \return RT of idx i
    ///
    const utils::RotoTransNode& RT(
        size_t idx);

    ///
    /// \brief Compute and return all the RTs at the position given by Q
    /// \param Q The generalized coordinates
    /// \param updateKin If the model should be updated
    /// \return All the RTs at the position given by Q
    ///
    std::vector<utils::RotoTransNode> RTs(
        const GeneralizedCoordinates& Q,
        bool updateKin = true);

    ///
    /// \brief Compute and return the RT of index idx at the position given by Q
    /// \param Q The generalized coordinates
    /// \param idx The index of the RT in the set
    /// \param updateKin If the model should be updated
    /// \return The RT of index idx at the position given by Q
    ///
    utils::RotoTransNode RT(
        const GeneralizedCoordinates&Q,
        size_t  idx,
        bool updateKin = true);

    ///
    /// \brief Return all the RTs on a specified segment
    /// \param Q The generalized coordinates
    /// \param idx The index of the segment
    /// \param updateKin If the model should be updated
    /// \return All the RTs on the segment of index idx
    ///
    std::vector<utils::RotoTransNode> segmentRTs(
        const GeneralizedCoordinates& Q,
        size_t  idx,
        bool updateKin = true);

    ///
    /// \brief Return the jacobian of the RTs
    /// \param Q The generalized coordinates
    /// \param updateKin If the model should be updated
    /// \return The jacobien of the RTs
    ///
    std::vector<utils::Matrix> RTsJacobian(
        const GeneralizedCoordinates &Q,
        bool updateKin = true);

    /// 
    /// \brief Return to an internal state similar to initial declaration
    virtual void clear();

protected:
    std::shared_ptr<std::vector<utils::RotoTransNode>>
            m_RTs; ///< All the RTs

};

}
}

#endif // BIORBD_UTILS_ROTOTRANSNODES_H
