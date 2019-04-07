
#ifndef _LTC4266_H__
#define _LTC4266_H__

#ifdef CONFIG_LTC_4266
#define     intstat                        0x00    // Interrupt Status Register, Read Only. Sumarizes events from 
                                                   //  registers 02h through 0Ah. 
#define ord_intstat_supply                    7    // Supply Interrupt. Set if any of the bits in register 0Ah are 
                                                   //  set. 
#define msk_intstat_supply                 0x80    //
#define ord_intstat_tstart                    6    // tSTART Interrupt. Set if any of the tSTART bits in the tsevn 
                                                   //  register (08h) are set. 
#define msk_intstat_tstart                 0x40    //
#define ord_intstat_tcut                      5    // tCUT Interrupt. Set if any of the tCUT bits in the fltevn 
                                                   //  register (06h) are set or if any of the tLIM bits in the 
                                                   //  tsevn register (08h) are set. 
#define msk_intstat_tcut                   0x20    //
#define ord_intstat_class                     4    // Class Interrupt. Set if any of the class bits in the detevn 
                                                   //  register (04h) are set. 
#define msk_intstat_class                  0x10    //
#define ord_intstat_det                       3    // Detect Interrupt. Set if any of the detect bits in the 
                                                   //  detevn register (04h) are set. 
#define msk_intstat_det                    0x08    //
#define ord_intstat_dis                       2    // Disconnect Interrupt. Set if any of the tDIS bits in the 
                                                   //  fltevn register (06h) are set. 
#define msk_intstat_dis                    0x04    //
#define ord_intstat_pwrgd                     1    // Power Good Interrupt. Set if any of the pwrgd bits in the 
                                                   //  pwrevn register (02h) are set. 
#define msk_intstat_pwrgd                  0x02    //
#define ord_intstat_pwrena                    0    // Power Enable Interrupt. Set if any of the pwrena bits in the 
                                                   //  pwrevn register (02h) are set. 
#define msk_intstat_pwrena                 0x01    //

// Register definition: intmask           
#define     intmask                        0x01    // Interrupt Mask Register, Read/Write. Bits in this register 
                                                   //  control which events will pull the _INT pin low. 
#define ord_intmask_supply                    7    // Supply events will pull the _INT pin low. 
#define msk_intmask_supply                 0x80    //
#define ord_intmask_tstart                    6    // tSTART events will pull the _INT pin low. 
#define msk_intmask_tstart                 0x40    //
#define ord_intmask_tcut                      5    // tCUT events will pull the _INT pin low. 
#define msk_intmask_tcut                   0x20    //
#define ord_intmask_class                     4    // Class events will pull the _INT pin low. 
#define msk_intmask_class                  0x10    //
#define ord_intmask_det                       3    // Detect events will pull the _INT pin low. 
#define msk_intmask_det                    0x08    //
#define ord_intmask_dis                       2    // Disconnect events will pull the _INT pin low. 
#define msk_intmask_dis                    0x04    //
#define ord_intmask_pwrgd                     1    // Power Good events will pull the _INT pin low. 
#define msk_intmask_pwrgd                  0x02    //
#define ord_intmask_pwrena                    0    // Power Enable events will pull the _INT pin low. 
#define msk_intmask_pwrena                 0x01    //

// Register definition: pwrevn            
#define     pwrevn                         0x02    // Power Event Register, Read Only. The lower 4 bits indicate 
                                                   //  that port power has turned on or off. The upper 4 bits 
                                                   //  indicate that the Power Good status of the port has 
                                                   //  changed. See the statpwr register at 10h. 
#define ord_pwrevn_pwrgd4                     7    // Port 4 Power Good Change. 
#define msk_pwrevn_pwrgd4                  0x80    //
#define ord_pwrevn_pwrgd3                     6    // Port 3 Power Good Change. 
#define msk_pwrevn_pwrgd3                  0x40    //
#define ord_pwrevn_pwrgd2                     5    // Port 2 Power Good Change. 
#define msk_pwrevn_pwrgd2                  0x20    //
#define ord_pwrevn_pwrgd1                     4    // Port 1 Power Good Change. 
#define msk_pwrevn_pwrgd1                  0x10    //
#define ord_pwrevn_pwrena4                    3    // Port 4 Power Status Change. 
#define msk_pwrevn_pwrena4                 0x08    //
#define ord_pwrevn_pwrena3                    2    // Port 3 Power Status Change. 
#define msk_pwrevn_pwrena3                 0x04    //
#define ord_pwrevn_pwrena2                    1    // Port 2 Power Status Change. 
#define msk_pwrevn_pwrena2                 0x02    //
#define ord_pwrevn_pwrena1                    0    // Port 1 Power Status Change. 
#define msk_pwrevn_pwrena1                 0x01    //

// Register definition: pwrevn_cor        
#define     pwrevn_cor                     0x03    // Power Event Register, Clear on Read. Identical to the pwrevn 
                                                   //  register at 02h except that reading from this registers 
                                                   //  clears both the pwrevn and the pwrevn_cor registers. 
#define ord_pwrevn_cor_pwrgd4                 7    // Port 4 Power Good Change. 
#define msk_pwrevn_cor_pwrgd4              0x80    //
#define ord_pwrevn_cor_pwrgd3                 6    // Port 3 Power Good Change. 
#define msk_pwrevn_cor_pwrgd3              0x40    //
#define ord_pwrevn_cor_pwrgd2                 5    // Port 2 Power Good Change. 
#define msk_pwrevn_cor_pwrgd2              0x20    //
#define ord_pwrevn_cor_pwrgd1                 4    // Port 1 Power Good Change. 
#define msk_pwrevn_cor_pwrgd1              0x10    //
#define ord_pwrevn_cor_pwrena4                3    // Port 4 Power Status Change. 
#define msk_pwrevn_cor_pwrena4             0x08    //
#define ord_pwrevn_cor_pwrena3                2    // Port 3 Power Status Change. 
#define msk_pwrevn_cor_pwrena3             0x04    //
#define ord_pwrevn_cor_pwrena2                1    // Port 2 Power Status Change. 
#define msk_pwrevn_cor_pwrena2             0x02    //
#define ord_pwrevn_cor_pwrena1                0    // Port 1 Power Status Change. 
#define msk_pwrevn_cor_pwrena1             0x01    //

// Register definition: detevn            
#define     detevn                         0x04    // Detect and Classification Event Register, Read Only. The 4 
                                                   //  lower bits indicate that the port has completed detecting a 
                                                   //  PD and the result is available in the appropriate statp 
                                                   //  register (0Ch to 0Fh). The 4 upper bits indicate that the 
                                                   //  port has completed classifying a PD and the result is 
                                                   //  available in port's statp register. 
#define ord_detevn_class4                     7    // Port 4 Classification Complete. 
#define msk_detevn_class4                  0x80    //
#define ord_detevn_class3                     6    // Port 3 Classification Complete. 
#define msk_detevn_class3                  0x40    //
#define ord_detevn_class2                     5    // Port 2 Classification Complete. 
#define msk_detevn_class2                  0x20    //
#define ord_detevn_class1                     4    // Port 1 Classification Complete. 
#define msk_detevn_class1                  0x10    //
#define ord_detevn_det4                       3    // Port 4 Detection Complete. 
#define msk_detevn_det4                    0x08    //
#define ord_detevn_det3                       2    // Port 3 Detection Complete. 
#define msk_detevn_det3                    0x04    //
#define ord_detevn_det2                       1    // Port 2 Detection Complete. 
#define msk_detevn_det2                    0x02    //
#define ord_detevn_det1                       0    // Port 1 Detection Complete. 
#define msk_detevn_det1                    0x01    //

