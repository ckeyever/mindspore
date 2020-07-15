/**
 * Copyright 2020 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DATASET_ENGINE_OPT_PASS_PRE_INJECTION_PASS_H_
#define DATASET_ENGINE_OPT_PASS_PRE_INJECTION_PASS_H_

#include <memory>
#include <vector>
#include "minddata/dataset/engine/opt/pass.h"

namespace mindspore {
namespace dataset {

class DatasetOp;

/// \class InjectionPass injection_pass.h
/// \brief This is a pre pass that drives the injection of any nodes that could not be directly injected from the api
///     parsing.
class InjectionPass : public TreePass {
  /// \class InjectionFinder
  /// \brief This is a nested node pass class who's job is to parse the tree and perform any identification logic for
  ///     operators that need to be injected.  It is run first by the main injection pass to find out what operators
  ///     it may need to inject.
  class InjectionFinder : public NodePass {
   public:
    /// \brief Constructor
    explicit InjectionFinder(InjectionPass *injection_pass);

    /// \brief Performs finder work for BuildVocabOp that has special rules about epoch control injection.
    /// \param[in] node The node being visited
    /// \param[inout] modified Indicator if the node was changed at all
    /// \return Status The error code return
    Status PreRunOnNode(std::shared_ptr<BuildVocabOp> node, bool *modified) override;

    /// \brief Temporary code to prevent the injection of epoch control when cache op is present.
    ///     Remove this code in cache op phase 2
    /// \param[in] node The node being visited
    /// \param[inout] modified Indicator if the node was changed at all
    /// \return Status The error code return
    Status PreRunOnNode(std::shared_ptr<CacheOp> node, bool *modified) override;

   private:
    InjectionPass *injection_pass_;
  };

 public:
  /// \brief Constructor
  InjectionPass();

  /// \brief Runs an injection pass to inject in operators needed at the pre pass stage
  /// \param[inout] tree The tree to operate on.
  /// \param[inout] Indicate of the tree was modified.
  /// \return Status The error code return
  Status RunOnTree(ExecutionTree *tree, bool *modified) override;

 private:
  bool epoch_ctrl_bypass_;
};
}  // namespace dataset
}  // namespace mindspore

#endif  // DATASET_ENGINE_OPT_PASS_PRE_INJECTION_PASS_H_
