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
#include <iostream>
#include <memory>

#include "common/common_test.h"
#include "mindspore/lite/src/common/file_utils.h"
#include "mindspore/lite/src/runtime/opencl/opencl_runtime.h"
#include "mindspore/lite/src/runtime/kernel/opencl/subgraph_opencl_kernel.h"
#include "mindspore/lite/src/runtime/kernel/opencl/kernel/conv2d_transpose.h"
#include "mindspore/core/utils/log_adapter.h"

namespace mindspore {
class TestConv2dTransposeOpenCL : public mindspore::CommonTest {
 public:
  TestConv2dTransposeOpenCL() {}
};

TEST_F(TestConv2dTransposeOpenCL, Conv2dTransposeFp32) {
  auto ocl_runtime = lite::opencl::OpenCLRuntime::GetInstance();
  ocl_runtime->Init();
  auto allocator = ocl_runtime->GetAllocator();
  int pad = 0;
  int n = 1;
  int h = 240;
  int w = 240;
  int kh = 2;
  int kw = 2;
  int ci = 128;
  int co = 128;
  int oh = 2 * h - 1 + 2 * (kh - 1 - pad) - kh + 1;
  int ow = 2 * w - 1 + 2 * (kw - 1 - pad) - kw + 1;

  size_t input_size;
  std::string input_path = "./test_data/conv2d_transpose/conv2d_transpose_fp32_input.bin";
  auto input_data = reinterpret_cast<float *>(mindspore::lite::ReadFile(input_path.c_str(), &input_size));
  if (input_data == nullptr) {
    MS_LOG(ERROR) << "input_data load error.";
    return;
  }

  size_t weight_size;
  std::string weight_path = "./test_data/conv2d_transpose/conv2d_transpose_fp32_weight.bin";
  auto weight_data = reinterpret_cast<float *>(mindspore::lite::ReadFile(weight_path.c_str(), &weight_size));
  if (weight_data == nullptr) {
    MS_LOG(ERROR) << "weight_data load error.";
    return;
  }

  size_t bias_size;
  std::string bias_path = "./test_data/conv2d_transpose/conv2d_transpose_fp32_bias.bin";
  auto bias_data = reinterpret_cast<float *>(mindspore::lite::ReadFile(bias_path.c_str(), &bias_size));
  if (bias_data == nullptr) {
    MS_LOG(ERROR) << "bias_data load error.";
    return;
  }
  std::vector<int> input_shape = {n, h, w, ci};
  auto tensor_x_ptr = std::make_unique<lite::tensor::Tensor>(TypeId(kNumberTypeFloat32), input_shape);
  auto tensor_x = tensor_x_ptr.get();
  if (tensor_x == nullptr) {
    MS_LOG(ERROR) << "tensor_x create error.";
    return;
  }

  std::vector<int> weight_shape = {co, kh, kw, ci};
  auto tensor_w_ptr = std::make_unique<lite::tensor::Tensor>(TypeId(kNumberTypeFloat32), weight_shape);
  auto tensor_w = tensor_w_ptr.get();
  if (tensor_w == nullptr) {
    MS_LOG(ERROR) << "tensor_w create error.";
    return;
  }
  tensor_w->SetData(weight_data);

  std::vector<int> bias_shape = {co};
  auto tensor_bias_ptr = std::make_unique<lite::tensor::Tensor>(TypeId(kNumberTypeFloat32), bias_shape);
  auto tensor_bias = tensor_bias_ptr.get();
  if (tensor_bias == nullptr) {
    MS_LOG(ERROR) << "tensor_bias create error.";
    return;
  }
  tensor_bias->SetData(bias_data);

  std::vector<int> out_shape = {1, oh, ow, co};
  auto tensor_out_ptr = std::make_unique<lite::tensor::Tensor>(TypeId(kNumberTypeFloat32), out_shape);
  auto tensor_out = tensor_out_ptr.get();
  if (tensor_out == nullptr) {
    MS_LOG(ERROR) << "tensor_out create error.";
    return;
  }
  std::vector<lite::tensor::Tensor *> inputs{tensor_x, tensor_w, tensor_bias};
  std::vector<lite::tensor::Tensor *> outputs{tensor_out};
  auto opParameter_ptr = std::make_unique<ConvParameter>();
  auto opParameter = opParameter_ptr.get();
  if (opParameter == nullptr) {
    MS_LOG(ERROR) << "opParameter create error.";
    return;
  }
  opParameter->kernel_h_ = kh;
  opParameter->kernel_w_ = kw;
  opParameter->stride_h_ = 2;
  opParameter->stride_w_ = 2;
  opParameter->pad_h_ = pad;
  opParameter->pad_w_ = pad;
  opParameter->input_channel_ = ci;
  opParameter->output_channel_ = co;
  auto arith_kernel_ptr = std::make_unique<kernel::Conv2dTransposeOpenCLKernel>(
    reinterpret_cast<OpParameter *>(opParameter), inputs, outputs);
  auto arith_kernel = arith_kernel_ptr.get();
  if (arith_kernel == nullptr) {
    MS_LOG(ERROR) << "arith_kernel create error.";
    return;
  }
  arith_kernel->Init();

  inputs[0]->MallocData(allocator);
  std::vector<kernel::LiteKernel *> kernels{arith_kernel};
  std::vector<lite::tensor::Tensor *> inputs_g{tensor_x};
  auto pGraph_ptr = std::make_unique<kernel::SubGraphOpenCLKernel>(inputs_g, outputs, kernels, kernels, kernels);
  auto pGraph = pGraph_ptr.get();
  if (pGraph == nullptr) {
    MS_LOG(ERROR) << "pGraph create error.";
    return;
  }

  pGraph->Init();
  memcpy(inputs[0]->Data(), input_data, input_size);
  pGraph->Run();

  std::cout << "==================output data=================" << std::endl;
  float *output_data = reinterpret_cast<float *>(tensor_out->Data());
  std::cout << std::endl;
  size_t output_size;
  std::string output_path = "./test_data/conv2d_transpose/conv2d_transpose_fp32_output.bin";
  auto correct_data = reinterpret_cast<float *>(mindspore::lite::ReadFile(output_path.c_str(), &output_size));
  if (correct_data == nullptr) {
    MS_LOG(ERROR) << "correct_data create error.";
    return;
  }
  int size_n = oh * ow * co;
  size_n = size_n > 100 ? 100 : size_n;
  for (int i = 0; i < size_n; i++) {
    std::cout << output_data[i] << ", ";
    if ((i + 1) % co == 0) {
      std::cout << std::endl;
    }
  }
  std::cout << std::endl;

  // compare
  CompareOutputData(output_data, correct_data, oh * ow * co, 0.00001);

  MS_LOG(INFO) << "Test Conv2dTransposeFp32 passed";
  lite::opencl::OpenCLRuntime::DeleteInstance();
}
}  // namespace mindspore
