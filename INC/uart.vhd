-- uart.vhd: UART controller - receiving part
-- Author(s): xbarto0g
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

-------------------------------------------------
entity UART_RX is
port(	
    CLK: 	    in std_logic;
	RST: 	    in std_logic;
	DIN: 	    in std_logic;
	DOUT: 	    out std_logic_vector(7 downto 0);
	DOUT_VLD: 	out std_logic := '0'
);
end UART_RX;  

-------------------------------------------------
architecture behavioral of UART_RX is
signal counter : std_logic_vector(4 downto 0);
signal bits_rec : std_logic_vector (3 downto 0);
signal cnt_en : std_logic;
signal read_en : std_logic;
signal word_vld : std_logic;
signal rec : std_logic;
signal inc : std_logic;

begin
    FSM: entity work.uart_fsm(behavioral)
    port map(
        CLK             => CLK,
        RST             => RST,
        DIN             => DIN,
		COUNTER         => counter,
        BITS_REC        => bits_rec,
        CNT_EN          => cnt_en,
		READ_EN         => read_en,
        WORD_VLD        => word_vld
    );
	 
	DOUT_VLD <= word_vld;
	
	counter_p: process(CLK, RST, rec, cnt_en)
	begin
		if (rec='1') then
			counter <= "00000";
		elsif (RST='1') or (cnt_en='0') then
			counter <= "00000";
		elsif rising_edge(CLK) then
			if (cnt_en='1') then
				counter <= counter + 1;
			end if;
		end if;
	end process;
	
	cmp_p: process(counter, read_en)
	begin
		if (counter(4)='1') then
			if (read_en='1') then
				rec <= '1';
			end if;
		else
			rec <= '0';
		end if;
	end process;

	bits_rec_p: process(RST, inc, read_en)
	begin
		if (RST='1') or (read_en='0') then
			bits_rec <= "0000";
		elsif rising_edge(inc) then
				bits_rec <= bits_rec + 1;
		end if;
	end process;
	
	bits_rec_inc: process(read_en, counter)
	begin
		if (read_en='1') and (counter="01000") then
			inc <= '1';
		else
			inc <= '0';
		end if;
	end process;
	
	dec_reg_p: process(RST, rec)
	begin
		if (RST='1') then 
			DOUT <= "00000000";
		elsif rising_edge(rec) then
			case bits_rec is
				when "0000" => 
					DOUT(0) <= DIN;
				when "0001" => 
					DOUT(1) <= DIN;
				when "0010" => 
					DOUT(2) <= DIN;
				when "0011" => 
					DOUT(3) <= DIN;
				when "0100" => 
					DOUT(4) <= DIN;
				when "0101" => 
					DOUT(5) <= DIN;
				when "0110" => 
					DOUT(6) <= DIN;
				when "0111" => 
					DOUT(7) <= DIN;
				when others => null;
			end case;
		end if;
	end process;
	
end behavioral;