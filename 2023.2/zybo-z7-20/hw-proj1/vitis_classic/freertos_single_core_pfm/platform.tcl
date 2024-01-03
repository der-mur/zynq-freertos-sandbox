# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct F:\Documents\GitHub\zynq-freertos-sandbox\2023.2\zybo-z7-20\hw-proj1\vitis_classic\freertos_single_core_pfm\platform.tcl
# 
# OR launch xsct and run below command.
# source F:\Documents\GitHub\zynq-freertos-sandbox\2023.2\zybo-z7-20\hw-proj1\vitis_classic\freertos_single_core_pfm\platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {freertos_single_core_pfm}\
-hw {F:\Documents\GitHub\zynq-freertos-sandbox\2023.2\zybo-z7-20\hw-proj1\xsa\hw_proj1_wrapper.xsa}\
-proc {ps7_cortexa9_0} -os {freertos10_xilinx} -no-boot-bsp -out {F:/Documents/GitHub/zynq-freertos-sandbox/2023.2/zybo-z7-20/hw-proj1/vitis_classic}

platform write
platform generate -domains 
platform active {freertos_single_core_pfm}
platform generate
platform clean
platform generate
platform config -create-boot-bsp
platform write
platform generate -domains zynq_fsbl 
platform clean
platform generate
platform clean
platform generate
