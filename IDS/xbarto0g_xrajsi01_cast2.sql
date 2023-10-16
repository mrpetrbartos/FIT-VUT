DROP TABLE utulek CASCADE CONSTRAINTS;
DROP TABLE veterinar CASCADE CONSTRAINTS;
DROP TABLE osetrovatel CASCADE CONSTRAINTS;
DROP TABLE pes CASCADE CONSTRAINTS;
DROP TABLE lecebny_plan CASCADE CONSTRAINTS;
DROP TABLE lek CASCADE CONSTRAINTS;

DROP TABLE pobyt;
DROP TABLE pracovni_pomer;
DROP TABLE vysetreni;
DROP TABLE lek_v_planu;


CREATE TABLE utulek
(
    registracni_cislo NUMBER PRIMARY KEY,
    nazev             VARCHAR2(255) NOT NULL,
    mesto             VARCHAR2(255) NOT NULL,
    ulice             VARCHAR2(255) NOT NULL,
    telefon           VARCHAR2(255) NOT NULL,
    CHECK (REGEXP_LIKE(telefon, '^(\+420)? ?[1-9][0-9]{2} ?[0-9]{3} ?[0-9]{3}$')),
    email             VARCHAR2(255) DEFAULT NULL,
    CHECK (REGEXP_LIKE(email, '^\w+([.-]?\w+)*@\w+([.-]?\w+)*(\.\w{2,3})+$'))
);

-- pro transformaci generalizace/specializace jsme zvolili prevod na tabulky pro podtypy s ponechanim atributu nadtypu
-- jelikoz musi mit kazdy veterinar take sve unikatni registracni cislo, nemohli jsme zvolit moznost slouceni
-- do jedne tabulky, jeslikoz by to vyzadovalo nastaveni daneho pole na volitelne a nebylo by mozne nastavit constraint unique
CREATE TABLE veterinar
(
    id                NUMBER GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    jmeno             VARCHAR2(255)    NOT NULL,
    prijmeni          VARCHAR2(255)    NOT NULL,
    telefon           VARCHAR2(255)    NOT NULL,
    CHECK (REGEXP_LIKE(telefon, '^(\+420)? ?[1-9][0-9]{2} ?[0-9]{3} ?[0-9]{3}$')),
    email             VARCHAR2(255) DEFAULT NULL,
    CHECK (REGEXP_LIKE(email, '^\w+([.-]?\w+)*@\w+([.-]?\w+)*(\.\w{2,3})+$')),
    registracni_cislo NUMBER(6) UNIQUE NOT NULL
);

CREATE TABLE osetrovatel
(
    id                 NUMBER GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    utulek_id          NUMBER        NOT NULL,
    jmeno              VARCHAR2(255) NOT NULL,
    prijmeni           VARCHAR2(255) NOT NULL,
    telefon            VARCHAR2(255) NOT NULL,
    CHECK (REGEXP_LIKE(telefon, '^(\+420)? ?[1-9][0-9]{2} ?[0-9]{3} ?[0-9]{3}$')),
    email              VARCHAR2(255) DEFAULT NULL,
    CHECK (REGEXP_LIKE(email, '^\w+([.-]?\w+)*@\w+([.-]?\w+)*(\.\w{2,3})+$')),
    napln_prace        VARCHAR2(255) DEFAULT NULL,
    hodinova_sazba     NUMBER        NOT NULL,
    odpracovane_hodiny NUMBER        DEFAULT 0,
    CHECK (odpracovane_hodiny >= 0),
    CONSTRAINT osetrovatel_utulek_fk
        FOREIGN KEY (utulek_id) REFERENCES utulek (registracni_cislo)
            ON DELETE CASCADE
);

CREATE TABLE pes
(
    cislo_cipu NUMBER PRIMARY KEY,
    jmeno      VARCHAR2(255) NOT NULL,
    plemeno    VARCHAR2(255) NOT NULL,
    vek        NUMBER DEFAULT NULL,
    CHECK (vek >= 0),
    velikost   VARCHAR2(255) NOT NULL,
    CHECK (velikost IN ('maly', 'stredni', 'velky')),
    pohlavi    VARCHAR2(10)  NOT NULL,
    CHECK (pohlavi IN ('pes', 'fena'))
);

