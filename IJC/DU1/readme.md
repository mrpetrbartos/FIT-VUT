# DU1

A) V rozhraní "bitset.h" definujte pro datovou strukturu typu pole bitù:

   Typ:
     typedef <DOPLNIT> bitset_t;
       Typ bitového pole (pro pøedávání parametru do funkce odkazem).

     typedef unsigned long bitset_index_t;
        Typ indexu do bitového pole.

   Makra:
     bitset_create(jmeno_pole,velikost)
       definuje a _nuluje_ promìnnou jmeno_pole
       (POZOR: opravdu musí _INICIALIZOVAT_ pole bez ohledu na
       to, zda je pole statické nebo automatické/lokální!
       Vyzkoušejte obì varianty, v programu pouijte lokální pole.)
       Pouijte  static_assert  pro kontrolu velikosti pole.
       Pø: static bitset_create(p,100); // p = pole 100 bitù, nulováno
           bitset_create(q,100000L);    // q = pole 100000 bitù, nulováno
           bitset_create(q,-100);       // chyba pøi pøekladu

     bitset_alloc(jmeno_pole,velikost)
       definuje promìnnou jmeno_pole tak, aby byla kompatibilní s polem
       vytvoøenım pomocí bitset_create, ale pole bude alokováno dynamicky.
       Pø: bitset_alloc(q,100000L); // q = pole 100000 bitù, nulováno
       Pouijte  assert  pro kontrolu maximální moné velikosti pole.
       Pokud alokace sele, ukonèete program s chybovım hlášením:
       "bitset_alloc: Chyba alokace pamìti"

     bitset_free(jmeno_pole)
       uvolní pamì dynamicky (bitset_alloc) alokovaného pole

     bitset_size(jmeno_pole)
       vrátí deklarovanou velikost pole v bitech (uloenou v poli)

     bitset_setbit(jmeno_pole,index,vıraz)
       nastaví zadanı bit v poli na hodnotu zadanou vırazem
       (nulovı vıraz == bit 0, nenulovı vıraz == bit 1)
       Pø: bitset_setbit(p,20,1);

     bitset_getbit(jmeno_pole,index)
       získá hodnotu zadaného bitu, vrací hodnotu 0 nebo 1
       Pø: if(bitset_getbit(p,i)==1) printf("1");
           if(!bitset_getbit(p,i))   printf("0");

   Kontrolujte meze polí. V pøípadì chyby volejte funkci

     error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu",
               (unsigned long)index, (unsigned long)mez).

   (Pouijte napøíklad modul error.c/error.h z pøíkladu b)

   Programy musí fungovat na 32 (gcc -m32) i 64bitové platformì.

   Podmínìnım pøekladem zajistìte, aby se pøi definovaném symbolu
   USE_INLINE místo tìchto maker definovaly inline funkce stejného jména
   všude kde je to moné (bez zmìn v následujícím testovacím pøíkladu!).
   Pozor: USE_INLINE nesmí bıt definováno ve zdrojovém textu --
          pøekládá se s argumentem -D (gcc -DUSE_INLINE ...).

   Program musí fungovat s inline funkcemi i pro vypnuté optimalizace -O0
   (ovìøte si to, vyaduje modul s externími definicemi inline funkcí).

   Pro vaši implementaci pouijte pole typu  unsigned long [].
   V tomto poli na indexu 0 bude velikost bitového pole v bitech.
   Implementace musí efektivnì vyuívat pamì (vyuít kadı
   bit pole a na posledních maximálnì CHAR_BIT*sizeof(unsigned long)-1 bitù).


   Jako testovací pøíklad implementujte funkci, která pouije algoritmus známı
   jako Eratostenovo síto (void Eratosthenes(bitset_t pole);) a pouijte ji
   pro vıpoèet posledních 10 prvoèísel ze všech prvoèísel od 2 do
   N=300000000 (300 milionù). (Doporuèuji program nejdøíve odladit pro N=100.)
   Funkci Eratosthenes napište do samostatného modulu "eratosthenes.c".

   Kadé prvoèíslo tisknìte na zvláštní øádek v poøadí
   vzestupném.  Netisknìte  nic  jiného  ne  prvoèísla (bude se
   automaticky  kontrolovat!).  Pro kontrolu správnosti prvoèísel
   mùete pouít program "factor" (./primes|factor).

   Naprogramujte (s vyuitím funkce clock()) mìøení doby bìhu programu v
   sekundách a vısledek vypište na stderr následujícím pøíkazem:
     fprintf(stderr, "Time=%.3g\n", (double)(clock()-start)/CLOCKS_PER_SEC);
   (Porovnejte s vısledkem programu "time ./primes".)

   Pro lokální pole budete potøebovat zvìtšit limit velikosti zásobníku.
   Na Unix-like systémech mùete pouít pøíkaz "ulimit -a" pro zjištìní velikosti
   limitu a potom "ulimit -s zadana_velikost_v_KiB" pøed spuštìním programu.
   (Toto názornì demonstruje nevhodnost pouívání velkıch lokálních polí.)

   Zdrojovı text programu se musí jmenovat "primes.c" !
   Napište Makefile tak, aby pøíkaz "make" vytvoøil všechny varianty:
     primes      pouívá makra
     primes-i    pouívá inline funkce
   a aby pøíkaz "make run" všechny varianty vytvoøil a spustil.

   (Pøi nesplnìní vıše uvedenıch podmínek: a 0 bodù.)

