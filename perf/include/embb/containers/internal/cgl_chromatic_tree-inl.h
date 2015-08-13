/*
 * Copyright (c) 2014-2015, Siemens AG. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef EMBB_CONTAINERS_INTERNAL_CGL_CHROMATIC_TREE_INL_H_
#define EMBB_CONTAINERS_INTERNAL_CGL_CHROMATIC_TREE_INL_H_

#include <assert.h>
#include <algorithm>

namespace embb {
namespace containers {
namespace internal {

template<typename Key, typename Value>
CGLChromaticTreeNode<Key, Value>::
CGLChromaticTreeNode(const Key& key, const Value& value, int weight,
                  Node* left, Node* right)
    : key_(key),
      value_(value),
      weight_(weight < 0 ? -weight : weight),
      is_leaf_(left == NULL),
      is_sentinel_(weight < 0),
      left_(left),
      right_(right) {}

template<typename Key, typename Value>
CGLChromaticTreeNode<Key, Value>::
CGLChromaticTreeNode(const Key& key, const Value& value, int weight)
    : key_(key),
      value_(value),
      weight_(weight < 0 ? -weight : weight),
      is_leaf_(true),
      is_sentinel_(weight < 0),
      left_(NULL),
      right_(NULL) {}

template<typename Key, typename Value>
inline const Key& CGLChromaticTreeNode<Key, Value>::GetKey() const {
  return key_;
}

template<typename Key, typename Value>
inline const Value& CGLChromaticTreeNode<Key, Value>::GetValue() const {
  return value_;
}

template<typename Key, typename Value>
inline int CGLChromaticTreeNode<Key, Value>::GetWeight() const {
  return weight_;
}

template<typename Key, typename Value>
inline typename CGLChromaticTreeNode<Key, Value>::Node*
CGLChromaticTreeNode<Key, Value>::GetLeft() const {
  return left_;
}

template<typename Key, typename Value>
typename CGLChromaticTreeNode<Key, Value>::Node*
CGLChromaticTreeNode<Key, Value>::GetRight() const {
  return right_;
}

template<typename Key, typename Value>
inline bool CGLChromaticTreeNode<Key, Value>::IsLeaf() const {
  return is_leaf_;
}

template<typename Key, typename Value>
inline bool CGLChromaticTreeNode<Key, Value>::IsSentinel() const {
  return is_sentinel_;
}

template<typename Key, typename Value>
void CGLChromaticTreeNode<Key, Value>::
ReplaceChild(Node* old_child, Node* new_child) {
  if (left_ == old_child) {
    left_ = new_child;
  } else if (right_ == old_child) {
    right_ = new_child;
  }
}

} // namespace internal


template<typename Key, typename Value, typename Compare, typename ValuePool>
CGLChromaticTree<Key, Value, Compare, ValuePool>::
CGLChromaticTree(size_t capacity, Key undefined_key, Value undefined_value,
              Compare compare)
    : undefined_key_(undefined_key),
      undefined_value_(undefined_value),
      compare_(compare),
      capacity_(capacity),
      node_pool_(2 + 5 + 2 * capacity_),
      entry_(node_pool_.Allocate(undefined_key_, undefined_value_, -1,
                                 node_pool_.Allocate(undefined_key_,
                                                     undefined_value_, -1),
                                 static_cast<Node*>(NULL))) {
  assert(entry_ != NULL);
  assert(entry_->GetLeft() != NULL);
  reg_ = NULL;
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
CGLChromaticTree<Key, Value, Compare, ValuePool>::
~CGLChromaticTree() {
  Destruct(entry_->GetLeft());
  FreeNode(entry_);
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
bool CGLChromaticTree<Key, Value, Compare, ValuePool>::
Get(const Key& key, Value& value) {
  if (reg_) reg_->StartOperation(embb::perf::TreeOpRegister::OP_GET);

  embb::base::SharedLock<embb::base::SharedMutex> lock(shared_mutex_);

  Node* grandparent;
  Node* parent;
  Node* leaf;
  Search(key, leaf, parent, grandparent);

  bool keys_are_equal = !leaf->IsSentinel() &&
                        !(compare_(key, leaf->GetKey()) ||
                          compare_(leaf->GetKey(), key));

  if (keys_are_equal) {
    value = leaf->GetValue();
  }

  if (reg_) reg_->EndOperation();
  return keys_are_equal;
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
bool CGLChromaticTree<Key, Value, Compare, ValuePool>::
TryInsert(const Key& key, const Value& value) {
  Value old_value;
  return TryInsert(key, value, old_value);
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
bool CGLChromaticTree<Key, Value, Compare, ValuePool>::
TryInsert(const Key& key, const Value& value, Value& old_value) {
  if (reg_) reg_->StartOperation(embb::perf::TreeOpRegister::OP_INSERT);

  embb::base::UniqueLock<embb::base::SharedMutex> lock(shared_mutex_);

  Node* new_leaf = NULL;
  Node* new_sibling = NULL;
  Node* new_parent = NULL;
  bool insertion_succeeded = false;
  bool added_violation = false;

  while (!insertion_succeeded) {
    if (reg_) reg_->Attempt();
    Node* grandparent;
    Node* parent;
    Node* leaf;
    Search(key, leaf, parent, grandparent);

    bool keys_are_equal = !leaf->IsSentinel() &&
                          !(compare_(key, leaf->GetKey()) ||
                            compare_(leaf->GetKey(), key));
    if (keys_are_equal) {
      // Reached leaf has a matching key: replace it with a new copy
      old_value = leaf->GetValue();
      new_parent = node_pool_.Allocate(key, value, leaf->GetWeight());
      if (new_parent == NULL) break;
      if (reg_) reg_->ShortPathTaken(true);

    // Reached leaf has a different key: add a new leaf
    } else {
      old_value = undefined_value_;

      new_leaf = node_pool_.Allocate(key, value, 1);
      if (new_leaf == NULL) break;
      new_sibling = node_pool_.Allocate(leaf->GetKey(), leaf->GetValue(), 1);
      if (new_sibling == NULL) break;

      int new_weight =
          leaf->IsSentinel() ? -1 :
            parent->IsSentinel() ? 1 :
              (leaf->GetWeight() - 1);
      if (leaf->IsSentinel() || compare_(key, leaf->GetKey())) {
        new_parent = node_pool_.Allocate(leaf->GetKey(), undefined_value_,
                                         new_weight, new_leaf, new_sibling);
      } else {
        new_parent = node_pool_.Allocate(key, undefined_value_,
                                         new_weight, new_sibling, new_leaf);
      }
      if (new_parent == NULL) break;
      if (reg_) reg_->ShortPathTaken(false);
    }

    parent->ReplaceChild(leaf, new_parent);

    FreeNode(leaf);

    added_violation = (parent->GetWeight() == 0 &&
                       new_parent->GetWeight() == 0);
    insertion_succeeded = true;
  }

  if (insertion_succeeded) {
    if (added_violation) CleanUp(key);
  } else {
    if (new_leaf != NULL)    FreeNode(new_leaf);
    if (new_sibling != NULL) FreeNode(new_sibling);
    if (new_parent != NULL)  FreeNode(new_parent);
  }

  if (reg_) reg_->EndOperation();
  return insertion_succeeded;
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
bool CGLChromaticTree<Key, Value, Compare, ValuePool>::
TryDelete(const Key& key) {
  Value old_value;
  return TryDelete(key, old_value);
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
bool CGLChromaticTree<Key, Value, Compare, ValuePool>::
TryDelete(const Key& key, Value& old_value) {
  if (reg_) reg_->StartOperation(embb::perf::TreeOpRegister::OP_DELETE);

  embb::base::UniqueLock<embb::base::SharedMutex> lock(shared_mutex_);

  Node* new_leaf = NULL;
  bool deletion_succeeded = false;
  bool added_violation = false;

  while (!deletion_succeeded) {
    if (reg_) reg_->Attempt();
    Node* grandparent;
    Node* parent;
    Node* leaf;
    Search(key, leaf, parent, grandparent);

    // Reached leaf has a different key - nothing to delete
    if (leaf->IsSentinel() || (compare_(key, leaf->GetKey()) ||
                             compare_(leaf->GetKey(), key))) {
      old_value = undefined_value_;
      deletion_succeeded = true;
      if (reg_) reg_->ShortPathTaken(true);
      break;
    }

    // Get the sibling
    Node* sibling = ((parent->GetLeft() == leaf) ?
                      parent->GetRight() : parent->GetLeft());

    int new_weight =
          parent->IsSentinel() ? -1 :
            grandparent->IsSentinel() ? 1 :
              (parent->GetWeight() + sibling->GetWeight());

    new_leaf = node_pool_.Allocate(
        sibling->GetKey(), sibling->GetValue(), new_weight,
        sibling->GetLeft(), sibling->GetRight());
    if (new_leaf == NULL) break;

    old_value = leaf->GetValue();

    grandparent->ReplaceChild(parent, new_leaf);

    FreeNode(parent);
    FreeNode(leaf);
    FreeNode(sibling);

    added_violation =  (new_weight > 1);
    deletion_succeeded = true;
  }

  if (deletion_succeeded) {
    if (added_violation) CleanUp(key);
  } else {
    if (new_leaf != NULL)    FreeNode(new_leaf);
  }

  if (reg_) reg_->EndOperation();
  return deletion_succeeded;
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
size_t CGLChromaticTree<Key, Value, Compare, ValuePool>::
GetCapacity() const {
  return capacity_;
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
const Value& CGLChromaticTree<Key, Value, Compare, ValuePool>::
GetUndefinedValue() const {
  return undefined_value_;
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
inline bool CGLChromaticTree<Key, Value, Compare, ValuePool>::
IsEmpty() const {
  return entry_->GetLeft()->IsLeaf();
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
void CGLChromaticTree<Key, Value, Compare, ValuePool>::
Search(const Key& key, Node*& leaf, Node*& parent, Node*& grandparent) {
  grandparent = NULL;
  parent      = entry_;
  leaf        = entry_->GetLeft();

  while (!leaf->IsLeaf()) {
    grandparent = parent;
    parent      = leaf;
    leaf        = (leaf->IsSentinel() || compare_(key, leaf->GetKey())) ?
                  leaf->GetLeft() : leaf->GetRight();
  }
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
inline bool CGLChromaticTree<Key, Value, Compare, ValuePool>::
HasChild(const Node* parent, const Node* child) const {
  return (parent->GetLeft() == child || parent->GetRight() == child);
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
void CGLChromaticTree<Key, Value, Compare, ValuePool>::
Destruct(Node* node) {
  if (!node->IsLeaf()) {
    Destruct(node->GetLeft());
    Destruct(node->GetRight());
  }
  FreeNode(node);
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
int CGLChromaticTree<Key, Value, Compare, ValuePool>::
GetHeight(const Node* node) const {
  int height = 0;
  if (node != NULL) {
    height = 1 + ::std::max(GetHeight(node->GetLeft()),
                            GetHeight(node->GetRight()));
  }
  return height;
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
bool CGLChromaticTree<Key, Value, Compare, ValuePool>::
IsBalanced() const {
  return IsBalanced(entry_->GetLeft());
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
bool CGLChromaticTree<Key, Value, Compare, ValuePool>::
IsBalanced(const Node* node) const {
  // Overweight violation
  bool has_violation = node->GetWeight() > 1;

  if (!has_violation && !node->IsLeaf()) {
    const Node* left  = node->GetLeft();
    const Node* right = node->GetRight();

    // Red-red violation
    has_violation = node->GetWeight() == 0 &&
                    (left->GetWeight() == 0 || right->GetWeight() == 0);

    // Check children
    if (!has_violation) {
      has_violation = !IsBalanced(left) || !IsBalanced(right);
    }
  }

  return !has_violation;
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
void CGLChromaticTree<Key, Value, Compare, ValuePool>::
FreeNode(Node* node) {
  node_pool_.Free(node);
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
bool CGLChromaticTree<Key, Value, Compare, ValuePool>::
CleanUp(const Key& key) {
  if (reg_) reg_->CleanupStart();
  Node* grandgrandparent;
  Node* grandparent;
  Node* parent;
  Node* leaf;
  bool reached_leaf = false;

  while (!reached_leaf) {
    if (reg_) reg_->CleanupAttempt();
    bool found_violation = false;

    grandgrandparent = entry_;
    grandparent = entry_;
    parent = entry_;
    leaf = entry_->GetLeft();

    reached_leaf = leaf->IsLeaf();
    while (!reached_leaf && !found_violation) {
      grandgrandparent = grandparent;
      grandparent = parent;
      parent = leaf;
      leaf = (leaf->IsSentinel() || compare_(key, leaf->GetKey())) ?
            leaf->GetLeft() : leaf->GetRight();

      // Check for violations
      if ((leaf->GetWeight() > 1) || (leaf->GetWeight() == 0 &&
                                      parent->GetWeight() == 0)) {
        embb_errors_t error = Rebalance(grandgrandparent, grandparent,
                                        parent, leaf);
        if (error == EMBB_NOMEM) {
          assert(false && "No memory for rebalancing!");
          return false;
        }
        if (reg_ && error == EMBB_BUSY) reg_->CleanupConflict();
        break;
      }

      reached_leaf = leaf->IsLeaf();
    }
  }

  if (reg_) reg_->CleanupEnd();
  return true;
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
embb_errors_t CGLChromaticTree<Key, Value, Compare, ValuePool>::
Rebalance(Node* u, Node* ux, Node* uxx, Node* uxxx) {
  // Verify that ux is still a child of u
  if (!HasChild(u, ux)) return EMBB_BUSY;

  //TODO: weakLLX(ux);
  Node* uxl = ux->GetLeft();
  Node* uxr = ux->GetRight();
  bool uxx_is_left = (uxx == uxl); (void)uxx_is_left;
  if (!HasChild(ux, uxx)) return EMBB_BUSY;

  //TODO: weakLLX(uxx);
  Node* uxxl = uxx->GetLeft();
  Node* uxxr = uxx->GetRight();
  bool uxxx_is_left = (uxxx == uxxl);
  if (!HasChild(uxx, uxxx)) return EMBB_BUSY;

  if (uxxx->GetWeight() > 1) {
    if (uxxx_is_left) {
      //TODO: weakLLX(uxxl);
      return OverweightLeft(u, ux, uxx, uxl, uxr, uxxl, uxxr, uxx_is_left);
    } else {
      //TODO: weakLLX(uxxr);
      return OverweightRight(u, ux, uxx, uxl, uxr, uxxl, uxxr, !uxx_is_left);
    }
  } else {
    assert(uxxx->GetWeight() == 0 && uxx->GetWeight() == 0); //Red-red violation
    if (uxx_is_left) {
      if (uxr->GetWeight() == 0) {
        //TODO: weakLLX(uxr);
        return BLK(u, ux, uxx, uxr);
      } else if (uxxx_is_left) {
        return RB1_L(u, ux, uxx);
      } else {
        //TODO: weakLLX(uxxr);
        return RB2_L(u, ux, uxx, uxxr);
      }
    } else {
      if (uxl->GetWeight() == 0) {
        //TODO: weakLLX(uxl);
        return BLK(u, ux, uxl, uxx);
      } else if (!uxxx_is_left) {
        return RB1_R(u, ux, uxx);
      } else {
        //TODO: weakLLX(uxxl);
        return RB2_R(u, ux, uxx, uxxl);
      }
    }
  }
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
embb_errors_t CGLChromaticTree<Key, Value, Compare, ValuePool>::
OverweightLeft(Node* u, Node* ux, Node* uxx, Node* uxl, Node* uxr, Node* uxxl,
               Node* uxxr, bool uxx_is_left) {
  // Let "Root" be the top of the overweight violation decision tree (see p.30)
  // Root -> Middle
  if (uxxr->GetWeight() == 0) {
    // Root -> Middle -> Left
    if (uxx->GetWeight() == 0) {
      // Root -> Middle -> Left -> Left
      if (uxx_is_left) {
        // Root -> Middle -> Left -> Left -> Left
        if (uxr->GetWeight() == 0) {
          //TODO: weakLLX(uxr);
          return BLK(u, ux, uxx, uxr);

        // Root -> Middle -> Left -> Left -> Right
        } else {
          assert(uxr->GetWeight() > 0);
          //TODO: weakLLX(uxxr);
          return RB2_L(u, ux, uxx, uxxr);
        }

      // Root -> Middle -> Left -> Right
      } else {
        assert(!uxx_is_left);
        // Root -> Middle -> Left -> Right -> Left
        if (uxl->GetWeight() == 0) {
          //TODO: weakLLX(uxl);
          return BLK(u, ux, uxl, uxx);

        // Root -> Middle -> Left -> Right -> Right
        } else {
          assert(uxl->GetWeight() > 0);
          return RB1_R(u, ux, uxx);
        }
      }

    // Root -> Middle -> Right
    } else {
      assert(uxx->GetWeight() > 0);
      //TODO: weakLLX(uxxr);
      // Note: we know that 'uxxr' is not a leaf because it has weight 0.
      Node* uxxrl = uxxr->GetLeft();
      //TODO: weakLLX(uxxrl);

      // Root -> Middle -> Right -> Left
      if (uxxrl->GetWeight() == 0) {
        return RB2_R(ux, uxx, uxxr, uxxrl);

      // Root -> Middle -> Right -> Middle
      } else if (uxxrl->GetWeight() == 1) {
        Node* uxxrll = uxxrl->GetLeft();
        Node* uxxrlr = uxxrl->GetRight();
        if (uxxrlr == NULL) return EMBB_BUSY;

        // Root -> Middle -> Right -> Middle -> Left
        if (uxxrlr->GetWeight() == 0) {
          //TODO: weakLLX(uxxrlr);
          return W4_L(ux, uxx, uxxl, uxxr, uxxrl, uxxrlr);

        // Root -> Middle -> Right -> Middle -> Right
        } else {
          assert(uxxrlr->GetWeight() > 0);
          // Root -> Middle -> Right -> Middle -> Right -> Left
          if (uxxrll->GetWeight() == 0) {
            //TODO: weakLLX(uxxrll);
            return W3_L(ux, uxx, uxxl, uxxr, uxxrl, uxxrll);

          // Root -> Middle -> Right -> Middle -> Right -> Right
          } else {
            assert(uxxrll->GetWeight() > 0);
            return W2_L(ux, uxx, uxxl, uxxr, uxxrl);
          }
        }

      // Root -> Middle -> Right -> Right
      } else {
        assert(uxxrl->GetWeight() > 1);
        return W1_L(ux, uxx, uxxl, uxxr, uxxrl);
      }
    }

  // Root -> Right
  } else if (uxxr->GetWeight() == 1) {
    //TODO: weakLLX(uxxr);
    Node* uxxrl = uxxr->GetLeft();
    Node* uxxrr = uxxr->GetRight();
    if (uxxrl == NULL) return EMBB_BUSY;

    // Root -> Right -> Left
    if (uxxrr->GetWeight() == 0) {
      //TODO: weakLLX(uxxrr);
      return W5_L(ux, uxx, uxxl, uxxr, uxxrr);

    // Root -> Right -> Right
    } else {
      assert(uxxrr->GetWeight() > 0);
      // Root -> Right -> Right -> Left
      if (uxxrl->GetWeight() == 0) {
        //TODO: weakLLX(uxxrl);
        return W6_L(ux, uxx, uxxl, uxxr, uxxrl);

      // Root -> Right -> Right -> Right
      } else {
        assert(uxxrl->GetWeight() > 0);
        return PUSH_L(ux, uxx, uxxl, uxxr);
      }
    }

  // Root -> Left
  } else {
    assert(uxxr->GetWeight() > 1);
    //TODO: weakLLX(uxxr);
    return W7(ux, uxx, uxxl, uxxr);
  }
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
embb_errors_t CGLChromaticTree<Key, Value, Compare, ValuePool>::
OverweightRight(Node* u, Node* ux, Node* uxx, Node* uxl, Node* uxr, Node* uxxl,
                Node* uxxr, bool uxx_is_right) {
  // Let "Root" be the top of the overweight violation decision tree (see p.30)
  // Root -> Middle
  if (uxxl->GetWeight() == 0) {
    // Root -> Middle -> Left
    if (uxx->GetWeight() == 0) {
      // Root -> Middle -> Left -> Left
      if (uxx_is_right) {
        // Root -> Middle -> Left -> Left -> Left
        if (uxl->GetWeight() == 0) {
          //TODO: weakLLX(uxl);
          return BLK(u, ux, uxl, uxx);

        // Root -> Middle -> Left -> Left -> Right
        } else {
          assert(uxl->GetWeight() > 0);
          //TODO: weakLLX(uxxl);
          return RB2_R(u, ux, uxx, uxxl);
        }

      // Root -> Middle -> Left -> Right
      } else {
        assert(!uxx_is_right);
        // Root -> Middle -> Left -> Right -> Left
        if (uxr->GetWeight() == 0) {
          //TODO: weakLLX(uxr);
          return BLK(u, ux, uxx, uxr);

        // Root -> Middle -> Left -> Right -> Right
        } else {
          assert(uxr->GetWeight() > 0);
          return RB1_L(u, ux, uxx);
        }
      }

    // Root -> Middle -> Right
    } else {
      assert(uxx->GetWeight() > 0);
      //TODO: weakLLX(uxxl);
      // Note: we know that 'uxxl' is not a leaf because it has weight 0.
      Node* uxxlr = uxxl->GetRight();
      //TODO: weakLLX(uxxlr);

      // Root -> Middle -> Right -> Left
      if (uxxlr->GetWeight() == 0) {
        return RB2_L(ux, uxx, uxxl, uxxlr);

      // Root -> Middle -> Right -> Middle
      } else if (uxxlr->GetWeight() == 1) {
        Node* uxxlrl = uxxlr->GetLeft();
        Node* uxxlrr = uxxlr->GetRight();
        if (uxxlrl == NULL) return EMBB_BUSY;

        // Root -> Middle -> Right -> Middle -> Left
        if (uxxlrl->GetWeight() == 0) {
          //TODO: weakLLX(uxxlrl);
          return W4_R(ux, uxx, uxxl, uxxr, uxxlr, uxxlrl);

        // Root -> Middle -> Right -> Middle -> Right
        } else {
          assert(uxxlrl->GetWeight() > 0);
          // Root -> Middle -> Right -> Middle -> Right -> Left
          if (uxxlrr->GetWeight() == 0) {
            //TODO: weakLLX(uxxlrr);
            return W3_R(ux, uxx, uxxl, uxxr, uxxlr, uxxlrr);

          // Root -> Middle -> Right -> Middle -> Right -> Right
          } else {
            assert(uxxlrr->GetWeight() > 0);
            return W2_R(ux, uxx, uxxl, uxxr, uxxlr);
          }
        }

      // Root -> Middle -> Right -> Right
      } else {
        assert(uxxlr->GetWeight() > 1);
        return W1_R(ux, uxx, uxxl, uxxr, uxxlr);
      }
    }

  // Root -> Right
  } else if (uxxl->GetWeight() == 1) {
    //TODO: weakLLX(uxxl);
    Node* uxxll = uxxl->GetLeft();
    Node* uxxlr = uxxl->GetRight();
    if (uxxll == NULL) return EMBB_BUSY;

    // Root -> Right -> Left
    if (uxxll->GetWeight() == 0) {
      //TODO: weakLLX(uxxll);
      return W5_R(ux, uxx, uxxl, uxxr, uxxll);

    // Root -> Right -> Right
    } else {
      assert(uxxll->GetWeight() > 0);
      // Root -> Right -> Right -> Left
      if (uxxlr->GetWeight() == 0) {
        //TODO: weakLLX(uxxlr);
        return W6_R(ux, uxx, uxxl, uxxr, uxxlr);

      // Root -> Right -> Right -> Right
      } else {
        assert(uxxlr->GetWeight() > 0);
        return PUSH_R(ux, uxx, uxxl, uxxr);
      }
    }

  // Root -> Left
  } else {
    assert(uxxl->GetWeight() > 1);
    //TODO: weakLLX(uxxl);
    return W7(ux, uxx, uxxl, uxxr);
  }
}

} // namespace containers
} // namespace embb

#endif // EMBB_CONTAINERS_INTERNAL_CGL_CHROMATIC_TREE_INL_H_
