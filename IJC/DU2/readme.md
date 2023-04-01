# DU2

## Zadání

V jazyku C napište program "tail.c", který ze zadaného
vstupního souboru vytiskne posledních 10 řádků. Není-li
zadán vstupní soubor, čte ze stdin.

Přepište C++ program do jazyka ISO C:

- Program počítá četnost slov ve vstupním textu,
  slovo je cokoli oddělené "bílým znakem"

Implementujte tabulku s rozptýlenými položkami (hash table) - viz dále.
Veškeré operace s tabulkou budou v samostatné knihovně (vytvořte statickou
i dynamickou/sdílenou verzi). V knihovně musí být prakticky každá funkce ve
zvláštním modulu -- to například umožní případnou výměnu htab_hash_function() ve vašem
staticky sestaveném programu

## Hodnocení

| Body  | Chyby                                  |
| ----- | -------------------------------------- |
| 10/15 | wordcount-dynamic: chyba pri linkovani |
