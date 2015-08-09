//////////////////////////////////////////////////////////////////////////////////////////////
/// \file VectorSpaceErrorEval.hpp
///
/// \author Sean Anderson, ASRL
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef STEAM_VECTOR_SPACE_ERROR_EVALUATOR_HPP
#define STEAM_VECTOR_SPACE_ERROR_EVALUATOR_HPP

#include <steam/evaluator/ErrorEvaluator.hpp>
#include <steam/state/VectorSpaceStateVar.hpp>

namespace steam {

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Error evaluator for a measured vector space state variable
//////////////////////////////////////////////////////////////////////////////////////////////
class VectorSpaceErrorEval : public ErrorEvaluator
{
public:

  /// Convenience typedefs
  typedef boost::shared_ptr<VectorSpaceErrorEval> Ptr;
  typedef boost::shared_ptr<const VectorSpaceErrorEval> ConstPtr;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Constructor
  //////////////////////////////////////////////////////////////////////////////////////////////
  VectorSpaceErrorEval(const Eigen::VectorXd& measurement, const VectorSpaceStateVar::ConstPtr& stateVec);

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Returns whether or not an evaluator contains unlocked state variables
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual bool isActive() const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Evaluate the measurement error
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual Eigen::VectorXd evaluate() const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Evaluate the measurement error and relevant Jacobians
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual Eigen::VectorXd evaluate(const Eigen::MatrixXd& lhs, std::vector<Jacobian<> >* jacs) const;

private:

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Measurement vector
  //////////////////////////////////////////////////////////////////////////////////////////////
  Eigen::VectorXd measurement_;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Vectorspace state variable
  //////////////////////////////////////////////////////////////////////////////////////////////
  VectorSpaceStateVar::ConstPtr stateVec_;

};

} // steam

#endif // STEAM_VECTOR_SPACE_ERROR_EVALUATOR_HPP
