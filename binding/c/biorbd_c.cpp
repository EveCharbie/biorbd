#include "biorbd_c.h"

#include "ModelReader.h"
#include "ModelWriter.h"
#include "RigidBody/GeneralizedAcceleration.h"
#include "RigidBody/GeneralizedCoordinates.h"
#include "RigidBody/GeneralizedTorque.h"
#include "RigidBody/GeneralizedVelocity.h"
#include "RigidBody/IMU.h"
#include "RigidBody/NodeSegment.h"
#include "RigidBody/Segment.h"
#include "Utils/Matrix.h"
#include "Utils/RotoTrans.h"
#include "Utils/RotoTransNode.h"
#include "Utils/String.h"
#include "Utils/Vector.h"
#include "rbdl/Dynamics.h"
#ifndef SKIP_KALMAN
#include "RigidBody/KalmanReconsIMU.h"
#endif

using namespace BIORBD_NAMESPACE;

Model* c_biorbdModel(const char* pathToModel) {
  return new Model(Reader::readModelFile(utils::String(pathToModel)));
}

void c_deleteBiorbdModel(Model* model) { delete model; }
void c_writeBiorbdModel(Model* model, const char* path) {
  Writer::writeModel(*model, utils::Path(path));
}

// Joints functions
void c_boneRotationSequence(Model* m, const char* segName, char* seq) {
  // Memory for seq must be already allocated
  utils::String sequence(m->segment(segName).seqR());
  snprintf(seq, sequence.length() + 1, "%s", sequence.c_str());
}
void c_localJCS(Model* m, int i, double* rt_out) {
  utils::RotoTrans RT(m->segment(static_cast<unsigned int>(i)).localJCS());
  dispatchRToutput(RT, rt_out);
}
void c_globalJCS(Model* m, const double* Q, double* jcs) {
  // Dispatch des données d'entrée
  rigidbody::GeneralizedCoordinates eQ(dispatchQinput(m, Q));

  // Récupérer JCS
  std::vector<utils::RotoTrans> pre_jcs = m->allGlobalJCS(eQ);

  // Dispatch de l'output
  dispatchRToutput(pre_jcs, jcs);
}
void c_inverseDynamics(
    Model* model,
    const double* q,
    const double* qdot,
    const double* qddot,
    double* tau) {
  rigidbody::GeneralizedCoordinates Q(dispatchQinput(model, q));
  rigidbody::GeneralizedVelocity Qdot(dispatchQinput(model, qdot));
  rigidbody::GeneralizedAcceleration Qddot(dispatchQinput(model, qddot));

  rigidbody::GeneralizedTorque Tau(*model);
  RigidBodyDynamics::InverseDynamics(*model, Q, Qdot, Qddot, Tau);

  dispatchTauOutput(Tau, tau);
}
void c_massMatrix(Model* model, const double* q, double* massMatrix) {
  unsigned int nQ(model->nbQ());
  rigidbody::GeneralizedCoordinates Q(dispatchQinput(model, q));

  RigidBodyDynamics::Math::MatrixNd Mass(nQ, nQ);
  Mass.setZero();
  RigidBodyDynamics::CompositeRigidBodyAlgorithm(*model, Q, Mass);

  // Remplir l'output
  for (unsigned int i = 0; i < nQ * nQ; ++i) {
    massMatrix[i] = Mass(i);
  }
}
void c_CoM(Model* model, const double* q, double* com) {
  rigidbody::GeneralizedCoordinates Q(dispatchQinput(model, q));

  utils::Vector3d CoM(model->CoM(Q));

  dispatchVectorOutput(CoM, com);
}
BIORBD_API_C int c_nSegments(BIORBD_NAMESPACE::Model* model) {
  return static_cast<int>(model->nbSegment());
}

// dof functions
int c_nRoot(Model* model) { return static_cast<int>(model->nbRoot()); }
int c_nQ(Model* model) { return static_cast<int>(model->nbQ()); }
int c_nQDot(Model* model) { return static_cast<int>(model->nbQdot()); }
int c_nQDDot(Model* model) { return static_cast<int>(model->nbQddot()); }
int c_nGeneralizedTorque(Model* model) {
  return static_cast<int>(model->nbGeneralizedTorque());
}

// Markers functions
int c_nMarkers(Model* model) { return static_cast<int>(model->nbMarkers()); }
void c_markersInLocal(Model* model, double* markPos) {
  // Prepare output
  dispatchMarkersOutput(model->markers(), markPos);
}
void c_markers(
    Model* model,
    const double* Q,
    double* markPos,
    bool removeAxis,
    bool updateKin) {
  // Prepare parameters
  rigidbody::GeneralizedCoordinates eQ(dispatchQinput(model, Q));

  // Call the main function
  std::vector<rigidbody::NodeSegment> pos(
      model->markers(eQ, removeAxis, updateKin));

  // Prepare output
  dispatchMarkersOutput(pos, markPos);
}
void c_addMarker(
    Model* model,
    const double* markPos,
    const char* name,
    const char* parentName,
    bool technical,
    bool anatomical,
    const char* axesToRemove) {
  int parent_int = static_cast<int>(model->GetBodyId(parentName));
  utils::Vector3d pos(dispatchMarkersInput(
      markPos));  // Position du marker dans le repère local
  model->addMarker(
      pos, name, parentName, technical, anatomical, axesToRemove, parent_int);
}

