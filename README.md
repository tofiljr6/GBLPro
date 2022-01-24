# <img width="20px;" src="./site/assets/logo3transparentbg.png"> GBLPro

GBLPro to zaawansowany technologiczne kompilator języka stworzonego przez dr Macieja Gębala, zaimplementowany na przedmiot **Języki Formalne i Teoria Translacji** na kierunku Informatyka Algorytmiczna na wydziale Informatyki i Telekomunikacji.

# Uruchomienie

W celu uruchomienia wpisz w terminalu:

`make`

a następnie:

`./kompilator <plik wejściowy> <plik wyjściowy>`

# Opis plików

- `code.cpp` i `code.hpp` - klasa generująca kod w asemblerze
- `data.cpp` i `data.hpp` - klasa służąca do zarządzania tablicą symboli
- `symbol.hpp` - definicja struktury przechowującej symbol
- `labels.hpp` - definicje etykiet dla instrukcji warunkowych i pętli
- `lexer.l` - lekser języka wejściowego
- `parser.ypp` - parser języka wejściowego

# Struktura języka

## Deklaracja zmiennych

Na początku programów, musimy napisać słowo kluczowe **VAR**, po nim wypisujemy zmienne jakie chcemy zadeklarować, oddzielone przecinkiem. Po skończeniu deklarowania zmienncyh piszemy **BEGIN**. 

Wszystkie zmienne są typu `long long`. Nie ma możliwości zadeklarowania jakiegoś innego typu.

```
VAR
    a, b, c, tab[-5:5]
BEGIN
    ...
```

Nazwy zmiennych mogą składać się wyłącznie z małych liter i nie mogą zawierać cyfr.

Możemy deklarować tablice. Po nazwie tablicy, w nawiasach kwadratowych piszemy początek i koniec przedziału. UWAGA! Możemy indeksować tablicę od ujemnych liczb. (patrz. na przykład powyżej).

## Ciało programu

Po zadeklarowaniu zmiennych jakie potrzebujemy, ciało programu znajduje się pomiędzy słowami kluczowymi **BEGIN** i **END**

```
VAR
    ...
BEGIN
    ...
END
```

## Przypisanie wartości zmiennym - ASSIGN

Aby przypisać wartość do zmiennej należy w ciele programu użyć słowa kluczowego **ASSIGN**.

```
VAR
    i, j, tab[-5:5]
BEGIN
    i ASSIGN 5;
    j ASSIGN -128;
    tab[-5] ASSIGN i;
    tab[i] ASSIGN tab[-5];
    ...
END
```

## Operacje na zmiennych

Działania arytmetyczne są wykonywane na liczbach całkowitych, ponadto dzielenie przez zero daje wynik 0 i resztę z dzielenia także 0.

**PLUS, MINUS, TIMES, DIV, MOD** oznaczają odpowiednio dodawanie, odejmowanie, mnożenie, dzielenie całkowito liczbowe i obliczanie reszty na liczbach całkowitych, ***reszta z dzielenia ma ten sam znak jak dzielnik***

```
VAR
    i, j, tab[-5:5]
BEGIN
    i ASSIGN 5 PLUS 9;
    j ASSIGN -128 MINUS i;
    tab[-5] ASSIGN i DIV 2;
    tab[i] ASSIGN tab[-5] TIMES tab[-5];
    ...
END
```

## Instrukcje warunkowa - IF oraz IF ELSE

Do dyspozycji mamy dwie bardzo popualrne funckje warunkowe. Działają dokłądnie tak samo jak w inncyh językach programowania.

```
    ...
BEGIN
    ...
    IF 5 GE 3 THEN
        ...
    ENDIF
    ...
    IF i LEQ arr[j] THEN
        ...
    ELSE
        ...
    ENDIF
    ...
END
```

## Instrukcje porównania

**EQ, NEQ, LE, GE, LEQ, GEQ** oznaczają odpowiednio relację, równa się, nie równa się, mniejsze, większe, mniejsze lub równe, większę lub równe.

## ( komentarze )

W programie można używać kometarzy, które znajdują sie w okrągłych nawiasach. Komentarze mogą być wielolinijkowe, ale nie mogą być zagnieżdzane.

Białe znaki w kodzie są pomijane.
```
    ...
BEGIN
    ...
    IF 5 GE 3 THEN ( jezelu 5 jest większe od 3 )
        ...
        
        (
            to sie nie wykona
            i ASSIGN 5;
            
        )
        
        ...
    ENDIF
    ...
END
```

## Pętla FOR

Pętlą **FOR** ma iterator lokalny, przyjmujący wartości od wartości stojącej po **FROM** do wartości stojącej po **TO** kolejno w ostępie +1 lub w odstępie -1 jeżli użyto słowa **DOWNTO**. Iterator lokalny nie deklarujemy w sekcji zmiennych.

Liczba iteracji pętli **FOR** jest ustalana na początku i nie podlega zmianie w trakcie wykonywania pętli (nawet jeśli zmieniają się wartości zmienncyh wyznaczającyh początek i koniec pętli).

Iterator pętli **FOR** nie może być modyfikowany wewnątrz pętli. Kompilaot w takim przypadku zgładza błąd.

```
    ...
BEGIN
    ...
    FOR i FROM 1 TO 5 DO
        WRITE i;
        ( i ASSIGN 10; to jest nie dozwolone !!! )
        a ASSIGN i + 5;
        WRITE a;
    ENDFOR
    ...
END
```

## Pętla WHILE

Pętla **WHILE** działa tak samo jak w inncyh językach programowania. Wykonuje sie dopóki jest spełniony warunek po słowe **WHILE**.

```
    ...
BEGIN
    ...
    WHILE i LE 5 DO
        WRITE i;
        i ASSIGN i PLUS 2;
    ENDWHILE
    ...
END
```

## Pętla REPEAT - UNTIL 

Pętla wykonuje się przynajmniej jeden raz, a potem tyle razy dopóki spełniony jest warunek po słowei **UNTIL**;

```
    ...
BEGIN
    ...
    i ASSIGN 0;
    REPEAT
        WRITE i;
        i ASSIGN i PLUS 1;
    UNTIL i LE 5;
    ...
END
```

## Instrukcja READ

Czyta wartość z linii komend (standardowe wejście) i przypisuje ją do zmiennej

```
    ...
BEGIN
    ...
    READ i;
    READ arr[4];
    ...
END
```

## Instrukcja WRITE

Wypisuje wartość na standardowym wyjściu.

```
    ...
BEGIN
    ...
    WRITE i;
    WRITE arr[4];
    WRITE i PLUS 5; (to nie zadziała.)
    ...
END
```

### TIMES, DIV - optymalizacje

Operacje te działają w czasie logarytmicznym.
