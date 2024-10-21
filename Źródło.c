//trzeba zamienic wys z szer! 
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>	//standardowe wejście/wyjście
#include <stdlib.h>	//standardowe funkcje, rand(), malloc(), realloc(), itd.
#include <string.h> //do napisów char
#include <locale.h>	//do funkcji setlocale()
#include <time.h>	//do funkcji obsługi czasu

struct obraz				//struktura, w tym przypadku jako zmienna globalna (poza main)
{							//jest to tylko deklaracja struktury, nie zmiennej strukturalnej
	char standard[2];		//standard musi być P2
	int szerokosc_obrazu;	//całkowita liczba dodatnia, kolumny
	int wysokosc_obrazu;	//całkowita liczba dodatnia, wiersze
	int glebia_szarosci;	//maksymalna wartość szarości (głębia szarości) – od 1 do 65535
	int** ilosc_pikseli;	//przyjmujące wartość od 0 do maksymalnej wartości szarości
							//wskaźnik na wskaźnik, bo potrzebna nam tablica dwuwymiarowa
	char nazwa_obrazu[20];	//powinno wystarczyć
};

void pobierz(FILE* plik)
{
	char piksel;
	char komentarz[50];				//zakładam, że komentarz może mieć maksymalnie 50 znaków
	do								//wykonuj dopóki nie napotkamy na koniec linii, czy spację, czy tabulator (biały znak)
	{
		fscanf(plik, "%c", &piksel); //%c, bo char
	} while (piksel == 32 || piksel == 10 || piksel == 9);  //aż do napotkania białego znaku: 32 to spacja, 10 koniec linii 9 to tabulacja, || - lub

	if (piksel == '#')				//# - komentarz; obowiązują zawsze do końca linii
	{
		fgets(komentarz, 50, plik); //funkcja fgets odczytuje znaki z pliku dopóki nie odczyta podanej ilości, nie napotka końca linii lub końca pliku; zapisuje znaki do podanej tablicy; przestawia pozycje w pliku za ostatni odczytany znak
		pobierz(plik);				//jeżeli natrafimy na # to czytamy aż do końca linii i to pomijamy, a potem wracamy do początku, czyli dalszej obsługi pliku, zastosowałam rekurencję
	}
	else							//jeśli nie to czytaj wartość kolejnego piksela
		fseek(plik, -1, SEEK_CUR);	//funkcja przesuwa pozycję w pliku o podaną liczbę znaków(int) (-1) względem aktualnego miejsca (SEEK_CUR), czyli o 1 w prawo
}									//po każdym wywolaniu pobierz(plik) przechodzimy do kolejnego znaku w tym pliku, dzięki funkcji fseek i to jest ciągle zapamiętywane

