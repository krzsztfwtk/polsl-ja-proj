# polsl-ja-proj

Kod źródłowy, karta projektu, prezentacja i raport📝 z projektu „Obliczanie wartości wielomianu za pomocą algorytmu Hornera”🔢, realizowanego na zajęciach projektowych z Języków Asemblerowych na Informatyce💻 – Semestr 5 na Politechnice Śląskiej🎓.

## Kod źródłowy

### Struktura katalogu `visual_studio_2022`

Zawiera solucję Visual Studio 2022 IDE, która zawiera 4 projekty:

- `App/` – główna aplikacja wykorzystująca algorytm Hornera napisana w C++
- `AsmDll/` – biblioteka DLL w assemblerze
- `CppDll/` – biblioteka DLL w języku C++
- `PythonGUI/` – graficzny interfejs użytkownika napisany w Pythonie

### Uwaga

**Solucja jest w taki sposób skonfigurowana, że kompilacja całości przez `[Crtl+Shift+B]` działa tylko w trybie `release`.**

## Dokumentacja projektu

### [Karta projektu](./docs/karta_projektu.pdf)

Zawiera tematu projektu i krótki opis tematu.

### [Prezentacja](./docs/prezentacja.pdf)

Przedstawia Algorytm Hornera oraz wygląd i funkcje interfejsu użytkownika.

### [Raport](./docs/raport.pdf)

Raport zawiera:

- Opis projektu i zastosowanego algorytmu.
- Pomiary wydajności bibliotek ASM i C++.
- Zrzuty ekranu GUI oraz wyniki działania.
- Wnioski końcowe.

### [Dokumentacja Doxygen](./docs/doxygen_refman.pdf)  

Jest dostępna w repozytorium w formacie PDF. Aby samodzielnie wygenerować dokumentację Doxygen należy wykonać poniższe polecenia w katalogu `visual_studio_2022`.

```bash
doxygen Doxyfile
cd doxygen_docs/latex/
make
```

Po wygenerowaniu dokumentacji będzie ona dostępna w plikach `visual_studio_2022/doxygen_docs/latex/refman.pdf` i `visual_studio_2022/doxygen_docs/html/index.html`.

Wszystkie materiały w folderze `docs/` zawierają kluczowe informacje dotyczące projektu. Jeśli czegoś nie wiesz, to zacznij tam!📂

## Licencja i zasady użytkowania

Udostępnienie projektu ma na celu wspieranie nauki i zdobywania wiedzy, a nie promowanie nieuczciwych działań akademickich.

### Możesz

- Przeglądać i wykorzystywać materiały edukacyjnie.
- Czerpać inspirację do własnych projektów.

### Nie możesz

- Przedstawiać tych materiałów jako własnych prac.
- Rozpowszechniać bez podania autorstwa.
- Składać tych treści jako własne zaliczenia projektowe.
