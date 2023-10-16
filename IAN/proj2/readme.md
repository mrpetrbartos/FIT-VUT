# Základní analýza crash dump

Vašou úlohou je za pomoci programu crash zistiť a následne popísať stav systému v čase panic - čo systém v danom momente robil a prečo kernel zpanikáril.

## Hodnocení

| Část | Body  | Chyby                                                                                                                                                                                                                                                                            |
|------|-------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 1    | 15/15 | Tvrdenie ze “nebolo mozne ukoncit ziadny proces” nie je pravdive. K nasilnemu ukoncenie procesu sa nedostalo lebo bol povoleny panic_on_oom, takze sa nepustal OOM-killer, ktory by inak nejaky process zabil. Chapem ale ze mozno islo len o nedokonalu formulaciu tvrdenia. :) |