void dodawanie_obrazu(struct obraz* obraz)
{
	FILE* plik;									//utworzenie struktury FILE* (uchwytu, identyfikatora) pliku, do którego będziemy się odwoływać
	do {
		printf("Proszę podać nazwę obrazu, którą chcesz dodać:\n8x8\nangel\nbird\ncells\neuro\nfigury\nflir\ngears\ngradient\nmerc\nplane\robo\nteddy\ntest\nzurawie\nzurawie-mydlane\n\n");
		printf("Chcę dodać: ");
		scanf("%s", obraz->nazwa_obrazu);		//aby przekazać zmienną strukturalną do funkcji gdzie miałaby być zmodyfikowana, należy to zrobić przez wskaźnik

		strcat(obraz->nazwa_obrazu, ".pgm");	//funkcja strcat scala dwa łańcuchy znaków w jeden; w naszym przypadku nazwę obrazu i rozszerzenie .pgm
		plik = fopen(obraz->nazwa_obrazu, "r");	//"r" – otwiera plik do odczytu, nazwa pliku – tablica znaków char*; w tym miejscu można podać pełną ścieżkę pliku lub samą nazwę pliku -> wtedy lokalizuje tam, gdzie jest program

		if (!plik)								//sprawdzamy czy plik istnieje
		{
			printf("\nBłąd. Niestety nie udało się otworzyć tego pliku. Zapis pliku nie będzie możliwy.\n");
		}
	} while (!plik);							//wykonuj dopóki plik nie istnieje, tzn. jeśli istnieje to zakończ pętlę!!!

	//"Poza pierwszą linią, mogą się one pojawić w każdym miejscu pliku." - dlatego przy odczycie standardu (pierwsza linia) nie zwracam uwagi na komentarze
	fscanf(plik, "%s", obraz->standard); //%s do struktur, fscanf czyta pojedynczy znak z pliku
	if (strcmp(obraz->standard, "P2") != 0) //funkcja strcmp porównuje napisy; zwraca wartość zero, gdy jeden napis jest taki sam jak drugi
	{
		printf("Przepraszamy, ale niestety standard tego pliku jest niezgodny z wymaganym do odczytu.");
		exit(0);								//zakończenie programu
	}

	//oczytujemy kolejną linię, czyli wymiary
	pobierz(plik);	//obsluga komentarzy
	fscanf(plik, "%d %d", &obraz->wysokosc_obrazu, &obraz->szerokosc_obrazu); //dwa razy %d bo dwie liczby

	//dynamiczna alokacja dwuwymiarowej macierzy dla pikseli obrazu + zabezpieczenie przez utratą pamięci
	obraz->ilosc_pikseli = malloc(obraz->szerokosc_obrazu * sizeof(int*));			//najpierw kolumny; alokujemy pamieć na liczbę kolumn w danej tablicy pikseli - stąd wskaźnik na int
	//przechowywać będziemy wskaźniki (do odpowiednich tablic jednowymiarowych przechowujących wiersze), dlatego jako argument funkcji sizeof() podajemy int *

	if (obraz->ilosc_pikseli != NULL)												//jeśli mamy tyle wolnej pamięci to wchodzimy do pętli
	{
		for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
		{
			obraz->ilosc_pikseli[i] = malloc(obraz->wysokosc_obrazu * sizeof(int)); //teraz dynamicznie alokujemy wiersze; alokujemy dynamicznie na poszczególne pola w danej kolumnie
		}
	}
	else																			//jeśli nie ma pamięci to zwalniamy ją
	{
		for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
		{
			free(obraz->ilosc_pikseli[i]);											//zwolnienie z wierszy
		}
		free(obraz->ilosc_pikseli);													//zwolnienie całej tablicy, pierwotnej tablicy wskaźników do wierszy
		printf("\nBłąd alokacji, przerwanie działania.\n");
		exit(0);																	//zakończenie programu
	}

	//odczytujemy kolejną linię, czyli głębię szarości
	pobierz(plik);
	fscanf(plik, "%d", &obraz->glebia_szarosci);

	//teraz odczytujemy piksele
	for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
	{
		for (int j = 0; j < obraz->wysokosc_obrazu; j = j + 1)
		{
			pobierz(plik);
			fscanf(plik, "%d", &obraz->ilosc_pikseli[i][j]);
		}
	}
	printf("\nDodano obraz %s.\n\n", obraz->nazwa_obrazu);
	fclose(plik);				//zamknięcie pliku czyści bufory danych powiązane z nim, zwalnia uchwyt oraz pozwala na otwieranie pliku na zewnątrz lub jego usunięcie;
}

void zapisz_obraz(struct obraz* obraz)
{
	FILE* plik;					//utworzenie struktury FILE* (uchwytu, identyfikatora) pliku, do którego będziemy się odwoływać

	printf("\nProszę podać nową nazwę obrazu: ");
	char nowy_obraz[20];
	scanf("%s", nowy_obraz);	//&s odczytuje sekwencje znaków nie będących białymi znakami, argument powinien być wskaźnikiem (czyli tablicą) na char
	printf("\n");
	strcat(nowy_obraz, ".pgm");		//funkcja strcat scala dwa łańcuchy znaków w jeden; w tym przypadku nową nazwę obrazu i rozszerzenie .pgm
									//otwarcie/utworzenie pliku oraz przypisanie go do utworzonego „uchwytu”. Funkcja fopen() przyjmuje dwa parametry : nazwę pliku i tryb obsługi pliku
	plik = fopen(nowy_obraz, "w");	//"w" – otwiera/tworzy plik do zapisu, usuwa jego starą treść

	//teraz zapisujemy wartości, zmienione w edycji lub nie
	fprintf(plik, "%s\n", obraz->standard);										//zapisujemy w pliku w pierwszej linii standard, \n - nowa linia, biały znak
	fprintf(plik, "%d %d\n", obraz->wysokosc_obrazu, obraz->szerokosc_obrazu);	//w drugiej linii zapisujemy wymiary obrazu
	fprintf(plik, "%d\n", obraz->glebia_szarosci);								//w trzeciej zapisujemy głębię szarości

	for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)						//zapis pikseli, najpierw kolumny
	{
		for (int j = 0; j < obraz->wysokosc_obrazu; j = j + 1)					//teraz wiersze
		{
			fprintf(plik, "%d ", obraz->ilosc_pikseli[i][j]);					//liczba + spacja
		}
		fprintf(plik, "\n");													//wstawiamy do pliku nową linię
	}
	printf("Obraz pomyślnie zapisano do pliku.\n");
}

void usun_obraz(struct obraz* obraz)
{
	for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
	{
		free(obraz->ilosc_pikseli[i]);										//zwolnienie z wierszy
	}
	free(obraz->ilosc_pikseli);												//zwolnienie całej tablicy, pierwotnej tablicy wskaźników do wierszy
	printf("Obraz usunięto.\n");
}