// Register definition: detevn_cor        
#define     detevn_cor                     0x05    // Detect and Classification Event Register, Clear on Read. 
                                                   //  Identical to the detevn register at 04h except that reading 
                                                   //  from this registers clears both the detevn and the 
                                                   //  detevn_cor registers. 
#define ord_detevn_cor_class4                 7    // Port 4 Classification Complete. 
#define msk_detevn_cor_class4              0x80    //
#define ord_detevn_cor_class3                 6    // Port 3 Classification Complete. 
#define msk_detevn_cor_class3              0x40    //
#define ord_detevn_cor_class2                 5    // Port 2 Classification Complete. 
#define msk_detevn_cor_class2              0x20    //
#define ord_detevn_cor_class1                 4    // Port 1 Classification Complete. 
#define msk_detevn_cor_class1              0x10    //
#define ord_detevn_cor_det4                   3    // Port 4 Detection Complete. 
#define msk_detevn_cor_det4                0x08    //
#define ord_detevn_cor_det3                   2    // Port 3 Detection Complete. 
#define msk_detevn_cor_det3                0x04    //
#define ord_detevn_cor_det2                   1    // Port 2 Detection Complete. 
#define msk_detevn_cor_det2                0x02    //
#define ord_detevn_cor_det1                   0    // Port 1 Detection Complete. 
#define msk_detevn_cor_det1                0x01    //

// Register definition: fltevn            
#define     fltevn                         0x06    // Fault Event Register, Read Only. The lower 4 bits indicate 
                                                   //  that the port has been turned off because the load current 
                                                   //  was above ICUT or ILIM for more than tICUT. Bits in upper 
                                                   //  nibble are set when the port has been turned off due to the 
                                                   //  load current falling below 5mA for more than tDIS. 
#define ord_fltevn_dis4                       7    // Port 4 disconnect time out (tDIS). 
#define msk_fltevn_dis4                    0x80    //
#define ord_fltevn_dis3                       6    // Port 3 disconnect time out (tDIS). 
#define msk_fltevn_dis3                    0x40    //
#define ord_fltevn_dis2                       5    // Port 2 disconnect time out (tDIS). 
#define msk_fltevn_dis2                    0x20    //
#define ord_fltevn_dis1                       4    // Port 1 disconnect time out (tDIS). 
#define msk_fltevn_dis1                    0x10    //
#define ord_fltevn_tcut4                      3    // Port 4 overcurrent time out (tCUT). 
#define msk_fltevn_tcut4                   0x08    //
#define ord_fltevn_tcut3                      2    // Port 3 overcurrent time out (tCUT). 
#define msk_fltevn_tcut3                   0x04    //
#define ord_fltevn_tcut2                      1    // Port 2 overcurrent time out (tCUT). 
#define msk_fltevn_tcut2                   0x02    //
#define ord_fltevn_tcut1                      0    // Port 1 overcurrent time out (tCUT). While the port was 
                                                   //  powered, current exceeded ICUT for longer than tCUT. 
#define msk_fltevn_tcut1                   0x01    //

// Register definition: fltevn_cor        
#define     fltevn_cor                     0x07    // Fault Event Register, Clear on Read. Identical to the fltevn 
                                                   //  register at 06h except that reading from this registers 
                                                   //  clears both the fltevn and the fltevn_cor registers. 
#define ord_fltevn_cor_dis4                   7    // Port 4 disconnect time out (tDIS). 
#define msk_fltevn_cor_dis4                0x80    //
#define ord_fltevn_cor_dis3                   6    // Port 3 disconnect time out (tDIS). 
#define msk_fltevn_cor_dis3                0x40    //
#define ord_fltevn_cor_dis2                   5    // Port 2 disconnect time out (tDIS). 
#define msk_fltevn_cor_dis2                0x20    //
#define ord_fltevn_cor_dis1                   4    // Port 1 disconnect time out (tDIS). 
#define msk_fltevn_cor_dis1                0x10    //
#define ord_fltevn_cor_tcut4                  3    // Port 4 overcurrent time out (tCUT). 
#define msk_fltevn_cor_tcut4               0x08    //
#define ord_fltevn_cor_tcut3                  2    // Port 3 overcurrent time out (tCUT). 
#define msk_fltevn_cor_tcut3               0x04    //
#define ord_fltevn_cor_tcut2                  1    // Port 2 overcurrent time out (tCUT). 
#define msk_fltevn_cor_tcut2               0x02    //
#define ord_fltevn_cor_tcut1                  0    // Port 1 overcurrent time out (tCUT). 
#define msk_fltevn_cor_tcut1               0x01    //

// Register definition: tsevn             
#define     tsevn                          0x08    // tSTART Event Register, Read Only. The lower nibble bits are 
                                                   //  set to indicate an overcurrent fault (and consequent port 
                                                   //  turn off) during port power up. The upper 4 bits are set 
                                                   //  when the corresponding port current limit timer has expired 
                                                   //  and the port has been turned off. See tlim registers 
                                                   //  (1Eh/1Fh). 
#define ord_tsevn_tlim4                       7    // Port 4 current limit time out. 
#define msk_tsevn_tlim4                    0x80    //
#define ord_tsevn_tlim3                       6    // Port 3 current limit time out. 
#define msk_tsevn_tlim3                    0x40    //
#define ord_tsevn_tlim2                       5    // Port 2 current limit time out. 
#define msk_tsevn_tlim2                    0x20    //
#define ord_tsevn_tlim1                       4    // Port 1 current limit time out. The length of the current 
                                                   //  limit (tLIM) is controlled by the tLIM1 field of the tlim12 
                                                   //  at 1Eh. 
#define msk_tsevn_tlim1                    0x10    //
#define ord_tsevn_tstart4                     3    // Port 4 startup overcurrent time out (tSTART). 
#define msk_tsevn_tstart4                  0x08    //
#define ord_tsevn_tstart3                     2    // Port 3 startup overcurrent time out (tSTART). 
#define msk_tsevn_tstart3                  0x04    //
#define ord_tsevn_tstart2                     1    // Port 2 startup overcurrent time out (tSTART). 
#define msk_tsevn_tstart2                  0x02    //
#define ord_tsevn_tstart1                     0    // Port 1 startup overcurrent time out (tSTART). While turning 
                                                   //  on the port, the PD has drawn more than 375mA for longer 
                                                   //  than tSTART causing the port to turn off. 
#define msk_tsevn_tstart1                  0x01    //

// Register definition: tsevn_cor         
#define     tsevn_cor                      0x09    // tSTART Event Register, Clear on Read. Identical to the tsevn 
                                                   //  register at 08h except that reading from this registers 
                                                   //  clears both the tsevn and the tsevn_cor registers. 
#define ord_tsevn_cor_tlim4                   7    // Port 4 current limit time out. 
#define msk_tsevn_cor_tlim4                0x80    //
#define ord_tsevn_cor_tlim3                   6    // Port 3 current limit time out. 
#define msk_tsevn_cor_tlim3                0x40    //
#define ord_tsevn_cor_tlim2                   5    // Port 2 current limit time out. 
#define msk_tsevn_cor_tlim2                0x20    //
#define ord_tsevn_cor_tlim1                   4    // Port 1 current limit time out. 
#define msk_tsevn_cor_tlim1                0x10    //
#define ord_tsevn_cor_tstart4                 3    // Port 4 startup overcurrent time out (tSTART). 
#define msk_tsevn_cor_tstart4              0x08    //
#define ord_tsevn_cor_tstart3                 2    // Port 3 startup overcurrent time out (tSTART). 
#define msk_tsevn_cor_tstart3              0x04    //
#define ord_tsevn_cor_tstart2                 1    // Port 2 startup overcurrent time out (tSTART). 
#define msk_tsevn_cor_tstart2              0x02    //
#define ord_tsevn_cor_tstart1                 0    // Port 1 startup overcurrent time out (tSTART). 
#define msk_tsevn_cor_tstart1              0x01    //

