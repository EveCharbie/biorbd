#define BIORBD_API_EXPORTS
#include "Utils/ExternalForceSet.h"

#include "BiorbdModel.h"
#include "RigidBody/Contacts.h"
#include "RigidBody/GeneralizedCoordinates.h"
#include "RigidBody/GeneralizedVelocity.h"
#include "RigidBody/Joints.h"
#include "RigidBody/NodeSegment.h"
#include "RigidBody/Segment.h"
#include "RigidBody/SoftContacts.h"
#include "RigidBody/SoftContactNode.h"

#include "Utils/SpatialVector.h"
#include "Utils/String.h"
#include "Utils/Vector3d.h"
#include "Utils/Rotation.h"
#include "Utils/RotoTransNode.h"

using namespace BIORBD_NAMESPACE;

utils::ExternalForceSet::ExternalForceSet(
    Model& model, 
    bool useLinearForces, 
    bool useSoftContacts
) :
    m_model(model),
    m_useLinearForces(useLinearForces),
    m_useSoftContacts(useSoftContacts),
    m_externalForces(std::vector<utils::SpatialVector>()),
    m_externalForcesInLocal(utils::ExternalForceSet::LocalForcesInternal()),
    m_linearForces(std::vector<std::pair<utils::Vector3d, rigidbody::NodeSegment>>())
{
    setZero();
}

utils::ExternalForceSet::~ExternalForceSet(){

}

void utils::ExternalForceSet::add(
    const utils::String& segmentName,
    const utils::SpatialVector& v
) 
{
    int dofCount(0); 
    for (int i = 0; i < static_cast<int>(m_model.nbSegment()); ++i) {
        auto& segment(m_model.segment(i));
        
        dofCount += segment.nbDof();

        if (segment.name().compare(segmentName)) continue;
        if (segment.nbDof() == 0) {
            throw std::runtime_error("It is not possible to add forces to a segment without degree of freedom");
        }

        m_externalForces[dofCount] += v; // Do not subtract 1 since 0 is used for the base ground 
    }
}

void utils::ExternalForceSet::add(
    const utils::String& segmentName,
    const utils::SpatialVector& v, 
    const utils::Vector3d& pointOfApplication
)
{
    utils::SpatialVector atOrigin = transportAtOrigin(
        v, 
        rigidbody::NodeSegment(utils::Vector3d(pointOfApplication, segmentName, segmentName))
    );
    add(segmentName, atOrigin);
}


#ifdef BIORBD_USE_CASADI_MATH

void utils::ExternalForceSet::add(
    utils::String& segmentName,
    const casadi::MX& v
)
{

}

void utils::ExternalForceSet::add(
    utils::String& segmentName,
    const RBDLCasadiMath::MX_Xd_SubMatrix& m
)
{

}

#endif

void utils::ExternalForceSet::addInSegmentReferenceFrame(
    const utils::SpatialVector& vector,
    const utils::String& segmentName,
    const utils::Vector3d& pointOfApplication
) {
    m_externalForcesInLocal.addRT(
        vector, 
        utils::RotoTransNode(
            utils::RotoTrans(utils::Matrix3d::Identity(), pointOfApplication), segmentName, segmentName
        )
    );
}

void utils::ExternalForceSet::addLinearForce(
    const utils::Vector3d& force,
    const utils::String& segmentName,
    const utils::Vector3d& pointOfApplication
) 
{
    addLinearForce(force, rigidbody::NodeSegment(utils::Vector3d(pointOfApplication, segmentName, segmentName)));
}

void utils::ExternalForceSet::addLinearForce(
    const utils::Vector3d& force,
    const rigidbody::NodeSegment& pointOfApplication
)
{
    if (!m_useLinearForces) throw std::runtime_error("It is not possible to add linear force if it was set to false");
    m_linearForces.push_back(std::make_pair(force, pointOfApplication));
}

std::vector<RigidBodyDynamics::Math::SpatialVector> utils::ExternalForceSet::computeRbdlSpatialVectors() {
    if (hasExternalForceInLocalReferenceFrame()) throw std::runtime_error("local reference frame requires Q when computing the Spatial Vectors");
    if (m_useLinearForces) throw std::runtime_error("useLinearForce requires Q when computing the Spatial Vectors");
    if (m_useSoftContacts) throw std::runtime_error("useSoftContacts requires Q and QDot when computing the Spatial Vectors");
    return computeRbdlSpatialVectors(rigidbody::GeneralizedCoordinates(m_model), rigidbody::GeneralizedVelocity(m_model), false);
}

