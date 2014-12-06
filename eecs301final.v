module eecs301final(clk50, clkspi, Enc1, Enc2, AccCS, MOSI, MISO, leds);
//needs decs for memory?
input clk50;
input Enc1, Enc2;
input MISO;
output wire clkspi, AccCS, MOSI;
output wire [7:0]leds;

my_nios2_system (clk50, 1, leds);

endmodule