# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: F:\Documents\GitHub\zynq-freertos-sandbox\2023.2\zybo-z7-20\hw-proj1\vitis_classic\simple_tasks_example1_system\_ide\scripts\debugger_simple_tasks_example1-default.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source F:\Documents\GitHub\zynq-freertos-sandbox\2023.2\zybo-z7-20\hw-proj1\vitis_classic\simple_tasks_example1_system\_ide\scripts\debugger_simple_tasks_example1-default.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw F:/Documents/GitHub/zynq-freertos-sandbox/2023.2/zybo-z7-20/hw-proj1/vitis_classic/freertos_single_core_pfm/export/freertos_single_core_pfm/hw/hw_proj1_wrapper.xsa -mem-ranges [list {0x40000000 0xbfffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
stop
source F:/Documents/GitHub/zynq-freertos-sandbox/2023.2/zybo-z7-20/hw-proj1/vitis_classic/simple_tasks_example1/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
rst -processor
targets -set -nocase -filter {name =~ "*A9*#0"}
dow F:/Documents/GitHub/zynq-freertos-sandbox/2023.2/zybo-z7-20/hw-proj1/vitis_classic/simple_tasks_example1/Debug/simple_tasks_example1.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "*A9*#0"}
con