// Register definition: supevn            
#define     supevn                         0x0A    // Supply Event Register, Read Only. Bits in this register are 
                                                   //  set to indicate problems with external power supplies, 
                                                   //  temperature, or processing, affecting all ports. 
#define ord_supevn_overtemp                   7    // Over Temperature. Set when die temperature is too high and 
                                                   //  operation of most functions are disabled. The tsd bit in 
                                                   //  the wdog register (42h) will remain set while the condition 
                                                   //  persists. 
#define msk_supevn_overtemp                0x80    //
#define ord_supevn_fetbad                     6    // FET failure (transient or total) 
#define msk_supevn_fetbad                 0x40    //
#define ord_supevn_uvlo3                      5    // Digital Supply (VDD) UVLO. Set when the LTC4266 comes out of 
                                                   //  Under Voltage Lockout (UVLO). 
#define msk_supevn_uvlo3                   0x20    //
#define ord_supevn_uvlo48                     4    // Analog Supply (VEE) Under Voltage Lock Out (UVLO). Set when 
                                                   //  the 48V supply is too low for the LTC4266 to operate 
                                                   //  properly. Cannot be cleared while the condition persists. 
#define msk_supevn_uvlo48                  0x10    //

// Register definition: supevn_cor        
#define     supevn_cor                     0x0B    // Supply Event Register, Clear on Read. Identical to the 
                                                   //  supevn register at 0Ah except that reading from this 
                                                   //  registers clears both the supevn and the supevn_cor 
                                                   //  registers. 
#define ord_supevn_cor_overtemp               7    // Over Temperature. 
#define msk_supevn_cor_overtemp            0x80    //
#define ord_supevn_cor_fetbad                 6    // FET failure (transient or total) 
#define msk_supevn_cor_fetbad              0x40    //
#define ord_supevn_cor_uvlo3                  5    // Digital Supply (VDD) UVLO. 
#define msk_supevn_cor_uvlo3               0x20    //
#define ord_supevn_cor_uvlo48                 4    // Analog Supply (VEE) Under Voltage Lock Out (UVLO). 
#define msk_supevn_cor_uvlo48              0x10    //

// Register definition: statp1            
#define     statp1                         0x0C    // Port 1 Status, Read Only. Shows the results of detection and 
                                                   //  classification measurements. 
#define ord_statp1_class                      4    // Last classification result for port 1. 0=Unknown; 1=Class 1; 
                                                   //  2=Class 2; 3=Class 3; 4=Class 4; 5=Reserved; 6=Class 0; 
                                                   //  7=Overcurrent. 
#define msk_statp1_class                   0x70    //
#define ord_statp1_detect                     0    // Last detection result for port 1. 0=Unknown; 1=Short; 2=Cpd 
                                                   //  Too High; 3=Low; 4=Good; 5=High; 6=Open; 7=Reserved. 
#define msk_statp1_detect                  0x07    //

// Register definition: statp2            
#define     statp2                         0x0D    // Port 2 Status, Read Only. Shows the results of detection and 
                                                   //  classification. See the statp1 register at 0Ch for a 
                                                   //  description of encoding. 
#define ord_statp2_class                      4    // Last classification result for port 2. 
#define msk_statp2_class                   0x70    //
#define ord_statp2_detect                     0    // Last detection result for port 2. 
#define msk_statp2_detect                  0x07    //

// Register definition: statp3            
#define     statp3                         0x0E    // Port 3 Status, Read Only. Shows the results of detection and 
                                                   //  classification. See the statp1 register at 0Ch for a 
                                                   //  description of encoding. 
#define ord_statp3_class                      4    // Last classification result for port 3. 
#define msk_statp3_class                   0x70    //
#define ord_statp3_detect                     0    // Last detection result for port 3. 
#define msk_statp3_detect                  0x07    //

// Register definition: statp4            
#define     statp4                         0x0F    // Port 4 Status, Read Only. Shows the results of detection and 
                                                   //  classification. See the statp1 register at 0Ch for a 
                                                   //  description of encoding. 
#define ord_statp4_class                      4    // Last classification result for port 4. 
#define msk_statp4_class                   0x70    //
#define ord_statp4_detect                     0    // Last detection result for port 4. 
#define msk_statp4_detect                  0x07    //

// Register definition: statpwr           
#define     statpwr                        0x10    // Power Status, Read Only. Indicates the power status of each 
                                                   //  port. The power good bits are set once the voltage between 
                                                   //  the OUT pin and the VEE pin falls below 2.4V (typ.). The 
                                                   //  power good bits are sticky and are only cleared when the 
                                                   //  port is powered off. The power enabled bits are set when 
                                                   //  power is applied to the port. 
#define ord_statpwr_pg4                       7    // Power good on port 4. 
#define msk_statpwr_pg4                    0x80    //
#define ord_statpwr_pg3                       6    // Power good on port 3. 
#define msk_statpwr_pg3                    0x40    //
#define ord_statpwr_pg2                       5    // Power good on port 2. 
#define msk_statpwr_pg2                    0x20    //
#define ord_statpwr_pg1                       4    // Power good on port 1. 
#define msk_statpwr_pg1                    0x10    //
#define ord_statpwr_pe4                       3    // Power enabled on port 4. 
#define msk_statpwr_pe4                    0x08    //
#define ord_statpwr_pe3                       2    // Power enabled on port 3. 
#define msk_statpwr_pe3                    0x04    //
#define ord_statpwr_pe2                       1    // Power enabled on port 2. 
#define msk_statpwr_pe2                    0x02    //
#define ord_statpwr_pe1                       0    // Power enabled on port 1. 
#define msk_statpwr_pe1                    0x01    //

// Register definition: statpin           
#define     statpin                        0x11    // Pin Status, Read Only. Bits in this register indicates the 
                                                   //  present logic level of input pins. The AD3\{en0 pins 
                                                   //  control the LTC4266 SMBus address. MID and AUTO pins are 
                                                   //  also shown. 
#define ord_statpin_ad3                       5    // SMBus address 3 (AD3 pin logic state). 
#define msk_statpin_ad3                    0x20    //
#define ord_statpin_ad2                       4    // SMBus address 2 (AD2 pin logic state). 
#define msk_statpin_ad2                    0x10    //
#define ord_statpin_ad1                       3    // SMBus address 1 (AD1 pin logic state). 
#define msk_statpin_ad1                    0x08    //
#define ord_statpin_ad0                       2    // SMBus address 0 (AD0 pin logic state). 
#define msk_statpin_ad0                    0x04    //
#define ord_statpin_mid                       1    // MID pin logic state. 
#define msk_statpin_mid                    0x02    //
#define ord_statpin_auto                      0    // AUTO pin logic state. 
#define msk_statpin_auto                   0x01    //

// Register definition: opmd              
#define     opmd                           0x12    // Operating Mode, Read/Write. Selects between 4 modes of 
                                                   //  operation for each of the 4 ports. The 2 bit fields for 
                                                   //  each port are interpreted as: 0=Shutdown; 1=Manual; 
                                                   //  2=Semi-Auto; 3=Auto. These modes determine if the port will 
                                                   //  automatically detect, class, and power a port on. 