// IMUs functions
int c_nIMUs(Model* model) { return static_cast<int>(model->nbIMUs()); }
void c_addIMU(
    Model* model,
    const double* imuRT,
    const char* name,
    const char* parentName,
    bool technical,
    bool anatomical) {
  utils::RotoTransNode pos(dispatchRTinput(imuRT), name, parentName);
  model->addIMU(pos, technical, anatomical);
}
void c_IMU(
    BIORBD_NAMESPACE::Model* model,
    const double* Qdouble,
    double* output,
    bool updateKin) {
  rigidbody::GeneralizedCoordinates Q = dispatchQinput(model, Qdouble);
  std::vector<rigidbody::IMU> allIMU = model->IMU(Q, updateKin);

  dispatchRToutput(allIMU, output);
}

// Kalman IMU
#ifndef SKIP_KALMAN
rigidbody::KalmanReconsIMU* c_BiorbdKalmanReconsIMU(
    Model* model,
    double* QinitialGuess,
    double freq,
    double noiseF,
    double errorF) {
  // Créer un pointeur sur un filtre de kalman
  rigidbody::KalmanReconsIMU* kalman = new rigidbody::KalmanReconsIMU(
      *model, rigidbody::KalmanParam(freq, noiseF, errorF));

  // Mettre le initial guess
  rigidbody::GeneralizedCoordinates e_QinitialGuess(*model);
  if (QinitialGuess != nullptr) {
    for (size_t i = 0; i < model->nbQ(); ++i) {
      e_QinitialGuess[static_cast<int>(i)] = QinitialGuess[i];
    }
    kalman->setInitState(&e_QinitialGuess);
  }

  return kalman;
}
void c_deleteBiorbdKalmanReconsIMU(rigidbody::KalmanReconsIMU* model) {
  delete model;
}
void c_BiorbdKalmanReconsIMUstep(
    Model* model,
    rigidbody::KalmanReconsIMU* kalman,
    double* imu,
    double* Q,
    double* QDot,
    double* QDDot) {
  // Copier les valeurs des matrices de rotation des IMUs dans un stl Vector
  utils::Vector T(3 * 3 * model->nbIMUs());  // Matrice 3*3 * nIMU
  for (unsigned int i = 0; i < model->nbIMUs(); ++i)
    for (unsigned int j = 0; j < 9; ++j) {  // matrice 3*3
      T[9 * i + j] = imu[9 * i + j];
    }
  // Se faire des entrés sur Q, QDot et QDDot
  rigidbody::GeneralizedCoordinates e_Q(*model);
  rigidbody::GeneralizedVelocity e_QDot(*model);
  rigidbody::GeneralizedAcceleration e_QDDot(*model);

  // Faire le filtre
  kalman->reconstructFrame(*model, T, &e_Q, &e_QDot, &e_QDDot);

  // Transcrire les réponses vers les arguments de sortie
  dispatchQoutput(e_Q, Q);
  dispatchQoutput(e_QDot, QDot);
  dispatchQoutput(e_QDDot, QDDot);
}
#endif

// Math functions
void c_matrixMultiplication(const double* M1, const double* M2, double* Mout) {
  // Recueillir les données d'entrée
  utils::RotoTrans mM1(dispatchRTinput(M1));
  utils::RotoTrans mM2(dispatchRTinput(M2));

  // Projeter et préparer les données de sortie
  dispatchRToutput(mM1.operator*(mM2), Mout);
}
void c_meanRT(const double* imuRT, unsigned int nFrame, double* imuRT_mean) {
  std::vector<utils::RotoTrans> m;

  // Dispatch des données d'entrée
  for (unsigned int i = 0; i < nFrame; ++i) {  // Pour tous les instants
    m.push_back(dispatchRTinput(&imuRT[i * 16]));
  }

  // Calcul de la moyenne
  utils::RotoTrans mMean = utils::RotoTrans::mean(m);

  // Dispatch des données de sortie
  dispatchRToutput(mMean, imuRT_mean);
}
void c_projectJCSinParentBaseCoordinate(
    const double* parent,
    const double* jcs,
    double* out) {
  // Recueillir les données d'entrée
  utils::RotoTrans aParent(dispatchRTinput(parent));
  utils::RotoTrans aJcs(dispatchRTinput(jcs));

  // Projeter et préparer les données de sortie
  dispatchRToutput(aParent.transpose().operator*(aJcs), out);
}
void c_transformMatrixToCardan(
    const double* M,
    const char* sequence,
    double* cardanOut) {
  utils::RotoTrans mM(dispatchRTinput(M));
  utils::String seq(sequence);

  utils::Vector cardan(utils::RotoTrans::toEulerAngles(mM, seq));

  // On assume que la mémoire pour cardanOut a déjà été octroyée
  dispatchDoubleOutput(cardan, cardanOut);
}