std::vector<RigidBodyDynamics::Math::SpatialVector> utils::ExternalForceSet::computeRbdlSpatialVectors(
    const rigidbody::GeneralizedCoordinates& Q,
    bool updateKin
) {

    if (m_useSoftContacts) throw std::runtime_error("useSoftContacts requires QDot when computing the Spatial Vectors");
    return computeRbdlSpatialVectors(Q, rigidbody::GeneralizedVelocity(m_model), updateKin);
}

std::vector<RigidBodyDynamics::Math::SpatialVector> utils::ExternalForceSet::computeRbdlSpatialVectors(
    const rigidbody::GeneralizedCoordinates& Q,
    const rigidbody::GeneralizedVelocity& QDot,
    bool updateKin
) {
    std::vector<utils::SpatialVector> tp(computeSpatialVectors(Q, QDot, updateKin));
    std::vector<RigidBodyDynamics::Math::SpatialVector> out;
    for (const auto& value : tp) {
        out.push_back(value);
    }
    return out;
}

std::vector<utils::SpatialVector> utils::ExternalForceSet::computeSpatialVectors() {
    if (hasExternalForceInLocalReferenceFrame()) throw std::runtime_error("local reference frame requires Q when computing the Spatial Vectors");
    if (m_useLinearForces) throw std::runtime_error("useLinearForce requires Q when computing the Spatial Vectors");
    if (m_useSoftContacts) throw std::runtime_error("useSoftContacts requires Q and QDot when computing the Spatial Vectors");
    return computeSpatialVectors(rigidbody::GeneralizedCoordinates(m_model), rigidbody::GeneralizedVelocity(m_model), false);
}

std::vector<utils::SpatialVector> utils::ExternalForceSet::computeSpatialVectors(
    const rigidbody::GeneralizedCoordinates& Q,
    bool updateKin
) {
    if (m_useSoftContacts) throw std::runtime_error("useSoftContacts requires QDot when computing the Spatial Vectors");
    return computeSpatialVectors(Q, rigidbody::GeneralizedVelocity(), updateKin);
}

std::vector<utils::SpatialVector> utils::ExternalForceSet::computeSpatialVectors(
    const rigidbody::GeneralizedCoordinates& Q,
    const rigidbody::GeneralizedVelocity& QDot,
    bool updateKin    
) 
{
#ifdef BIORBD_USE_CASADI_MATH
    updateKin = true;
#endif
    if (updateKin) {
        m_model.UpdateKinematicsCustom(&Q, m_useSoftContacts ? &QDot : nullptr, nullptr);
    }

    std::vector<utils::SpatialVector> out;
    for (const auto& value : m_externalForces) {
        out.push_back(value);
    }
    if (hasExternalForceInLocalReferenceFrame()) combineLocalReferenceFrameForces(Q, out);
    if (m_useLinearForces) combineLinearForces(Q, out);
    if (m_useSoftContacts) combineSoftContactForces(Q, QDot, out);

    return out;
}


bool utils::ExternalForceSet::hasExternalForceInLocalReferenceFrame() const {
    return m_externalForcesInLocal.size() > 0;
}

void utils::ExternalForceSet::setZero()
{
    m_externalForces.clear();

    // Null Spatial vector nul to fill the final table
    utils::SpatialVector sv_zero(0., 0., 0., 0., 0., 0.);
    m_externalForces.push_back(sv_zero); // The first one is associated with the universe

    // Dispatch the forces
    for (int i = 0; i < static_cast<int>(m_model.nbSegment()); ++i) {
        unsigned int nDof(m_model.segment(i).nbDof());
        for (unsigned int i = 0; i < nDof; ++i) {
            m_externalForces.push_back(sv_zero); // Put a sv_zero on each DoF
        }
    }

    // Reset other elements of the class too
    m_linearForces.clear();
    m_externalForcesInLocal.clear();
}

void utils::ExternalForceSet::combineLocalReferenceFrameForces(
    const rigidbody::GeneralizedCoordinates& Q,
    std::vector<utils::SpatialVector>& out
)
{
    // NOTE: since combineExternalPushes is necessarily called from internal as protected method
    // we assume updateKinematics was already done
    
    const std::vector<utils::SpatialVector>& vectors(m_externalForcesInLocal.m_vectors);
    std::vector<utils::RotoTransNode> nodes(m_externalForcesInLocal.RTs(Q, false));

    for (int i = 0; i < m_externalForcesInLocal.size(); i++) {
        const utils::SpatialVector& vector(vectors[i]);
        const utils::RotoTransNode& roto(utils::RotoTrans(nodes[i].rot()));
        const utils::Vector3d& pointOfApplication(nodes[i].trans());

        // Rotate the forces in global reference frame
        utils::Vector3d forceInGrf(vector.force());
        forceInGrf.applyRT(roto);

        utils::Vector3d momentInGrf(vector.moment());
        momentInGrf.applyRT(roto);

        // Find on which segment index this external force is applied to
        int dofCount(0);
        utils::String parentName = nodes[i].parent();
        for (unsigned int j = 0; j < m_model.nbSegment(); j++) {
            rigidbody::Segment segment = m_model.segment(j);
            dofCount += segment.nbDof();
            if (!segment.parent().compare(parentName)) break;
        }

        // Transport the force to the global reference frame
        out[dofCount] += transportAtOrigin(utils::SpatialVector(momentInGrf, forceInGrf), pointOfApplication);
    }
    return;
}

