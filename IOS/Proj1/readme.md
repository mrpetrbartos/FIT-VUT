# DU1
Shellový skript pro analýzu záznamů osob s prokázanou nákazou koronavirem způsobujícím onemocnění COVID-19 na území České republiky. Skript bude filtrovat záznamy a poskytovat základní statistiky podle zadání uživatele.
## Použití
`corona [-h] [FILTERS] [COMMAND] [LOG [LOG2 [...]]`

COMMAND může být jeden z:
- infected — spočítá počet nakažených.
- merge — sloučí několik souborů se záznamy do jednoho, zachovávající původní pořadí (hlavička bude ve výstupu jen jednou).
- gender — vypíše počet nakažených pro jednotlivá pohlaví.
- age — vypíše statistiku počtu nakažených osob dle věku (bližší popis je níže).
- daily — vypíše statistiku nakažených osob pro jednotlivé dny.
- monthly — vypíše statistiku nakažených osob pro jednotlivé měsíce.
- yearly — vypíše statistiku nakažených osob pro jednotlivé roky.
- countries — vypíše statistiku nakažených osob pro jednotlivé země nákazy (bez ČR, tj. kódu CZ).
- districts — vypíše statistiku nakažených osob pro jednotlivé okresy.
- regions — vypíše statistiku nakažených osob pro jednotlivé kraje.

FILTERS může být kombinace následujících (každý maximálně jednou):
- -a DATETIME — after: jsou uvažovány pouze záznamy PO tomto datu (včetně tohoto data). DATETIME je formátu YYYY-MM-DD.
- -b DATETIME — before: jsou uvažovány pouze záznamy PŘED tímto datem (včetně tohoto data).
- -g GENDER — jsou uvažovány pouze záznamy nakažených osob daného pohlaví. GENDER může být M (muži) nebo Z (ženy).
- -s [WIDTH] u příkazů gender, age, daily, monthly, yearly, countries, districts a regions vypisuje data ne číselně, ale graficky v podobě histogramů. Nepovinný parametr WIDTH nastavuje šířku histogramů, tedy délku nejdelšího řádku, na WIDTH. Tedy, WIDTH musí být kladné celé číslo. Pokud není parametr WIDTH uveden, řídí se šířky řádků požadavky uvedenými níže.
- (nepovinný, viz níže) -d DISTRICT_FILE — pro příkaz districts vypisuje místo LAU 1 kódu okresu jeho jméno. Mapování kódů na jména je v souboru DISTRICT_FILE
- (nepovinný, viz níže) -r REGIONS_FILE — pro příkaz regions vypisuje místo NUTS 3 kódu kraje jeho jméno. Mapování kódů na jména je v souboru REGIONS_FILE
- -h — vypíše nápovědu s krátkým popisem každého příkazu a přepínače.