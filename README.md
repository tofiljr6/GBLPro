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