void obrot_o_k90(struct obraz* obraz)
{
	int** tablica_tymczasowa;
	tablica_tymczasowa = malloc(obraz->szerokosc_obrazu * sizeof(int*));			//kolumny; przechowywać będziemy wskaźniki (do odpowiednich tablic jednowymiarowych przechowujących wiersze), dlatego jako argument funkcji sizeof() podajemy int *
	if (tablica_tymczasowa != NULL)													//jeśli mamy tyle wolnej pamięci to wchodzimy do pętli
	{
		for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
		{
			tablica_tymczasowa[i] = malloc(obraz->wysokosc_obrazu * sizeof(int));	//teraz dynamicznie alokujemy wiersze; alokujemy dynamicznie na poszczególne pola w danej kolumnie
		}
	}
	else																			//jeśli nie ma pamięci to zwalniamy ją
	{
		for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
		{
			free(tablica_tymczasowa[i]);											//zwolnienie z wierszy
		}
		free(tablica_tymczasowa);													//zwolnienie całej tablicy, pierwotnej tablicy wskaźników do wierszy
		printf("Błąd alokacji, przerwanie działania.\n");
		exit(0);	//zakończenie programu
	}

	//przepisujemy obraz do tymczasowej
	for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
	{
		for (int j = 0; j < obraz->wysokosc_obrazu; j = j + 1)
		{
			tablica_tymczasowa[i][j] = obraz->ilosc_pikseli[i][j];
		}
	}

	if (obraz->szerokosc_obrazu != obraz->wysokosc_obrazu)
	{
		int pomocnicza = obraz->szerokosc_obrazu;										//zamieniamy wymiary
		obraz->szerokosc_obrazu = obraz->wysokosc_obrazu;
		obraz->wysokosc_obrazu = pomocnicza;

		//realokacja + zabezpieczenie
		obraz->ilosc_pikseli = malloc(obraz->szerokosc_obrazu * sizeof(int*));			//najpierw kolumny; alokujemy pamieć na liczbę kolumn w danej tablicy pikseli - stąd wskaźnik na int
		//przechowywać będziemy wskaźniki (do odpowiednich tablic jednowymiarowych przechowujących wiersze), dlatego jako argument funkcji sizeof() podajemy int *

		if (obraz->ilosc_pikseli != NULL)												//jeśli mamy tyle wolnej pamięci to wchodzimy do pętli
		{
			for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
			{
				obraz->ilosc_pikseli[i] = malloc(obraz->wysokosc_obrazu * sizeof(int)); //teraz dynamicznie alokujemy wiersze; alokujemy dynamicznie na poszczególne pola w danej kolumnie
			}
		}
		else																			//jeśli nie ma pamięci to zwalniamy ją
		{
			for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
			{
				free(obraz->ilosc_pikseli[i]);											//zwolnienie z wierszy
			}
			free(obraz->ilosc_pikseli);													//zwolnienie całej tablicy, pierwotnej tablicy wskaźników do wierszy
			printf("Błąd alokacji, przerwanie działania.\n");
			exit(0);																	//zakończenie programu
		}
	}

	//obrót w lewo
	for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
	{
		for (int j = 0; j < obraz->wysokosc_obrazu; j = j + 1)
		{
			obraz->ilosc_pikseli[i][j] = tablica_tymczasowa[obraz->wysokosc_obrazu - 1 - j][i];
			/*
			dlaczego tak? bo na przykład gdy mamy tablicę 3x2, dostaniemy tablicę 2x3 i teraz dla i=0, j=0 w nowej tablicy chcemy
			pobrać wartość z [1][0] ze starej, więc bierzemy [(wysokosc = 2) - 1 - (j = 0)][i = 0], czyli [1][0] jak chcieliśmy
			*/
		}
	}

	//zwalniamy tablicę tymczasową
	for (int i = 0; i < obraz->wysokosc_obrazu; i = i + 1)
	{
		free(tablica_tymczasowa[i]);
	}
	free(tablica_tymczasowa);
}

void histogram_zapis(struct obraz* obraz)		//histogram mówi nam ile pikseli danego koloru znajduje się na obrazie
{
	FILE* plik;
	printf("Proszę podać nazwę pliku:\n");
	char nazwa_pliku[20];
	scanf("%s", nazwa_pliku);					//&s odczytuje sekwencje znaków nie będących białymi znakami, argument powinien być wskaźnikiem (czyli tablicą) na char
	strcat(nazwa_pliku, ".csv");				//funkcja strcat scala dwa łańcuchy znaków w jeden; w tym przypadku nową nazwę obrazu i rozszerzenie .csv
	//otwarcie/utworzenie pliku oraz przypisanie go do utworzonego „uchwytu”. Funkcja fopen() przyjmuje dwa parametry : nazwę pliku i tryb obsługi pliku
	plik = fopen(nazwa_pliku, "w");				//"w" – otwiera/tworzy plik do zapisu, usuwa jego starą treść

	//"Histogram tworzy się przez zsumowanie wszystkich pikseli dla każdego odcienia szarości(np. do tablicy)"
	for (int piksel = 0; piksel < obraz->glebia_szarosci; piksel = piksel + 1)
	{
		int odcien_szarosci = 0;				//ile ich jest, nie odcień
		for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
		{
			for (int j = 0; j < obraz->wysokosc_obrazu; j = j + 1)
			{
				if (obraz->ilosc_pikseli[i][j] == piksel)
					odcien_szarosci = odcien_szarosci + 1;
			}
		}
		fprintf(plik, "%d;%d\n", piksel, odcien_szarosci); //wpisujemy do pliku
	}
	fclose(plik);											//zamknięcie pliku czyści bufory danych powiązane z nim, zwalnia uchwyt oraz pozwala na otwieranie pliku na zewnątrz lub jego usunięcie;
}

