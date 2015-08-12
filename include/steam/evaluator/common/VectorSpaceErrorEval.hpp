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
template<int MEAS_DIM = Eigen::Dynamic, int MAX_STATE_DIM = Eigen::Dynamic>
class VectorSpaceErrorEval : public ErrorEvaluator<MEAS_DIM,MAX_STATE_DIM>::type
{
public:

  /// Convenience typedefs
  typedef boost::shared_ptr<VectorSpaceErrorEval<MEAS_DIM,MAX_STATE_DIM> > Ptr;
  typedef boost::shared_ptr<const VectorSpaceErrorEval<MEAS_DIM,MAX_STATE_DIM> > ConstPtr;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Constructor
  //////////////////////////////////////////////////////////////////////////////////////////////
  VectorSpaceErrorEval(const Eigen::Matrix<double,MEAS_DIM,1>& measurement,
                       const VectorSpaceStateVar::ConstPtr& stateVec);

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Returns whether or not an evaluator contains unlocked state variables
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual bool isActive() const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Evaluate the measurement error
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual Eigen::Matrix<double,MEAS_DIM,1> evaluate() const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Evaluate the measurement error and relevant Jacobians
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual Eigen::Matrix<double,MEAS_DIM,1> evaluate(const Eigen::Matrix<double,MEAS_DIM,MEAS_DIM>& lhs,
                                                    std::vector<Jacobian<MEAS_DIM,MAX_STATE_DIM> >* jacs) const;

private:

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Measurement vector
  //////////////////////////////////////////////////////////////////////////////////////////////
  Eigen::Matrix<double,MEAS_DIM,1> measurement_;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Vectorspace state variable
  //////////////////////////////////////////////////////////////////////////////////////////////
  VectorSpaceStateVar::ConstPtr stateVec_;

};

typedef VectorSpaceErrorEval<Eigen::Dynamic,Eigen::Dynamic> VectorSpaceErrorEvalX;

} // steam

#include <steam/evaluator/common/VectorSpaceErrorEval-inl.hpp>

#endif // STEAM_VECTOR_SPACE_ERROR_EVALUATOR_HPP
