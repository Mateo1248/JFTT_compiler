Instrukcja obsługi kompilatora prostego języka imperatywnego napisanego na laboratoria z kursu Języki Formalne i Techniki Translacji.

Autor: Mateusz Kościelniak
Nr indeksu: 244973

Pliki:
Makefile - plik służący do kompilacji projektu,
parser.y - plik BISONa,
compiler.cpp - plik zawierający główne funkcje kompilatora, 
lexer.l— plik FLEXa.

Użyte narzędzia:
gcc 7.4.0
g++ 7.4.0
flex 2.6.4
bison (GNU Bison) 3.0.4

Sposób użycia:
W celu skompilowania programu należy uzyć polecenia make w folderze projektu, program wyjsciowy pojawi się pod nazwą compiler.

Uruchamianie programu:
Kompilator uruchamia się komendą ./compiler po czym trzeba podać 2 parametry, pierwszym jest kod wejściowy, drugim natomiast kod wyjściowy, 2 parametr jest opcjonalny, w przypadku jego braku kod asemblera wyświetli się na standardowe wyjście.

Aby odczytać kod z plik test.txt i zapisać skompilowany kod do pliku out.txt należy posłużyć sie poleceniem:
./compiler test.txt out.txt
