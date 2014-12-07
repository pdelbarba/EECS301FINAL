module Sync(SPIClock, ADC_bus, ADC_data, ADCss);

input SPIClock;
input [11:0] ADC_bus;

output reg [11:0] ADC_data;
output reg ADCss;

integer count;

initial
		count = 0;

always @(posedge SPIClock)
begin
	count = count + 1;
	if (count == 32)
	begin
		ADC_data <= ADC_bus;
	end
	else if (count == 16)
		ADCss = 1;
	else if (count == 64)
	begin
		count = 0;
		ADCss = 0;
	end
end
endmodule