void progowanie(struct obraz* obraz)	//"Polega on na rozdziale obrazu na dwie części – białą i czarną, względem poziomu szarości pikseli."
{
	int poziom_szarosci;
	do
	{
		printf("Zakres: [0; %d]", obraz->glebia_szarosci);
		printf("\nProszę podać próg z zakresu.\n");
		char napis[20];
		scanf("%s", napis);
		int n = strlen(napis);
		for (int i = 0; i < n; i++)
		{
			if (napis[i] >= 48 && napis[i] <= 57)
			{
				poziom_szarosci = atoi(napis);
			}
			else
			{
				poziom_szarosci = -1;
				printf("Niepoprawna wartość. Proszę podać wartość progu z zakresu.\n");
			}
		}
	} while (poziom_szarosci < 0 || poziom_szarosci > obraz->glebia_szarosci);

	//rozdział obrazu na dwie części – białą i czarną, względem poziomu szarości pikseli
	for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
	{
		for (int j = 0; j < obraz->wysokosc_obrazu; j = j + 1)
		{
			if (obraz->ilosc_pikseli[i][j] <= poziom_szarosci)	//jeśli dany piksel (jego głebia szarości) jest mniejszy lub równy obranemu poziomowi to przyjmujemy go jako biały - 0
			{
				obraz->ilosc_pikseli[i][j] = 0;
			}
			else												//jeśli głębia szarości jest większa od obranego poziomu to piksel przyjmuje wartość max czyli wartość głębi szarości
			{
				obraz->ilosc_pikseli[i][j] = obraz->glebia_szarosci;
			}
		}
	}
}

void szum_pieprz_sol(struct obraz* obraz)
{
	double procent_pikseli;
	/*wprowadzenie z klawiatury
	do
	{
		printf("\nProszę podać maksymalną wartość procentową zaszumionych pikseli:\n");
		char napis[20];
		scanf("%s", napis);
		int n = strlen(napis);
		for (int i = 0; i < n; i++)
		{
			if (napis[i] >= 48 && napis[i] <= 57)
			{
				procent_pikseli = atof(napis);
			}
			else
			{
				procent_pikseli = -1;
				printf("Niepoprawna wartość. Wartość procentowa może wynosić od 0 do 100%.\n");
			}
		}
	} while (procent_pikseli < 0 || procent_pikseli >= 100);
	*/
	
	//losowanie automatyczne:
	procent_pikseli = rand() % 101 / 10;								//wylosowanie liczby z zakresu od 1 do 100, dzielone przez 10 - aby nie było za dużo
																		//po wielu próbach dzielenie przez 10 było optymalnym rozwiązaniem
	for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
	{
		for (int j = 0; j < obraz->wysokosc_obrazu; j = j + 1)
		{
			int szum = rand() % 401;									//im większy zakres tym mniejsza szansa na zaszumienie danego piksela, zakres od 1 do 400 dawał optymalne rozwiązanie
			/*
			wylosowanie liczby z zakresu od 1 do 400, określa, który akurat piksel będzie zaszumiony;
			gdy np. wylosuje się liczba 346 i jest ona dużo większa od wylosowanych pikseli (których jest np. 20) to
			danego piksela nie zaszumiamy, a gdy wylosuje się np. liczba 20 to dany piksel zostanie zaszumiony
			*/
			if (szum <= procent_pikseli)
			{
				if (szum <= 2 * procent_pikseli / 3)					//zakładamy, że 2/3 zaszumionych pikseli będzie biała, a reszta czarna
					obraz->ilosc_pikseli[i][j] = 0;						//biały piksel

				else
					obraz->ilosc_pikseli[i][j] = obraz->glebia_szarosci; //czarny piksel
			}
		}
	}
}

