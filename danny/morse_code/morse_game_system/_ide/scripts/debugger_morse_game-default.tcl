# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: /home/danny/Documents/fpga_club/danny/morse_code/morse_game_system/_ide/scripts/debugger_morse_game-default.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source /home/danny/Documents/fpga_club/danny/morse_code/morse_game_system/_ide/scripts/debugger_morse_game-default.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Digilent Zybo Z7 210351B10529A" && level==0 && jtag_device_ctx=="jsn-Zybo Z7-210351B10529A-23727093-0"}
fpga -file /home/danny/Documents/fpga_club/danny/morse_code/morse_game/_ide/bitstream/design_1_wrapper.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw /home/danny/Documents/fpga_club/danny/morse_code/platform/export/platform/hw/design_1_wrapper.xsa -mem-ranges [list {0x40000000 0xbfffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source /home/danny/Documents/fpga_club/danny/morse_code/morse_game/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow /home/danny/Documents/fpga_club/danny/morse_code/morse_game/Debug/morse_game.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "*A9*#0"}
con
