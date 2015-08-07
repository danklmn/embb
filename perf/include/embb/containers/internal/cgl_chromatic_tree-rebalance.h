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

//
// This file was created automatically by a code generator.
// Any direct changes will be lost after rebuild of the project.
//

#ifndef EMBB_CONTAINERS_INTERNAL_CGL_CHROMATIC_TREE_REBALANCE_H_
#define EMBB_CONTAINERS_INTERNAL_CGL_CHROMATIC_TREE_REBALANCE_H_

embb_errors_t BLK(Node*& u,
          Node*& ux,
          Node*& uxl,
          Node*& uxr) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxl;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxl = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), 1,
        uxl->GetLeft(), uxl->GetRight());
    if (nxl == NULL) break;
    nxr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), 1,
        uxr->GetLeft(), uxr->GetRight());
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), IsSentinel(u) ? 1 : ux->GetWeight() - 1,
        nxl, nxr);
    if (nx == NULL) break;

    u->ReplaceChild(ux, nx);
    FreeNode(ux);
    FreeNode(uxl);
    FreeNode(uxr);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t PUSH_L(Node*& u,
             Node*& ux,
             Node*& uxl,
             Node*& uxr) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxl;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxl = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), uxl->GetWeight() - 1,
        uxl->GetLeft(), uxl->GetRight());
    if (nxl == NULL) break;
    nxr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), 0,
        uxr->GetLeft(), uxr->GetRight());
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), IsSentinel(u) ? 1 : ux->GetWeight() + 1,
        nxl, nxr);
    if (nx == NULL) break;

    u->ReplaceChild(ux, nx);
    FreeNode(ux);
    FreeNode(uxl);
    FreeNode(uxr);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t PUSH_R(Node*& u,
             Node*& ux,
             Node*& uxl,
             Node*& uxr) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxr;
  Node* nxl;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), uxr->GetWeight() - 1,
        uxr->GetLeft(), uxr->GetRight());
    if (nxr == NULL) break;
    nxl = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), 0,
        uxl->GetLeft(), uxl->GetRight());
    if (nxl == NULL) break;
    nx = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), IsSentinel(u) ? 1 : ux->GetWeight() + 1,
        nxl, nxr);
    if (nx == NULL) break;

    u->ReplaceChild(ux, nx);
    FreeNode(ux);
    FreeNode(uxl);
    FreeNode(uxr);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxr) FreeNode(nxr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t RB1_L(Node*& u,
            Node*& ux,
            Node*& uxl) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxr = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 0,
        uxl->GetRight(), ux->GetRight());
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), ux->GetWeight(),
        uxl->GetLeft(), nxr);
    if (nx == NULL) break;

    u->ReplaceChild(ux, nx);
    FreeNode(ux);
    FreeNode(uxl);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t RB1_R(Node*& u,
            Node*& ux,
            Node*& uxr) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxl;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxl = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 0,
        ux->GetLeft(), uxr->GetLeft());
    if (nxl == NULL) break;
    nx = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), ux->GetWeight(),
        nxl, uxr->GetRight());
    if (nx == NULL) break;

    u->ReplaceChild(ux, nx);
    FreeNode(ux);
    FreeNode(uxr);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t RB2_L(Node*& u,
            Node*& ux,
            Node*& uxl,
            Node*& uxlr) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxl;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxl = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), 0,
        uxl->GetLeft(), uxlr->GetLeft());
    if (nxl == NULL) break;
    nxr = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 0,
        uxlr->GetRight(), ux->GetRight());
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        uxlr->GetKey(), uxlr->GetValue(), ux->GetWeight(),
        nxl, nxr);
    if (nx == NULL) break;

    u->ReplaceChild(ux, nx);
    FreeNode(ux);
    FreeNode(uxl);
    FreeNode(uxlr);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t RB2_R(Node*& u,
            Node*& ux,
            Node*& uxr,
            Node*& uxrl) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxr;
  Node* nxl;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), 0,
        uxrl->GetRight(), uxr->GetRight());
    if (nxr == NULL) break;
    nxl = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 0,
        ux->GetLeft(), uxrl->GetLeft());
    if (nxl == NULL) break;
    nx = node_pool_.Allocate(
        uxrl->GetKey(), uxrl->GetValue(), ux->GetWeight(),
        nxl, nxr);
    if (nx == NULL) break;

    u->ReplaceChild(ux, nx);
    FreeNode(ux);
    FreeNode(uxr);
    FreeNode(uxrl);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxr) FreeNode(nxr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W1_L(Node*& u,
           Node*& ux,
           Node*& uxl,
           Node*& uxr,
           Node*& uxrl) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxll;
  Node* nxlr;
  Node* nxl;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxll = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), uxl->GetWeight() - 1,
        uxl->GetLeft(), uxl->GetRight());
    if (nxll == NULL) break;
    nxlr = node_pool_.Allocate(
        uxrl->GetKey(), uxrl->GetValue(), uxrl->GetWeight() - 1,
        uxrl->GetLeft(), uxrl->GetRight());
    if (nxlr == NULL) break;
    nxl = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        nxll, nxlr);
    if (nxl == NULL) break;
    nx = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), ux->GetWeight(),
        nxl, uxr->GetRight());
    if (nx == NULL) break;

    u->ReplaceChild(ux, nx);
    FreeNode(ux);
    FreeNode(uxl);
    FreeNode(uxr);
    FreeNode(uxrl);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxll) FreeNode(nxll);
    if (nxlr) FreeNode(nxlr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W1_R(Node*& u,
           Node*& ux,
           Node*& uxl,
           Node*& uxr,
           Node*& uxlr) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxrr;
  Node* nxrl;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxrr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), uxr->GetWeight() - 1,
        uxr->GetLeft(), uxr->GetRight());
    if (nxrr == NULL) break;
    nxrl = node_pool_.Allocate(
        uxlr->GetKey(), uxlr->GetValue(), uxlr->GetWeight() - 1,
        uxlr->GetLeft(), uxlr->GetRight());
    if (nxrl == NULL) break;
    nxr = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        nxrl, nxrr);
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), ux->GetWeight(),
        uxl->GetLeft(), nxr);
    if (nx == NULL) break;

    u->ReplaceChild(ux, nx);
    FreeNode(ux);
    FreeNode(uxl);
    FreeNode(uxr);
    FreeNode(uxlr);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxrr) FreeNode(nxrr);
    if (nxrl) FreeNode(nxrl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W2_L(Node*& u,
           Node*& ux,
           Node*& uxl,
           Node*& uxr,
           Node*& uxrl) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxll;
  Node* nxlr;
  Node* nxl;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxll = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), uxl->GetWeight() - 1,
        uxl->GetLeft(), uxl->GetRight());
    if (nxll == NULL) break;
    nxlr = node_pool_.Allocate(
        uxrl->GetKey(), uxrl->GetValue(), 0,
        uxrl->GetLeft(), uxrl->GetRight());
    if (nxlr == NULL) break;
    nxl = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        nxll, nxlr);
    if (nxl == NULL) break;
    nx = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), ux->GetWeight(),
        nxl, uxr->GetRight());
    if (nx == NULL) break;

    u->ReplaceChild(ux, nx);
    FreeNode(ux);
    FreeNode(uxl);
    FreeNode(uxr);
    FreeNode(uxrl);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxll) FreeNode(nxll);
    if (nxlr) FreeNode(nxlr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W2_R(Node*& u,
           Node*& ux,
           Node*& uxl,
           Node*& uxr,
           Node*& uxlr) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxrr;
  Node* nxrl;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxrr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), uxr->GetWeight() - 1,
        uxr->GetLeft(), uxr->GetRight());
    if (nxrr == NULL) break;
    nxrl = node_pool_.Allocate(
        uxlr->GetKey(), uxlr->GetValue(), 0,
        uxlr->GetLeft(), uxlr->GetRight());
    if (nxrl == NULL) break;
    nxr = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        nxrl, nxrr);
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), ux->GetWeight(),
        uxl->GetLeft(), nxr);
    if (nx == NULL) break;

    u->ReplaceChild(ux, nx);
    FreeNode(ux);
    FreeNode(uxl);
    FreeNode(uxr);
    FreeNode(uxlr);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxrr) FreeNode(nxrr);
    if (nxrl) FreeNode(nxrl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W3_L(Node*& u,
           Node*& ux,
           Node*& uxl,
           Node*& uxr,
           Node*& uxrl,
           Node*& uxrll) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxlll;
  Node* nxll;
  Node* nxlr;
  Node* nxl;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxlll = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), uxl->GetWeight() - 1,
        uxl->GetLeft(), uxl->GetRight());
    if (nxlll == NULL) break;
    nxll = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        nxlll, uxrll->GetLeft());
    if (nxll == NULL) break;
    nxlr = node_pool_.Allocate(
        uxrl->GetKey(), uxrl->GetValue(), 1,
        uxrll->GetRight(), uxrl->GetRight());
    if (nxlr == NULL) break;
    nxl = node_pool_.Allocate(
        uxrll->GetKey(), uxrll->GetValue(), 0,
        nxll, nxlr);
    if (nxl == NULL) break;
    nx = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), ux->GetWeight(),
        nxl, uxr->GetRight());
    if (nx == NULL) break;

    u->ReplaceChild(ux, nx);
    FreeNode(ux);
    FreeNode(uxl);
    FreeNode(uxr);
    FreeNode(uxrl);
    FreeNode(uxrll);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxlll) FreeNode(nxlll);
    if (nxll) FreeNode(nxll);
    if (nxlr) FreeNode(nxlr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W3_R(Node*& u,
           Node*& ux,
           Node*& uxl,
           Node*& uxr,
           Node*& uxlr,
           Node*& uxlrr) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxrrr;
  Node* nxrr;
  Node* nxrl;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxrrr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), uxr->GetWeight() - 1,
        uxr->GetLeft(), uxr->GetRight());
    if (nxrrr == NULL) break;
    nxrr = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        uxlrr->GetRight(), nxrrr);
    if (nxrr == NULL) break;
    nxrl = node_pool_.Allocate(
        uxlr->GetKey(), uxlr->GetValue(), 1,
        uxlr->GetLeft(), uxlrr->GetLeft());
    if (nxrl == NULL) break;
    nxr = node_pool_.Allocate(
        uxlrr->GetKey(), uxlrr->GetValue(), 0,
        nxrl, nxrr);
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), ux->GetWeight(),
        uxl->GetLeft(), nxr);
    if (nx == NULL) break;

    u->ReplaceChild(ux, nx);
    FreeNode(ux);
    FreeNode(uxl);
    FreeNode(uxr);
    FreeNode(uxlr);
    FreeNode(uxlrr);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxrrr) FreeNode(nxrrr);
    if (nxrr) FreeNode(nxrr);
    if (nxrl) FreeNode(nxrl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W4_L(Node*& u,
           Node*& ux,
           Node*& uxl,
           Node*& uxr,
           Node*& uxrl,
           Node*& uxrlr) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxll;
  Node* nxrl;
  Node* nxl;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxll = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), uxl->GetWeight() - 1,
        uxl->GetLeft(), uxl->GetRight());
    if (nxll == NULL) break;
    nxrl = node_pool_.Allocate(
        uxrlr->GetKey(), uxrlr->GetValue(), 1,
        uxrlr->GetLeft(), uxrlr->GetRight());
    if (nxrl == NULL) break;
    nxl = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        nxll, uxrl->GetLeft());
    if (nxl == NULL) break;
    nxr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), 0,
        nxrl, uxr->GetRight());
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        uxrl->GetKey(), uxrl->GetValue(), ux->GetWeight(),
        nxl, nxr);
    if (nx == NULL) break;

    u->ReplaceChild(ux, nx);
    FreeNode(ux);
    FreeNode(uxl);
    FreeNode(uxr);
    FreeNode(uxrl);
    FreeNode(uxrlr);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxll) FreeNode(nxll);
    if (nxrl) FreeNode(nxrl);
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W4_R(Node*& u,
           Node*& ux,
           Node*& uxl,
           Node*& uxr,
           Node*& uxlr,
           Node*& uxlrl) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxrr;
  Node* nxlr;
  Node* nxr;
  Node* nxl;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxrr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), uxr->GetWeight() - 1,
        uxr->GetLeft(), uxr->GetRight());
    if (nxrr == NULL) break;
    nxlr = node_pool_.Allocate(
        uxlrl->GetKey(), uxlrl->GetValue(), 1,
        uxlrl->GetLeft(), uxlrl->GetRight());
    if (nxlr == NULL) break;
    nxr = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        uxlr->GetRight(), nxrr);
    if (nxr == NULL) break;
    nxl = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), 0,
        uxl->GetLeft(), nxlr);
    if (nxl == NULL) break;
    nx = node_pool_.Allocate(
        uxlr->GetKey(), uxlr->GetValue(), ux->GetWeight(),
        nxl, nxr);
    if (nx == NULL) break;

    u->ReplaceChild(ux, nx);
    FreeNode(ux);
    FreeNode(uxl);
    FreeNode(uxr);
    FreeNode(uxlr);
    FreeNode(uxlrl);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxrr) FreeNode(nxrr);
    if (nxlr) FreeNode(nxlr);
    if (nxr) FreeNode(nxr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W5_L(Node*& u,
           Node*& ux,
           Node*& uxl,
           Node*& uxr,
           Node*& uxrr) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxll;
  Node* nxl;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxll = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), uxl->GetWeight() - 1,
        uxl->GetLeft(), uxl->GetRight());
    if (nxll == NULL) break;
    nxl = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        nxll, uxr->GetLeft());
    if (nxl == NULL) break;
    nxr = node_pool_.Allocate(
        uxrr->GetKey(), uxrr->GetValue(), 1,
        uxrr->GetLeft(), uxrr->GetRight());
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), ux->GetWeight(),
        nxl, nxr);
    if (nx == NULL) break;

    u->ReplaceChild(ux, nx);
    FreeNode(ux);
    FreeNode(uxl);
    FreeNode(uxr);
    FreeNode(uxrr);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxll) FreeNode(nxll);
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W5_R(Node*& u,
           Node*& ux,
           Node*& uxl,
           Node*& uxr,
           Node*& uxll) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxrr;
  Node* nxr;
  Node* nxl;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxrr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), uxr->GetWeight() - 1,
        uxr->GetLeft(), uxr->GetRight());
    if (nxrr == NULL) break;
    nxr = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        uxl->GetRight(), nxrr);
    if (nxr == NULL) break;
    nxl = node_pool_.Allocate(
        uxll->GetKey(), uxll->GetValue(), 1,
        uxll->GetLeft(), uxll->GetRight());
    if (nxl == NULL) break;
    nx = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), ux->GetWeight(),
        nxl, nxr);
    if (nx == NULL) break;

    u->ReplaceChild(ux, nx);
    FreeNode(ux);
    FreeNode(uxl);
    FreeNode(uxr);
    FreeNode(uxll);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxrr) FreeNode(nxrr);
    if (nxr) FreeNode(nxr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W6_L(Node*& u,
           Node*& ux,
           Node*& uxl,
           Node*& uxr,
           Node*& uxrl) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxll;
  Node* nxl;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxll = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), uxl->GetWeight() - 1,
        uxl->GetLeft(), uxl->GetRight());
    if (nxll == NULL) break;
    nxl = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        nxll, uxrl->GetLeft());
    if (nxl == NULL) break;
    nxr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), 1,
        uxrl->GetRight(), uxr->GetRight());
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        uxrl->GetKey(), uxrl->GetValue(), ux->GetWeight(),
        nxl, nxr);
    if (nx == NULL) break;

    u->ReplaceChild(ux, nx);
    FreeNode(ux);
    FreeNode(uxl);
    FreeNode(uxr);
    FreeNode(uxrl);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxll) FreeNode(nxll);
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W6_R(Node*& u,
           Node*& ux,
           Node*& uxl,
           Node*& uxr,
           Node*& uxlr) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxrr;
  Node* nxr;
  Node* nxl;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxrr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), uxr->GetWeight() - 1,
        uxr->GetLeft(), uxr->GetRight());
    if (nxrr == NULL) break;
    nxr = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        uxlr->GetRight(), nxrr);
    if (nxr == NULL) break;
    nxl = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), 1,
        uxl->GetLeft(), uxlr->GetLeft());
    if (nxl == NULL) break;
    nx = node_pool_.Allocate(
        uxlr->GetKey(), uxlr->GetValue(), ux->GetWeight(),
        nxl, nxr);
    if (nx == NULL) break;

    u->ReplaceChild(ux, nx);
    FreeNode(ux);
    FreeNode(uxl);
    FreeNode(uxr);
    FreeNode(uxlr);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxrr) FreeNode(nxrr);
    if (nxr) FreeNode(nxr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W7(Node*& u,
         Node*& ux,
         Node*& uxl,
         Node*& uxr) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxl;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxl = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), uxl->GetWeight() - 1,
        uxl->GetLeft(), uxl->GetRight());
    if (nxl == NULL) break;
    nxr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), uxr->GetWeight() - 1,
        uxr->GetLeft(), uxr->GetRight());
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), IsSentinel(u) ? 1 : ux->GetWeight() + 1,
        nxl, nxr);
    if (nx == NULL) break;

    u->ReplaceChild(ux, nx);
    FreeNode(ux);
    FreeNode(uxl);
    FreeNode(uxr);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

#endif // EMBB_CONTAINERS_INTERNAL_CGL_CHROMATIC_TREE_REBALANCE_H_