#define ord_opmd_opmd4                        6    // Operating mode, port 4. 
#define msk_opmd_opmd4                     0xC0    //
#define ord_opmd_opmd3                        4    // Operating mode, port 3. 
#define msk_opmd_opmd3                     0x30    //
#define ord_opmd_opmd2                        2    // Operating mode, port 2. 
#define msk_opmd_opmd2                     0x0C    //
#define ord_opmd_opmd1                        0    // Operating mode, port 1. 
#define msk_opmd_opmd1                     0x03    //

// Register definition: disena            
#define     disena                         0x13    // Disconnect Sensing Enable, Read/Write. Enables disconnect 
                                                   //  sensing on a per-port basis. This is used to determine if a 
                                                   //  PD has been disconnected from a port. If the load current 
                                                   //  falls below 5mA for tDIS the port will be turned off. 
#define ord_disena_eml_ac4                    7    // Emulate AC disconnect on port 4. 
#define msk_disena_eml_ac4                 0x80    //
#define ord_disena_eml_ac3                    6    // Emulate AC disconnect on port 3. 
#define msk_disena_eml_ac3                 0x40    //
#define ord_disena_eml_ac2                    5    // Emulate AC disconnect on port 2. 
#define msk_disena_eml_ac2                 0x20    //
#define ord_disena_eml_ac1                    4    // Emulate AC disconnect on port 1. Because AC disconnect is 
                                                   //  emulated with DC disconnect, setting this bit is equivalent 
                                                   //  to setting the dc1 bit. 
#define msk_disena_eml_ac1                 0x10    //
#define ord_disena_dc4                        3    // Enable DC disconnect on port 4. 
#define msk_disena_dc4                     0x08    //
#define ord_disena_dc3                        2    // Enable DC disconnect on port 3. 
#define msk_disena_dc3                     0x04    //
#define ord_disena_dc2                        1    // Enable DC disconnect on port 2. 
#define msk_disena_dc2                     0x02    //
#define ord_disena_dc1                        0    // Enable DC disconnect on port 1. 
#define msk_disena_dc1                     0x01    //

// Register definition: detena            
#define     detena                         0x14    // Detect and Class Enable, Read/Write. Enables PD detection 
                                                   //  and classification on each of the ports. These bits can 
                                                   //  also be set by writing to the detpb register at 18h. The 
                                                   //  behavior of these bits depends on the port's operating mode 
                                                   //  (see opmd at 12h). %{bus,mpb,pgm: clear by writing a 1 to 
                                                   //  the bit(s). Set by writing to detpb. The detpb will 
                                                   //  automatically set these bits if pressed 
#define ord_detena_cls4                       7    // Enable classification on port 4. 
#define msk_detena_cls4                    0x80    //
#define ord_detena_cls3                       6    // Enable classification on port 3. 
#define msk_detena_cls3                    0x40    //
#define ord_detena_cls2                       5    // Enable classification on port 2. 
#define msk_detena_cls2                    0x20    //
#define ord_detena_cls1                       4    // Enable classification on port 1. In auto and semi-auto modes 
                                                   //  setting this bit enables classification following a 
                                                   //  successful detect. In manual mode setting this bit causes 
                                                   //  the port to perform one classification then clear the bit. 
                                                   //  In shutdown mode this bit has no effect. 
#define msk_detena_cls1                    0x10    //
#define ord_detena_det4                       3    // Enable detection on port 4. 
#define msk_detena_det4                    0x08    //
#define ord_detena_det3                       2    // Enable detection on port 3. 
#define msk_detena_det3                    0x04    //
#define ord_detena_det2                       1    // Enable detection on port 2. 
#define msk_detena_det2                    0x02    //
#define ord_detena_det1                       0    // Enable detection on port 1. In auto and semi-auto modes 
                                                   //  setting this bit enables detection. The LTC4266 will 
                                                   //  periodically detect and report the result. In manual mode 
                                                   //  when this bit is set the port performs one detection then 
                                                   //  clears the bit. In shutdown mode this bit has no effect. 
#define msk_detena_det1                    0x01    //

// Register definition: midspan           
#define     midspan                        0x15    // Midspan Backoff Enable, Read/Write. Enables midspan backoff 
                                                   //  timing. When these bits are set the port will wait the 
                                                   //  backoff time of 2.5s (typ.) between detects. 
#define ord_midspan_midsp4                    3    // Enable midspan backoff on port 4. 
#define msk_midspan_midsp4                 0x08    //
#define ord_midspan_midsp3                    2    // Enable midspan backoff on port 3. 
#define msk_midspan_midsp3                 0x04    //
#define ord_midspan_midsp2                    1    // Enable midspan backoff on port 2. 
#define msk_midspan_midsp2                 0x02    //
#define ord_midspan_midsp1                    0    // Enable midspan backoff on port 1. 
#define msk_midspan_midsp1                 0x01    //

// Register definition: tconf             
#define     tconf                          0x16    // Timing Configuration, Read/Write. Changes the length of the 
                                                   //  tSTART, tCUT, tDIS timers. 
#define ord_tconf_tstart                      4    // tSTART timing control. 0=60ms (typ.); 1=30ms (typ.); 2=120ms 
                                                   //  (typ.); 3=240ms (typ.). 
#define msk_tconf_tstart                   0x30    //
#define ord_tconf_tcut                        2    // tCUT timing control. 0=60ms (typ.); 1=30ms (typ.); 2=120ms 
                                                   //  (typ.); 3=240ms (typ.). 
#define msk_tconf_tcut                     0x0C    //
#define ord_tconf_tdis                        0    // tDIS timing control. 0=360ms (typ.); 1=90ms (typ.); 2=180ms 
                                                   //  (typ.); 3=720ms (typ.). 
#define msk_tconf_tdis                     0x03    //

// Register definition: mconf             
#define     mconf                          0x17    // Miscellaneous Configuration, Read/Write. Controls the _INT 
                                                   //  pin and the affect of the _MSD pin on each port. Also 
                                                   //  configures fast port readback and detect-on-change 
                                                   //  reporting. 
#define ord_mconf_intena                      7    // Interrupt pin enable. When this bit is cleared the _INT pin 
                                                   //  will be high impedance regardless of the state of registers 
                                                   //  00h and 01h. 
#define msk_mconf_intena                   0x80    //
#define ord_mconf_detchg                      6    // When this bit is set, detect events are generated only when 
                                                   //  the result is different from the last detect result on that 
                                                   //  port. When this bit is cleared, a port's bit the detevn 
                                                   //  register is set everytime the LTC4266 performs detection on 
                                                   //  that port. 
#define msk_mconf_detchg                   0x40    //
#define ord_mconf_fasti                       4    // When this bit is set, the LTC4266 measures a port's current 
                                                   //  to 9 bits accuracy in shorter measurement time. The 9 bit 
                                                   //  result is shifted so it's LSB coresponds with the 14 bit 
                                                   //  result the LTC4266 reports if this bit is cleared. 
#define msk_mconf_fasti                    0x10    //
#define ord_mconf_msdmsk4                     3    // _MSD pin mask, port 4. When this bit is set pulling the _MSD 
                                                   //  Pin low will reset port 4. 
#define msk_mconf_msdmsk4                  0x08    //
#define ord_mconf_msdmsk3                     2    // _MSD pin mask, port 3. When this bit is set pulling the _MSD 
                                                   //  Pin low will reset port 3. 
#define msk_mconf_msdmsk3                  0x04    //
#define ord_mconf_msdmsk2                     1    // _MSD pin mask, port 2. When this bit is set pulling the _MSD 
                                                   //  Pin low will reset port 2. 
#define msk_mconf_msdmsk2                  0x02    //
#define ord_mconf_msdmsk1                     0    // _MSD pin mask, port 1. When this bit is set pulling the _MSD 
                                                   //  Pin low will reset port 1. 