(7b)

Poznámky:  Eratosthenovo síto (pøibliná specifikace):
   1) Nulujeme bitové pole  p  o rozmìru N,
      p[0]=1; p[1]=1; // 0 a 1 nejsou prvoèísla
      index i nastavit na 2
   2) Vybereme nejmenší index i, takovı, e p[i]==0.
      Potom je i prvoèíslo
   3) Pro všechny násobky i nastavíme bit p[n*i] na 1
      ('vyškrtneme' všechny násobky i - nejsou to prvoèísla)
   4) i++; dokud nejsme za sqrt(N), opakujeme bod 2 a 4
      (POZOR: sestavit s matematickou knihovnou parametrem -lm)
   5) Vısledek: v poli p jsou na prvoèíselnıch indexech hodnoty 0

   https://en.wikipedia.org/wiki/Prime_number

   Efektivita vıpoètu: cca 2.1s na Intel i5-4690 @ 3.50GHz (gcc -O2)
   Porovnejte efektivitu obou variant (makra vs. inline funkce).
   Zamyslete se, jak by se ovìøila efektivita pro (neinline) funkce.



B) Napište modul "error.c" s rozhraním v "error.h", kterı definuje
   funkci void warning_msg(const char *fmt, ...) a
   funkci void error_exit(const char *fmt, ...). Tyto funkce mají
   stejné parametry jako printf(); tisknou text "CHYBA: " a potom
   chybové hlášení podle formátu fmt. Vše se tiskne do stderr
   (funkcí vfprintf) a potom pouze error_exit ukonèí program voláním
   funkce exit(1).  Pouijte definice ze stdarg.h.

 * Napište modul "ppm.c" s rozhraním "ppm.h",
   ve kterém definujete typ:

     struct ppm {
        unsigned xsize;
        unsigned ysize;
        char data[];    // RGB bajty, celkem 3*xsize*ysize
     };

   a funkci:

     struct ppm * ppm_read(const char * filename);
        naète obsah PPM souboru do touto funkcí dynamicky
        alokované struktury. Pøi chybì formátu pouije funkci warning_msg
        a vrátí NULL.  Pozor na "memory leaks".

     void ppm_free(struct ppm *p);
        uvolní pamì dynamicky alokovanou v ppm_read

    Mùete doplnit další funkce, ale pro DU1 to není nutné.
    [Zamyslete se nad (ne)vhodností pouití warning_msg() a promyslete
    alternativní zpùsoby hlášení rùznıch chyb.]

    Mùete omezit max. velikost obrazovıch dat vhodnım implementaèním
    limitem (napø 8000*8000*3).

    Popis formátu PPM najdete na Internetu, implementujte pouze
    binární variantu P6 s barvami 0..255 a bez komentáøù:
      "P6" <ws>+
      <xsizetxt> <ws>+ <ysizetxt> <ws>+
      "255" <ws>
      <binární data, 3*xsize*ysize bajtù RGB>
      <EOF>

  * Napište testovací program "steg-decode.c", kde ve funkci main naètete ze
    souboru zadaného jako jedinı argument programu obrázek ve formátu PPM
    a v nìm najdete uloenou "tajnou" zprávu. Zprávu vytisknete na stdout.

    Zpráva je øetìzec znakù (char, vèetnì '\0') uloenı po jednotlivıch bitech
    (poèínaje LSb) na nejniších bitech (LSb) vybranıch bajtù barevnıch sloek
    v datech obrázku. Dekódování ukonèete po dosaení '\0'.
    Pro DU1 budou vybrané bajty urèeny prvoèísly (poèínaje od 29) -- pouijte
    Eratostenovo síto podobnì jako v pøíkladu "primes.c" a zaènìte prvoèíslem 29.
    Velikost bitového pole musí odpovídat velikosti obrazovıch dat RGB.

    Program pouije error_exit v pøípadì chyby ètení souboru (chybnı formát),
    a v pøípadì, e zpráva není korektnì ukonèena '\0'. Pøedpokládejte
    monost uloení textu zprávy ve formátu UTF-8.

    Pouijte program "make" pro pøeklad/sestavení programu.
    Testovací pøíkaz:  ./steg-decode du1-obrazek.ppm

    Zájemci si mohou vytvoøit i program "steg-encode.c" (nehodnotí se).
    Zamyslete se nad (ne)vhodností pouití implementaèních limitù.

(8b)

   Zaøiïte, aby pøíkaz "make" bez parametrù vytvoøil všechny spustitelné
   soubory pro DU1.  Pøi zmìnì kteréhokoli souboru musí pøeloit jen zmìnìnı
   soubor a závislosti. Pokud bude Makefile vypadat jako skript, odeètou se 3b.