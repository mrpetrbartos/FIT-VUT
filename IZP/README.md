# Ověřování síly hesel (práce s textem)

Cílem projektu je vytvořit program, který na vstupu dostane sadu hesel a pro každé z nich ověří, jestli heslo splňuje všechna (pevně zadaná) požadovaná pravidla. Ta hesla, která projdou kontrolou, budou vypisována na výstup, ostatní budou zahozena.

## Syntax spuštění

```
./pwcheck LEVEL PARAM [--stats]
```

LEVEL - celé číslo v intervalu [1, 4], které určuje požadovanou úroveň bezpečnosti PARAM - kladné celé číslo, které určuje dodatečný parametr pravidel (viz níže) --stats - pokud je zadané, určuje, zda se na konci programu mají vypsat souhrnné statistiky analyzovaných hesel

# Hodnocení

| Body    | Chyby                                                                                                                                                                                                                                                                                                                                                                 |
| ------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 8.55/10 | -0.3: nektere nazvy funkci nejsou prilis vhodne </br> -0.1: nektere nazvy promennych nejsou prilis vhodne a na prvni pohled nepomahaji v pochopeni umyslu </br> -0.25: zadne vlastni funkce nemaji popis jejich signatury (co delaji/prijimaji/vraceji) </br> -0.25: i s komentarem jsem se musel na chvilku zamyslet co to dela </br> chybejici newlines u statistik |
