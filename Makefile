LOCAL_DIR=/home/centos/scharr-paint-filter
VITIS_VISION_L1=/home/centos/Vitis_Libraries/vision/L1
AWS_PFM=xilinx_aws-vu9p-f1_shell-v04261818_201920_2
PFM=~/aws-fpga/Vitis/aws_platform/${AWS_PFM}/${AWS_PFM}.xpfm


DEVICE_INCLUDES=-I${LOCAL_DIR}/src -I$(VITIS_VISION_L1)/include
XILINX_INCLUDES=-I${XILINX_XRT}/include -I${XILINX_VIVADO}/include
HOST_INCLUDES= ${XILINX_INCLUDES} -I${LOCAL_DIR}/src -I${VITIS_VISION_L1}/include
HOST_LIBS=-lopencv_core -lopencv_imgproc -lopencv_features2d -lopencv_flann -lopencv_video -lopencv_calib3d -lopencv_highgui  -L${XILINX_XRT}/lib/ -L/usr/lib64/ -lOpenCL -lpthread -lrt 

xcl:
	v++ -f ${PFM} -t ${XCL_EMULATION_MODE} --log_dir output -c -g ${DEVICE_INCLUDES} -k scharr_accel -o build/scharr.xo src/xf_scharr_accel.cpp
	v++ -f ${PFM} -t ${XCL_EMULATION_MODE} --log_dir=output -l -g -j 7 -o krnl_scharr.xclbin build/scharr.xo

host:
	g++ -std=c++11 ${XILINX_INCLUDES} -c src/xcl2.cpp -o build/xcl2.o
	g++ -std=c++11 ${HOST_INCLUDES} -o scharr_example src/xf_scharr_tb.cpp build/xcl2.o ${HOST_LIBS}