void c_solveLinearSystem(
    const double* A,
    int nCols,
    int nRows,
    const double* b,
    double* x) {
  utils::Matrix matA(dispatchMatrixInput(A, nRows, nCols));
  utils::Vector vecB(dispatchVectorInput(b, nRows));

  // Solving using Eigen
  utils::Vector solX(matA.householderQr().solve(vecB));

  dispatchVectorOutput(solX, x);
}

// Fonctions de dispatch des données d'entré ou de sortie
utils::Vector3d dispatchMarkersInput(const double* pos) {
  return utils::Vector3d(pos[0], pos[1], pos[2]);
}
void dispatchMarkersOutput(
    const std::vector<rigidbody::NodeSegment>& allMarkers,
    double* markers) {
  // Warning markers must already be allocated!
  for (size_t i = 0; i < allMarkers.size(); ++i) {
    for (size_t j = 0; j < 3; ++j) {
      markers[i * 3 + j] = allMarkers[i][static_cast<int>(j)];
    }
  }
}

rigidbody::GeneralizedCoordinates dispatchQinput(
    Model* model,
    const double* Q) {
  rigidbody::GeneralizedCoordinates eQ(*model);
  for (int i = 0; i < static_cast<int>(model->nbQ()); ++i) {
    eQ[i] = Q[i];
  }
  return eQ;
}
void dispatchQoutput(const rigidbody::GeneralizedCoordinates& eQ, double* Q) {
  // Warnging Q must already be allocated
  for (unsigned int i = 0; i < eQ.size(); ++i) {
    Q[i] = eQ[i];
  }
}
void dispatchTauOutput(const rigidbody::GeneralizedTorque& eTau, double* Tau) {
  // Warnging Q must already be allocated
  for (unsigned int i = 0; i < eTau.size(); ++i) {
    Tau[i] = eTau[i];
  }
}
void dispatchDoubleOutput(const utils::Vector& e, double* d) {
  // Warning output must already be allocated
  for (unsigned int i = 0; i < e.size(); ++i) {
    d[i] = e[i];
  }
}
utils::RotoTrans dispatchRTinput(const double* rt) {
  utils::RotoTrans pos;
  pos << rt[0], rt[4], rt[8], rt[12], rt[1], rt[5], rt[9], rt[13], rt[2], rt[6],
      rt[10], rt[14], rt[3], rt[7], rt[11], rt[15];
  return pos;
}
void dispatchRToutput(const utils::RotoTrans& rt_in, double* rt_out) {
  // Attention la mémoire doit déjà être allouée pour rt_out
  for (unsigned int i = 0; i < 16; ++i) {
    rt_out[i] = rt_in(i % 4, i / 4);
  }
}
void dispatchRToutput(
    const std::vector<utils::RotoTrans>& rt_in,
    double* rt_out) {
  // Attention la mémoire doit déjà être allouée pour rt_out
  for (unsigned int i = 0; i < rt_in.size(); ++i) {
    for (unsigned int j = 0; j < 16; ++j) {
      rt_out[i * 16 + j] = rt_in[i](j % 4, j / 4);
    }
  }
}
void dispatchRToutput(
    const std::vector<rigidbody::IMU>& rt_in,
    double* rt_out) {
  // Attention la mémoire doit déjà être allouée pour rt_out
  for (unsigned int i = 0; i < rt_in.size(); ++i) {
    for (unsigned int j = 0; j < 16; ++j) {
      rt_out[i * 16 + j] = rt_in[i](j % 4, j / 4);
    }
  }
}

utils::Matrix dispatchMatrixInput(const double* matXd, int nRows, int nCols) {
  utils::Matrix res(
      static_cast<unsigned int>(nRows), static_cast<unsigned int>(nCols));
  for (int j = 0; j < nCols; ++j) {
    for (int i = 0; i < nRows; ++i) {
      res(i, j) = matXd[i + j * nRows];
    }
  }
  return res;
}

utils::Vector dispatchVectorInput(const double* vecXd, int nElements) {
  utils::Vector res(static_cast<unsigned int>(nElements));
  for (int i = 0; i < nElements; ++i) {
    res(i) = vecXd[i];
  }
  return res;
}
void dispatchVectorOutput(const utils::Vector& vect, double* vect_out) {
  // Warnging vect_out must already be allocated
  for (int i = 0; i < vect.size(); i++) {
    vect_out[i] = vect[i];
  }
}

//// Spécifique à des projets (IMU sous Unity)
// S2MLIBRARY_UNITY_API void c_alignSpecificAxisWithParentVertical(const double*
// parentRT, const double * childRT, int idxAxe, double * rotation){
//     // Matrices à aliger (axe 2 de r1 avec axe idxAxe de r2)
//     s2mAttitude r1(dispatchRTinput(parentRT));
//     s2mAttitude r2(dispatchRTinput(childRT));

//    s2mAttitude rotationMat =
//    s2mIMU_Unity_Optim::alignSpecificAxisWithParentVertical(r1, r2, idxAxe);

//    dispatchRToutput(rotationMat, rotation);
//}
