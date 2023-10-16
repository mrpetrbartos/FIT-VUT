# Síťový sniffer

Cílem druhého projektu bylo vytvořit aplikaci, která bude analyzovat síťový provoz.

## Instalace

K sestavení na systému Linux je nutné mít nainstalovaný GNU Make, aplikace samotná
je dodržuje standard C++20, je tedy nutné mít také dostatečně novou verzi programu
GCC, pomocí kterého je aplikace překládána. Pro spárvný běh programu je důležitý
hlavičkový soubor `pcap.h`, tekrý musí být pro překlad k dispozici. K samotnému
sestavení slouží příkaz:

```shell
make
```

Pro sestavení na Windows by bylo třeba na stoj nainstalovat `WinPcap`, zbavit se
práce s `getopt` a pracovat s hlavičkovým souborem `Winsock2.h`. Podpora další
platformy by efektivně znásobila velikost codebase, což není účelem tohoto projektu,
a tak není systém Windows podporován.

## Spuštění

Aplikace se na Linuxu spouští následovně:

```shell
./ipk-sniffer [-i interface | --interface interface] {-p port [--tcp|-t] [--udp|-u]} 
[--arp] [--icmp4] [--icmp6] [--igmp] [--mld] {-n num}
```

vysvětlení jednotlivých parametrů lze nalézt po spuštění programu s argumentem --help

```shell
./ipk-sniffer --help
```

případně je dohledatelné na stránce se zadáním projektu [1]. Při spuštění programu se
záměrem zachytávat pakety je nutné program spustit s dostatečnými pravomocemi (příkaz `sudo`).

## Teorie

Níže je uvedeno pár teoretických poznatků, které pomou při porozumění kódu pracujícím s daty.

### Tvar dat

Jelikož jsou informace přenášeny ve formě nul a jedniček (případně změny napětí), je nutné
standardizovat tvar informací. V případě síťových protokolů se jednotlivé specifikace nachází
pod hromadným označením RFC. Všechna RFC jsou volně dostupná na adrese <https://www.ietf.org/standards/rfcs/>.
Pro jednoduchost uveďme RFC 9293 popisující Transmission Control Protocol [2]:

```
    0                   1                   2                   3  
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1  
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  
   |          Source Port          |       Destination Port        |  
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  
    ...
```

V naší aplikaci z paketu vyčítáme zdrojový a cílový port. V případě, že by data již nebyla
uložena ve struktuře díky pomocným knihovnám, museli bychom z TCP paketu vyčíst prvních 16 bitů
(pozor, to jsou bity 0 až 15) pro zdrojový a dalších 16 bitů pro cílový port. Díky znalosti, že
port může nabývat hodnot 0 až 2^16 - 1 také můžeme ošetřit vstupní data.

### Zapouzdření a odpouzdření

Během přechodu mezi vrstvami TCP/IP dochází k takzvanému zapouzdření (encapsulation)
a odpouzdření (decapsulation) dat. Zapouzdřením se rozumí přibalení dalších informací
k přenášeným datům, tzv. hlaviček (headers). Na začátek dat z vyšší vrstvy je tedy
přibalena informace protokolu dané vrstvy. Na straně příjemce pak dochází k odpouzdření,
tedy čtení a zbavení se části dat, která byla pro danou vrstvu přidána. Pro úplnost je
nutné zmínit, že jsou přibalovány i tzv. patičky (footers), s těmi ale není v programu
nijak explicitně pracováno [3].

## Implementační detaily

V následující sekci popíši jednotlivé čísti programu a způsob jejich implementace.

### Kontrola argumentů

Linuxové prostředí poskytuje knihovnu `getopt.h` a funkci `getopt_long()` [4], pomocí které
jednoduše iterovat přes poskytnuté argumenty. U argumentů `p` a `n` je nutné kontrolovat,
zdali se jedná opravdu o číselné hodnoty. V případě zadání argumentu `i` je nutné kontrolovat,
zdali následuje nějaké síťové rozhraní. Pokud ano, je dané rozhraní vyhledáno v seznamu dostupných
rozhraní, pokud v seznamu není, je program ukončen s chybou. Je také nutné kontrolovat počet
ostatních argumentů, hlavně v případě, kdy je argument `i` zadán bez rozhraní. V takovém případě
musí být jediným argumentem.

