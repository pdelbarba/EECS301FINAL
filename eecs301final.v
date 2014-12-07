module eecs301final(clk50, nios_Dout, clkADC, ADC_Din, ADC_Dout, ADC_ss);

input clk50, ADC_Dout;
output wire clkADC, ADC_Din, ADC_ss;
output wire [9:0]nios_Dout;

wire clkLCD, clkNIOS, PLL_locked;
wire [11:0] ADC_bus, ADC_data;

/////////////////// PLL ///////////////////////////////////////////////////////
PLL (, clk50, clkNIOS, clkADC, clkLCD, PLL_locked);

/////////////////// NIOS Clock domain 0, 99Mhz ////////////////////////////////

my_nios2_system (clkNIOS, PLL_locked, ADC_data, nios_Dout);

/////////////////// SPI Clock domain 1, 18Mhz /////////////////////////////////

Sync (clkADC, ADC_bus, ADC_data, ADC_ss);

ADCio (clkADC, ADC_Din, ADC_Dout, ADC_bus, ADC_ss);

/////////////////// LCD Clock domain 2, 9Mhz //////////////////////////////////

//lcd control here

endmodule