CREATE TABLE lecebny_plan
(
    id      NUMBER GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    pes_id  NUMBER NOT NULL,
    zacatek DATE   NOT NULL,
    konec   DATE DEFAULT NULL,
    popis   CLOB   NOT NULL,
    CONSTRAINT plan_pes_fk
        FOREIGN KEY (pes_id) REFERENCES pes (cislo_cipu)
            ON DELETE CASCADE
);

CREATE TABLE lek
(
    kod_leku       VARCHAR2(255) PRIMARY KEY,
    CHECK (REGEXP_LIKE(kod_leku, '^[A-Z]{2}\d{6}$')),
    nazev          VARCHAR2(255) NOT NULL,
    kusu_na_sklade NUMBER        NOT NULL
);

CREATE TABLE pobyt
(
    id        NUMBER GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    pes_id    NUMBER NOT NULL,
    utulek_id NUMBER NOT NULL,
    zacatek   DATE   NOT NULL,
    konec     DATE DEFAULT NULL,
    CONSTRAINT pobyt_pes_fk
        FOREIGN KEY (pes_id) REFERENCES pes (cislo_cipu)
            ON DELETE CASCADE,
    CONSTRAINT pobyt_utulek_fk
        FOREIGN KEY (utulek_id) REFERENCES utulek (registracni_cislo)
            ON DELETE CASCADE
);

CREATE TABLE pracovni_pomer
(
    id                NUMBER GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    veterinar_id      NUMBER NOT NULL,
    utulek_id         NUMBER NOT NULL,
    vznik             DATE   NOT NULL,
    pausalni_poplatek NUMBER NOT NULL,
    CONSTRAINT pomer_veterinar_fk
        FOREIGN KEY (veterinar_id) REFERENCES veterinar (id)
            ON DELETE CASCADE,
    CONSTRAINT pomer_utulek_fk
        FOREIGN KEY (utulek_id) REFERENCES utulek (registracni_cislo)
            ON DELETE CASCADE
);

CREATE TABLE vysetreni
(
    id           NUMBER GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    veterinar_id NUMBER NOT NULL,
    pes_id       NUMBER NOT NULL,
    typ          VARCHAR2(255) DEFAULT NULL,
    diagnoza     CLOB   NOT NULL,
    CONSTRAINT vysetreni_veterinar_fk
        FOREIGN KEY (veterinar_id) REFERENCES veterinar (id)
            ON DELETE CASCADE,
    CONSTRAINT vysetreni_pes_fk
        FOREIGN KEY (pes_id) REFERENCES pes (cislo_cipu)
            ON DELETE CASCADE
);

CREATE TABLE lek_v_planu
(
    id        NUMBER GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    plan_id   NUMBER        NOT NULL,
    lek_id    VARCHAR2(255) NOT NULL,
    davkovani VARCHAR2(255) NOT NULL,
    vysazeni  DATE DEFAULT NULL,
    CONSTRAINT lp_plan_fk
        FOREIGN KEY (plan_id) REFERENCES lecebny_plan (id)
            ON DELETE CASCADE,
    CONSTRAINT lp_lek_fk
        FOREIGN KEY (lek_id) REFERENCES lek (kod_leku)
            ON DELETE CASCADE
);