### Zahájení zachytávání

Po úspěšné kontrole argumentů je program připraven začít zachytávat pakety, k čemuž je v programu
využívána knihovna `pcap` [5]. Před samotným zachytáváním je nutno zkompilovat samotný zachytavací
program. K jeho přípravě je nutné znát adresu sítě, ale také mít k dispozici handle sloužící k
zachytávání paketů.

Jako poslední věc je nutné vytvořit filtr. Díky jeho jednoduché syntaxi [6] je možné jeho tvorbu
zalgoritmizovat. Jednotlivé filtry, které je možné zadat jako argumenty programu, jsou uloženy
v poli. Po zadání jednotlivých argumentů je nastavována hodnota v bitovém poli, která reprezentuje
umístění daného filtru v seznamu. K tvorbě filtru pak stačí iterovat přes jednotlivé bity a v případě,
že je bit nastaven na 1, je řetězec ze seznamu vložen do seznamu uchovávající filtr. V případě, že
nejsou žádné bity nastaveny na 1, jsou do výsledného filtru přidány všechny jednotlivé filtry.

```cpp
    std::vector<std::string> modes = { ... };
    std::string filter;
    bool noFilter = options.filters.none();
    for (std::size_t i = 0; i < options.filters.size(); ++i)
    {
        if (options.filters[i] || noFilter)
            filter.append("(" + modes[i] + ") or ");
        ...
    }
```

Po zkompilování a nastavení filtru je možné začít zachytávat pakety.

### Zachytávání

Zachycené pakety jsou zpracovány funkcí `captureCallback`. Základní informace, které z paketu získáváme,
jsou čas, MAC adresy příjemce a odesílatele, délka paketu a obsah paketu.

Jelikož je nutné tisknout časovou informaci tak, aby splňovala normu RFC 3339 [7], bylo nutné vytvořit novou funkci
(`printTimestamp`), která tuto funkčnost zaručuje - hojně jsou využívány schopnosti funkce `std::put_time`
a chybějící informace jednoduše doplňovány pomocí manipulace s řetězci.

Dále bylo nutné vytvořit funkci pro tisk obsahu paketu. Obsah paketu, který je možno nalézt v proměnné `bytes`,
je rozdělen na kousky po 16 bytech - délce jednoho řádku výpisu - do proměnné `buff`. Byte je prvně přetypován
tak, aby ho bylo možné vytisknout v hexadecimální podobě. Po vypsání celého řádku je za mezeru vypsán v podobě,
která je koncovému uživateli přívětivější, tedy jako znaky z ASCII tabulky. Hodnoty, které nelze tisknout, jsou
reprezentovány tečkou.

V případě paketů nesoucí informace z protokolu ARP bylo nutné implementovat novou funkci pro tisknutí IPv4 adresy,
jelikož se způsob uchování dat liší od toho, ve kterém jsou data uchována v paketu nesoucím informace z protokolu IP.

Pokud paket nesl informace z vyšších vrstev síťového stacku, bylo nutné ručně provádět odpouzdření. V případě
přechodu z Ethernetu na IPv4/IPv6/ARP je situace jednoduchá - posuneme pointer na strukturu o takový počet bytů,
jaká je délka Ethernetové hlavičky, situace u IPv6 je obdobná, délka hlavičky je normou [8] fixně dána jeho 40 bytů.
V případě IPv4 situace není tak jednoduchá, jelikož je její délka proměnná, je tedy nutné z IP paketu vyčíst
délku hlavičky a o tu se posunout (je nutné si dávat pozor, že délka hlavičky je uchována jako počet DWORDS).

### Ukončení programu

