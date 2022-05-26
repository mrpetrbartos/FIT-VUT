-- uart_fsm.vhd: UART controller - finite state machine
-- Author(s): xbarto0g
--
library ieee;
use ieee.std_logic_1164.all;

-------------------------------------------------
entity UART_FSM is
port(
   CLK : in std_logic;
   RST : in std_logic;
   DIN : in std_logic;
   COUNTER : in std_logic_vector(4 downto 0);
   BITS_REC : in std_logic_vector(3 downto 0);
   CNT_EN : out std_logic;
   READ_EN : out std_logic;
   WORD_VLD : out std_logic
   );
end entity UART_FSM;

-------------------------------------------------
architecture behavioral of UART_FSM is
type state is (IDLE, COM_BEGIN, READ_DATA, COM_END, VALIDATE);
signal pstate : state := IDLE;
signal nstate : state;


begin
 
   pstate_reg: process(RST, CLK)
   begin
      if (RST='1') then
         pstate <= IDLE;
      elsif rising_edge(CLK) then 
         pstate <= nstate;
      end if;
   end process;

   nstate_logic: process(pstate, DIN, COUNTER, BITS_REC)
   begin
      case pstate is
         when IDLE =>
            if (DIN='0') then 
               nstate <= COM_BEGIN;
				else 
					nstate <= pstate;
            end if;
         when COM_BEGIN =>
            if (COUNTER="10110") then
               nstate <= READ_DATA;
				else 
					nstate <= pstate;
            end if;
         when READ_DATA =>
            if (BITS_REC="1000") then
               nstate <= COM_END;
				else 
					nstate <= pstate;
            end if;
         when COM_END =>
            if (COUNTER="10000") then
               nstate <= VALIDATE;
				else 
					nstate <= pstate;
            end if;
         when VALIDATE => 
				nstate <= IDLE;
			when others => null;
      end case;
   end process;
	
	output_process: process(pstate)
		begin
			READ_EN <= '0';
			CNT_EN <= '0';
			WORD_VLD <= '0';

			if (pstate = READ_DATA) then
				READ_EN <= '1'; 
			end if;
			  
			if (pstate = COM_BEGIN or pstate = READ_DATA or pstate = COM_END) then
				CNT_EN <= '1';
			end if;

			if (pstate = VALIDATE) then
				WORD_VLD <= '1';
			end if;
		end process;

end behavioral;