void utils::ExternalForceSet::combineLinearForces(
    const rigidbody::GeneralizedCoordinates& Q, 
    std::vector<utils::SpatialVector>& out
) const
{
    // NOTE: since combineExternalPushes is necessarily called from internal as protected method
    // we assume updateKinematics was already done
#ifdef BIORBD_USE_CASADI_MATH
    bool updateKin = true;
#else
    bool updateKin = false;
#endif

    // Do not waste time computing forces on empty vector
    if (m_linearForces.size() == 0) return;

    int dofCount(0);
    for (int i = 0; i < static_cast<int>(m_model.nbSegment()); ++i) {
        const rigidbody::Segment& segment(m_model.segment(i));
        dofCount += segment.nbDof();
        
        for (auto& e : m_linearForces) {    
            const rigidbody::NodeSegment& pointOfApplication = e.second;
            if (pointOfApplication.parent().compare(segment.name())) continue;

            const utils::Vector3d& force = e.first;
            rigidbody::NodeSegment pointOfApplicationInGlobal(
                RigidBodyDynamics::CalcBodyToBaseCoordinates(m_model, Q, segment.id(), pointOfApplication, updateKin),
                pointOfApplication.Node::name(),
                pointOfApplication.parent(),
                pointOfApplication.isTechnical(),
                pointOfApplication.isAnatomical(),
                pointOfApplication.axesToRemove(),
                pointOfApplication.parentId()
            );
            
            // Add the force to the force vector (do not subtract 1 because 0 is the base)
            out[dofCount] += transportForceAtOrigin(force, pointOfApplicationInGlobal);
        }
    }
}

void utils::ExternalForceSet::combineSoftContactForces(
    const rigidbody::GeneralizedCoordinates& Q,
    const rigidbody::GeneralizedVelocity& QDot,
    std::vector<utils::SpatialVector>& out
) const
{
    // NOTE: since combineSoftContactForces is necessarily called from internal as protected method
    // we assume updateKinematics was already done
#ifdef BIORBD_USE_CASADI_MATH
    bool updateKin = true;
#else
    bool updateKin = false;
#endif

    // Do not waste time computing forces on empty vector
    if (m_model.nbSoftContacts() == 0) return;


    int dofCount(0);
    for (int i = 0; i < static_cast<int>(m_model.nbSegment()); ++i) {
        const rigidbody::Segment& segment(m_model.segment(i));
        dofCount += segment.nbDof();
    
        for (int j = 0; j < m_model.nbSoftContacts(); j++) {
            rigidbody::SoftContactNode& contact(m_model.softContact(j));
            if (contact.parent().compare(segment.name())) continue;

            // Add the force to the force vector (do not subtract 1 because 0 is the base)
            out[dofCount] += contact.computeForceAtOrigin(m_model, Q, QDot, updateKin);
        }
    }
}

utils::SpatialVector utils::ExternalForceSet::transportForceAtOrigin(
    const utils::Vector3d& force,
    const rigidbody::NodeSegment& pointOfApplication
) const
{
    // Fill only if direction is enabled
    utils::Vector3d newForce(0., 0., 0.);
    for (auto axis : pointOfApplication.availableAxesIndices()){
        newForce.block(axis, 0, 1, 1) = force.block(axis, 0, 1, 1);
    }

    // Transport to Origin (Bour's formula)
    utils::SpatialVector out(0., 0., 0., 0., 0., 0.);
    out.block(0, 0, 3, 1) = newForce.cross(-pointOfApplication);
    out.block(3, 0, 3, 1) = newForce;

    return out;
}

utils::SpatialVector utils::ExternalForceSet::transportAtOrigin(
    const utils::SpatialVector& v,
    const rigidbody::NodeSegment& pointOfApplication
) const
{
    // Transport to Origin (Bour's formula)
    utils::SpatialVector out(transportForceAtOrigin(v.force(), pointOfApplication));
    out.block(0, 0, 3, 1) += v.moment();

    return out;
}
