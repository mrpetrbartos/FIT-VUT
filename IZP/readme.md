# Ov��ov�n� s�ly hesel (pr�ce s textem)
C�lem projektu je vytvo�it program, kter� na vstupu dostane sadu hesel a pro ka�d� z nich ov���, jestli heslo spl�uje v�echna (pevn� zadan�) po�adovan� pravidla. Ta hesla, kter� projdou kontrolou, budou vypisov�na na v�stup, ostatn� budou zahozena.

## Syntax spu�t�n�
```
./pwcheck LEVEL PARAM [--stats] 
```
LEVEL - cel� ��slo v intervalu [1, 4], kter� ur�uje po�adovanou �rove� bezpe�nosti
PARAM - kladn� cel� ��slo, kter� ur�uje dodate�n� parametr pravidel (viz n�e)
--stats - pokud je zadan�, ur�uje, zda se na konci programu maj� vypsat souhrnn� statistiky analyzovan�ch hesel