void filtr_gaussa(struct obraz* obraz)
{
	int** tablica_tymczasowa;
	tablica_tymczasowa = malloc(obraz->szerokosc_obrazu * sizeof(int*));	//kolumny; przechowywać będziemy wskaźniki (do odpowiednich tablic jednowymiarowych przechowujących wiersze), dlatego jako argument funkcji sizeof() podajemy int *
	if (tablica_tymczasowa != NULL)											//jeśli mamy tyle wolnej pamięci to wchodzimy do pętli
	{
		for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
		{
			tablica_tymczasowa[i] = malloc(obraz->wysokosc_obrazu * sizeof(int));//teraz dynamicznie alokujemy wiersze; alokujemy dynamicznie na poszczególne pola w danej kolumnie
		}
	}
	else																		//jeśli nie ma pamięci to zwalniamy ją
	{
		for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
		{
			free(tablica_tymczasowa[i]);										//zwolnienie z wierszy
		}
		free(tablica_tymczasowa);												//zwolnienie całej tablicy, pierwotnej tablicy wskaźników do wierszy
		printf("Błąd alokacji, przerwanie działania.\n");
		exit(0);	//zakończenie programu
	}

	//przepisujemy obraz do tymczasowej
	for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
	{
		for (int j = 0; j < obraz->wysokosc_obrazu; j = j + 1)
		{
			tablica_tymczasowa[i][j] = obraz->ilosc_pikseli[i][j];
		}
	}

	//podobnny do filtra średniej ruchomej z programu 1

	int pomocnicza[9];	//pomocnicza tablica
	/*
	instrukcja: "wartości pikseli pod maską są mnożone przez odpowiednie wagi i sumowane, następnie dzielone przez sumę wag"
	*/

	for (int i = 1; i < obraz->szerokosc_obrazu - 1; i = i + 1)
	{
		for (int j = 1; j < obraz->wysokosc_obrazu - 1; j = j + 1)
		{
			//wagi podane były w instrukcji
			//niżej wszystkie możliwości konfiguracji - okno 3x3
			pomocnicza[0] = 1 * tablica_tymczasowa[i - 1][j - 1];
			pomocnicza[1] = 2 * tablica_tymczasowa[i - 1][j];
			pomocnicza[2] = 1 * tablica_tymczasowa[i - 1][j + 1];
			pomocnicza[3] = 2 * tablica_tymczasowa[i][j - 1];
			pomocnicza[4] = 4 * tablica_tymczasowa[i][j];
			pomocnicza[5] = 2 * tablica_tymczasowa[i][j + 1];
			pomocnicza[6] = 1 * tablica_tymczasowa[i + 1][j - 1];
			pomocnicza[7] = 2 * tablica_tymczasowa[i + 1][j];
			pomocnicza[8] = 1 * tablica_tymczasowa[i + 1][j + 1];

			int srednia = (pomocnicza[0] + pomocnicza[1] + pomocnicza[2] + pomocnicza[3] + pomocnicza[4] + pomocnicza[5] + pomocnicza[6] + pomocnicza[7] + pomocnicza[8]) / (1 + 2 + 1 + 2 + 4 + 2 + 1 + 2 + 1);	//dzielone przez sumę wag
			obraz->ilosc_pikseli[i][j] = srednia;
		}
	}

	//zwalniamy tablicę tymczasową
	for (int i = 0; i < obraz->wysokosc_obrazu; i = i + 1)
	{
		free(tablica_tymczasowa[i]);
	}
	free(tablica_tymczasowa);
}

