module ADCio(SPIClock, ADC_Din, ADC_Dout, ADCbus, ADCss);

input SPIClock, ADCss, ADC_Dout;

integer count, init;

output reg [11:0] ADCbus;
output reg ADC_Din;

initial
	begin
		count = 0;
		ADC_Din = 0;
		init = 0;
	end

always @(posedge SPIClock)
begin
	if (~ADCss)
	begin
		count = count + 1;
		if (init < 2)
		begin
			if (count < 12)
				ADC_Din = 1;
			init = init + 1;
		end
		
		else
		begin
			//Control block for writing to ADC_Din
			case(count)
			0:		ADC_Din = 1;
			1:		ADC_Din = 0;
			2:		ADC_Din = 0;
			3:		ADC_Din = 0;
			4:		ADC_Din = 0;
			5:		ADC_Din = 0;
			6:		ADC_Din = 1;
			7:		ADC_Din = 1;
			8:		ADC_Din = 0;
			9:		ADC_Din = 0;
			10:	ADC_Din = 0;
			11:	ADC_Din = 1;
			endcase
			//Control block for reading from ADC_Dout and packaging into ADCbus
			case(count)
			4:		ADCbus[11] = ADC_Dout;
			5:		ADCbus[10] = ADC_Dout;
			6:		ADCbus[9] = ADC_Dout;
			7:		ADCbus[8] = ADC_Dout;
			8:		ADCbus[7] = ADC_Dout;
			9:		ADCbus[6] = ADC_Dout;
			10:	ADCbus[5] = ADC_Dout;
			11:	ADCbus[4] = ADC_Dout;
			12:	ADCbus[3] = ADC_Dout;
			13:	ADCbus[2] = ADC_Dout;
			14:	ADCbus[1] = ADC_Dout;
			15:	ADCbus[0] = ADC_Dout;
			endcase
		end
	end
	
	if (ADCss)
		count = 0;
end	
endmodule