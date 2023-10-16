# Klient ke vzdálené kalkulačce

Cílem prvního projektu bylo vytvořit aplikaci, která bude komunikovat se vzdáleným serverem pomocí IPK Calculator Protocol.

## Instalace

Program je nutno sestavit pomocí příkazu make:

```shell
make
```

V případě sestavení na platformě Windows jsou nutná následující opatření:

- Nainstalovaná aplikace MinGW [1]
- Aplikace MinGW je přidána do systémového prostředí v nastavení či přímo v konzoli (v případě instalace na jiný oddíl je třeba `C:` zaměnit za patřičnou značku)

```cmd
set PATH=%PATH%;C:\MinGW\bin
```

- `mingw32-make.exe` je nutno přejmenovat na `make.exe`

## Spuštění

Aplikace se na Linuxu spouští následovně:

```shell
ipkcpc [--help] -h <host> -p <port> -m <mode> 
```

- host je IPv4 adresa, případně doménové jméno
- port je serverový port
- mode je buď `tcp`, nebo `udp`
- jsou podporovány i dlouhé verze jako `--host`, `--port` či `--mode`

Windows verze má pořadí argumentů pevné, spouští se tedy následovně:

```cmd
ipkcpc <host> <port> <mode>
```

## Implementační detaily

V následující sekci popíši jednotlivé čísti programu a způsob jejich implementace, který se může v závislosti na platformě lišit. Kostra kódu byla postavena na souborech poskytnutých doc. Ryšavým, za což mu tímto děkuji [2].

### Kontrola argumentů

Linuxové prostředí poskytuje knihovnu `getopt.h` a funkci `getopt_long()` [3], pomocí které lze jednoduše iterovat přes poskytnuté argumenty. Argument host není nijak validován, serverový port je převáděn pomocí funkce `strtol()` na jeho číselnou reprezentaci, je tedy jednoduché detekovat nevhodně zadaný argument. Následně je prověřeno, že port spadá do rozmezí 0-65535, tedy vrchním limitem stanoveným formátem segmentu [4]. Mode musí být nutně malými písmeny, jakékoliv jiné verze těchto slov nejsou akceptovány.

Jelikož Windows neposkytuje žádnou knihovnou pro kontrolu argumentů, rozhodl jsem se u Windows mutace nepodporovat libovolné pořadí argumentů a akceptuji je pouze v pořadí, jaké je uvedeno v sekci Spuštění. Jejich kontrola je pak stejná.

### Tvorba spojení

Po úspěšné kontrole argumentů se klient pokusí vytvořit spojení se serverem. Pro práci se sockety je na Windows nutné nejdříve použít funkci `WSAStartup()` sloužící k práci s Winsocket DLL [5]. Poté probíhá získávání adresy a překlad doménového jména, kde je rovnou prověřena jeho platnost. Získaná data jsou zpracována a uložena do vnitřní struktury `sockaddr_in`. Poté je vytvořen socket. Jelikož TCP nemá koncept zpráv, je nutné to zohlednit při tvorbě socketu - zatímco při režimu TCP je typ nastaven na `SOCK_STREAM`, u UDP je zvolen typ `SOCK_DGRAM` [6]. Následně je nastaven timeout pro komunikaci v režimu UDP a to na 10 sekund - zabráníme tak dlouhému čekání na odpověď ze strany serveru.

### Komunikace

Způsob komunikace je nehledě na zvoleném režimu podobný. Na začátku komunikace v režimu TCP je nutné vytvoření spojení. Poté již program očekává vstup od uživatele. Po zadání požadavku jsou v případě UDP verze předsunuty 2 byty - první nulový říká serveru, o jaký typ operace jde, druhý v sobě uchovává délku zbylé zprávy (payloadu) - jeho velikost 1 byte [7] limituje zprávu na 255 bytů/znaků. Po zpracování zprávy je odeslána serveru a klient začne čekat na odpověď. Zde případně dojde k timeoutu, pokud se odpověď od serveru nedostaví dostatečně rychle. V ideální aplikaci by bylo vhodné jak příjem, tak odesílání vložit do smyčky. Tento vstup je vždy možno přerušit stisknutím CTRL-C nebo CTRL-D, kdy dojde k ukončení spojení a následnému ukončení programu, v opačném případě může uživatel opět zadat zprávu a čekat na odpověď ze serveru.

### Ukončení programu

Při ukončení programu - ať už chybovému, nebo iniciovaném uživatelem je nutno správně ukončit komunikaci. Nejdříve je nutno uzavřít socket pro jakoukoliv další komunikaci, ať už psaní, nebo zápis, toho docílíme příkazem `shutdown()`, až poté je možné ukončit komunikaci pomocí příkazu `close()` [8].

## Testování

Při testování bylo použito následující prostředí:

