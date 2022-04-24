# Ovìøování síly hesel (práce s textem)
Cílem projektu je vytvoøit program, který na vstupu dostane sadu hesel a pro každé z nich ovìøí, jestli heslo splòuje všechna (pevnì zadaná) požadovaná pravidla. Ta hesla, která projdou kontrolou, budou vypisována na výstup, ostatní budou zahozena.

## Syntax spuštìní
```
./pwcheck LEVEL PARAM [--stats] 
```
LEVEL - celé èíslo v intervalu [1, 4], které urèuje požadovanou úroveò bezpeènosti
PARAM - kladné celé èíslo, které urèuje dodateèný parametr pravidel (viz níže)
--stats - pokud je zadané, urèuje, zda se na konci programu mají vypsat souhrnné statistiky analyzovaných hesel