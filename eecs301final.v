module eecs301final (clk50, LEDG, clkSDRAM, DRAM_CKE,DRAM_ADDR, DRAM_BA, DRAM_CS_N, DRAM_CAS_N, DRAM_RAS_N,DRAM_WE_N, DRAM_DQ, DRAM_UDQM, DRAM_LDQM);
//input [7:0] SW;
//input [0:0] KEY;
input clk50;
output [7:0] LEDG;
output [11:0] DRAM_ADDR;
output [1:0] DRAM_BA;
output DRAM_CAS_N, DRAM_RAS_N, clkSDRAM;
output DRAM_CKE, DRAM_CS_N, DRAM_WE_N, DRAM_UDQM, DRAM_LDQM;
inout [15:0] DRAM_DQ;

wire PLLlocked;

PLL PLL (,clk50, clkNIOS, clkSDRAM, PLLlocked);

my_nios2_system NiosII (	clkNIOS,
							PLLlocked,
							LEDG,
							/*DRAM_ADDR,
							DRAM_BA,
							DRAM_CAS_N,
							DRAM_CKE,
							DRAM_CS_N,
							DRAM_DQ,
							DRAM_UDQM,
							DRAM_RAS_N,
							DRAM_WE_N*/);
endmodule