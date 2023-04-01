-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2022 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Petr Bartoš <xbarto0g AT stud.fit.vutbr.cz>
--
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.std_logic_arith.ALL;
USE ieee.std_logic_unsigned.ALL;

-- ----------------------------------------------------------------------------
-- Entity declaration
-- ----------------------------------------------------------------------------
ENTITY cpu IS
  PORT (
    CLK : IN STD_LOGIC; -- hodinovy signal
    RESET : IN STD_LOGIC; -- asynchronni reset procesoru
    EN : IN STD_LOGIC; -- povoleni cinnosti procesoru

    -- synchronni pamet RAM
    DATA_ADDR : OUT STD_LOGIC_VECTOR(12 DOWNTO 0); -- adresa do pameti
    DATA_WDATA : OUT STD_LOGIC_VECTOR(7 DOWNTO 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
    DATA_RDATA : IN STD_LOGIC_VECTOR(7 DOWNTO 0); -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
    DATA_RDWR : OUT STD_LOGIC; -- cteni (0) / zapis (1)
    DATA_EN : OUT STD_LOGIC; -- povoleni cinnosti

    -- vstupni port
    IN_DATA : IN STD_LOGIC_VECTOR(7 DOWNTO 0); -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
    IN_VLD : IN STD_LOGIC; -- data platna
    IN_REQ : OUT STD_LOGIC; -- pozadavek na vstup data

    -- vystupni port
    OUT_DATA : OUT STD_LOGIC_VECTOR(7 DOWNTO 0); -- zapisovana data
    OUT_BUSY : IN STD_LOGIC; -- LCD je zaneprazdnen (1), nelze zapisovat
    OUT_WE : OUT STD_LOGIC -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
  );
END cpu;
-- ----------------------------------------------------------------------------
-- Architecture declaration
-- ----------------------------------------------------------------------------
ARCHITECTURE behavioral OF cpu IS
  SIGNAL mux1_sel : STD_LOGIC;
  SIGNAL mux1_out : STD_LOGIC_VECTOR(12 DOWNTO 0);

  SIGNAL mux2_sel : STD_LOGIC_VECTOR(1 DOWNTO 0);
  SIGNAL mux2_out : STD_LOGIC_VECTOR(7 DOWNTO 0);

  SIGNAL cnt_data : STD_LOGIC_VECTOR(7 DOWNTO 0);
  SIGNAL cnt_inc : STD_LOGIC;
  SIGNAL cnt_dec : STD_LOGIC;
  SIGNAL cnt_set_one : STD_LOGIC;

  SIGNAL pc_data : STD_LOGIC_VECTOR(12 DOWNTO 0);
  SIGNAL pc_inc : STD_LOGIC;
  SIGNAL pc_dec : STD_LOGIC;

  SIGNAL ptr_data : STD_LOGIC_VECTOR(11 DOWNTO 0);
  SIGNAL ptr_inc : STD_LOGIC;
  SIGNAL ptr_dec : STD_LOGIC;

  TYPE fsm_state IS (
    s_waiting, s_fetch, s_decode, s_ptr_inc, s_ptr_dec,
    s_inc_0, s_inc_1, s_inc_2, s_dec_0,
    s_dec_1, s_dec_2, s_print_0, s_print_1,
    s_read_0, s_read_1, s_read_2, s_null, s_other,
    s_while_0, s_while_1, s_while_2, s_while_3, s_while_end_0,
    s_while_end_1, s_while_end_2, s_while_end_3, s_while_end_4,
    s_do, s_do_end_0, s_do_end_1, s_do_end_2,
    s_do_end_3, s_do_end_4
  );
  SIGNAL p_state : fsm_state := s_waiting;
  SIGNAL n_state : fsm_state := s_waiting;

BEGIN
  cnt : PROCESS (RESET, CLK, cnt_inc, cnt_dec, cnt_set_one)
  BEGIN
    IF (RESET = '1') THEN
      cnt_data <= (OTHERS => '0');
    ELSIF (CLK'EVENT AND CLK = '1') THEN
      IF (cnt_inc = '1') THEN
        cnt_data <= cnt_data + 1;
      ELSIF (cnt_dec = '1') THEN
        cnt_data <= cnt_data - 1;
      ELSIF (cnt_set_one = '1') THEN
        cnt_data <= (0 => '1', OTHERS => '0');
      END IF;
    END IF;
  END PROCESS cnt;

  pc : PROCESS (RESET, CLK, pc_inc, pc_dec)
  BEGIN
    IF (RESET = '1') THEN
      pc_data <= (OTHERS => '0');
    ELSIF (CLK'EVENT AND CLK = '1') THEN
      IF (pc_inc = '1') THEN
        pc_data <= pc_data + 1;
      ELSIF (pc_dec = '1') THEN
        pc_data <= pc_data - 1;
      END IF;
    END IF;
  END PROCESS;

  ptr : PROCESS (RESET, CLK, ptr_inc, ptr_dec)
  BEGIN
    IF (RESET = '1') THEN
      ptr_data <= (OTHERS => '0');
    ELSIF (CLK'EVENT AND CLK = '1') THEN
      IF (ptr_inc = '1') THEN
        ptr_data <= ptr_data + 1;
      ELSIF (ptr_dec = '1') THEN
        ptr_data <= ptr_data - 1;
      END IF;
    END IF;
  END PROCESS;

  mux1 : PROCESS (CLK, RESET, mux1_sel)
  BEGIN
    IF (RESET = '1') THEN
      mux1_out <= (OTHERS => '0');
    ELSIF (CLK'EVENT AND CLK = '1') THEN
      CASE mux1_sel IS
        WHEN '0' =>
          mux1_out <= pc_data;
        WHEN '1' =>
          mux1_out <= '1' & ptr_data;
        WHEN OTHERS =>
          mux1_out <= (OTHERS => '0');
      END CASE;
    END IF;
  END PROCESS;

  DATA_ADDR <= mux1_out;

  mux2 : PROCESS (CLK, RESET, mux2_sel)
  BEGIN
    IF (RESET = '1') THEN
      mux2_out <= (OTHERS => '0');
    ELSIF (CLK'EVENT AND CLK = '1') THEN
      CASE mux2_sel IS
        WHEN "00" =>
          mux2_out <= IN_DATA;
        WHEN "01" =>
          mux2_out <= DATA_RDATA - 1;
        WHEN "10" =>
          mux2_out <= DATA_RDATA + 1;
        WHEN OTHERS =>
          mux2_out <= (OTHERS => '0');
      END CASE;
    END IF;
  END PROCESS;

  DATA_WDATA <= mux2_out;
  OUT_DATA <= DATA_RDATA;

  fsm_pstate : PROCESS (CLK, RESET)
  BEGIN
    IF (RESET = '1') THEN
      p_state <= s_waiting;
    ELSIF (CLK'event) AND (CLK = '1') THEN
      IF EN = '1' THEN
        p_state <= n_state;
      END IF;
    END IF;
  END PROCESS;

  fsm_nstate : PROCESS (p_state, cnt_data, OUT_BUSY, IN_VLD, DATA_RDATA)
  BEGIN
    DATA_RDWR <= '0';
    DATA_EN <= '0';
    IN_REQ <= '0';
    OUT_WE <= '0';

    cnt_dec <= '0';
    cnt_inc <= '0';
    cnt_set_one <= '0';

    pc_inc <= '0';
    pc_dec <= '0';

    ptr_inc <= '0';
    ptr_dec <= '0';

    mux1_sel <= '1';
    mux2_sel <= "00";

    CASE p_state IS
      WHEN s_waiting =>
        n_state <= s_fetch;
        mux1_sel <= '0';

      WHEN s_fetch =>
        DATA_EN <= '1';
        n_state <= s_decode;

      WHEN s_decode =>
        CASE DATA_RDATA IS
          WHEN X"00" => n_state <= s_null;
          WHEN X"28" => n_state <= s_do;
          WHEN X"29" => n_state <= s_do_end_0;
          WHEN X"2B" => n_state <= s_inc_0;
          WHEN X"2C" => n_state <= s_read_0;
          WHEN X"2D" => n_state <= s_dec_0;
          WHEN X"2E" => n_state <= s_print_0;
          WHEN X"3C" => n_state <= s_ptr_dec;
          WHEN X"3E" => n_state <= s_ptr_inc;
          WHEN X"5B" => n_state <= s_while_0;
          WHEN X"5D" => n_state <= s_while_end_0;
          WHEN OTHERS => n_state <= s_other;
        END CASE;

        -- PTR ← 0x1000 + (PTR + 1) % 0x1000, PC ← PC + 1
      WHEN s_ptr_inc =>
        ptr_inc <= '1';
        pc_inc <= '1';
        n_state <= s_waiting;

        -- PTR ← 0x1000 + (PTR - 1) % 0x1000, PC ← PC + 1
      WHEN s_ptr_dec =>
        ptr_dec <= '1';
        pc_inc <= '1';
        n_state <= s_waiting;

        -- DATA RDATA ← mem[PTR]
      WHEN s_inc_0 =>
        DATA_EN <= '1';
        DATA_RDWR <= '0';
        n_state <= s_inc_1;

      WHEN s_inc_1 =>
        mux2_sel <= "10";
        DATA_EN <= '1';
        n_state <= s_inc_2;

        -- mem[PTR] ← DATA RDATA + 1, PC ← PC + 1
      WHEN s_inc_2 =>
        DATA_EN <= '1';
        DATA_RDWR <= '1';
        pc_inc <= '1';
        n_state <= s_waiting;

        -- DATA RDATA ← mem[PTR]
      WHEN s_dec_0 =>
        DATA_EN <= '1';
        DATA_RDWR <= '0';
        n_state <= s_dec_1;

      WHEN s_dec_1 =>
        mux2_sel <= "01";
        DATA_EN <= '1';
        n_state <= s_dec_2;

        -- mem[PTR] ← DATA RDATA - 1, PC ← PC + 1
      WHEN s_dec_2 =>
        DATA_EN <= '1';
        DATA_RDWR <= '1';
        pc_inc <= '1';
        n_state <= s_waiting;

      WHEN s_print_0 =>
        DATA_EN <= '1';
        DATA_RDWR <= '0';
        n_state <= s_print_1;

        -- while (OUT BUSY) {}
        -- OUT DATA ← mem[PTR], PC ← PC + 1
      WHEN s_print_1 =>
        IF (OUT_BUSY = '1') THEN
          mux1_sel <= '1';
          DATA_EN <= '1';
          DATA_RDWR <= '0';
          n_state <= s_print_1;
        ELSIF (OUT_BUSY = '0') THEN
          OUT_WE <= '1';
          pc_inc <= '1';
          n_state <= s_waiting;
        END IF;

        -- IN REQ ← 1
      WHEN s_read_0 =>
        mux2_sel <= "00";
        IN_REQ <= '1';
        n_state <= s_read_1;

        -- while (!IN VLD) {}
      WHEN s_read_1 =>
        IF (IN_VLD = '0') THEN
          mux2_sel <= "00";
          IN_REQ <= '1';
          n_state <= s_read_1;
        ELSIF (IN_VLD = '1') THEN
          IN_REQ <= '0';
          n_state <= s_read_2;
        END IF;

        -- mem[PTR] ← IN DATA, PC ← PC + 1
      WHEN s_read_2 =>
        DATA_EN <= '1';
        DATA_RDWR <= '1';
        pc_inc <= '1';
        n_state <= s_waiting;

        -- PC ← PC + 1
      WHEN s_while_0 =>
        DATA_RDWR <= '0';
        DATA_EN <= '1';
        pc_inc <= '1';
        n_state <= s_while_1;

        -- if (mem[PTR] == 0)
        -- CNT ← 1
      WHEN s_while_1 =>
        IF (DATA_RDATA = "00000000") THEN
          cnt_set_one <= '1';
          mux1_sel <= '0';
          n_state <= s_while_2;
        ELSE
          n_state <= s_waiting;
        END IF;

      WHEN s_while_2 =>
        DATA_EN <= '1';
        n_state <= s_while_3;

        -- while (CNT != 0)
        --   c ← mem[PC]
        --   if (c == ’[’) CNT ← CNT + 1 elsif (c == ’]’) CNT ← CNT - 1
        --   PC ← PC + 1
      WHEN s_while_3 =>
        IF (cnt_data = "00000000") THEN
          n_state <= s_waiting;
        ELSE
          IF (DATA_RDATA = X"5B") THEN
            cnt_inc <= '1';
          ELSIF (DATA_RDATA = X"5D") THEN
            cnt_dec <= '1';
          END IF;
          n_state <= s_while_2;
          mux1_sel <= '0';
          pc_inc <= '1';
        END IF;

      WHEN s_while_end_0 =>
        n_state <= s_while_end_1;
        DATA_EN <= '1';
        DATA_RDWR <= '0';

        -- if (mem[PTR] == 0)
        --   PC ← PC + 1
        -- else
        --   CNT ← 1, PC ← PC - 1
      WHEN s_while_end_1 =>
        IF (DATA_RDATA = "00000000") THEN
          n_state <= s_waiting;
          pc_inc <= '1';
        ELSE
          n_state <= s_while_end_2;
          cnt_set_one <= '1';
          pc_dec <= '1';
        END IF;

        -- while (CNT != 0)
        --   c ← mem[PC]
      WHEN s_while_end_2 =>
        IF (cnt_data = "00000000") THEN
          n_state <= s_waiting;
        ELSE
          n_state <= s_while_end_3;
          mux1_sel <= '0';
        END IF;

      WHEN s_while_end_3 =>
        DATA_EN <= '1';
        n_state <= s_while_end_4;

        -- if (c == ’]’) CNT ← CNT + 1 elsif (c == ’[’) CNT ← CNT - 1
        -- if (CNT == 0) PC ← PC + 1 else PC ← PC - 1
      WHEN s_while_end_4 =>
        IF (DATA_RDATA = X"5B") THEN
          cnt_dec <= '1';
        ELSIF (DATA_RDATA = X"5D") THEN
          cnt_inc <= '1';
        END IF;
        IF (cnt_data = "00000001" AND DATA_RDATA = X"5B") THEN
          pc_inc <= '1';
        ELSE
          pc_dec <= '1';
        END IF;
        n_state <= s_while_end_2;

      WHEN s_do =>
        pc_inc <= '1';
        n_state <= s_waiting;

      WHEN s_do_end_0 =>
        DATA_EN <= '1';
        DATA_RDWR <= '0';
        mux1_sel <= '0';
        n_state <= s_do_end_1;

        -- if (mem[PTR] == 0)
        --   PC ← PC + 1
        -- else
        --   CNT ← 1, PC ← PC - 1
      WHEN s_do_end_1 =>
        IF (DATA_RDATA = "00000000") THEN
          n_state <= s_waiting;
          pc_inc <= '1';
        ELSE
          n_state <= s_do_end_2;
          cnt_set_one <= '1';
          pc_dec <= '1';
        END IF;

        -- while (CNT != 0)
        --   c ← mem[PC]
      WHEN s_do_end_2 =>
        IF (cnt_data = "00000000") THEN
          n_state <= s_waiting;
        ELSE
          mux1_sel <= '0';
          n_state <= s_do_end_3;
        END IF;

      WHEN s_do_end_3 =>
        DATA_EN <= '1';
        n_state <= s_do_end_4;

        -- if (c == ’]’) CNT ← CNT + 1 elsif (c == ’[’) CNT ← CNT - 1
        -- if (CNT == 0) PC ← PC + 1 else PC ← PC - 1
      WHEN s_do_end_4 =>
        IF (DATA_RDATA = X"28") THEN
          cnt_dec <= '1';
        ELSIF (DATA_RDATA = X"29") THEN
          cnt_inc <= '1';
        END IF;
        IF (cnt_data = "00000001" AND DATA_RDATA = X"28") THEN
          pc_inc <= '1';
        ELSE
          pc_dec <= '1';
        END IF;
        n_state <= s_do_end_2;

        -- PC ← PC
      WHEN s_null =>
        n_state <= s_null;

        -- PC ← PC + 1
      WHEN s_other =>
        pc_inc <= '1';
        n_state <= s_waiting;

    END CASE;
  END PROCESS;

END behavioral;
