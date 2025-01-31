//////////////////////////////////////////////////////////////////////////////////////////////
/// \file EvalTreeNodeBase.hpp
/// \brief
/// \author Sean Anderson, ASRL
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef STEAM_EVAL_TREE_NODE_BASE_HPP
#define STEAM_EVAL_TREE_NODE_BASE_HPP

#include <vector>
#include <steam/problem/Jacobian.hpp>

namespace steam {

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Type-less and interface class for a node in a block-automatic evaluation tree.
///        While the true strength of block automatic evaluation is in Jacobian evaluation,
///        note that the most efficient implementation involves first evaluating the nominal
///        solution of the nonlinear function at each level of the evaluator chain.
//////////////////////////////////////////////////////////////////////////////////////////////
class EvalTreeNodeBase
{
 public:

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Default constructor
  //////////////////////////////////////////////////////////////////////////////////////////////
  EvalTreeNodeBase();

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Destructor
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual ~EvalTreeNodeBase();

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Use when node was allocated using a pool. This function causes the object to
  ///        release itself back to the pool it was allocated from. While the user is
  ///        responsible for releasing the top level node, this interface method is required
  ///        in order for this node to release its children.
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual void release() = 0;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Release children and reset internal indexing to zero.
  //////////////////////////////////////////////////////////////////////////////////////////////
  virtual void reset();

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Add child evaluation node (order of addition is preserved)
  //////////////////////////////////////////////////////////////////////////////////////////////
  void addChild(EvalTreeNodeBase* newChild);

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Get number of child evaluation nodes
  //////////////////////////////////////////////////////////////////////////////////////////////
  size_t numChildren() const;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Get child node at index
  //////////////////////////////////////////////////////////////////////////////////////////////
  EvalTreeNodeBase* childAt(unsigned int index) const;

 protected:

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Maximum children
  //////////////////////////////////////////////////////////////////////////////////////////////
  static const unsigned int MAX_NUM_CHILDREN_ = 8;

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Children nodes
  //////////////////////////////////////////////////////////////////////////////////////////////
  EvalTreeNodeBase* children_[MAX_NUM_CHILDREN_];

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Number of child nodes
  //////////////////////////////////////////////////////////////////////////////////////////////
  unsigned int numChildren_;
};

} // steam

#endif // STEAM_EVAL_TREE_NODE_BASE_HPP
