# Trasování systému

Ďalšie požiadavky:

- Váš nástroj vo výslednej forme by mal byť program alebo script (ďalej budem používať "script") spustitelný z príkazovej riadky (s root právami).
Minimálny vstup, ktorý váš script musí spracovať bude reťazec (string) reprezentujúci meno procesu. (vo zvyšku zadania ho budem referencovať ako "target_process")
- Akékoľvek ďaľšie vstupy, prípadne návod pre obsluhu vášho scriptu adekvátne zdokumentujte a priložte, prípadne vypíšte pseudo-manpage cez prepínač '-h'.
- Váš script bude monitorovať všetky procesy s názvom (execname) "target_process" stringu.
Váš script musí zbierať a vypisovať tieto informácie:

- Vypíšte vždy keď niektorý target_process vytvorí nový process (explicitne: systémové volania fork(), vfork() a clone()).
- Vypíšte vždy keď nejaký process spustí program s target_process v mene súboru (explicitne: systémové volania execve() a execveat()).
- Štatistické informácie ako dlho každý target_process (individuálne) reálne bežal (actively running / executing) na každom CPU (individuálne).
- Štatistické informácie ako dlho bol každý target_process (individuálne) na každom CPU (individuálne) prerušený vysoko-rezolučnými časovačmi/prerušeniami ("hrtimer_interrupt").
- Počas prerušenia bude process považovaný za "nebežiaci", tzn. čas prerušení sa logicky nepočíta ako run-time.
HINT: Nižšie, za príkladom výstupu, si prečítajte jeden zo spôsobov ako sa tieto veci dajú merať.

Ďalšie požiadavky na výpis štatistík:

- Predstavte si tabuľky PIDxCPU s bunkami obsahujúcimi časovú hodnotu pre "running" a "interrupted".
- Štatistiku monitorovania run-time a prerušení vypíšte každých 5 sekúnd, prípadne aj pri ukončení scriptu.
- Údaje vypíšte v celočíselných percentách, relatívne ku meranému úseku (100% = plných 5 sekúnd run-time). POZN: Stačí celočíselná aritmetika.
- Časová presnosť (granularita) údajov stačí v mikrosekundách [us].
- Ak zaznamenaný čas prerušení nepresiahne aspoň 1% (čož bude vačšinou), údaj vypíšte v celých mikrosekundách.
Konkrétny formát výstupu je čiste na vás, ale musia z neho byť ihneď zrejmé všetky vyžadované informácie.

## Hodnocení

| Část | Body  | Chyby                                                                                                                                                                                                                                                                                                                                                                                           |
|------|-------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 1    | 14/15 | Nezachytava “execveat()” syscall - bohuzial ‘kprocess.exec’ hookuje iba execve syscall (viz. /usr/share/systemtap/tapset/kprocess.stp) ... Neprakticky necistis pole procesov - predstav si ze by tvoj skript bezal niekolko dni a medzi tym by monitoroval priebezny 10000-ce PID - to vsetko ti tam ostava v poliach cez ktore iterujes. Lepsie je polia pri ypise cistit pomocou `delete` ;) |
