/*
 * Copyright 2019 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _XF_SCHARR_CONFIG_H_
#define _XF_SCHARR_CONFIG_H_

#include "hls_stream.h"
#include "ap_int.h"
#include "common/xf_common.hpp"
#include "common/xf_utility.hpp"
#include "core/xf_arithm.hpp"
#include "imgproc/xf_extra_utility.h"
#include "imgproc/xf_scharr.hpp"
#include "imgproc/xf_channel_combine.hpp"
#include "imgproc/xf_channel_extract.hpp"
#include "imgproc/xf_threshold.hpp"
#undef _XF_THRESHOLD_HPP_
#include "imgproc/xf_paintmask.hpp"
#include "imgproc/xf_cvt_color.hpp"
#include "xf_config_params.h"

/* config width and height */
#define WIDTH 3840
#define HEIGHT 2160

typedef unsigned uint32_t;

/*  define the input and output types  */
#if NO
#define NPC1 XF_NPPC1
#endif

#if RO
#define NPC1 XF_NPPC8
#endif

#if GRAY
#define IN_TYPE XF_8UC1
#define OUT_TYPE XF_8UC1 // XF_16SC1 //
#define CH_TYPE XF_GRAY
#else
#define IN_TYPE XF_8UC3
#define OUT_TYPE XF_8UC3 // XF_16SC1 //
#define CH_TYPE XF_RGB
#endif

void scharr_accel(xf::cv::Mat<IN_TYPE, HEIGHT, WIDTH, NPC1>& _src,
                  xf::cv::Mat<OUT_TYPE, HEIGHT, WIDTH, NPC1>& _dst);

#endif // _XF_SCHARR_CONFIG_H_
