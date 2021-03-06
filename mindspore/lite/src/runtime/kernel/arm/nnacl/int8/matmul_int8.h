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

#ifndef MINDSPORE_LITE_SRC_RUNTIME_KERNEL_ARM_NNACL_INT8_MATMUL_H_
#define MINDSPORE_LITE_SRC_RUNTIME_KERNEL_ARM_NNACL_INT8_MATMUL_H_

#include <string.h>
#include "nnacl/op_base.h"
#include "nnacl/matmul_parameter.h"

#ifdef __cplusplus
extern "C" {
#endif
void MatMulInt8(const int8_t *a, const int8_t *b, int *c, const int row8, const int col8, const int deep,
                const int a_zp, const int b_zp);
void MatMulInt8_16x4(const int8_t *a, const int8_t *b, int *dst, int row_4, int col_4, int deep_16,
                     const int *input_sum, const int *bias);
void RowMajor2Row8MajorInt8(int8_t *src_ptr, int8_t *dst_ptr, int row, int col);
void RowMajor2Col8MajorInt8(int8_t *src_ptr, int8_t *dst_ptr, int row, int col);
void RowMajor2Row16x4MajorInt8(void *src_ptr, void *dst_ptr, int row, int col);

#ifdef ENABLE_ARM64
void RowMajor2Row4x16Major(int8_t *src, int row, int col, int8_t *dst, int col_16);
void RowMajor2Col16x4Major(int8_t *src, int row, int col, int8_t *dst, int row_16);
void RowMajor2Asums(int8_t *a, int row, int col, int b_zp, int *dst);
void RowMajor2Bbias(int8_t *b, int row, int col, int a_zp, int b_zp, int *bias, int *dst);
void Row4x4Major2RowMajor(int8_t *src, int row4, int8_t *dst, int row, int cow);

// bias = bias + depth * a_zp * b_zp - a_zp * b_sums
void MatmulInt8Neon64(const int8_t *a, const int8_t *b, int8_t *dst, int row4, int col4, int deep16, const int *a_sums,
                      const int *bias, int act_min, int act_max, int out_zp, int multiplier, int left_shift,
                      int right_shift);

void MatMulR4Int8Neon64(const int8_t *a, const int8_t *b, int32_t *dst, int row4, int col4, int deep16,
                        const int *input_sum, const int *bias);
#endif
#ifdef __cplusplus
}
#endif

#endif  // MINDSPORE_LITE_SRC_BACKEND_ARM_NNACL_INT8_MATMUL_H_