INSERT INTO utulek (registracni_cislo, nazev, mesto, ulice, email, telefon)
VALUES ('66522173', 'Psí útulek Bouchalka', 'Ostrava', 'Masarykova 654', 'bouchalka@seznam.cz', '+420419358107');
INSERT INTO utulek (registracni_cislo, nazev, mesto, ulice, email, telefon)
VALUES (23706273, 'Voříškov', 'Praha', 'Grohova 45', 'voriskov@seznam.cz', '550517628');
INSERT INTO utulek (registracni_cislo, nazev, mesto, ulice, email, telefon)
VALUES (12923045, 'Psí sen', 'Olomouc', 'Grohova 12', 'psisen@centrum.cz', '+420 863 542 283');
INSERT INTO utulek (registracni_cislo, nazev, mesto, ulice, email, telefon)
VALUES (43299683, 'Psi k osvojení', 'Brno', 'Brněnská 13', 'psikosvojeni@seznam.cz', '685654222');
INSERT INTO utulek (registracni_cislo, nazev, mesto, ulice, email, telefon)
VALUES (68284304, 'Útulek pro pejsky', 'Olomouc', 'Brněnská 49', 'utulekpropejsky@centrum.cz', '578147289');

INSERT INTO veterinar (jmeno, prijmeni, telefon, email, registracni_cislo)
VALUES ('Matej', 'Rusinák', '419358108', 'ruskygamin@seznam.cz', 507038);
INSERT INTO veterinar (jmeno, prijmeni, telefon, registracni_cislo)
VALUES ('Josef', 'Novák', '550517621', 625353);
INSERT INTO veterinar (jmeno, prijmeni, telefon, registracni_cislo)
VALUES ('Štěpán', 'Uhlíř', '+420 863 542 288', 155658);
INSERT INTO veterinar (jmeno, prijmeni, telefon, registracni_cislo)
VALUES ('Karel', 'Vémola', '685654229', 979106);
INSERT INTO veterinar (jmeno, prijmeni, telefon, registracni_cislo)
VALUES ('Lukáš', 'Herberk', '578147281', 463031);

INSERT INTO osetrovatel (utulek_id, jmeno, prijmeni, telefon, hodinova_sazba, odpracovane_hodiny)
VALUES (66522173, 'Karolína', 'Modrá', '437318730', 269, 25);
INSERT INTO osetrovatel (utulek_id, jmeno, prijmeni, telefon, hodinova_sazba, odpracovane_hodiny)
VALUES (23706273, 'Josef', 'Krátký', '751777752', 332, 92);
INSERT INTO osetrovatel (utulek_id, jmeno, prijmeni, telefon, hodinova_sazba, odpracovane_hodiny)
VALUES (12923045, 'Adéla', 'Dlouhá', '+420 753 137 330', 343, 40);
INSERT INTO osetrovatel (utulek_id, jmeno, prijmeni, telefon, hodinova_sazba, odpracovane_hodiny)
VALUES (43299683, 'Kristýna', 'Železná', '+420 128 521 811', 206, 5);
INSERT INTO osetrovatel (utulek_id, jmeno, prijmeni, telefon, hodinova_sazba, odpracovane_hodiny)
VALUES (68284304, 'Pavel', 'Krakonoš', '+420 630 685 452', 232, 87);

INSERT INTO pes (jmeno, cislo_cipu, vek, plemeno, velikost, pohlavi)
VALUES ('Čiko', 1892908481, 5, 'Kokršpaněl', 'maly', 'pes');
INSERT INTO pes (jmeno, cislo_cipu, vek, plemeno, velikost, pohlavi)
VALUES ('Ringo', 9928015680, 5, 'Labrador', 'stredni', 'fena');
INSERT INTO pes (jmeno, cislo_cipu, vek, plemeno, velikost, pohlavi)
VALUES ('Bagel', 2268360587, 2, 'Pitbull', 'stredni', 'pes');
INSERT INTO pes (jmeno, cislo_cipu, vek, plemeno, velikost, pohlavi)
VALUES ('Apolo', 8734903955, 10, 'Německý ovčák', 'maly', 'pes');
INSERT INTO pes (jmeno, cislo_cipu, vek, plemeno, velikost, pohlavi)
VALUES ('Baddy', 5297564575, 12, 'Labrador', 'velky', 'fena');

INSERT INTO lecebny_plan (pes_id, zacatek, konec, popis)
VALUES (2268360587, TO_DATE('18-10-2020', 'DD-MM-YYYY'), TO_DATE('12-10-2022', 'DD-MM-YYYY'),
        'Pes bude léčen Aminophyllinem po dobu 3 měsíců, kde po každém měsíci bude absolvovat kontrolu');
