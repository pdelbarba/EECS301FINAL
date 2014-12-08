module eecs301final(	clk50,
							nios_Dout, 
							clkADC, 
							ADC_Din, 
							ADC_Dout,
							ADC_ss,addr,
							ba,
							cas_n,
							cke,
							cs_n,
							dq,
							dqm,
							ras_n,
							we_n,
							clkSDRAM);
							
input clk50, ADC_Dout;
 
output wire clkADC, ADC_Din, ADC_ss;
output wire [9:0] nios_Dout;

output wire [11:0] addr;
output wire [1:0] ba, dqm;
output wire cas_n, cke, cs_n, ras_n, we_n, clkSDRAM;

inout [15:0]dq;

wire clkLCD, clkNIOS, PLL_locked;
wire [11:0] ADC_bus, ADC_data;

/////////////////// PLL ///////////////////////////////////////////////////////
PLL (, clk50, clkNIOS, clkADC, clkLCD, clkSDRAM, PLL_locked);

/////////////////// NIOS Clock domain 0, 99Mhz ////////////////////////////////

my_nios2_system (	clkNIOS,
						PLL_locked, 
						ADC_data,
						nios_Dout,
						addr,
						ba,
						cas_n,
						cke,
						cs_n,
						dq,
						dqm,
						ras_n,
						we_n);

/////////////////// SPI Clock domain 1, 18Mhz /////////////////////////////////

Sync (clkADC, ADC_bus, ADC_data, ADC_ss);

ADCio (clkADC, ADC_Din, ADC_Dout, ADC_bus, ADC_ss);

/////////////////// LCD Clock domain 2, 9Mhz //////////////////////////////////

//lcd control here

endmodule