- server spuštěn na virtuálním stroji s NixOS, který byl poskytnut instruktory - zde byl vypnut firewall a jeho síťová karta byla nakonfigurována v režimu síťový most
- linuxová verze byla testována na OS Arch Linux, který spuštěn pomocí WSL - gcc version 12.2.1
- verze pro Windows byla spouštěna na Microsoft Windows 10, verze 22H2

Při vývoji byla komunikace testována pomocí nástroje Wireshark [9], zde jsem kontroloval, zda přenáším informace, které opravu přenášet chci, teda hlavně hlavičku v případě UDP režimu.

Ve finální verzi byl testován normální běh programu v obou režimech - tedy zahájení komunikace, dotazy na výpočet a korektní ukončení. Dále byly testovány případy, kdy uživatel ukončí komunikaci pomocí jedné z dříve uvedených klávesových zkratek. V neposlední řadě byly testovány kontroly argumentů, ale i dotazy na neexistující server či kontrola vypršení UDP timeoutu. Níže uvádím příklady testování:

```shell
./ipkcpc -m tcp -h 172.16.1.163 -p 2023
HELLO
HELLO
SOLVE (+ 1 2)
RESULT 3
SOLVE (/ 4 18)     
RESULT 2/9
BYE
BYE
```

```shell
./ipkcpc -m tcp -h 172.16.1.163 -p 2023
HELLO
HELLO
^CBYE
```

```shell
./ipkcpc -m tcp -h neexistujici-server.cz -p 2023
ERROR: Couldn't find host neexistujici-server.cz
```

```cmd
ipkcpc.exe --help
Client for the IPK Calculator Protocol

Usage: ipkcpc <host> <port> <mode>
  host is the IPv4 address of the server
  port the server port
  mode is either tcp or udp
```

```cmd
ipkcpc.exe localhost 16 TcP
ERROR: Client cannot start with provided arguments
```

```cmd
ipkcpc.exe localhost 80 udp
(+ 1 2)
ERROR: Couldn't receive data
```

---

Při vývoji jsem následoval metodologii Trunk Based Development - tedy pushoval do master větve po malých, otestovaných částech kódu. Pro tento typ projektu mi přijde vhodnější jak GitFlow, vyvaruji se merge hell, navíc QE i vývoj dělá stejný člověk.

---
[1] Mingw - Minimalist GNU for Windows. In SourceForge [online]. 2023. [cit. 2023-03-21]. Dostupné z: <https://sourceforge.net/projects/mingw/>

[2] RYŠAVÝ, O., 2015. Stubs. In git [online]. [cit. 2023-03-21]. Dostupné z: <https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs>

[3] Getopt(3) - linux manual page. In man7 [online]. [cit. 2023-03-21]. Dostupné z: <https://man7.org/linux/man-pages/man3/getopt.3.html>

[4] EDDY, W. RFC FT-IETF-TCPM-rfc793bis: Transmission control protocol (TCP). In IETF Datatracker [online]. 2022. [cit. 2023-03-21]. Dostupné z: <https://datatracker.ietf.org/doc/html/rfc9293>

[5] Initializing winsock - win32 apps. In Win32 apps | Microsoft Learn [online]. [cit. 2023-03-21]. Dostupné z: <https://learn.microsoft.com/en-us/windows/win32/winsock/initializing-winsock>

[6] Types of sockets supported by TCP/IP. In IBM [online]. [cit. 2023-03-21]. Dostupné z: <https://www.ibm.com/docs/en/ztpf/1.1.0.15?topic=sockets-types-supported-by-tcpip>

[7] KOUTENSKÝ, M., 2023. Protocol. In git [online]. [cit. 2023-03-21]. Dostupné z: <https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Project%201>

[8] In Closing a socket (the GNU C library). In GNU [online]. [cit. 2023-03-21]. Dostupné z: <https://www.gnu.org/software/libc/manual/html_node/Closing-a-Socket.html>

[9] Wireshark · go deep. In Wireshark [online]. [cit. 2023-03-21]. Dostupné z: <https://www.wireshark.org/>

## Hodnocení

| Část | Body     | Chyby                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      |
|------|----------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 1    | 18/20 | -0,5: teorie/kapitola zcela chybí (očekáván byl popis fungování použitých technologií - TCP, UDP, Sockety, ...)<br>-0,5: testování/chybí explicitní popis jednotlivých testovaných situací, vyhodnocení <br>+0,5: bibliografie/ukázkové použití bibliografických citací - jak formát, tak použití v textu<br>*: git/pěkné využití sémantických commit popisků, relevantní popis obsahu, repozitář neobsahuje nesmysly, někdy by se ovšem hodily další podrobnosti k obsahu commitu (nejen "nadpis")<br>+1: kód/podpora pro Windows |