INSERT INTO lecebny_plan (pes_id, zacatek, popis)
VALUES (8734903955, TO_DATE('25-02-2022', 'DD-MM-YYYY'),
        'Pes bude léčen Gabapentin po dobu 1 roku, žádné častější kontroly nejsou vyžadovány');

INSERT INTO lek (kod_leku, nazev, kusu_na_sklade)
VALUES ('AB664461', 'Carprofen', 465);
INSERT INTO lek (kod_leku, nazev, kusu_na_sklade)
VALUES ('GS781645', 'Gabapentin', 56);
INSERT INTO lek (kod_leku, nazev, kusu_na_sklade)
VALUES ('LV197462', 'Nitroxynil', 162);
INSERT INTO lek (kod_leku, nazev, kusu_na_sklade)
VALUES ('OK646134', 'Aminophylline', 0);
INSERT INTO lek (kod_leku, nazev, kusu_na_sklade)
VALUES ('XD645312', 'Trimethoprim', 16);

INSERT INTO pobyt (pes_id, utulek_id, zacatek)
VALUES (1892908481, 66522173, TO_DATE('13-02-2013', 'DD-MM-YYYY'));
INSERT INTO pobyt (pes_id, utulek_id, zacatek)
VALUES (9928015680, 23706273, TO_DATE('23-11-2011', 'DD-MM-YYYY'));
INSERT INTO pobyt (pes_id, utulek_id, zacatek)
VALUES (2268360587, 12923045, TO_DATE('12-05-2022', 'DD-MM-YYYY'));
INSERT INTO pobyt (pes_id, utulek_id, zacatek)
VALUES (8734903955, 43299683, TO_DATE('05-08-2019', 'DD-MM-YYYY'));
INSERT INTO pobyt (pes_id, utulek_id, zacatek)
VALUES (5297564575, 68284304, TO_DATE('17-10-2017', 'DD-MM-YYYY'));

INSERT INTO pracovni_pomer (veterinar_id, utulek_id, vznik, pausalni_poplatek)
VALUES (1, 66522173, TO_DATE('15-09-2019', 'DD-MM-YYYY'), 27500);
INSERT INTO pracovni_pomer (veterinar_id, utulek_id, vznik, pausalni_poplatek)
VALUES (2, 23706273, TO_DATE('10-02-2020', 'DD-MM-YYYY'), 22500);
INSERT INTO pracovni_pomer (veterinar_id, utulek_id, vznik, pausalni_poplatek)
VALUES (3, 12923045, TO_DATE('01-02-2019', 'DD-MM-YYYY'), 18000);
INSERT INTO pracovni_pomer (veterinar_id, utulek_id, vznik, pausalni_poplatek)
VALUES (4, 43299683, TO_DATE('27-11-2021', 'DD-MM-YYYY'), 29250);
INSERT INTO pracovni_pomer (veterinar_id, utulek_id, vznik, pausalni_poplatek)
VALUES (5, 68284304, TO_DATE('11-06-2019', 'DD-MM-YYYY'), 20000);

INSERT INTO vysetreni (veterinar_id, pes_id, typ, diagnoza)
VALUES (3, 1892908481, 'vstupní', 'Zánět hltanu a hrtanu, zahlenění, kašel, zvýšená teplota');
INSERT INTO vysetreni (veterinar_id, pes_id, typ, diagnoza)
VALUES (1, 9928015680, 'preventivní', 'Kožní psinka, zvýšená teplota, objevuje se nechutenství a skleslost');
INSERT INTO vysetreni (veterinar_id, pes_id, typ, diagnoza)
VALUES (4, 2268360587, 'preventivní', 'Nebyly nalezeny žádne nemoci, pes je zdravý');

INSERT INTO lek_v_planu (plan_id, lek_id, davkovani)
VALUES (1, 'OK646134', '1-0-1');
INSERT INTO lek_v_planu (plan_id, lek_id, davkovani)
VALUES (2, 'GS781645', '1-1-1');
