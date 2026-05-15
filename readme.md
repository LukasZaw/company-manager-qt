# Aplikacja do zarządzania firmą (Qt / C++)

## Opis projektu

Desktopowa aplikacja napisana w C++ z wykorzystaniem frameworka Qt, służąca do kompleksowego zarządzania firmą. System umożliwia zarządzanie pracownikami, produktami oraz magazynem, a także generowanie raportów i zestawień.

# Główne funkcjonalności

## 1. Zarządzanie pracownikami

### Funkcje

- [x] Dodawanie pracownika
- [x] Edytowanie danych pracownika
- [x] Usuwanie pracownika
- [x] Przeglądanie listy pracowników
- [x] Wyszukiwanie pracowników
- [x] Filtrowanie pracowników
- [x] Przypisywanie pracowników do działów

### Dane pracownika

- ID
- Imię
- Nazwisko
- Email
- Telefon
- Stanowisko
- Dział
- Data zatrudnienia
- Status (aktywny / nieaktywny)
- Notatki

## 2. Zarządzanie produktami

### Funkcje

- [x] Dodawanie produktu
- [x] Edytowanie produktu
- [x] Usuwanie produktu
- [x] Lista produktów
- [x] Wyszukiwanie produktów
- [x] Filtrowanie produktów
- [ ] Przypisywanie produktu do lokalizacji
- [ ] Śledzenie stanu magazynowego

### Dane produktu

- ID
- Nazwa produktu
- Kod produktu (SKU)
- Kategoria
- Cena
- Ilość
- Jednostka
- Lokalizacja
- Opis

## 3. Zarządzanie magazynem

### Funkcje

- [x] Rejestrowanie przyjęć produktów
- [x] Rejestrowanie wydań produktów
- [x] Historia operacji magazynowych
- [x] Aktualizacja stanów magazynowych
- [ ] Przenoszenie produktów między lokalizacjami

### Operacje magazynowe

- Przyjęcie towaru
- Wydanie towaru
- Przesunięcie międzymagazynowe
- Korekta stanu

### Dane operacji

- ID operacji
- Typ operacji
- Produkt
- Ilość
- Data
- Pracownik
- Lokalizacja źródłowa
- Lokalizacja docelowa
- Uwagi

## 4. Raporty i zestawienia

### Raporty magazynowe

- Aktualny stan magazynu
- Lista produktów poniżej minimalnego stanu
- Historia przyjęć
- Historia wydań
- Ruch magazynowy

### Raporty pracowników

- Lista pracowników
- Pracownicy według działów
- Przypisanie do zadań
- Aktywność pracowników

### Możliwości eksportu

- Export do CSV
- Export do JSON

# Architektura aplikacji

Aplikacja oparta o architekturę MVC:

- Model — dane (pracownicy, produkty, magazyn)
- View — interfejs Qt (QWidget / QMainWindow)
- Controller — logika aplikacji

# Baza danych

SQLite (Qt SQL)

# Interfejs użytkownika

## Główne okno

- Menu górne
- Pasek nawigacji
- Obszar roboczy
- Status bar

## Zakładki

- Pracownicy
- Produkty
- Magazyn
- Raporty

# Technologie

- C++
- Qt 5
- Qt Widgets
- Qt Designer
- Qt SQL
- SQLite
- Model/View (QTableView)

# Przyszłe rozszerzenia

- Logowanie użytkowników
- Role użytkowników
- Uprawnienia
- Wiele magazynów
- Wykresy statystyk
- Dashboard
- Powiadomienia

# Wymagania

- Qt 5.15+
- Kompilator C++17
- CMake lub qmake

# Budowanie projektu (CMake)

```
mkdir build
cd build
cmake ..
make
```

---

# Autor

Projekt edukacyjny – github.com/lukaszaw
