//////////////////////////////////////////////////////////////////////////////////////////////
/// \file LandmarkStateVar.hpp
///
/// \author Sean Anderson, ASRL
//////////////////////////////////////////////////////////////////////////////////////////////

#include <steam/state/LandmarkStateVar.hpp>

namespace steam {
namespace se3 {

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor from a global 3D point
//////////////////////////////////////////////////////////////////////////////////////////////
LandmarkStateVar::LandmarkStateVar(const Eigen::Vector3d& v_0) : StateVariable(3) {
  this->setHomogeneous(v_0);
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Update the landmark state from the 3-dimensional perturbation
//////////////////////////////////////////////////////////////////////////////////////////////
bool LandmarkStateVar::update(const Eigen::VectorXd& perturbation) {

  if (perturbation.size() != this->getPerturbDim()) {
    throw std::runtime_error("During attempt to update a state variable, the provided "
                             "perturbation (VectorXd) was not the correct size.");
  }

  // todo: speed this up ? http://eigen.tuxfamily.org/dox/TopicWritingEfficientProductExpression.html
  //this->value_.head<3>() += perturbation;
  this->setHomogeneous((this->value_.head<3>() + perturbation)/this->value_[3]);
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Clone method
//////////////////////////////////////////////////////////////////////////////////////////////
StateVariableBase::Ptr LandmarkStateVar::clone() const {
  return LandmarkStateVar::Ptr(new LandmarkStateVar(*this));
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Check if a reference frame was set for the landmark.
//////////////////////////////////////////////////////////////////////////////////////////////
bool LandmarkStateVar::hasReferenceFrame() const {
  return refFrame_;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Set value -- mostly for landmark initialization
//////////////////////////////////////////////////////////////////////////////////////////////
void LandmarkStateVar::set(const Eigen::Vector3d& v) {
  this->setHomogeneous(v);
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Get the reference frame transform evaluator
//////////////////////////////////////////////////////////////////////////////////////////////
const TransformEvaluator::ConstPtr& LandmarkStateVar::getReferenceFrame() const {
  return refFrame_;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Get point transformed into the global (or base) frame.
//////////////////////////////////////////////////////////////////////////////////////////////
Eigen::Vector4d LandmarkStateVar::getGlobalValue() const {
  if (this->hasReferenceFrame()) {
    return refFrame_->evaluate().inverse() * this->value_;
  } else {
    return this->value_;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Homogeneously scale point from raw xyz point
//////////////////////////////////////////////////////////////////////////////////////////////
void LandmarkStateVar::setHomogeneous(const Eigen::Vector3d& v) {

  // Set scale
//  const double range = v.norm();
//  if (range < 15.0) {
//    this->value_[3] = 1.0;
//  } else {
//    this->value_[3] = 1.0/(range*range);
//  }
  //this->value_[3] = 1.0;
  this->value_[3] = 1.0/v.squaredNorm();

  // Set scaled xyz coordinates
  this->value_.head<3>() = this->value_[3] * v;
}

} // se3
} // steam
