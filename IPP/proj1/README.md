# Implementační dokumentace k 1. úloze do IPP 2022/2023

Jméno a příjmení: Petr Bartoš

Login: xbarto0g

### Reprezentace instrukcí

Instrukce jsou uloženy v asociativním poli, kde instrukce je klíčem, její parametry
jsou pak uloženy v poli jako hodnota. Tento přístup umožňuje instrukce lehce přidávat
nebo upravovat bez nutnosti větších zásahů do kódu.

### Zpracování jednotlivých řádků

Prázdné řádky a řádky tvořeny pouze komentářem jsou ihned vyfiltrovány. Nejdříve je
pomocí tohoto cyklu kontrolováno, zdali zdrojový kód začíná identifikátorem jazyka.
Neprázdné řádky jsou pak normalizovány - jsou odstraněny sekvence více bílých znaků
a useknuty všechny zakomentované části. Řádek je pak možné rozdělit pomocí mezer na
jednotlivé operandy, které jsou jednotlivě zpracovány.

### Kontroly

Po rozdělení na jednotlivé operandy je nejdříve kontrolováno jméno instrukce. Můžeme
využít toho, jakým způsobem jsou instrukce uloženy, a přes pole iterovat. V případě
nálezu rovnou získáváme informaci o tom, kolik dalších tokenů očekáváme. Další tokeny
jsou pak kontrolovány podle jejich typu - každý typ má vlastní funkci. Z velké míry
jsou tokeny kontrolovány pomocí regulárních výrazů, ty syntakticky jednodušší pak
pomocí jednoduché práce s poli či řetězci.

### Generování výstupního kódu

Výstupní kód je generován za běhu programu. Na začátku běhu je vygenerován kořenový
prvek `program`. Po kontrole správnosti názvu instrukce je vygenerován prvek pro danou
instrukci a jsou mu přiřazeny patřičné atributy. Prvky pro argumenty jsou generovány
ještě před jejich kontrolou, jelikož kontrolní funkce v některých případech vrací
potřebné argumenty a jejich tvorba je pak výrazně zjednodušena. V případě, že běh
programu neskončí chybou, je celá struktura po zformátování vytisknuta na standardní
výstup.

## Hodnocení

| Část | Body  | Chyby                                                                                                                           |
|------|-------|---------------------------------------------------------------------------------------------------------------------------------|
| 1    | 7.7/8 | Lexikální analýza (detekce chyb): 85 %<br>Syntaktická analýza (detekce chyb): 91 %<br>Zpracování instrukcí (včetně chyb): 100 % |
