# [abstraktus aprašymas]
Kai įjungtas "Send simultaniously" parametras, sistema nelaukia SMS delivery repot pranešimo

* * *
# [detalus aprašymas]
Vadybininkė Diana B. perdavė žinią iš Meksikos kliento Syscom, kuris naudoja ESIM320.

Klientas apibūdino problema taip: viena iš jiems reikalingų ESIM320 funkcijų veikia atvirkščiai. Kai SMS enabled varnelė uždėta - Alert SMS negauna, o kai disabled - SMS gauna.

Patikrinau, klientas teisingai nurodė, kad problema egzistuoja. Tačiau iš tiesų visa įspėjamųjų SMS (Alert SMS) siuntimo logika kiek painesnė. Tad pateikiu jūsų peržiūrai keletą šios funkcijos trūkumų:

Pagrindinis bug'as yra dėl Send Alarm SMS to All Admins simultaneously. Kai ši funkcija yra įjungta, o pas juos būtent ir yra įjungta (Main Settings prie Admin numerių), žinutė yra siunčiama visuomet, nepaisant to, kas nustatyta CT meniu Inputs/Outputs Control skyriuje (SMS enabled/disable su ar be įrašyto teksto).

Man asmeniškai nepavyko atkartoti būtent jų scenarijaus (t.y. kai su SMS siuntimu viskas vyksta atvirkščiai), nors ir buvo momentas, kai atrodė, jog pagavau identišką bug'ą. Tačiau tai buvo tik vienkartinis, atsitiktinis sutapimas.

Dabartinė logika maždaug tokia (kai įjungtas Send Alarm SMS to All Admins simultaneously) :

- SMS enabled ir tekstas įrašytas - siunčia ir veikia gerai.

- SMS disabled ir tekstas įrašytas -siunčia (veikia blogai).

- SMS enabled ir tekstas neįrašytas -įprastai siunčia prieš tai įrašytą tekstą, kartais nesiunčia visai (veikia blogai).

- SMS disabled ir tekstas neįrašytas - dažniausiai siunčia prieš tai įrašytą tekstą, kartais neišsiunčia po pirmo zonos suveikdinimo (veikia blogai).

Galbūt yra ir kitų tolesnių sekų/kombinacijų, bandant įjungti/išjungti SMS ir keisti/trinti tekstą, tačiau negaliu to pagrįsti pats, kadangi testavau "ant stalo", o ne realioje situacijoje, kur pats įrenginys prijungtas prie vartų.

* * *
# [atkartojimo instrukcija]
1. Paleisti CTool
2. Nustatyti "SMS Enabled" ant Zone į "Disabled"
3. Sutrigerinti "Zone 1"
4. Stebėti ar gaunamas SMS. (gaunamas - *NOT PASS*, negaunamas - *PASS*)

* * *
# [panašūs atvejai]
ref [#7422](http://192.168.3.202:70/redmine/issues/7422)

* * *
# [funkcijos specifikacija]
**Nuoroda į specifikaciją:** [ESIM320 Zone notification feature](https://eldesalarms.com/wp-content/uploads/2016/09/ESIM320-user-manual-2016-05-11.pdf)
arba 
ESIM320 turi 5 išorines fizines zonas. Kieviena iš jų turi parametrą "SMS Enabled". Kai "enabled", aliarmo atveju, SMS bus įtrauktas nurodytas Zonos tekstas

