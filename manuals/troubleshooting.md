# General Troubleshooting

## Board Checks 

1. Ensure jumpers are correctly placed. 

2. The micro-USB is connected to the "PROG UART" port on the board (PGOOD red LED should light up) 

## Vivado 

### Linux

#### Board isn't being recognized by the hardware manager

Try installing cable drivers. 

This was the case for me using Vivado 2024.2 on Debian 12. 

1. Run the `install_drivers` script located in `/tools/Xilinx/Vivado/2024.2/data/xicom/cable_drivers/lin64/install_script/install_drivers`

Note: That location is the default directory. Your machine may differ. 