#define msk_mconf_msdmsk1                  0x01    //

// Register definition: detpb             
#define     detpb                          0x18    // Detection and Classification Restart Pushbutton, Set Only. 
                                                   //  This register provides a means to set bits in the detena 
                                                   //  register at 14h without affecting other bits in that 
                                                   //  register. For example the detena register is 02h, writing 
                                                   //  24h to this register will change the detena register to 
                                                   //  26h. 
#define ord_detpb_cls4                        7    // Set bit 7 in the detena reg. at 14h, which enables 
                                                   //  classification on port 4. 
#define msk_detpb_cls4                     0x80    //
#define ord_detpb_cls3                        6    // Set bit 6 in the detena reg. at 14h, which enables 
                                                   //  classification on port 3. 
#define msk_detpb_cls3                     0x40    //
#define ord_detpb_cls2                        5    // Set bit 5 in the detena reg. at 14h, which enables 
                                                   //  classification on port 2. 
#define msk_detpb_cls2                     0x20    //
#define ord_detpb_cls1                        4    // Set bit 4 in the detena reg. at 14h, which enables 
                                                   //  classification on port 1. 
#define msk_detpb_cls1                     0x10    //
#define ord_detpb_det4                        3    // Set bit 3 in the detena reg. at 14h, which enables detection 
                                                   //  on port 4. 
#define msk_detpb_det4                     0x08    //
#define ord_detpb_det3                        2    // Set bit 2 in the detena reg. at 14h, which enables detection 
                                                   //  on port 3. 
#define msk_detpb_det3                     0x04    //
#define ord_detpb_det2                        1    // Set bit 1 in the detena reg. at 14h, which enables detection 
                                                   //  on port 2. 
#define msk_detpb_det2                     0x02    //
#define ord_detpb_det1                        0    // Set bit 0 in the detena reg. at 14h, which enables detection 
                                                   //  on port 1. 
#define msk_detpb_det1                     0x01    //

// Register definition: pwrpb             
#define     pwrpb                          0x19    // Power On/Off Pushbutton, Set Only. Force ports to power on 
                                                   //  or off regardless of the status of the port. 
#define ord_pwrpb_off4                        7    // Reset port 4. 
#define msk_pwrpb_off4                     0x80    //
#define ord_pwrpb_off3                        6    // Reset port 3. 
#define msk_pwrpb_off3                     0x40    //
#define ord_pwrpb_off2                        5    // Reset port 2. 
#define msk_pwrpb_off2                     0x20    //
#define ord_pwrpb_off1                        4    // Reset port 1. 
#define msk_pwrpb_off1                     0x10    //
#define ord_pwrpb_on4                         3    // Turn port 4 power on 
#define msk_pwrpb_on4                      0x08    //
#define ord_pwrpb_on3                         2    // Turn port 3 power on 
#define msk_pwrpb_on3                      0x04    //
#define ord_pwrpb_on2                         1    // Turn port 2 power on 
#define msk_pwrpb_on2                      0x02    //
#define ord_pwrpb_on1                         0    // Turn port 1 power on 
#define msk_pwrpb_on1                      0x01    //

// Register definition: rstpb             
#define     rstpb                          0x1A    // Reset Pushbutton, Set Only. Clear interrupts, reset the 
                                                   //  LTC4266 or individual ports. %{bus,mpb,pgm:The processor 
                                                   //  cannot read from this register. When an SMBus transaction 
                                                   //  sets a rst bit; the corresponding off bit in the pwrpb 
                                                   //  register is set. 
#define ord_rstpb_intclr                      7    // Clear all interrupt registers. 
#define msk_rstpb_intclr                   0x80    //
#define ord_rstpb_pinclr                      6    // Clear interrupt pin. 
#define msk_rstpb_pinclr                   0x40    //
#define ord_rstpb_rstall                      4    // Reset the LTC4266. Setting this bit is identical to pulling 
                                                   //  the _RESET pin low. 
#define msk_rstpb_rstall                   0x10    //
#define ord_rstpb_rst4                        3    // Reset port 4. 
#define msk_rstpb_rst4                     0x08    //
#define ord_rstpb_rst3                        2    // Reset port 3. 
#define msk_rstpb_rst3                     0x04    //
#define ord_rstpb_rst2                        1    // Reset port 2. 
#define msk_rstpb_rst2                     0x02    //
#define ord_rstpb_rst1                        0    // Reset port 1. 
#define msk_rstpb_rst1                     0x01    //

// Register definition: id                
#define     id                             0x1B    // Chip Identification Register, Read Only. LTC4259 compatible, 
                                                   //  i.e. the device and revision ID are the same as the 
                                                   //  LTC4259A. 
#define ord_id_dev                            3    // Device identification number. 
#define msk_id_dev                         0xF8    //
#define ord_id_rev                            0    // Revision number. 
#define msk_id_rev                         0x07    //

// Register definition: tlim12            
#define     tlim12                         0x1E    // tLIM timer duration for ports 1 and 2. The port's tLIM timer 
                                                   //  counts up when the port is in current limit and resets when 
                                                   //  the port is not in current limit. If the timer expires then 
                                                   //  the port turns off and the tLIM bit in register 08h is set. 
#define ord_tlim12_tlim2                      4    // tLIM timer duration on port 2. 
#define msk_tlim12_tlim2                   0xF0    //
#define ord_tlim12_tlim1                      0    // tLIM timer duration on port 1. Timer duration is 1.71ms 
                                                   //  (typ.) times the value in this field. If this field is 0, 
                                                   //  the timer is disabled. 
#define msk_tlim12_tlim1                   0x0F    //

// Register definition: tlim34            
#define     tlim34                         0x1F    // tLIM timer duration for ports 3 and 4. See description of 
                                                   //  tlim12 at 1Eh. 
#define ord_tlim34_tlim4                      4    // tLIM timer duration on port 4. 
#define msk_tlim34_tlim4                   0xF0    //
#define ord_tlim34_tlim3                      0    // tLIM timer duration on port 3. 
#define msk_tlim34_tlim3                   0x0F    //

// Register definition: icut12            
#define     icut12                         0x2A    // Overcurrent Cutoff Level, Read/Write. Sets the current 
                                                   //  cutoff levels for ports 1 and 2. Assumes RSENSE = 0.5\{ohm. 
#define ord_icut12_icut2                      4    // Adjustable overcurrent cutoff level, port 2. 
#define msk_icut12_icut2                   0x70    //
#define ord_icut12_icut1                      0    // Adjustable overcurrent cutoff level, port 1. The 
                                                   //  corresponding typical ICUT levels are 0=375mA; 1=562mA; 
                                                   //  2=675mA; 3=750mA; 4=825mA; 5=937mA; 6=110mA 7=188mA. If the 
                                                   //  hpen1 bit in the hpen register at 44h is set, this field is 
                                                   //  ignored and the cut1 register at 47h controls the 
                                                   //  overcurrent threshold. 
#define msk_icut12_icut1                   0x07    //

// Register definition: icut34            
#define     icut34                         0x2B    // Overcurrent Cutoff Level, Read/Write. Sets the current 
                                                   //  cutoff levels for ports 3 and 4. Assumes RSENSE = 0.5\{ohm. 
                                                   //  See register icut12 at 2Ah for a description of encoding. 
#define ord_icut34_icut4                      4    // Adjustable overcurrent cutoff level, port 4. 
#define msk_icut34_icut4                   0x70    //
#define ord_icut34_icut3                      0    // Adjustable overcurrent cutoff level, port 3. 
#define msk_icut34_icut3                   0x07    //

