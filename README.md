# Projekt: Przetwarzanie Obrazów

## Opis Projektu

Celem projektu jest stworzenie programu do przetwarzania obrazów w formacie **PGM**. Program umożliwia wczytywanie obrazów, ich modyfikację (np. obrót, progowanie, dodawanie szumu), zapis zmodyfikowanych obrazów, a także dynamiczne zarządzanie kolekcją obrazów w pamięci.

---

## Funkcjonalności programu:

### 1. Menu użytkownika
   - Interaktywne menu umożliwiające wybór operacji do wykonania, takich jak wczytywanie obrazów, przetwarzanie obrazu, zapis obrazu, przeglądanie listy obrazów itp.

### 2. Funkcja wczytująca plik .PGM
   - Funkcja pyta o nazwę pliku i wczytuje obraz w formacie **PGM** (z obsługą komentarzy w pliku).

### 3. Struktura na obraz i zmienna strukturalna
   - Deklaracja struktury `Obraz`, która zawiera dane dotyczące obrazu, takie jak:
     - Wymiary obrazu (szerokość, wysokość),
     - Maksymalna wartość pikseli,
     - Dwuwymiarowa macierz pikseli.
   - Inicjalizacja zmiennej strukturalnej typu `Obraz` i wypełnienie jej danymi.

### 4. Dynamiczna alokacja dwuwymiarowej macierzy pikseli
   - Funkcja alokująca pamięć dla dwuwymiarowej tablicy pikseli, z obsługą potencjalnych błędów alokacji oraz zabezpieczeniem przed wyciekiem pamięci.

### 5. Funkcja zapisująca plik .PGM
   - Funkcja umożliwia zapis obrazu do pliku **PGM** z zapytaniem o nazwę pliku wyjściowego.

### 6. Dynamiczna tablica (baza) do przechowywania wielu obrazów
   - Program umożliwia dynamiczne zarządzanie wieloma obrazami w pamięci. Funkcjonalności obejmują:
     - **Dodawanie obrazu** do tablicy.
     - **Usuwanie obrazu** z tablicy.
     - **Wyświetlanie listy obrazów** w bazie.
     - **Wybieranie „aktywnego” obrazu**, na którym będą wykonywane operacje (np. przetwarzanie lub zapis do pliku).

### 7. Funkcje przetwarzania obrazów

   - **Obrót o 90·k stopni**:
     - Funkcja obraca obraz o wielokrotność 90 stopni (90, 180, 270, 360).
   
   - **Histogram**:
     - Funkcja generuje histogram obrazu i zapisuje go do pliku **.CSV**.

   - **Przetwarzanie obrazu** (do wyboru jedno):
     - **Progowanie**: Ustawia dowolny próg dla wartości pikseli.
     - **Negatyw**: Tworzy negatyw obrazu, odwracając wartości pikseli.
     - **Odbicie względem osi**: Odbija obraz wzdłuż osi pionowej lub poziomej.

   - **Dodawanie szumu typu „pieprz i sól”**:
     - Funkcja dodaje losowy szum do obrazu, zmieniając losowe piksele na czarne (pieprz) lub białe (sól).

   - **Filtracja obrazu** (do wyboru jedno):
     - **Filtr Gaussa**: Używa filtru Gaussa o oknie 3x3, aby wygładzić obraz.
     - **Filtr medianowy**: Zastosowanie filtru medianowego o oknie 3x3 do redukcji szumu.