int main()
{
	int ilosc_obrazow = 0;
	struct obraz* lista_obrazow = NULL; //deklaracja zmiennej strukturalnej, jest to tablica, w której będziemy przechowywać struktury
										//wskaźnik, aby można było łatwo edytować ewentualne zmiany
										//to jest właśnie nasza dynamiczna baza do przechowywania w pamięci i obsługi obrazów
										//NULL, bo na poczatku po odpaleniu programu nie ma zadnego obrazu na liscie, jest ona pusta
	setlocale(LC_ALL, "polish_poland");	//do polskich znaków
	srand(time(NULL));					//liczby pseudolosowe - losowanie

	int ktory;							//który obraz jest aktywny?
	int wybor;
	printf("				MENU WYBORU\nProszę wybrać jedną z poniższych opcji i zatwierdzić klawiszem enter:\n");
	do
	{
		printf("1. Dodanie obrazu.\n2. Wyświetlenie listy obrazów.\n3. Zapis wprowadzonych zmian.\n4. Usunięcie obrazu z listy.\n5. Edycja obrazu.\n6. Zakończenie pracy programu.\n");

		do
		{
			char napis[20];
			scanf("%s", napis);
			int n = strlen(napis);
			for (int i = 0; i < n; i++)
			{
				if (napis[i] >= 48 && napis[i] <= 57)
				{
					wybor = atoi(napis);
				}
				else
				{
					wybor = -1;
					printf("Niepoprawna wartość. Proszę podać liczbę z przedziału od 1 do 6.\n");
				}
			}
		} while (wybor <= 0);

		switch (wybor)
		{
		case 1:
		{
			printf("\nWybór 1. Dodanie obrazu.\n");
			ilosc_obrazow = ilosc_obrazow + 1;														//zwiększamy liczbę obrazów o 1
			//"Chcąc alokować, realokować lub zwalniać pamięć w osobnej funkcji, musisz przekazać adres do wskaźnika, na którym pamięć będzie alokowana.Czyli: dla tablic jednowymiarowych przekazuje się wskaźnik na wskaźnik" - instrukcja
			struct obraz* lista_obrazow_tymczasowa;													//realokujemy pamięć, bo wczytujemy obraz, więc potrzebna nam ta pamięć
			lista_obrazow_tymczasowa = realloc(lista_obrazow, ilosc_obrazow * sizeof(struct obraz));//nowy przydział pamięci, powiększa się obszar tablicy, pierwszy parametr to adres początku tablicy utworzonej za pomocą funkcji malloc(), drugi to nowy rozmiar pamięci w bajtach
			if (lista_obrazow_tymczasowa != NULL)													//bardzo ważne zabezpieczenie przed utratą danych
			{
				lista_obrazow = lista_obrazow_tymczasowa;
			}
			else
			{
				printf("Nie udało się powiększyć tablicy.\n");
				lista_obrazow = malloc(1 * sizeof(double));											//nie udało się powiększyć, a więc tablica ma jeden element
			}
			dodawanie_obrazu(&lista_obrazow[ilosc_obrazow - 1]);									//wywołanie funkcji
			break;																					//ilość jest pomniejszona o 1, bo numerowanie w tablicy jest od 0, a ilość normalnie od 1
		}																							//do funkcji przekazuję wskaźnik na tablicę, więc żeby wyjąć wartość trzeba dać &

		case 2:
		{
			if (ilosc_obrazow >= 1)																	//tylko jeśli w ogóle mamy jakiś obraz to wykonujemy
			{
				printf("\nWybór 2. Wyświetlenie lisy obrazów:\n");
				for (int i = 0; i < ilosc_obrazow; i = i + 1)
				{
					printf("%d. %s\n", i + 1, lista_obrazow[i].nazwa_obrazu);						//%d do numeracji obrazu, %s do napisów; i + 1 bo gdy i jest 0 to numer obrazu to 1, a jednoczesnie pierwszy element to zerowy numer w tablicy
				}
			}

			else
				printf("Brak obrazów do wyświetlenia. Proszę najpierw wybrać opcję 1 i dodać obrazy.\n");
			break;
		}
	
		case 3:
		{
			if (ilosc_obrazow >= 1)
			{
				printf("\nWybór 3. Zapis wprowadzonych zmian.\nKtóry obraz chcesz zapisać?\n");
				for (int i = 0; i < ilosc_obrazow; i = i + 1)					//wypisujemy obrazy, które posiadamy
				{
					printf("%d. %s\n", i + 1, lista_obrazow[i].nazwa_obrazu);	//%s to struktury, %d do int; i + 1 bo gdy i jest 0 to numer obrazu to 1, a jednoczesnie pierwszy element to zerowy numer w tablicy
				}

				do
				{
					char napis[20];
					scanf("%s", napis);
					int n = strlen(napis);
					for (int i = 0; i < n; i++)
					{
						if (napis[i] >= 48 && napis[i] <= 57)
						{
							ktory = atoi(napis);
						}
						else
						{
							ktory = -1;
							printf("Niepoprawna wartość. Proszę podać liczbę spośród wypisanych wyżej.\n");
						}
					}
				} while (ktory <= 0);

				if (ktory > 0 && ktory <= ilosc_obrazow)						//musi być większy od 0 i mniejszy od posiadanej ilości
				{
					zapisz_obraz(&lista_obrazow[ktory - 1]);					//ilość jest pomniejszona o 1, bo numerowanie w tablicy jest od 0, a ilość normalnie od 1
					break;
				}
				else
				{
					printf("W spisie nie istnieje obraz o podanym numerze. Proszę najpierw dodać obraz do listy.\n");
					break;
				}
			}
			else
			{
				printf("W spisie nie ma jeszcze obrazów. Proszę najpierw dodać obraz do listy\n");
				break;
			}
		}
		
		case 4:
		{
			if (ilosc_obrazow >= 1)
			{
				printf("\nWybór 4. Usunięcie obrazu z listy.\nKtóry obraz chcesz usunąć?\n");
				for (int i = 0; i < ilosc_obrazow; i = i + 1)					//wypisujemy obrazy, które posiadamy
				{
					printf("%d. %s\n", i + 1, lista_obrazow[i].nazwa_obrazu);	//%s to struktury, %d do int; i + 1 bo gdy i jest 0 to numer obrazu to 1, a jednoczesnie pierwszy element to zerowy numer w tablicy
				}

				do
				{
					char napis[20];
					scanf("%s", napis);
					int n = strlen(napis);
					for (int i = 0; i < n; i++)
					{
						if (napis[i] >= 48 && napis[i] <= 57)
						{
							ktory = atoi(napis);
						}
						else
						{
							ktory = -1;
							printf("Niepoprawna wartość. Proszę podać liczbę spośród wypisanych wyżej.\n");
						}
					}
				} while (ktory <= 0);

				if (ktory > 0 && ktory <= ilosc_obrazow)
				{
					if (ktory == ilosc_obrazow)							//gdy jest tylko jeden obraz i jeden usuwamy
					{
						usun_obraz(&lista_obrazow[ilosc_obrazow - 1]);	//ilość jest pomniejszona o 1, bo numerowanie w tablicy jest od 0, a ilość normalnie od 1
						ilosc_obrazow = ilosc_obrazow - 1;				//zmniejszamy liczbę obrazów, bo jeden usunęliśmy

						//zabezpieczenie
						struct obraz* lista_obrazow_tymczasowa;													//realokujemy pamięć, bo wczytujemy obraz, więc potrzebna nam ta pamięć
						lista_obrazow_tymczasowa = realloc(lista_obrazow, ilosc_obrazow * sizeof(struct obraz));//nowy przydział pamięci, powiększa się obszar tablicy, pierwszy parametr to adres początku tablicy utworzonej za pomocą funkcji malloc(), drugi to nowy rozmiar pamięci w bajtach
						if (lista_obrazow_tymczasowa != NULL)													//bardzo ważne zabezpieczenie przed utratą danych
						{
							lista_obrazow = lista_obrazow_tymczasowa;
						}
						else
						{
							lista_obrazow = malloc(1 * sizeof(double));					//nie udało się powiększyć, a więc tablica ma jeden element
						}
					}
					else																//gdy jest więcej niż jeden obraz
					{
						struct obraz zamieniony;
						zamieniony = lista_obrazow[ilosc_obrazow - 1];					//za zamienionego dajemy ostatni obraz z listy
						lista_obrazow[ilosc_obrazow - 1] = lista_obrazow[ktory - 1];	//za ostatni dajemy ten, który chcemy usunąć
						lista_obrazow[ktory - 1] = zamieniony;							//na miejsce usuniętego dajemy zamienionego (tego, który był wcześniej jako ostatni)

						usun_obraz(&lista_obrazow[ilosc_obrazow - 1]);					//ilość jest pomniejszona o 1, bo numerowanie w tablicy jest od 0, a ilość normalnie od 1
						ilosc_obrazow = ilosc_obrazow - 1;								//zmniejszamy liczbę obrazów, bo jeden usunęliśmy

						//zabezpieczenie
						struct obraz* lista_obrazow_tymczasowa;													//realokujemy pamięć, bo wczytujemy obraz, więc potrzebna nam ta pamięć
						lista_obrazow_tymczasowa = realloc(lista_obrazow, ilosc_obrazow * sizeof(struct obraz));//nowy przydział pamięci, powiększa się obszar tablicy, pierwszy parametr to adres początku tablicy utworzonej za pomocą funkcji malloc(), drugi to nowy rozmiar pamięci w bajtach
						if (lista_obrazow_tymczasowa != NULL)													//bardzo ważne zabezpieczenie przed utratą danych
						{
							lista_obrazow = lista_obrazow_tymczasowa;
						}
						else
						{
							lista_obrazow = malloc(1 * sizeof(double));											//nie udało się powiększyć, a więc tablica ma jeden element
						}
					}
				}
				else
				{
					printf("W spisie nie istnieje obraz o podanym numerze. Proszę najpierw dodać obraz do listy.\n ");
				}
				break;
			}
			else
			{
				printf("W spisie nie ma jeszcze obrazów. Proszę najpierw dodać obraz do listy.\n");
				break;
			}
		}
		
		case 5:
		{
			printf("\nWybór 5. Edycja obrazu.\n");
			if (ilosc_obrazow >= 1)
			{
				printf("\nKtóry obraz będzie przeznaczony do edycji?\n");
				for (int i = 0; i < ilosc_obrazow; i = i + 1)
				{
					printf("%d. %s\n", i + 1, lista_obrazow[i].nazwa_obrazu); //%s to struktury, %d do int; i + 1 bo gdy i jest 0 to numer obrazu to 1, a jednoczesnie pierwszy element to zerowy numer w tablicy
				}

				do
				{
					char napis[20];
					scanf("%s", napis);
					int n = strlen(napis);
					for (int i = 0; i < n; i++)
					{
						if (napis[i] >= 48 && napis[i] <= 57)
						{
							ktory = atoi(napis);
						}
						else
						{
							ktory = -1;
							printf("Niepoprawna wartość. Proszę podać liczbę spośród wypisanych wyżej.\n");
						}
					}
				} while (ktory <= 0);

				if (ktory > 0 && ktory <= ilosc_obrazow)
				{
					int wybor2;
					do
					{
						printf("\nProszę wybrać jedną z poniższych opcji i zatwierdzić klawiszem enter:\n");
						printf("1. Obrót o 90 stopni.\n2. Obrót o 180 stopni.\n3. Obrót o 270 stopni.\n4. Histogram z zapisem do pliku .CSV.\n5. Progowanie z dowolnym progiem.\n6. Dodawanie szumu typu pieprz i sól.\n7. Rozmycie za pomocą filtru Gaussa.\n8. Cofnij.\n");
						do
						{
							char napis[20];
							scanf("%s", napis);
							int n = strlen(napis);
							for (int i = 0; i < n; i++)
							{
								if (napis[i] >= 48 && napis[i] <= 57)
								{
									wybor2 = atoi(napis);
								}
								else
								{
									wybor2 = -1;
									printf("Niepoprawna wartość. Proszę podać liczbę z przedziału od 1 do 8.\n");
								}
							}
						} while (wybor2 <= 0);

						switch (wybor2)
						{
						case 1:
						{
							printf("Wybór 1. Obrót o 90 stopni.\n");
							obrot_o_k90(&lista_obrazow[ktory - 1]);	//czemu wybor - 1, bo gdy np będzie to obraz nr 2 to wybor = 2, ale jednocześnie zajmuje on miejsce pierwsze w tablicy, bo zerowe zajmuje obraz nr 1
							printf("Dokonano obrotu o 90 stopni.\n");
							break;
						}

						case 2:										//po prostu dwa obroty, wtedy będzie 180 stopni
						{
							printf("Wybór 2. Obrót o 180 stopni.\n");
							obrot_o_k90(&lista_obrazow[ktory - 1]);
							obrot_o_k90(&lista_obrazow[ktory - 1]);
							printf("Dokonano obrotu o 180 stopni.\n");
							break;
						}

						case 3:										//3 obroty
						{
							printf("Wybór 3. Obrót o 270 stopni.\n");
							obrot_o_k90(&lista_obrazow[ktory - 1]);
							obrot_o_k90(&lista_obrazow[ktory - 1]);
							obrot_o_k90(&lista_obrazow[ktory - 1]);
							printf("Dokonano obrotu o 270 stopni.\n");
							break;
						}

						case 4:
						{
							printf("Wybór 4. Histogram z zapisem do pliku .CSV.\n");
							histogram_zapis(&lista_obrazow[ktory - 1]);
							printf("Dokonano histogramu wraz z zapisem do pliku.\n");
							break;
						}


						case 5:
						{
							printf("Wybor 5. Progowanie z dowolnym progiem.\n");
							progowanie(&lista_obrazow[ktory - 1]);
							printf("Dokonano progowania.\n");
							break;
						}

						case 6:
						{
							printf("Wybór 6. Dodawanie szumu typu pieprz i sól.\n");
							szum_pieprz_sol(&lista_obrazow[ktory - 1]);
							printf("Dokonano zaszumienia typu pieprz i sól.\n");
							break;
						}

						case 7:
						{
							printf("Wybór 7. Rozmycie za pomocą filtru Gaussa.\n");
							filtr_gaussa(&lista_obrazow[ktory - 1]);
							printf("Dokonano rozmycia obrazu za pomocą filtru Gaussa.\n");
							break;
						}

						case 8:
						{
							printf("Wybór 8. Cofnij.\n");
							break;
						}

						default:
						{
							printf("Niestety takiej opcji nie ma w menu wyboru. Proszę podać ponownie:\n");
							break;
						}
						}
					} while (wybor2 != 8);
					break;
				}
				else
				{
					printf("W spisie nie istnieje obraz o podanym numerze. Proszę najpierw dodać obraz do listy.\n");
					break;
				}
			}
			else
			{
				printf("W spisie nie ma jeszcze obrazów. Proszę najpierw dodać obraz do listy.\n");
				break;
			}
		}

		case 6:
		{
			printf("\nWybór: 6. Zakończenie pracy w programie.\n");
			//"Pamiętaj o zwalnianiu pamięci wszystkich obrazów, jak i samej bazy, przy wychodzeniu z programu"
			if (ilosc_obrazow > 0)	//zwalniamy pamięć
			{
				for (int i = ilosc_obrazow - 1; i >= 0; i = i - 1) //ilość jest pomniejszona o 1, bo numerowanie w tablicy jest od 0, a ilość normalnie od 1
				{
					usun_obraz(&lista_obrazow[i]);
				}
				free(lista_obrazow);
			}
			break;
		}

		default:
		{
			printf("Niestety takiej opcji nie ma w menu wyboru. Proszę podać ponownie:\n");
		}
		}
		printf("\n");
	} while (wybor != 6);
	return 0;
}
