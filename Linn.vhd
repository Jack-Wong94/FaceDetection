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
		clk			=>clk,
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
		read_pro1:process
			file file_input	: TEXT open READ_MORE is :"input.text";
			varaible in_line	: LINE;
			 
		
	
	

	
	
	
	
	