library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

LIBRARY std;
USE std.textio.all;
use ieee.std_logic_textio.all;

Entity top_l is
	generic
		(
			no_img	: integer :=2;
			no_row	: integer :=128;
			no_col	: integer :=128
		);
end top_l;

ARCHITECTURE behavior of top_l is

	COMPONENT top
	PORT(
	clk	: IN std_logic;
	Valid_in : IN std_logic;
	Valid_out : OUT std_logic;
	Data_in : IN std_logic_vector(23 downto 0);
	Data_out : OUT std_logic_vector(23 downto 0)
	);
	--inputs
	signal clk	:std_logic :='0';
	signal Valid_in :std_logic := '0';
	signal Data_in : std_logic_vector(23 downto 0):= (others =>'0');
	signal Data_q :std_logic_vector(23 downto 0) :=(others=>'0');
	signal Valid_q :std_logic_vector :='0';
	--outputs
	signal Valid_out :std_logic;
	signal Data_out :std_logic_vector(23 downto 0);
	
	constant clk_period :time :=10ns;
	
	begin
	uut:top
	PORT MAP(
		clk			=> clk,
		Valid_in		=> Valid_q,
		Valid_out	=> Valid_out,
		Data_in		=>Data_q,
		Data_out		=> Data_out
	);
	clk_process :process
	begin
		clk <= '0';
		wait for clk_period/2;
		clk <='1';
		wait for clk_period/2;
	end process;
--read input from text
		read_proc_1:process
			file file_input	: TEXT open READ_MORE is :"input.text";
			variable in_line	: LINE;
			variable value_in	 : std_logic_vector(23 downto 0);
			
		begin
			wait for clk_period/2;
			wait for clk_period*10;
			for i in 1 to no_img loop
				Valid_in <= '1';
				for f in 1 to (no_col*no_row) loop
					readline(file_input, in_line);
					hread (in_line,value_in);
					data_in <= value_in;
					wait for clk_period
				end loop;
				Valid_in <='0';
				wait for clk_period*100;
			end loop;
			
			Valid_in <= '0';
			data_in <=(others =>'0');
			wait;
		end process;
		
	--write output to txt
	wr_proc:process(clk);
	variable my_line	: line;
	variable ln	:std_logic_vector(23 downto 0);
	file in_file	:text open write_Mode is "output.txt";
	begin
		if rising_edge(clk) then 
			if (Valid_out ='1') then
			 ln :=Data_out;
			 hwrite(my_line,ln);
			end if;
		end if;
	end process;
	
process(clk);
begin	
	if rising_edge(clk) then
		Valid_q <= Valid_in;
		Data_q <= Data_in;
	end if;
end process;

End;