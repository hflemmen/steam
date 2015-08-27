//////////////////////////////////////////////////////////////////////////////////////////////
/// \file SimpleBundleAdjustmentFullRel.cpp
/// \brief A sample usage of the STEAM Engine library for a bundle adjustment problem
///        with relative landmarks and poses.
///
/// \author Michael Warren, ASRL
//////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <lgmath.hpp>
#include <steam.hpp>
#include <steam/data/ParseBA.hpp>

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Example that loads and solves simple bundle adjustment problems
//////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv) {

  ///
  /// Parse Dataset - sphere of relative pose measurements (fairly dense loop closures)
  ///

  // Get filename
  std::string filename;
  if (argc < 2) {
    filename = "../../include/steam/data/stereo_simulated.txt";
    //filename = "../../include/steam/data/stereo_simulated_window1.txt";
    //filename = "../../include/steam/data/stereo_simulated_window2.txt";
    std::cout << "Parsing default file: " << filename << std::endl << std::endl;
  } else {
    filename = argv[1];
    std::cout << "Parsing file: " << filename << std::endl << std::endl;
  }

  // Load dataset
  steam::data::SimpleBaDataset dataset = steam::data::parseSimpleBaDataset(filename);
  std::cout << "Problem has: " << dataset.frames_gt.size() << " poses" << std::endl;
  std::cout << "             " << dataset.land_gt.size() << " landmarks" << std::endl;
  std::cout << "            ~" << double(dataset.meas.size())/dataset.frames_gt.size() << " meas per pose" << std::endl << std::endl;

  ///
  /// Setup and Initialize States
  ///

  // fixed vehicle to camera transform
  steam::se3::TransformEvaluator::Ptr pose_c_v = steam::se3::FixedTransformEvaluator::MakeShared(dataset.T_cv);

  // Ground truth
  std::vector<steam::se3::TransformStateVar::Ptr> poses_gt_k_0;
  std::vector<steam::se3::LandmarkStateVar::Ptr> landmarks_gt;

  // State variable containers (and related data)
  std::vector<steam::se3::LandmarkStateVar::Ptr> landmarks_ic;
  std::vector<steam::se3::TransformStateVar::Ptr> relposes_ic_k_kp;

  // Setup ground-truth poses
  for (unsigned int i = 0; i < dataset.frames_gt.size(); i++) {
    steam::se3::TransformStateVar::Ptr temp(new steam::se3::TransformStateVar(dataset.frames_gt[i].T_k0));
    poses_gt_k_0.push_back(temp);
  }

  // Setup ground-truth landmarks
  for (unsigned int i = 0; i < dataset.land_gt.size(); i++) {
    steam::se3::LandmarkStateVar::Ptr temp(new steam::se3::LandmarkStateVar(dataset.land_gt[i].point));
    landmarks_gt.push_back(temp);
  }

  // Set a fixed identity transform that will be used to initialize landmarks in their parent frame
  steam::se3::FixedTransformEvaluator::Ptr tf_identity = steam::se3::FixedTransformEvaluator::MakeShared(lgmath::se3::Transformation());

  // Create all the relative transforms from the initial poses
  for (unsigned int i = 1; i < dataset.frames_ic.size(); i++) {
    steam::se3::TransformStateVar::Ptr transform_vk_vkp(new steam::se3::TransformStateVar(dataset.frames_ic[i].T_k0/dataset.frames_ic[i-1].T_k0));
    relposes_ic_k_kp.push_back(transform_vk_vkp);
  }

  // Size vector for landmarks -- initialize while going through measurements
  landmarks_ic.resize(dataset.land_ic.size());

  // Record the frame in which the landmark is first seen in order to set up transforms correctly
  std::map<unsigned, unsigned> landmark_map;

  ///
  /// Setup Cost Terms
  ///

  // Steam cost terms
  steam::CostTermCollection<4,6>::Ptr costTerms(new steam::CostTermCollection<4,6>());

  // Setup shared noise and loss function
  steam::NoiseModel<4>::Ptr sharedCameraNoiseModel(new steam::NoiseModel<4>(dataset.noise));
  steam::L2LossFunc::Ptr sharedLossFunc(new steam::L2LossFunc());

  // Setup camera intrinsics
  steam::StereoCameraErrorEval::CameraIntrinsics::Ptr sharedIntrinsics(
        new steam::StereoCameraErrorEval::CameraIntrinsics());
  sharedIntrinsics->b  = dataset.camParams.b;
  sharedIntrinsics->fu = dataset.camParams.fu;
  sharedIntrinsics->fv = dataset.camParams.fv;
  sharedIntrinsics->cu = dataset.camParams.cu;
  sharedIntrinsics->cv = dataset.camParams.cv;


  // Generate cost terms for camera measurements
  for (unsigned int i = 0; i < dataset.meas.size(); i++) {

    // Get pose reference
    unsigned int frameIdx = dataset.meas[i].frameID;

    // Get landmark reference
    unsigned int landmarkIdx = dataset.meas[i].landID;

    // Setup landmark if first time
    if (!landmarks_ic[landmarkIdx]) {

      // Transform into local reference frame
      Eigen::Vector4d p_v0; p_v0.head<3>() = dataset.land_ic[landmarkIdx].point; p_v0[3] = 1.0;
      lgmath::se3::Transformation tf_l0 = dataset.frames_ic[frameIdx].T_k0/dataset.frames_ic[0].T_k0;
      Eigen::Vector4d p_vl = tf_l0 * p_v0;

      // Insert the landmark
      landmarks_ic[landmarkIdx] = steam::se3::LandmarkStateVar::Ptr(new steam::se3::LandmarkStateVar(p_vl.head<3>()));

      // Keep a record of its 'parent' frame
      landmark_map[landmarkIdx] = frameIdx;
    }

    // Get landmark reference
    steam::se3::LandmarkStateVar::Ptr& landVar = landmarks_ic[landmarkIdx];

    // Construct transform evaluator between two camera frames (a and b) that have observations
    steam::se3::TransformEvaluator::Ptr tf_vb_va;
    if(landmark_map[landmarkIdx] == frameIdx) {

        // In this case, the transform remains fixed as an identity transform
        tf_vb_va = tf_identity;

    } else {

      // Initialize from first relative transform
      unsigned int firstObsIndex = landmark_map[landmarkIdx];
      tf_vb_va = steam::se3::TransformStateEvaluator::MakeShared(relposes_ic_k_kp[firstObsIndex]);

      // Compose through the chain of transforms
      for(unsigned int j = firstObsIndex + 1; j < frameIdx; j++) {
        tf_vb_va = steam::se3::compose(steam::se3::TransformStateEvaluator::MakeShared(relposes_ic_k_kp[j]), tf_vb_va);
      }
    }

    // Compose with camera to vehicle transform
    steam::se3::TransformEvaluator::Ptr pose_cb_va = steam::se3::compose(pose_c_v, tf_vb_va);

    // Construct error function
    steam::StereoCameraErrorEval::Ptr errorfunc(new steam::StereoCameraErrorEval(
            dataset.meas[i].data, sharedIntrinsics, pose_cb_va, landVar));

    // Construct cost term
    steam::CostTerm<4,6>::Ptr cost(new steam::CostTerm<4,6>(errorfunc, sharedCameraNoiseModel, sharedLossFunc));
    costTerms->add(cost);
  }

  ///
  /// Make Optimization Problem
  ///

  // Initialize problem
  steam::OptimizationProblem problem;

  // Add pose variables
  for (unsigned int i = 0; i < relposes_ic_k_kp.size(); i++) {
    problem.addStateVariable(relposes_ic_k_kp[i]);
  }

  // Add landmark variables
  for (unsigned int i = 0; i < landmarks_ic.size(); i++) {
    problem.addStateVariable(landmarks_ic[i]);
  }

  // Add cost terms
  problem.addCostTermCollection(costTerms);

  ///
  /// Setup Solver and Optimize
  ///

  // Initialize parameters (enable verbose mode)
  steam::DoglegGaussNewtonSolver::Params params;
  params.verbose = true;

  // Make solver
  steam::DoglegGaussNewtonSolver solver(&problem, params);

  // Optimize
  solver.optimize();

  return 0;
}