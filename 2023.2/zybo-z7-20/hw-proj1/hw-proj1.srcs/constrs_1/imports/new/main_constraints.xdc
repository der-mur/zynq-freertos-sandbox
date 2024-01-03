#========================================================#
# === PS UART0 (EMIO) => PMOD JB ===
#========================================================#
set_property IOSTANDARD LVCMOS33 [get_ports UART0_TX]
set_property IOSTANDARD LVCMOS33 [get_ports UART0_RX]

set_property PACKAGE_PIN W8 [get_ports UART0_TX]
set_property PACKAGE_PIN U7 [get_ports UART0_RX]


#========================================================#
# === TTC0/1 => PMOD JC ===
#========================================================#
set_property IOSTANDARD LVCMOS33 [get_ports TTC0_WAVE0_OUT]
set_property IOSTANDARD LVCMOS33 [get_ports TTC0_WAVE1_OUT]
set_property IOSTANDARD LVCMOS33 [get_ports TTC0_WAVE2_OUT]
set_property IOSTANDARD LVCMOS33 [get_ports TTC1_WAVE0_OUT]
set_property IOSTANDARD LVCMOS33 [get_ports TTC1_WAVE1_OUT]
set_property IOSTANDARD LVCMOS33 [get_ports TTC1_WAVE2_OUT]

# TTC0_WAVE0 => PIN 1 = V15
# TTC0_WAVE1 => PIN 2 = W15
# TTC0_WAVE2 => PIN 3 = T11
# TTC1_WAVE0 => PIN 7 = W14
# TTC1_WAVE1 => PIN 8 = Y14
# TTC1_WAVE2 => PIN 9 = T12
set_property PACKAGE_PIN V15 [get_ports TTC0_WAVE0_OUT]
set_property PACKAGE_PIN W15 [get_ports TTC0_WAVE1_OUT]
set_property PACKAGE_PIN T11 [get_ports TTC0_WAVE2_OUT]
# PIN 4 (T10) NOT USED
set_property PACKAGE_PIN W14 [get_ports TTC1_WAVE0_OUT]
set_property PACKAGE_PIN Y14 [get_ports TTC1_WAVE1_OUT]
set_property PACKAGE_PIN T12 [get_ports TTC1_WAVE2_OUT]
# PIN 10 (U12) NOT USED



#========================================================#
# === AXI GPIO1 ===
#========================================================#

set_property IOSTANDARD LVCMOS33 [get_ports gpio1_out*]
set_property IOSTANDARD LVCMOS33 [get_ports gpio1_in*]

# === CHANNEL 1 ===

#=============================
# === lED Board => PMOD JD ===
#=============================

# PMOD JD PIN 1 = LD0 = T14
# PMOD JD PIN 2 = LD1 = T15
# PMOD JD PIN 3 = LD2 = P14
# PMOD JD PIN 4 = LD3 = R14
# PMOD JD PIN 7 = LD4 = U14
# PMOD JD PIN 8 = LD5 = U15
# PMOD JD PIN 9 = LD6 = V17
# PMOD JD PIN 10 = LD7 = V18

set_property PACKAGE_PIN T14 [get_ports {gpio1_out[0]}]
set_property PACKAGE_PIN T15 [get_ports {gpio1_out[1]}]
set_property PACKAGE_PIN P14 [get_ports {gpio1_out[2]}]
set_property PACKAGE_PIN R14 [get_ports {gpio1_out[3]}]
set_property PACKAGE_PIN U14 [get_ports {gpio1_out[4]}]
set_property PACKAGE_PIN U15 [get_ports {gpio1_out[5]}]
set_property PACKAGE_PIN V17 [get_ports {gpio1_out[6]}]
set_property PACKAGE_PIN V18 [get_ports {gpio1_out[7]}]


# PMOD JE Pins 5-8
set_property PACKAGE_PIN V13 [get_ports {gpio1_out[8]}]
set_property PACKAGE_PIN U17 [get_ports {gpio1_out[9]}]
set_property PACKAGE_PIN T17 [get_ports {gpio1_out[10]}]
set_property PACKAGE_PIN Y17 [get_ports {gpio1_out[11]}]

# === AXI GPIO2 CHANNEL 1 ===

#===============
# gpio1_in[3:0]
#===============
# Board Push-buttons: BTN2 - BTN3
set_property PACKAGE_PIN K19 [get_ports {gpio1_in[0]}]
set_property PACKAGE_PIN Y16 [get_ports {gpio1_in[1]}]

# Board Switches: SW2 - SW3
set_property PACKAGE_PIN W13 [get_ports {gpio1_in[2]}]
set_property PACKAGE_PIN T16 [get_ports {gpio1_in[3]}]


#========================================================#
# === AXI GPIO0 ===
#========================================================#
set_property IOSTANDARD LVCMOS33 [get_ports gpio0_out*]
set_property IOSTANDARD LVCMOS33 [get_ports gpio0_in*]

#===============
# gpio0_out[7:0]
#===============
# Board LEDs: LD0 - LD3
set_property PACKAGE_PIN M14 [get_ports {gpio0_out[0]}]
set_property PACKAGE_PIN M15 [get_ports {gpio0_out[1]}]
set_property PACKAGE_PIN G14 [get_ports {gpio0_out[2]}]
set_property PACKAGE_PIN D18 [get_ports {gpio0_out[3]}]

# PMOD JE Pins 1-4
set_property PACKAGE_PIN V12 [get_ports {gpio0_out[4]}]
set_property PACKAGE_PIN W16 [get_ports {gpio0_out[5]}]
set_property PACKAGE_PIN J15 [get_ports {gpio0_out[6]}]
set_property PACKAGE_PIN H15 [get_ports {gpio0_out[7]}]

#===============
# gpio0_in[3:0]
#===============
# Board Push-buttons: BTN0 - BTN1
set_property PACKAGE_PIN K18 [get_ports {gpio0_in[0]}]
set_property PACKAGE_PIN P16 [get_ports {gpio0_in[1]}]


# Board Switches: SW0 - SW1
set_property PACKAGE_PIN G15 [get_ports {gpio0_in[2]}]
set_property PACKAGE_PIN P15 [get_ports {gpio0_in[3]}]