Program je ukončen s chybou, pokud uživatel správně nezadá argumenty, případně když není možné správně zajistit
zachytávaní paketů. Pokud je program spuštěn správně a je možné zachytávat pakety, program se sám ukončí po
sběru všech paketů (v základu 1). Pokud uživatel zadá argumenty, které způsobí, že program běží dlouho,
je možné program ukončit pomocí klávesové zkratky CTRL+C.  

## Testování

Program byl testován v následujícím prostředí:

- NixOS poskytnutý instruktory kurzu, který byl spuštěn pomocí Oracle VM VirtualBox, síťová karta byla v režimu
síťový most, kdy sdílí s fyzickým zařízením adaptér, avšak má vlastní identitu
- Arch Linux, který byl spuštěn pomocí WSL 2, verze Windows 10 je pak 22H2

Při testování byl hojně využíván program Wireshark [9]. Jelikož je jeho funkčnost identická s kýženou
funkčností aplikace, bylo jednoduché jednotlivé zachycené pakety porovnávat oproti záznamům ve Wireshark.

Ač bylo možné mnoho testovacích případů vyvolávat ručně, příkladem jsou například ICMP a nástroj `ping`
nebo ARP a nástroj `arping`, mnoho dalších bylo ručně testovat namáhavé. V tomto případě byl použit
nástroj `tcpreplay`. V kombinaci s knihovnou záznamů [10] bylo možné testovat stejný případ několikrát,
navíc velmi jednoduše. Výsledek zachycený v programu pak byl porovnán s záchytem z programu Wireshark,
stejná data pak znamenala průchod testem.

Ač bylo hlavně testováno, zdali program zachytává a vrací data ve správném formátu, manuálně bylo testováno
i chování při zpracování argumentů - tedy chybné zpracování nebo zadávání jednotlivých filtrů, počtu zachycených
paketů, případně omezení portu. Níže jsou uvedeny některé testovací případy.

- neexistující rozhraní

```shell
sudo ./ipk-sniffer -i foo
ERROR: Provided interface isn't valid
```

- výpis dostupných rozhraní

```shell
sudo ./ipk-sniffer -i
eth0
any
lo
docker0
...
```

- ARP dotaz (doplněn příkazem `arping 192.168.0.1` v jiném shellu)

```shell
sudo ./ipk-sniffer -i eth0 --arp
timestamp: 2023-04-16T19:18:11.908+02:00
src MAC: 00:15:5d:81:53:f6
dst MAC: ff:ff:ff:ff:ff:ff
frame length: 42 bytes
src IP: 172.30.59.187
dst IP: 192.168.0.1

0x0000: ff ff ff ff ff ff 00 15 5d 81 53 f6 08 06 00 01  ........ ].S.....
0x0010: 08 00 06 04 00 01 00 15 5d 81 53 f6 ac 1e 3b bb  ........ ].S...;.
0x0020: ff ff ff ff ff ff c0 a8 00 01 
```

- zachycení jakéhokoliv paketu na rozhraní eth0

```shell
sudo ./ipk-sniffer -i eth0      
timestamp: 2023-04-16T23:22:25.337+02:00
src MAC: 00:15:5d:df:34:69
dst MAC: ff:ff:ff:ff:ff:ff
frame length: 86 bytes
src IP: 172.30.48.1
dst IP: 172.30.63.255
src port: 57621
dst port: 57621

0x0000: ff ff ff ff ff ff 00 15 5d df 34 69 08 00 45 00  ........ ].4i..E.
0x0010: 00 48 3f 7d 00 00 80 11 32 eb ac 1e 30 01 ac 1e  .H?}.... 2...0...
0x0020: 3f ff e1 15 e1 15 00 34 65 01 53 70 6f 74 55 64  ?......4 e.SpotUd
0x0030: 70 30 48 09 55 ed d8 e8 c6 05 00 01 00 04 48 95  p0H.U... ......H.
0x0040: c2 03 49 d9 2b a1 e4 c2 39 71 3b 1a 88 a2 99 95  ..I.+... 9q;.....
0x0050: ab f3 33 59 6f d1                                ..3Yo.
```

