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

#include "xf_scharr_config.h"

extern "C" {
void scharr_accel(ap_uint<INPUT_PTR_WIDTH>* img_inp,
                  ap_uint<OUTPUT_PTR_WIDTH>* img_out,
                  int rows,
                  int cols,
				  int threshold) {
// clang-format off
    #pragma HLS INTERFACE m_axi     port=img_inp  offset=slave bundle=gmem1
    #pragma HLS INTERFACE m_axi     port=img_out  offset=slave bundle=gmem2
// clang-format on

// clang-format off
    
    #pragma HLS INTERFACE s_axilite port=rows     
    #pragma HLS INTERFACE s_axilite port=cols     
    #pragma HLS INTERFACE s_axilite port=return
    // clang-format on

    xf::cv::Mat<XF_8UC3, HEIGHT, WIDTH, NPC1> in_mat(rows, cols);
    // clang-format off
    #pragma HLS stream variable=in_mat.data depth=2
    // clang-format on
    xf::cv::Mat<XF_8UC3, HEIGHT, WIDTH, NPC1> in_mat1(rows, cols);
    // clang-format off
    #pragma HLS stream variable=in_mat1.data depth=2
    // clang-format on
    xf::cv::Mat<XF_8UC3, HEIGHT, WIDTH, NPC1> in_mat2(rows, cols);
    #pragma HLS stream variable=in_mat2.data depth=2
    xf::cv::Mat<XF_8UC3, HEIGHT, WIDTH, NPC1> in_mat3(rows, cols);
    #pragma HLS stream variable=in_mat3.data depth=2
    xf::cv::Mat<XF_8UC3, HEIGHT, WIDTH, NPC1> in_mat4(rows, cols);
    #pragma HLS stream variable=in_mat4.data depth=2
    xf::cv::Mat<XF_8UC3, HEIGHT, WIDTH, NPC1> in_mat5(rows, cols);
    #pragma HLS stream variable=in_mat5.data depth=2

    xf::cv::Mat<XF_8UC3, HEIGHT, WIDTH, NPC1> out_mat(rows, cols);
    #pragma HLS stream variable=in_mat.data depth=2

    xf::cv::Mat<XF_8UC1, HEIGHT, WIDTH, NPC1> _dstgx(rows, cols);
    #pragma HLS stream variable=_dstgx.data depth=2

    xf::cv::Mat<XF_8UC1, HEIGHT, WIDTH, NPC1> _dstgy(rows, cols);
    #pragma HLS stream variable=_dstgy.data depth=2

    xf::cv::Mat<XF_8UC1, HEIGHT, WIDTH, NPC1> gray_mat(rows, cols);
    #pragma HLS stream variable=gray_mat.data depth=2

    xf::cv::Mat<XF_8UC1, HEIGHT, WIDTH, NPC1> mask(rows, cols);
    #pragma HLS stream variable=mask.data depth=2
    xf::cv::Mat<XF_8UC1, HEIGHT, WIDTH, NPC1> clamped_mask(rows, cols);
    #pragma HLS stream variable=clamped_mask.data depth=2
    xf::cv::Mat<XF_8UC1, HEIGHT, WIDTH, NPC1> mask_r(rows, cols);
    #pragma HLS stream variable=mask_r.data depth=2
    xf::cv::Mat<XF_8UC1, HEIGHT, WIDTH, NPC1> mask_g(rows, cols);
    #pragma HLS stream variable=mask_g.data depth=2
    xf::cv::Mat<XF_8UC1, HEIGHT, WIDTH, NPC1> mask_b(rows, cols);
    #pragma HLS stream variable=mask_b.data depth=2

    xf::cv::Mat<XF_8UC1, HEIGHT, WIDTH, NPC1> r_chan(rows, cols);
    #pragma HLS stream variable=r_chan.data depth=2
    xf::cv::Mat<XF_8UC1, HEIGHT, WIDTH, NPC1> g_chan(rows, cols);
    #pragma HLS stream variable=g_chan.data depth=2
    xf::cv::Mat<XF_8UC1, HEIGHT, WIDTH, NPC1> b_chan(rows, cols);
    #pragma HLS stream variable=b_chan.data depth=2

    xf::cv::Mat<XF_8UC1, HEIGHT, WIDTH, NPC1> out_r_chan(rows, cols);
    #pragma HLS stream variable=out_r_chan.data depth=2
    xf::cv::Mat<XF_8UC1, HEIGHT, WIDTH, NPC1> out_g_chan(rows, cols);
    #pragma HLS stream variable=out_g_chan.data depth=2
    xf::cv::Mat<XF_8UC1, HEIGHT, WIDTH, NPC1> out_b_chan(rows, cols);
    #pragma HLS stream variable=out_b_chan.data depth=2

    #pragma HLS DATAFLOW

    xf::cv::Array2xfMat<INPUT_PTR_WIDTH, XF_8UC3, HEIGHT, WIDTH, NPC1>(img_inp, in_mat);

    // Split input stream into 2
    xf::cv::duplicateMat<XF_8UC3, HEIGHT, WIDTH, NPC1>(in_mat, in_mat1, in_mat2);

    xf::cv::rgb2gray<XF_8UC3, XF_8UC1, HEIGHT, WIDTH, NPC1>(in_mat1, gray_mat);

    // Run Scharr on input and combine xy results to mask
    xf::cv::Scharr<XF_BORDER_CONSTANT, XF_8UC1, XF_8UC1, HEIGHT, WIDTH, NPC1>(gray_mat, _dstgx, _dstgy);
    xf::cv::add<XF_CONVERT_POLICY_SATURATE, XF_8UC1, HEIGHT, WIDTH, NPC1>(_dstgx, _dstgy, mask);
    xf::cv::Threshold<XF_THRESHOLD_TYPE_TOZERO, XF_8UC1, HEIGHT, WIDTH, NPC1>(mask, clamped_mask, threshold, 0);

   // Duplicate input to get 3 copies
    xf::cv::duplicateMat_3<XF_8UC3, HEIGHT, WIDTH, NPC1>(in_mat2, in_mat3, in_mat4, in_mat5);

    // Split in_mat into three channels
    xf::cv::extractChannel<XF_8UC3, XF_8UC1, HEIGHT, WIDTH, NPC1>(in_mat3, r_chan, XF_EXTRACT_CH_R);
    xf::cv::extractChannel<XF_8UC3, XF_8UC1, HEIGHT, WIDTH, NPC1>(in_mat4, g_chan, XF_EXTRACT_CH_G);
    xf::cv::extractChannel<XF_8UC3, XF_8UC1, HEIGHT, WIDTH, NPC1>(in_mat5, b_chan, XF_EXTRACT_CH_B);

    // Duplicate mask to get 3 copies
    xf::cv::duplicateMat_3<XF_8UC1, HEIGHT, WIDTH, NPC1>(clamped_mask, mask_r, mask_g, mask_b);

//    // Run paint mask on each of the channels
    unsigned char col_r[1];
    col_r[0]=255;
    unsigned char col_gb[1];
    col_gb[0]=0;
    xf::cv::paintmask<XF_8UC1, XF_8UC1, HEIGHT, WIDTH, NPC1>(r_chan, mask_r, out_r_chan, col_r);
    xf::cv::paintmask<XF_8UC1, XF_8UC1, HEIGHT, WIDTH, NPC1>(g_chan, mask_g, out_g_chan, col_gb);
    xf::cv::paintmask<XF_8UC1, XF_8UC1, HEIGHT, WIDTH, NPC1>(b_chan, mask_b, out_b_chan, col_gb);

    // This is smoother than paintmask but too saturated - would need to erode then blur perhaps?
//    xf::cv::add<XF_CONVERT_POLICY_SATURATE, XF_8UC1, HEIGHT, WIDTH, NPC1>(r_chan, mask_r, out_r_chan);
//    xf::cv::subtract<XF_CONVERT_POLICY_SATURATE, XF_8UC1, HEIGHT, WIDTH, NPC1>(g_chan, mask_g, out_g_chan);
//    xf::cv::subtract<XF_CONVERT_POLICY_SATURATE, XF_8UC1, HEIGHT, WIDTH, NPC1>(b_chan, mask_b, out_b_chan);

    // Channel combine to final_image
    xf::cv::merge<XF_8UC1, XF_8UC3, HEIGHT, WIDTH>(out_b_chan, out_g_chan, out_r_chan, out_mat);


    xf::cv::xfMat2Array<OUTPUT_PTR_WIDTH, XF_8UC3, HEIGHT, WIDTH, NPC1>(out_mat, img_out);
}
}
