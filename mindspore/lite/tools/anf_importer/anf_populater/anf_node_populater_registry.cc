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

#include "tools/anf_importer/anf_populater/anf_node_populater_registry.h"
#include <string>
namespace mindspore {
namespace lite {
AnfNodePopulaterRegistry::~AnfNodePopulaterRegistry() {
  for (auto ite : populaters) {
    if (ite.second != nullptr) {
      delete ite.second;
      ite.second = nullptr;
    }
  }
}
AnfNodePopulaterRegistry *AnfNodePopulaterRegistry::GetInstance() {
  static AnfNodePopulaterRegistry instance;
  return &instance;
}
AnfNodePopulater *AnfNodePopulaterRegistry::GetNodePopulater(const std::string &name) {
  if (populaters.find(name) == populaters.end()) {
    return nullptr;
  }
  return populaters[name];
}
void AnfNodePopulaterRegistry::SetNodePopulater(const std::string &name, AnfNodePopulater *populater) {
  populaters[name] = populater;
}
}  // namespace lite
}  // namespace mindspore