// Register definition: ip1lsb            
#define     ip1lsb                         0x30    // Port 1 Current Measurement LSB, Read Only. 
#define ord_ip1lsb_ip1lsb                     0    // Port 1 Current LSB. 1 LSB = 61.035\{microA when Rsense = 
                                                   //  0.5\{ohm or 122.07\{microA when Rsense = 0.25\{ohm 
#define msk_ip1lsb_ip1lsb                  0xFF    //

// Register definition: ip1msb            
#define     ip1msb                         0x31    // Port 1 Current Measurement MSB, Read Only. 
#define ord_ip1msb_ip1msb                     0    // Port 1 Current MSB. 
#define msk_ip1msb_ip1msb                  0xFF    //

// Register definition: vp1lsb            
#define     vp1lsb                         0x32    // Port 1 Voltage Measurement LSB, Read Only. 
#define ord_vp1lsb_vp1lsb                     0    // Port 1 Voltage LSB. 1 LSB = 5.9667mV when Rsense = 0.5\{ohm 
                                                   //  or 11.9334mV when Rsense = 0.25\{ohm 
#define msk_vp1lsb_vp1lsb                  0xFF    //

// Register definition: vp1msb            
#define     vp1msb                         0x33    // Port 1 Voltage Measurement MSB, Read Only. 
#define ord_vp1msb_vp1msb                     0    // Port 1 Voltage MSB. 
#define msk_vp1msb_vp1msb                  0xFF    //

// Register definition: ip2lsb            
#define     ip2lsb                         0x34    // Port 2 Current Measurement LSB, Read Only.. 
#define ord_ip2lsb_ip2lsb                     0    // Port 2 Current LSB. 1 LSB = 61.035\{microA when Rsense = 
                                                   //  0.5\{ohm or 122.07\{microA when Rsense = 0.25\{ohm 
#define msk_ip2lsb_ip2lsb                  0xFF    //

// Register definition: ip2msb            
#define     ip2msb                         0x35    // Port 2 Current Measurement MSB, Read Only.. 
#define ord_ip2msb_ip2msb                     0    // Port 2 Current MSB. 
#define msk_ip2msb_ip2msb                  0xFF    //

// Register definition: vp2lsb            
#define     vp2lsb                         0x36    // Port 2 Voltage Measurement LSB, Read Only.. 
#define ord_vp2lsb_vp2lsb                     0    // Port 2 Voltage LSB. 1 LSB = 5.9667mV when Rsense = 0.5\{ohm 
                                                   //  or 11.9334mV when Rsense = 0.25\{ohm 
#define msk_vp2lsb_vp2lsb                  0xFF    //

// Register definition: vp2msb            
#define     vp2msb                         0x37    // Port 2 Voltage Measurement MSB, Read Only. 
#define ord_vp2msb_vp2msb                     0    // Port 2 Voltage MSB. 
#define msk_vp2msb_vp2msb                  0xFF    //

// Register definition: ip3lsb            
#define     ip3lsb                         0x38    // Port 3 Current Measurement LSB, Read Only.. 
#define ord_ip3lsb_ip3lsb                     0    // Port 3 Current LSB. 1 LSB = 61.035\{microA when Rsense = 
                                                   //  0.5\{ohm or 122.07\{microA when Rsense = 0.25\{ohm 
#define msk_ip3lsb_ip3lsb                  0xFF    //

// Register definition: ip3msb            
#define     ip3msb                         0x39    // Port 3 Current Measurement MSB, Read Only.. 
#define ord_ip3msb_ip3msb                     0    // Port 3 Current MSB. 
#define msk_ip3msb_ip3msb                  0xFF    //

// Register definition: vp3lsb            
#define     vp3lsb                         0x3A    // Port 3 Voltage Measurement LSB, Read Only.. 
#define ord_vp3lsb_vp3lsb                     0    // Port 3 Voltage LSB. 1 LSB = 5.9667mV when Rsense = 0.5\{ohm 
                                                   //  or 11.9334mV when Rsense = 0.25\{ohm 
#define msk_vp3lsb_vp3lsb                  0xFF    //

// Register definition: vp3msb            
#define     vp3msb                         0x3B    // Port 3 Voltage Measurement MSB, Read Only. 
#define ord_vp3msb_vp3msb                     0    // Port 3 Voltage MSB. 
#define msk_vp3msb_vp3msb                  0xFF    //

// Register definition: ip4lsb            
#define     ip4lsb                         0x3C    // Port 4 Current Measurement LSB, Read Only.. 
#define ord_ip4lsb_ip4lsb                     0    // Port 4 Current LSB. 1 LSB = 61.035\{microA when Rsense = 
                                                   //  0.5\{ohm or 122.07\{microA when Rsense = 0.25\{ohm 
#define msk_ip4lsb_ip4lsb                  0xFF    //

// Register definition: ip4msb            
#define     ip4msb                         0x3D    // Port 4 Current Measurement MSB, Read Only.. 
#define ord_ip4msb_ip4msb                     0    // Port 4 Current MSB. 
#define msk_ip4msb_ip4msb                  0xFF    //

// Register definition: vp4lsb            
#define     vp4lsb                         0x3E    // Port 4 Voltage Measurement LSB, Read Only.. 
#define ord_vp4lsb_vp4lsb                     0    // Port 4 Voltage LSB. 1 LSB = 5.9667mV when Rsense = 0.5\{ohm 
                                                   //  or 11.9334mV when Rsense = 0.25\{ohm 
#define msk_vp4lsb_vp4lsb                  0xFF    //

// Register definition: vp4msb            
#define     vp4msb                         0x3F    // Port 4 Voltage Measurement MSB, Read Only. 
#define ord_vp4msb_vp4msb                     0    // Port 4 Voltage MSB. 
#define msk_vp4msb_vp4msb                  0xFF    //

// Register definition: dblpwr            
#define     dblpwr                         0x40    // Double Power Register, Read/Write. Sets the current limit to 
                                                   //  be double the 802.3af level on a per port basis. Assumes 
                                                   //  RSENSE = 0.5\{ohm. 
#define ord_dblpwr_dbl4                       7    // Double the current limit (ILIM) on port 4 to 850 mA (typ.). 
#define msk_dblpwr_dbl4                    0x80    //
#define ord_dblpwr_dbl3                       6    // Double the current limit (ILIM) on port 3 to 850 mA (typ.). 
#define msk_dblpwr_dbl3                    0x40    //
#define ord_dblpwr_dbl2                       5    // Double the current limit (ILIM) on port 2 to 850 mA (typ.). 
#define msk_dblpwr_dbl2                    0x20    //
#define ord_dblpwr_dbl1                       4    // Double the current limit (ILIM) on port 1 to 850 mA (typ.). 
                                                   //  If the hpen1 bit in the hpen register at 44h is set, this 
                                                   //  bit is ignored and the lim1 register at 48h controls the 
                                                   //  overcurrent threshold. 
#define msk_dblpwr_dbl1                    0x10    //

// Register definition: firmware          
#define     firmware                       0x41    // Firmware Revision Code, Read Only. 
#define ord_firmware_rev                      0    // Firmware Revision Code. 
#define msk_firmware_rev                   0xFF    //

// Register definition: wdog              
#define     wdog                           0x42    // Watchdog Register, Read/Write. The LTC4266 monitors activity 
                                                   //  on the SCL pin. If the SCL pin does not transition high or 
                                                   //  low in 2.5 seconds (typ.) the wdstat bit is set and ports 
                                                   //  are reset. Also displays current status of over/under 
                                                   //  voltage conditions and thermal shutdown. 
