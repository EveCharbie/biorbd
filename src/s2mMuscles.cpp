#define BIORBD_API_EXPORTS
#include "s2mMuscles.h"

#include "s2mMuscle.h"
#include "Utils/Error.h"
#include "s2mGroupeMusculaire.h"
#include "Utils/GenCoord.h"
#include "Utils/Tau.h"
#include "s2mMuscleStateDynamics.h"
#include "s2mMuscleForce.h"

s2mMuscles::s2mMuscles(){

}

s2mMuscles::~s2mMuscles(){

}


void s2mMuscles::addMuscleGroup(
        const biorbd::utils::String &name,
        const biorbd::utils::String &originName,
        const biorbd::utils::String &insertionName){
    if (m_mus.size() > 0)
        biorbd::utils::Error::error(getGroupId(name)==-1, "Muscle group already defined");

    m_mus.push_back(s2mGroupeMusculaire(name, originName, insertionName));
}

int s2mMuscles::getGroupId(const biorbd::utils::String &name) const{
    for (unsigned int i=0; i<m_mus.size(); ++i)
        if (!name.compare(m_mus[i].name()))
            return static_cast<int>(i);
    return -1;
}

s2mGroupeMusculaire &s2mMuscles::muscleGroup_nonConst(unsigned int idx)
{
    biorbd::utils::Error::error(idx<nbMuscleGroups(), "Idx asked is higher than number of muscle groups");
    return m_mus[idx];
}

const s2mGroupeMusculaire &s2mMuscles::muscleGroup(unsigned int idx) const{
    biorbd::utils::Error::error(idx<nbMuscleGroups(), "Idx asked is higher than number of muscle groups");
    return m_mus[idx];
}
const s2mGroupeMusculaire &s2mMuscles::muscleGroup(const biorbd::utils::String& name) const{
    int idx = getGroupId(name);
    biorbd::utils::Error::error(idx!=-1, "Group name could not be found");
    return muscleGroup(static_cast<unsigned int>(idx));
}

// From muscle activation (return muscle force)
biorbd::utils::Tau s2mMuscles::muscularJointTorque(
        s2mJoints& m,
        const std::vector<s2mMuscleStateDynamics> &state,
        Eigen::VectorXd & F,
        bool updateKin,
        const biorbd::utils::GenCoord* Q,
        const biorbd::utils::GenCoord* QDot){

    // Update de la position musculaire
    if (updateKin > 0)
        updateMuscles(m,*Q,*QDot,updateKin);

    std::vector<std::vector<std::shared_ptr<s2mMuscleForce>>> force_tp = musclesForces(m, state, false);
    F = Eigen::VectorXd::Zero(static_cast<unsigned int>(force_tp.size()));
    for (unsigned int i=0; i<force_tp.size(); ++i)
        F(i) = (force_tp[i])[0]->norme();

    return muscularJointTorque(m, F, false, Q, QDot);
}

// From muscle activation (do not return muscle force)
biorbd::utils::Tau s2mMuscles::muscularJointTorque(
        s2mJoints& m,
        const std::vector<s2mMuscleStateDynamics>& state,
        bool updateKin,
        const biorbd::utils::GenCoord* Q,
        const biorbd::utils::GenCoord* QDot){
    biorbd::utils::GenCoord dummy;
    return muscularJointTorque(m, state, dummy, updateKin, Q, QDot);
}

// From Muscular Force
biorbd::utils::Tau s2mMuscles::muscularJointTorque(
        s2mJoints& m,
        const Eigen::VectorXd& F,
        bool updateKin,
        const biorbd::utils::GenCoord* Q,
        const biorbd::utils::GenCoord* QDot){

    // Update de la position musculaire
    if (updateKin > 0)
        updateMuscles(m,*Q,*QDot,updateKin);

    // Récupérer la matrice jacobienne et
    // récupérer les forces de chaque muscles
    biorbd::utils::Matrix jaco(musclesLengthJacobian(m));

    // Calcul de la réaction des forces sur les corps
    return biorbd::utils::Tau(-jaco.transpose() * F);
}

std::vector<std::vector<std::shared_ptr<s2mMuscleForce>>> s2mMuscles::musclesForces(
        s2mJoints& m,
        const std::vector<s2mMuscleStateDynamics> &state,
        bool updateKin,
        const biorbd::utils::GenCoord* Q,
        const biorbd::utils::GenCoord* QDot){

    // Update de la position musculaire
    if (updateKin > 0)
        updateMuscles(m,*Q,*QDot,updateKin);

    // Variable de sortie
    std::vector<std::vector<std::shared_ptr<s2mMuscleForce>>> forces; // Tous les muscles/Deux pointeurs par muscles (origine/insertion)

    unsigned int cmpMus(0);
    std::vector<s2mGroupeMusculaire>::iterator grp=m_mus.begin();
    for (unsigned int i=0; i<m_mus.size(); ++i) // groupe musculaire
        for (unsigned int j=0; j<(*(grp+i)).nbMuscles(); ++j){
            // forces musculaire
            forces.push_back((*(grp+i)).muscle(j)->force(*(state.begin()+cmpMus)));
            cmpMus++;
        }

    // Les forces
    return forces;
}

