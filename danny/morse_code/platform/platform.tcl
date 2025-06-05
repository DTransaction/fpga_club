# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/danny/Documents/fpga_club/danny/morse_code/platform/platform.tcl
# 
# OR launch xsct and run below command.
# source /home/danny/Documents/fpga_club/danny/morse_code/platform/platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {platform}\
-hw {/home/danny/Documents/fpga_club/danny/morse_code/morse_code_vivado/design_1_wrapper.xsa}\
-proc {ps7_cortexa9_0} -os {standalone} -out {/home/danny/Documents/fpga_club/danny/morse_code}

platform write
platform generate -domains 
platform active {platform}
platform clean
platform generate