#define ord_wdog_tsd                          7    // TSD monitor. This bit is set when the chip is in thermal 
                                                   //  shutdown. 
#define msk_wdog_tsd                       0x80    //
#define ord_wdog_ov48                         6    // OV48 monitor. This bit is set when the 48V supply is above 
                                                   //  safe limits. 
#define msk_wdog_ov48                      0x40    //
#define ord_wdog_uv48                         5    // UV48 monitor. This bit is set when the 48V supply is too low 
                                                   //  to meet IEEE 802.3 specifications. 
#define msk_wdog_uv48                      0x20    //
#define ord_wdog_wddis                        1    // Watchdog disable. When this field is set to 1011b the 
                                                   //  watchdog is disabled. Otherwise the SMBus watchdog is 
                                                   //  enabled. 
#define msk_wdog_wddis                     0x1E    //
#define ord_wdog_wdstat                       0    // Status of the watchdog. When the watchdog times out, this 
                                                   //  bit is set and all ports are reset. This bit must be clear 
                                                   //  for ports to be enabled. 
#define msk_wdog_wdstat                    0x01    //

// Register definition: devid             
#define     devid                          0x43    // Device Identification Register, Read/Write. 
#define ord_devid_id                          5    // Device ID, Read/Write. 
#define msk_devid_id                       0xE0    //
#define ord_devid_rev                         0    // Device revision RO. 
#define msk_devid_rev                      0x07    //

// Register definition: hpen              
#define     hpen                           0x44    // High Power Enable Register, Read/Write. This register 
                                                   //  enables the features in address 46h through 5Fh and 
                                                   //  disables the use of overcurrent (ICUT) and current limit 
                                                   //  (ILIM) settings from registers 2Ah, 2Bh, and 40h on a per 
                                                   //  port basis. 
#define ord_hpen_hpen4                        3    // Enable high power features on port 4. When this bit is 
                                                   //  cleared, high power features are disabled and registers 55h 
                                                   //  through 58h have no effect. 
#define msk_hpen_hpen4                     0x08    //
#define ord_hpen_hpen3                        2    // Enable high power features on port 3. When this bit is 
                                                   //  cleared, high power features are disabled and registers 50h 
                                                   //  through 53h have no effect. 
#define msk_hpen_hpen3                     0x04    //
#define ord_hpen_hpen2                        1    // Enable high power features on port 2. When this bit is 
                                                   //  cleared, high power features are disabled and registers 4Bh 
                                                   //  through 4Eh have no effect. 
#define msk_hpen_hpen2                     0x02    //
#define ord_hpen_hpen1                        0    // Enable high power features on port 1. When this bit is 
                                                   //  cleared, high power features are disabled and registers 46h 
                                                   //  through 49h have no effect. 
#define msk_hpen_hpen1                     0x01    //

// Register definition: hpmd1             
#define     hpmd1                          0x46    // High Power Modes Port 1, Read/Write. Controls high power 
                                                   //  modes and features on port 1. 
#define ord_hpmd1_legen                       1    // Enable detection of legacy PDs by sensing for large 
                                                   //  capaitance on port 1. When this bit is set a PD with a 
                                                   //  large common mode capacitance will be reported as a vaild 
                                                   //  signature (code 4 in the statp1 register). 
#define msk_hpmd1_legen                    0x02    //
#define ord_hpmd1_pongen                      0    // Enable Ping-Pong classification on port 1. 
#define msk_hpmd1_pongen                   0x01    //

// Register definition: cut1              
#define     cut1                           0x47    // Port 1 Overcurrent Cutoff Level, Read/Write. If the hpen1 
                                                   //  bit in the hpen register at 44h is set, this register 
                                                   //  controls the overcurrent and DC disconnect thresholds. 
                                                   //  Otherwise the overcurrent threshold is controlled by the 
                                                   //  cut12 register at 2Ah. See table IC1 for settings 
                                                   //  information. 
#define ord_cut1_rdis                         7    // Adjust DC disconnect threshold on port 1 for sense resistor 
                                                   //  value: 0=0.5\{ohm, 1=0.25\{ohm. 
#define msk_cut1_rdis                      0x80    //
#define ord_cut1_cutrng                       6    // Adjust overcurrent threshold range on port 1. See Table IC1 
#define msk_cut1_cutrng                    0x40    //
#define ord_cut1_cut                          0    // Set threshold of overcurrent comparator on port 1. See Table 
                                                   //  IC1. 
#define msk_cut1_cut                       0x3F    //

// Register definition: lim1              
#define     lim1                           0x48    // Current limit and foldback profiles for port 1, Read/Write. 
                                                   //  If the hpen1 bit in the hpen register at 44h is set, this 
                                                   //  register adjusts the current limit on port 1. Otherwise the 
                                                   //  dbl1 bit in register dblpwr at 40h controls the current 
                                                   //  limit. 
#define ord_lim1_lim                          0    // Current Limit Setting. See Table IL1 for information on 
                                                   //  configuring ILIM. 
#define msk_lim1_lim                       0xFF    //
#define ord_lim1_rlim                         7    // Adjust current limit on port 1 for sense resistor value: 
                                                   //  0=0.5owh. 1=0.25ohm. 
#define msk_lim1_rlim                      0x80    //
#define ord_lim1_vf_1                         5    // Each bit turns ona 1x af current limit that foldsback when 
                                                   //  Vout-Vee > 10 (typ.). 
#define msk_lim1_vf_1                      0x60    //
#define ord_lim1_vf_p5                        4    // Turn on 0.5x af current limit that foldsback when Vout-Vee > 
                                                   //  10 (typ.). 
#define msk_lim1_vf_p5                     0x10    //
#define ord_lim1_noaf                         3    // Turn off 0.75x current limit that foldsback when AGND-Vout < 
                                                   //  30 (typ.). When this bit is clear, the 0.75x limit adds to 
                                                   //  the always on 0.25x current limit, creating an af compliant 
                                                   //  limit. 
#define msk_lim1_noaf                      0x08    //
#define ord_lim1_vp_1                         1    // Each bit turns on a 1x af current limit that foldsback when 
                                                   //  AGNC-Vout < 30 (typ.). 
#define msk_lim1_vp_1                      0x06    //
#define ord_lim1_vp_p5                        0    // Turn on 0.5x af current limit that foldsback when AGNC-Vout 
                                                   //  < 30 (typ.). 
#define msk_lim1_vp_p5                     0x01    //

// Register definition: hpstat1           
#define     hpstat1                        0x49    // Status of Advanced and High Power Features on Port 1, Read 
                                                   //  Only. 
#define ord_hpstat1_fetbad                    1    // The External FET has likely failed. See the FETBAD section 
                                                   //  for more information. 
#define msk_hpstat1_fetbad                 0x02    //
#define ord_hpstat1_pongpd                    0    // This bit is set when class 4 is measured during both events 
                                                   //  in a ping-pong classification. 
#define msk_hpstat1_pongpd                 0x01    //

// Register definition: hpmd2             
#define     hpmd2                          0x4B    // High Power Modes Port 2, Read/Write. Controls high power 
                                                   //  modes and features on port 2. 
#define ord_hpmd2_legen                       1    // Enable detection of legacy PDs by sensing for large 
                                                   //  capaitance on port 2. When this bit is set a PD with a 
                                                   //  large common mode capacitance will be reported as a vaild 
                                                   //  signature (code 4 in the statp2 register). 
#define msk_hpmd2_legen                    0x02    //
#define ord_hpmd2_pongen                      0    // Enable Ping-Pong classification on port 2. 
#define msk_hpmd2_pongen                   0x01    //