unsigned int s2mMuscles::nbMuscleGroups() const {
    return static_cast<unsigned int>(m_mus.size());
}

biorbd::utils::Matrix s2mMuscles::musclesLengthJacobian(s2mJoints &m)
{
    biorbd::utils::Matrix tp(biorbd::utils::Matrix::Zero(nbMuscleTotal(), m.nbDof()));
    unsigned int cmpMus(0);
    for (unsigned int i=0; i<nbMuscleGroups(); ++i){ // groupe musculaire
        for (unsigned int j=0; j<(m_mus[i]).nbMuscles(); ++j){
            // forces musculaire
            tp.block(cmpMus,0,1,m.nbDof()) = (m_mus[i]).muscle(j)->position().jacobianLength();
            ++cmpMus;
        }
    }
    return tp;

}

biorbd::utils::Matrix s2mMuscles::musclesLengthJacobian(
        s2mJoints& m,
        const biorbd::utils::GenCoord &Q){

    // Update de la position musculaire
    updateMuscles(m, Q, true);
    return musclesLengthJacobian(m);
}


unsigned int s2mMuscles::nbMuscleTotal() const{
    unsigned int total(0);
    for (unsigned int grp=0; grp<m_mus.size(); ++grp) // groupe musculaire
        total += m_mus[grp].nbMuscles();
    return total;
}

void s2mMuscles::updateMuscles(
        s2mJoints& m,
        const biorbd::utils::GenCoord& Q,
        const biorbd::utils::GenCoord& QDot,
        bool updateKin){

    // Updater tous les muscles
    int updateKinTP;
    if (updateKin)
        updateKinTP = 2;
    else
        updateKinTP = 0;

    std::vector<s2mGroupeMusculaire>::iterator grp=m_mus.begin();
    for (unsigned int i=0; i<m_mus.size(); ++i) // groupe musculaire
        for (unsigned int j=0; j<(*(grp+i)).nbMuscles(); ++j){
            (*(grp+i)).muscle(j)->updateOrientations(m, Q, QDot, updateKinTP);
            updateKinTP=1;
        }
}
void s2mMuscles::updateMuscles(
        s2mJoints& m,
        const biorbd::utils::GenCoord& Q,
        bool updateKin){

    // Updater tous les muscles
    int updateKinTP;
    if (updateKin)
        updateKinTP = 2;
    else
        updateKinTP = 0;

    // Updater tous les muscles
    std::vector<s2mGroupeMusculaire>::iterator grp=m_mus.begin();
    for (unsigned int i=0; i<m_mus.size(); ++i) // groupe musculaire
        for (unsigned int j=0; j<(*(grp+i)).nbMuscles(); ++j){
            (*(grp+i)).muscle(j)->updateOrientations(m, Q,updateKinTP);
            updateKinTP=1;
        }
}
void s2mMuscles::updateMuscles(
        std::vector<std::vector<s2mNodeMuscle>>& musclePointsInGlobal,
        std::vector<biorbd::utils::Matrix> &jacoPointsInGlobal,
        const biorbd::utils::GenCoord& QDot){
    std::vector<s2mGroupeMusculaire>::iterator grp=m_mus.begin();
    unsigned int cmpMuscle = 0;
    for (unsigned int i=0; i<m_mus.size(); ++i) // groupe musculaire
        for (unsigned int j=0; j<(*(grp+i)).nbMuscles(); ++j){
            (*(grp+i)).muscle(j)->updateOrientations(musclePointsInGlobal[cmpMuscle], jacoPointsInGlobal[cmpMuscle], QDot);
            ++cmpMuscle;
        }
}
void s2mMuscles::updateMuscles(
        std::vector<std::vector<s2mNodeMuscle>>& musclePointsInGlobal,
        std::vector<biorbd::utils::Matrix> &jacoPointsInGlobal){
    // Updater tous les muscles
    std::vector<s2mGroupeMusculaire>::iterator grp=m_mus.begin();
    unsigned int cmpMuscle = 0;
    for (unsigned int i=0; i<m_mus.size(); ++i) // groupe musculaire
        for (unsigned int j=0; j<(*(grp+i)).nbMuscles(); ++j){
            (*(grp+i)).muscle(j)->updateOrientations(musclePointsInGlobal[cmpMuscle], jacoPointsInGlobal[cmpMuscle]);
            ++cmpMuscle;
        }
}