- zachycení DHCPv6 komunikace (doplněn příkazem `tcpreplay -i eth0 DHCPv6.cap` v jiném shellu)

```shell
sudo ./ipk-sniffer -i eth0 -t -u -p 547
timestamp: 2023-04-17T18:38:16.852+02:00
src MAC: 08:00:27:fe:8f:95
dst MAC: 33:33:00:01:00:02
frame length: 114 bytes
src IP: fe80::a00:27ff:fefe:8f95
dst IP: ff02::1:2
src port: 546
dst port: 547

0x0000: 33 33 00 01 00 02 08 00 27 fe 8f 95 86 dd 60 00  33...... '.....`.
0x0010: 00 00 00 3c 11 01 fe 80 00 00 00 00 00 00 0a 00  ...<.... ........
0x0020: 27 ff fe fe 8f 95 ff 02 00 00 00 00 00 00 00 00  '....... ........
0x0030: 00 00 00 01 00 02 02 22 02 23 00 3c ad 08 01 10  ......." .#.<....
0x0040: 08 74 00 01 00 0e 00 01 00 01 1c 39 cf 88 08 00  .t...... ...9....
0x0050: 27 fe 8f 95 00 06 00 04 00 17 00 18 00 08 00 02  '....... ........
0x0060: 00 00 00 19 00 0c 27 fe 8f 95 00 00 0e 10 00 00  ......'. ........
0x0070: 15 18
```

---
[1] VESELÝ, V., 2023. zeta. In git [online]. [cit. 2023-04-16]. Dostupné z:
<https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Project%202/zeta>

[2] EDDY, W. RFC FT-IETF-TCPM-rfc793bis: Transmission control protocol (TCP). In IETF Datatracker
[online]. 2022. [cit. 2023-04-16]. Dostupné z: <https://datatracker.ietf.org/doc/html/rfc9293>

[3] JACOBS, D. Intro to encapsulation and Decapsulation in networking: TechTarget.
In Networking [online]. 2022. [cit. 2023-04-16].
Dostupné z: <https://www.techtarget.com/searchnetworking/tip/Intro-to-encapsulation-and-decapsulation-in-networking>

[4] Getopt(3) - linux manual page. In man7 [online]. [cit. 2023-04-16]. Dostupné z:
<https://man7.org/linux/man-pages/man3/getopt.3.html>

[5] PCAP(3) - linux man page. In pcap(3): Packet Capture library - Linux man page [online]. [cit. 2023-04-16].
Dostupné z: <https://linux.die.net/man/3/pcap>

[6] PCAP-filter(7) man page. In pcap-filter(7) man page | TCPDUMP & LIBPCAP [online]. [cit. 2023-04-16].
Dostupné z: <https://www.tcpdump.org/manpages/pcap-filter.7.html>

[7] NEWMAN, C. - KLYNE, G. RFC FT-IETF-IMPP-datetime: Date and time on the internet: Timestamps. In IETF Datatracker
[online]. 2002. [cit. 2023-04-16]. Dostupné z: <https://datatracker.ietf.org/doc/html/rfc3339>

[8] DEERING, D.S.E. - HINDEN, B. RFC FT-ietf-6man-rfc2460bis: Internet protocol, version 6 (ipv6) specification.
In IETF Datatracker [online]. 2017. [cit. 2023-04-16]. Dostupné z: <https://datatracker.ietf.org/doc/html/rfc8200>

[9] Wireshark · go deep. In Wireshark [online]. [cit. 2023-04-16]. Dostupné z: <https://www.wireshark.org/>

[10] Packet captures. In Packet Captures - PacketLife.net [online]. [cit. 2023-04-16].
Dostupné z: <https://packetlife.net/captures/>

## Hodnocení

| Část | Body     | Chyby |
|------|----------|-------|
| 1    | 17.25/20 | -     |

NOTE: u pcap_open_live mělo být podle zadání `promisc` nastaveno na 1, body jsem dostal i tak