// Register definition: cut2              
#define     cut2                           0x4C    // Port 2 Overcurrent Cutoff Level, Read/Write. If the hpen2 
                                                   //  bit in the hpen register at 44h is set, this register 
                                                   //  controls the overcurrent and DC disconnect thresholds. 
                                                   //  Otherwise the overcurrent threshold is controlled by the 
                                                   //  cut12 register at 2Ah. See table IC1 for settings 
                                                   //  information. 
#define ord_cut2_rdis                         7    // Adjust DC disconnect threshold on port 2 for sense resistor 
                                                   //  value: 0=0.5\{ohm, 1=0.25\{ohm. 
#define msk_cut2_rdis                      0x80    //
#define ord_cut2_cutrng                       6    // Adjust overcurrent threshold range on port 2. See Table IC1 
#define msk_cut2_cutrng                    0x40    //
#define ord_cut2_cut                          0    // Set threshold of overcurrent comparator on port 2. See Table 
                                                   //  IC1. 
#define msk_cut2_cut                       0x3F    //

// Register definition: lim2              
#define     lim2                           0x4D    // Current limit and foldback profiles for port 2, Read/Write. 
                                                   //  If the hpen2 bit in the hpen register at 44h is set, this 
                                                   //  register adjusts the current limit on port 2. Otherwise the 
                                                   //  dbl2 bit in register dblpwr at 40h controls the current 
                                                   //  limit. 
#define ord_lim2_lim                          0    // Current Limit Setting. See Table IL1 for information on 
                                                   //  configuring ILIM. 
#define msk_lim2_lim                       0xFF    //

// Register definition: hpstat2           
#define     hpstat2                        0x4E    // Status of Advanced and High Power Features on Port 2, Read 
                                                   //  Only. 
#define ord_hpstat2_fetbad                    1    // The External FET has likely failed. See the FETBAD section 
                                                   //  for more information. 
#define msk_hpstat2_fetbad                 0x02    //
#define ord_hpstat2_pongpd                    0    // This bit is set when class 4 is measured during both events 
                                                   //  in a ping-pong classification. 
#define msk_hpstat2_pongpd                 0x01    //

// Register definition: hpmd3             
#define     hpmd3                          0x50    // High Power Modes Port 3, Read/Write. Controls high power 
                                                   //  modes and features on port 3. 
#define ord_hpmd3_legen                       1    // Enable detection of legacy PDs by sensing for large 
                                                   //  capaitance on port 3. When this bit is set a PD with a 
                                                   //  large common mode capacitance will be reported as a vaild 
                                                   //  signature (code 4 in the statp3 register). 
#define msk_hpmd3_legen                    0x02    //
#define ord_hpmd3_pongen                      0    // Enable Ping-Pong classification on port 3. 
#define msk_hpmd3_pongen                   0x01    //

// Register definition: cut3              
#define     cut3                           0x51    // Port 3 Overcurrent Cutoff Level, Read/Write. If the hpen3 
                                                   //  bit in the hpen register at 44h is set, this register 
                                                   //  controls the overcurrent and DC disconnect thresholds. 
                                                   //  Otherwise the overcurrent threshold is controlled by the 
                                                   //  cut34 register at 2Bh. See table IC1 for settings 
                                                   //  information. 
#define ord_cut3_rdis                         7    // Adjust DC disconnect threshold on port 3 for sense resistor 
                                                   //  value: 0=0.5\{ohm, 1=0.25\{ohm. 
#define msk_cut3_rdis                      0x80    //
#define ord_cut3_cutrng                       6    // Adjust overcurrent threshold range on port 3. See Table IC1 
#define msk_cut3_cutrng                    0x40    //
#define ord_cut3_cut                          0    // Set threshold of overcurrent comparator on port 3. See Table 
                                                   //  IC1. 
#define msk_cut3_cut                       0x3F    //

// Register definition: lim3              
#define     lim3                           0x52    // Current limit and foldback profiles for port 3, Read/Write. 
                                                   //  If the hpen3 bit in the hpen register at 44h is set, this 
                                                   //  register adjusts the current limit on port 3. Otherwise the 
                                                   //  dbl3 bit in register dblpwr at 40h controls the current 
                                                   //  limit. 
#define ord_lim3_lim                          0    // Current Limit Setting. See Table IL1 for information on 
                                                   //  configuring ILIM. 
#define msk_lim3_lim                       0xFF    //

// Register definition: hpstat3           
#define     hpstat3                        0x53    // Status of Advanced and High Power Features on Port 3, Read 
                                                   //  Only. 
#define ord_hpstat3_fetbad                    1    // The External FET has likely failed. See the FETBAD section 
                                                   //  for more information. 
#define msk_hpstat3_fetbad                 0x02    //
#define ord_hpstat3_pongpd                    0    // This bit is set when class 4 is measured during both events 
                                                   //  in a ping-pong classification. 
#define msk_hpstat3_pongpd                 0x01    //

// Register definition: hpmd4             
#define     hpmd4                          0x55    // High Power Modes Port 4, Read/Write. Controls high power 
                                                   //  modes and features on port 4. 
#define ord_hpmd4_legen                       1    // Enable detection of legacy PDs by sensing for large 
                                                   //  capaitance on port 4. When this bit is set a PD with a 
                                                   //  large common mode capacitance will be reported as a vaild 
                                                   //  signature (code 4 in the statp4 register). 
#define msk_hpmd4_legen                    0x02    //
#define ord_hpmd4_pongen                      0    // Enable Ping-Pong classification on port 4. 
#define msk_hpmd4_pongen                   0x01    //

// Register definition: cut4              
#define     cut4                           0x56    // Port 4 Overcurrent Cutoff Level, Read/Write. If the hpen4 
                                                   //  bit in the hpen register at 44h is set, this register 
                                                   //  controls the overcurrent and DC disconnect thresholds. 
                                                   //  Otherwise the overcurrent threshold is controlled by the 
                                                   //  cut34 register at 2Bh. See table IC1 for settings 
                                                   //  information. 
#define ord_cut4_rdis                         7    // Adjust DC disconnect threshold on port 4 for sense resistor 
                                                   //  value: 0=0.5\{ohm, 1=0.25\{ohm. 
#define msk_cut4_rdis                      0x80    //
#define ord_cut4_cutrng                       6    // Adjust overcurrent threshold range on port 4. See Table IC1 
#define msk_cut4_cutrng                    0x40    //
#define ord_cut4_cut                          0    // Set threshold of overcurrent comparator on port 4. See Table 
                                                   //  IC1. 
#define msk_cut4_cut                       0x3F    //

// Register definition: lim4              
#define     lim4                           0x57    // Current limit and foldback profiles for port 4, Read/Write. 
                                                   //  If the hpen4 bit in the hpen register at 44h is set, this 
                                                   //  register adjusts the current limit on port 4. Otherwise the 
                                                   //  dbl4 bit in register dblpwr at 40h controls the current 
                                                   //  limit. 
#define ord_lim4_lim                          0    // Current Limit Setting. See Table IL1 for information on 
                                                   //  configuring ILIM. 
#define msk_lim4_lim                       0xFF    //

// Register definition: hpstat4           
#define     hpstat4                        0x58    // Status of Advanced and High Power Features on Port 4, Read 
                                                   //  Only. 
#define ord_hpstat4_fetbad                    1    // The External FET has likely failed. See the FETBAD section 
                                                   //  for more information. 
#define msk_hpstat4_fetbad                 0x02    //
#define ord_hpstat4_pongpd                    0    // This bit is set when class 4 is measured during both events 
                                                   //  in a ping-pong classification. 
#define msk_hpstat4_pongpd                 0x01    //

#endif
#endif
