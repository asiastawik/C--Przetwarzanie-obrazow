//trzeba zamienic wys z szer! 
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>	//standardowe wejœcie/wyjœcie
#include <stdlib.h>	//standardowe funkcje, rand(), malloc(), realloc(), itd.
#include <string.h> //do napisów char
#include <locale.h>	//do funkcji setlocale()
#include <time.h>	//do funkcji obs³ugi czasu

struct obraz				//struktura, w tym przypadku jako zmienna globalna (poza main)
{							//jest to tylko deklaracja struktury, nie zmiennej strukturalnej
	char standard[2];		//standard musi byæ P2
	int szerokosc_obrazu;	//ca³kowita liczba dodatnia, kolumny
	int wysokosc_obrazu;	//ca³kowita liczba dodatnia, wiersze
	int glebia_szarosci;	//maksymalna wartoœæ szaroœci (g³êbia szaroœci) – od 1 do 65535
	int** ilosc_pikseli;	//przyjmuj¹ce wartoœæ od 0 do maksymalnej wartoœci szaroœci
							//wskaŸnik na wskaŸnik, bo potrzebna nam tablica dwuwymiarowa
	char nazwa_obrazu[20];	//powinno wystarczyæ
};

void pobierz(FILE* plik)
{
	char piksel;
	char komentarz[50];				//zak³adam, ¿e komentarz mo¿e mieæ maksymalnie 50 znaków
	do								//wykonuj dopóki nie napotkamy na koniec linii, czy spacjê, czy tabulator (bia³y znak)
	{
		fscanf(plik, "%c", &piksel); //%c, bo char
	} while (piksel == 32 || piksel == 10 || piksel == 9);  //a¿ do napotkania bia³ego znaku: 32 to spacja, 10 koniec linii 9 to tabulacja, || - lub

	if (piksel == '#')				//# - komentarz; obowi¹zuj¹ zawsze do koñca linii
	{
		fgets(komentarz, 50, plik); //funkcja fgets odczytuje znaki z pliku dopóki nie odczyta podanej iloœci, nie napotka koñca linii lub koñca pliku; zapisuje znaki do podanej tablicy; przestawia pozycje w pliku za ostatni odczytany znak
		pobierz(plik);				//je¿eli natrafimy na # to czytamy a¿ do koñca linii i to pomijamy, a potem wracamy do pocz¹tku, czyli dalszej obs³ugi pliku, zastosowa³am rekurencjê
	}
	else							//jeœli nie to czytaj wartoœæ kolejnego piksela
		fseek(plik, -1, SEEK_CUR);	//funkcja przesuwa pozycjê w pliku o podan¹ liczbê znaków(int) (-1) wzglêdem aktualnego miejsca (SEEK_CUR), czyli o 1 w prawo
}									//po ka¿dym wywolaniu pobierz(plik) przechodzimy do kolejnego znaku w tym pliku, dziêki funkcji fseek i to jest ci¹gle zapamiêtywane

void dodawanie_obrazu(struct obraz* obraz)
{
	FILE* plik;									//utworzenie struktury FILE* (uchwytu, identyfikatora) pliku, do którego bêdziemy siê odwo³ywaæ
	do {
		printf("Proszê podaæ nazwê obrazu, któr¹ chcesz dodaæ:\n8x8\nangel\nbird\ncells\neuro\nfigury\nflir\ngears\ngradient\nmerc\nplane\robo\nteddy\ntest\nzurawie\nzurawie-mydlane\n\n");
		printf("Chcê dodaæ: ");
		scanf("%s", obraz->nazwa_obrazu);		//aby przekazaæ zmienn¹ strukturaln¹ do funkcji gdzie mia³aby byæ zmodyfikowana, nale¿y to zrobiæ przez wskaŸnik

		strcat(obraz->nazwa_obrazu, ".pgm");	//funkcja strcat scala dwa ³añcuchy znaków w jeden; w naszym przypadku nazwê obrazu i rozszerzenie .pgm
		plik = fopen(obraz->nazwa_obrazu, "r");	//"r" – otwiera plik do odczytu, nazwa pliku – tablica znaków char*; w tym miejscu mo¿na podaæ pe³n¹ œcie¿kê pliku lub sam¹ nazwê pliku -> wtedy lokalizuje tam, gdzie jest program

		if (!plik)								//sprawdzamy czy plik istnieje
		{
			printf("\nB³¹d. Niestety nie uda³o siê otworzyæ tego pliku. Zapis pliku nie bêdzie mo¿liwy.\n");
		}
	} while (!plik);							//wykonuj dopóki plik nie istnieje, tzn. jeœli istnieje to zakoñcz pêtlê!!!

	//"Poza pierwsz¹ lini¹, mog¹ siê one pojawiæ w ka¿dym miejscu pliku." - dlatego przy odczycie standardu (pierwsza linia) nie zwracam uwagi na komentarze
	fscanf(plik, "%s", obraz->standard); //%s do struktur, fscanf czyta pojedynczy znak z pliku
	if (strcmp(obraz->standard, "P2") != 0) //funkcja strcmp porównuje napisy; zwraca wartoœæ zero, gdy jeden napis jest taki sam jak drugi
	{
		printf("Przepraszamy, ale niestety standard tego pliku jest niezgodny z wymaganym do odczytu.");
		exit(0);								//zakoñczenie programu
	}

	//oczytujemy kolejn¹ liniê, czyli wymiary
	pobierz(plik);	//obsluga komentarzy
	fscanf(plik, "%d %d", &obraz->wysokosc_obrazu, &obraz->szerokosc_obrazu); //dwa razy %d bo dwie liczby

	//dynamiczna alokacja dwuwymiarowej macierzy dla pikseli obrazu + zabezpieczenie przez utrat¹ pamiêci
	obraz->ilosc_pikseli = malloc(obraz->szerokosc_obrazu * sizeof(int*));			//najpierw kolumny; alokujemy pamieæ na liczbê kolumn w danej tablicy pikseli - st¹d wskaŸnik na int
	//przechowywaæ bêdziemy wskaŸniki (do odpowiednich tablic jednowymiarowych przechowuj¹cych wiersze), dlatego jako argument funkcji sizeof() podajemy int *

	if (obraz->ilosc_pikseli != NULL)												//jeœli mamy tyle wolnej pamiêci to wchodzimy do pêtli
	{
		for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
		{
			obraz->ilosc_pikseli[i] = malloc(obraz->wysokosc_obrazu * sizeof(int)); //teraz dynamicznie alokujemy wiersze; alokujemy dynamicznie na poszczególne pola w danej kolumnie
		}
	}
	else																			//jeœli nie ma pamiêci to zwalniamy j¹
	{
		for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
		{
			free(obraz->ilosc_pikseli[i]);											//zwolnienie z wierszy
		}
		free(obraz->ilosc_pikseli);													//zwolnienie ca³ej tablicy, pierwotnej tablicy wskaŸników do wierszy
		printf("\nB³¹d alokacji, przerwanie dzia³ania.\n");
		exit(0);																	//zakoñczenie programu
	}

	//odczytujemy kolejn¹ liniê, czyli g³êbiê szaroœci
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
	fclose(plik);				//zamkniêcie pliku czyœci bufory danych powi¹zane z nim, zwalnia uchwyt oraz pozwala na otwieranie pliku na zewn¹trz lub jego usuniêcie;
}

void zapisz_obraz(struct obraz* obraz)
{
	FILE* plik;					//utworzenie struktury FILE* (uchwytu, identyfikatora) pliku, do którego bêdziemy siê odwo³ywaæ

	printf("\nProszê podaæ now¹ nazwê obrazu: ");
	char nowy_obraz[20];
	scanf("%s", nowy_obraz);	//&s odczytuje sekwencje znaków nie bêd¹cych bia³ymi znakami, argument powinien byæ wskaŸnikiem (czyli tablic¹) na char
	printf("\n");
	strcat(nowy_obraz, ".pgm");		//funkcja strcat scala dwa ³añcuchy znaków w jeden; w tym przypadku now¹ nazwê obrazu i rozszerzenie .pgm
									//otwarcie/utworzenie pliku oraz przypisanie go do utworzonego „uchwytu”. Funkcja fopen() przyjmuje dwa parametry : nazwê pliku i tryb obs³ugi pliku
	plik = fopen(nowy_obraz, "w");	//"w" – otwiera/tworzy plik do zapisu, usuwa jego star¹ treœæ

	//teraz zapisujemy wartoœci, zmienione w edycji lub nie
	fprintf(plik, "%s\n", obraz->standard);										//zapisujemy w pliku w pierwszej linii standard, \n - nowa linia, bia³y znak
	fprintf(plik, "%d %d\n", obraz->wysokosc_obrazu, obraz->szerokosc_obrazu);	//w drugiej linii zapisujemy wymiary obrazu
	fprintf(plik, "%d\n", obraz->glebia_szarosci);								//w trzeciej zapisujemy g³êbiê szaroœci

	for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)						//zapis pikseli, najpierw kolumny
	{
		for (int j = 0; j < obraz->wysokosc_obrazu; j = j + 1)					//teraz wiersze
		{
			fprintf(plik, "%d ", obraz->ilosc_pikseli[i][j]);					//liczba + spacja
		}
		fprintf(plik, "\n");													//wstawiamy do pliku now¹ liniê
	}
	printf("Obraz pomyœlnie zapisano do pliku.\n");
}

void usun_obraz(struct obraz* obraz)
{
	for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
	{
		free(obraz->ilosc_pikseli[i]);										//zwolnienie z wierszy
	}
	free(obraz->ilosc_pikseli);												//zwolnienie ca³ej tablicy, pierwotnej tablicy wskaŸników do wierszy
	printf("Obraz usuniêto.\n");
}

void obrot_o_k90(struct obraz* obraz)
{
	int** tablica_tymczasowa;
	tablica_tymczasowa = malloc(obraz->szerokosc_obrazu * sizeof(int*));			//kolumny; przechowywaæ bêdziemy wskaŸniki (do odpowiednich tablic jednowymiarowych przechowuj¹cych wiersze), dlatego jako argument funkcji sizeof() podajemy int *
	if (tablica_tymczasowa != NULL)													//jeœli mamy tyle wolnej pamiêci to wchodzimy do pêtli
	{
		for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
		{
			tablica_tymczasowa[i] = malloc(obraz->wysokosc_obrazu * sizeof(int));	//teraz dynamicznie alokujemy wiersze; alokujemy dynamicznie na poszczególne pola w danej kolumnie
		}
	}
	else																			//jeœli nie ma pamiêci to zwalniamy j¹
	{
		for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
		{
			free(tablica_tymczasowa[i]);											//zwolnienie z wierszy
		}
		free(tablica_tymczasowa);													//zwolnienie ca³ej tablicy, pierwotnej tablicy wskaŸników do wierszy
		printf("B³¹d alokacji, przerwanie dzia³ania.\n");
		exit(0);	//zakoñczenie programu
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
		obraz->ilosc_pikseli = malloc(obraz->szerokosc_obrazu * sizeof(int*));			//najpierw kolumny; alokujemy pamieæ na liczbê kolumn w danej tablicy pikseli - st¹d wskaŸnik na int
		//przechowywaæ bêdziemy wskaŸniki (do odpowiednich tablic jednowymiarowych przechowuj¹cych wiersze), dlatego jako argument funkcji sizeof() podajemy int *

		if (obraz->ilosc_pikseli != NULL)												//jeœli mamy tyle wolnej pamiêci to wchodzimy do pêtli
		{
			for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
			{
				obraz->ilosc_pikseli[i] = malloc(obraz->wysokosc_obrazu * sizeof(int)); //teraz dynamicznie alokujemy wiersze; alokujemy dynamicznie na poszczególne pola w danej kolumnie
			}
		}
		else																			//jeœli nie ma pamiêci to zwalniamy j¹
		{
			for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
			{
				free(obraz->ilosc_pikseli[i]);											//zwolnienie z wierszy
			}
			free(obraz->ilosc_pikseli);													//zwolnienie ca³ej tablicy, pierwotnej tablicy wskaŸników do wierszy
			printf("B³¹d alokacji, przerwanie dzia³ania.\n");
			exit(0);																	//zakoñczenie programu
		}
	}

	//obrót w lewo
	for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
	{
		for (int j = 0; j < obraz->wysokosc_obrazu; j = j + 1)
		{
			obraz->ilosc_pikseli[i][j] = tablica_tymczasowa[obraz->wysokosc_obrazu - 1 - j][i];
			/*
			dlaczego tak? bo na przyk³ad gdy mamy tablicê 3x2, dostaniemy tablicê 2x3 i teraz dla i=0, j=0 w nowej tablicy chcemy
			pobraæ wartoœæ z [1][0] ze starej, wiêc bierzemy [(wysokosc = 2) - 1 - (j = 0)][i = 0], czyli [1][0] jak chcieliœmy
			*/
		}
	}

	//zwalniamy tablicê tymczasow¹
	for (int i = 0; i < obraz->wysokosc_obrazu; i = i + 1)
	{
		free(tablica_tymczasowa[i]);
	}
	free(tablica_tymczasowa);
}

void histogram_zapis(struct obraz* obraz)		//histogram mówi nam ile pikseli danego koloru znajduje siê na obrazie
{
	FILE* plik;
	printf("Proszê podaæ nazwê pliku:\n");
	char nazwa_pliku[20];
	scanf("%s", nazwa_pliku);					//&s odczytuje sekwencje znaków nie bêd¹cych bia³ymi znakami, argument powinien byæ wskaŸnikiem (czyli tablic¹) na char
	strcat(nazwa_pliku, ".csv");				//funkcja strcat scala dwa ³añcuchy znaków w jeden; w tym przypadku now¹ nazwê obrazu i rozszerzenie .csv
	//otwarcie/utworzenie pliku oraz przypisanie go do utworzonego „uchwytu”. Funkcja fopen() przyjmuje dwa parametry : nazwê pliku i tryb obs³ugi pliku
	plik = fopen(nazwa_pliku, "w");				//"w" – otwiera/tworzy plik do zapisu, usuwa jego star¹ treœæ

	//"Histogram tworzy siê przez zsumowanie wszystkich pikseli dla ka¿dego odcienia szaroœci(np. do tablicy)"
	for (int piksel = 0; piksel < obraz->glebia_szarosci; piksel = piksel + 1)
	{
		int odcien_szarosci = 0;				//ile ich jest, nie odcieñ
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
	fclose(plik);											//zamkniêcie pliku czyœci bufory danych powi¹zane z nim, zwalnia uchwyt oraz pozwala na otwieranie pliku na zewn¹trz lub jego usuniêcie;
}

void progowanie(struct obraz* obraz)	//"Polega on na rozdziale obrazu na dwie czêœci – bia³¹ i czarn¹, wzglêdem poziomu szaroœci pikseli."
{
	int poziom_szarosci;
	do
	{
		printf("Zakres: [0; %d]", obraz->glebia_szarosci);
		printf("\nProszê podaæ próg z zakresu.\n");
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
				printf("Niepoprawna wartoœæ. Proszê podaæ wartoœæ progu z zakresu.\n");
			}
		}
	} while (poziom_szarosci < 0 || poziom_szarosci > obraz->glebia_szarosci);

	//rozdzia³ obrazu na dwie czêœci – bia³¹ i czarn¹, wzglêdem poziomu szaroœci pikseli
	for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
	{
		for (int j = 0; j < obraz->wysokosc_obrazu; j = j + 1)
		{
			if (obraz->ilosc_pikseli[i][j] <= poziom_szarosci)	//jeœli dany piksel (jego g³ebia szaroœci) jest mniejszy lub równy obranemu poziomowi to przyjmujemy go jako bia³y - 0
			{
				obraz->ilosc_pikseli[i][j] = 0;
			}
			else												//jeœli g³êbia szaroœci jest wiêksza od obranego poziomu to piksel przyjmuje wartoœæ max czyli wartoœæ g³êbi szaroœci
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
		printf("\nProszê podaæ maksymaln¹ wartoœæ procentow¹ zaszumionych pikseli:\n");
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
				printf("Niepoprawna wartoœæ. Wartoœæ procentowa mo¿e wynosiæ od 0 do 100%.\n");
			}
		}
	} while (procent_pikseli < 0 || procent_pikseli >= 100);
	*/
	
	//losowanie automatyczne:
	procent_pikseli = rand() % 101 / 10;								//wylosowanie liczby z zakresu od 1 do 100, dzielone przez 10 - aby nie by³o za du¿o
																		//po wielu próbach dzielenie przez 10 by³o optymalnym rozwi¹zaniem
	for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
	{
		for (int j = 0; j < obraz->wysokosc_obrazu; j = j + 1)
		{
			int szum = rand() % 401;									//im wiêkszy zakres tym mniejsza szansa na zaszumienie danego piksela, zakres od 1 do 400 dawa³ optymalne rozwi¹zanie
			/*
			wylosowanie liczby z zakresu od 1 do 400, okreœla, który akurat piksel bêdzie zaszumiony;
			gdy np. wylosuje siê liczba 346 i jest ona du¿o wiêksza od wylosowanych pikseli (których jest np. 20) to
			danego piksela nie zaszumiamy, a gdy wylosuje siê np. liczba 20 to dany piksel zostanie zaszumiony
			*/
			if (szum <= procent_pikseli)
			{
				if (szum <= 2 * procent_pikseli / 3)					//zak³adamy, ¿e 2/3 zaszumionych pikseli bêdzie bia³a, a reszta czarna
					obraz->ilosc_pikseli[i][j] = 0;						//bia³y piksel

				else
					obraz->ilosc_pikseli[i][j] = obraz->glebia_szarosci; //czarny piksel
			}
		}
	}
}

void filtr_gaussa(struct obraz* obraz)
{
	int** tablica_tymczasowa;
	tablica_tymczasowa = malloc(obraz->szerokosc_obrazu * sizeof(int*));	//kolumny; przechowywaæ bêdziemy wskaŸniki (do odpowiednich tablic jednowymiarowych przechowuj¹cych wiersze), dlatego jako argument funkcji sizeof() podajemy int *
	if (tablica_tymczasowa != NULL)											//jeœli mamy tyle wolnej pamiêci to wchodzimy do pêtli
	{
		for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
		{
			tablica_tymczasowa[i] = malloc(obraz->wysokosc_obrazu * sizeof(int));//teraz dynamicznie alokujemy wiersze; alokujemy dynamicznie na poszczególne pola w danej kolumnie
		}
	}
	else																		//jeœli nie ma pamiêci to zwalniamy j¹
	{
		for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
		{
			free(tablica_tymczasowa[i]);										//zwolnienie z wierszy
		}
		free(tablica_tymczasowa);												//zwolnienie ca³ej tablicy, pierwotnej tablicy wskaŸników do wierszy
		printf("B³¹d alokacji, przerwanie dzia³ania.\n");
		exit(0);	//zakoñczenie programu
	}

	//przepisujemy obraz do tymczasowej
	for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
	{
		for (int j = 0; j < obraz->wysokosc_obrazu; j = j + 1)
		{
			tablica_tymczasowa[i][j] = obraz->ilosc_pikseli[i][j];
		}
	}

	//podobnny do filtra œredniej ruchomej z programu 1

	int pomocnicza[9];	//pomocnicza tablica
	/*
	instrukcja: "wartoœci pikseli pod mask¹ s¹ mno¿one przez odpowiednie wagi i sumowane, nastêpnie dzielone przez sumê wag"
	*/

	for (int i = 1; i < obraz->szerokosc_obrazu - 1; i = i + 1)
	{
		for (int j = 1; j < obraz->wysokosc_obrazu - 1; j = j + 1)
		{
			//wagi podane by³y w instrukcji
			//ni¿ej wszystkie mo¿liwoœci konfiguracji - okno 3x3
			pomocnicza[0] = 1 * tablica_tymczasowa[i - 1][j - 1];
			pomocnicza[1] = 2 * tablica_tymczasowa[i - 1][j];
			pomocnicza[2] = 1 * tablica_tymczasowa[i - 1][j + 1];
			pomocnicza[3] = 2 * tablica_tymczasowa[i][j - 1];
			pomocnicza[4] = 4 * tablica_tymczasowa[i][j];
			pomocnicza[5] = 2 * tablica_tymczasowa[i][j + 1];
			pomocnicza[6] = 1 * tablica_tymczasowa[i + 1][j - 1];
			pomocnicza[7] = 2 * tablica_tymczasowa[i + 1][j];
			pomocnicza[8] = 1 * tablica_tymczasowa[i + 1][j + 1];

			int srednia = (pomocnicza[0] + pomocnicza[1] + pomocnicza[2] + pomocnicza[3] + pomocnicza[4] + pomocnicza[5] + pomocnicza[6] + pomocnicza[7] + pomocnicza[8]) / (1 + 2 + 1 + 2 + 4 + 2 + 1 + 2 + 1);	//dzielone przez sumê wag
			obraz->ilosc_pikseli[i][j] = srednia;
		}
	}

	//zwalniamy tablicê tymczasow¹
	for (int i = 0; i < obraz->wysokosc_obrazu; i = i + 1)
	{
		free(tablica_tymczasowa[i]);
	}
	free(tablica_tymczasowa);
}

int main()
{
	int ilosc_obrazow = 0;
	struct obraz* lista_obrazow = NULL; //deklaracja zmiennej strukturalnej, jest to tablica, w której bêdziemy przechowywaæ struktury
										//wskaŸnik, aby mo¿na by³o ³atwo edytowaæ ewentualne zmiany
										//to jest w³aœnie nasza dynamiczna baza do przechowywania w pamiêci i obs³ugi obrazów
										//NULL, bo na poczatku po odpaleniu programu nie ma zadnego obrazu na liscie, jest ona pusta
	setlocale(LC_ALL, "polish_poland");	//do polskich znaków
	srand(time(NULL));					//liczby pseudolosowe - losowanie

	int ktory;							//który obraz jest aktywny?
	int wybor;
	printf("				MENU WYBORU\nProszê wybraæ jedn¹ z poni¿szych opcji i zatwierdziæ klawiszem enter:\n");
	do
	{
		printf("1. Dodanie obrazu.\n2. Wyœwietlenie listy obrazów.\n3. Zapis wprowadzonych zmian.\n4. Usuniêcie obrazu z listy.\n5. Edycja obrazu.\n6. Zakoñczenie pracy programu.\n");

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
					printf("Niepoprawna wartoœæ. Proszê podaæ liczbê z przedzia³u od 1 do 6.\n");
				}
			}
		} while (wybor <= 0);

		switch (wybor)
		{
		case 1:
		{
			printf("\nWybór 1. Dodanie obrazu.\n");
			ilosc_obrazow = ilosc_obrazow + 1;														//zwiêkszamy liczbê obrazów o 1
			//"Chc¹c alokowaæ, realokowaæ lub zwalniaæ pamiêæ w osobnej funkcji, musisz przekazaæ adres do wskaŸnika, na którym pamiêæ bêdzie alokowana.Czyli: dla tablic jednowymiarowych przekazuje siê wskaŸnik na wskaŸnik" - instrukcja
			struct obraz* lista_obrazow_tymczasowa;													//realokujemy pamiêæ, bo wczytujemy obraz, wiêc potrzebna nam ta pamiêæ
			lista_obrazow_tymczasowa = realloc(lista_obrazow, ilosc_obrazow * sizeof(struct obraz));//nowy przydzia³ pamiêci, powiêksza siê obszar tablicy, pierwszy parametr to adres pocz¹tku tablicy utworzonej za pomoc¹ funkcji malloc(), drugi to nowy rozmiar pamiêci w bajtach
			if (lista_obrazow_tymczasowa != NULL)													//bardzo wa¿ne zabezpieczenie przed utrat¹ danych
			{
				lista_obrazow = lista_obrazow_tymczasowa;
			}
			else
			{
				printf("Nie uda³o siê powiêkszyæ tablicy.\n");
				lista_obrazow = malloc(1 * sizeof(double));											//nie uda³o siê powiêkszyæ, a wiêc tablica ma jeden element
			}
			dodawanie_obrazu(&lista_obrazow[ilosc_obrazow - 1]);									//wywo³anie funkcji
			break;																					//iloœæ jest pomniejszona o 1, bo numerowanie w tablicy jest od 0, a iloœæ normalnie od 1
		}																							//do funkcji przekazujê wskaŸnik na tablicê, wiêc ¿eby wyj¹æ wartoœæ trzeba daæ &

		case 2:
		{
			if (ilosc_obrazow >= 1)																	//tylko jeœli w ogóle mamy jakiœ obraz to wykonujemy
			{
				printf("\nWybór 2. Wyœwietlenie lisy obrazów:\n");
				for (int i = 0; i < ilosc_obrazow; i = i + 1)
				{
					printf("%d. %s\n", i + 1, lista_obrazow[i].nazwa_obrazu);						//%d do numeracji obrazu, %s do napisów; i + 1 bo gdy i jest 0 to numer obrazu to 1, a jednoczesnie pierwszy element to zerowy numer w tablicy
				}
			}

			else
				printf("Brak obrazów do wyœwietlenia. Proszê najpierw wybraæ opcjê 1 i dodaæ obrazy.\n");
			break;
		}
	
		case 3:
		{
			if (ilosc_obrazow >= 1)
			{
				printf("\nWybór 3. Zapis wprowadzonych zmian.\nKtóry obraz chcesz zapisaæ?\n");
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
							printf("Niepoprawna wartoœæ. Proszê podaæ liczbê spoœród wypisanych wy¿ej.\n");
						}
					}
				} while (ktory <= 0);

				if (ktory > 0 && ktory <= ilosc_obrazow)						//musi byæ wiêkszy od 0 i mniejszy od posiadanej iloœci
				{
					zapisz_obraz(&lista_obrazow[ktory - 1]);					//iloœæ jest pomniejszona o 1, bo numerowanie w tablicy jest od 0, a iloœæ normalnie od 1
					break;
				}
				else
				{
					printf("W spisie nie istnieje obraz o podanym numerze. Proszê najpierw dodaæ obraz do listy.\n");
					break;
				}
			}
			else
			{
				printf("W spisie nie ma jeszcze obrazów. Proszê najpierw dodaæ obraz do listy\n");
				break;
			}
		}
		
		case 4:
		{
			if (ilosc_obrazow >= 1)
			{
				printf("\nWybór 4. Usuniêcie obrazu z listy.\nKtóry obraz chcesz usun¹æ?\n");
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
							printf("Niepoprawna wartoœæ. Proszê podaæ liczbê spoœród wypisanych wy¿ej.\n");
						}
					}
				} while (ktory <= 0);

				if (ktory > 0 && ktory <= ilosc_obrazow)
				{
					if (ktory == ilosc_obrazow)							//gdy jest tylko jeden obraz i jeden usuwamy
					{
						usun_obraz(&lista_obrazow[ilosc_obrazow - 1]);	//iloœæ jest pomniejszona o 1, bo numerowanie w tablicy jest od 0, a iloœæ normalnie od 1
						ilosc_obrazow = ilosc_obrazow - 1;				//zmniejszamy liczbê obrazów, bo jeden usunêliœmy

						//zabezpieczenie
						struct obraz* lista_obrazow_tymczasowa;													//realokujemy pamiêæ, bo wczytujemy obraz, wiêc potrzebna nam ta pamiêæ
						lista_obrazow_tymczasowa = realloc(lista_obrazow, ilosc_obrazow * sizeof(struct obraz));//nowy przydzia³ pamiêci, powiêksza siê obszar tablicy, pierwszy parametr to adres pocz¹tku tablicy utworzonej za pomoc¹ funkcji malloc(), drugi to nowy rozmiar pamiêci w bajtach
						if (lista_obrazow_tymczasowa != NULL)													//bardzo wa¿ne zabezpieczenie przed utrat¹ danych
						{
							lista_obrazow = lista_obrazow_tymczasowa;
						}
						else
						{
							lista_obrazow = malloc(1 * sizeof(double));					//nie uda³o siê powiêkszyæ, a wiêc tablica ma jeden element
						}
					}
					else																//gdy jest wiêcej ni¿ jeden obraz
					{
						struct obraz zamieniony;
						zamieniony = lista_obrazow[ilosc_obrazow - 1];					//za zamienionego dajemy ostatni obraz z listy
						lista_obrazow[ilosc_obrazow - 1] = lista_obrazow[ktory - 1];	//za ostatni dajemy ten, który chcemy usun¹æ
						lista_obrazow[ktory - 1] = zamieniony;							//na miejsce usuniêtego dajemy zamienionego (tego, który by³ wczeœniej jako ostatni)

						usun_obraz(&lista_obrazow[ilosc_obrazow - 1]);					//iloœæ jest pomniejszona o 1, bo numerowanie w tablicy jest od 0, a iloœæ normalnie od 1
						ilosc_obrazow = ilosc_obrazow - 1;								//zmniejszamy liczbê obrazów, bo jeden usunêliœmy

						//zabezpieczenie
						struct obraz* lista_obrazow_tymczasowa;													//realokujemy pamiêæ, bo wczytujemy obraz, wiêc potrzebna nam ta pamiêæ
						lista_obrazow_tymczasowa = realloc(lista_obrazow, ilosc_obrazow * sizeof(struct obraz));//nowy przydzia³ pamiêci, powiêksza siê obszar tablicy, pierwszy parametr to adres pocz¹tku tablicy utworzonej za pomoc¹ funkcji malloc(), drugi to nowy rozmiar pamiêci w bajtach
						if (lista_obrazow_tymczasowa != NULL)													//bardzo wa¿ne zabezpieczenie przed utrat¹ danych
						{
							lista_obrazow = lista_obrazow_tymczasowa;
						}
						else
						{
							lista_obrazow = malloc(1 * sizeof(double));											//nie uda³o siê powiêkszyæ, a wiêc tablica ma jeden element
						}
					}
				}
				else
				{
					printf("W spisie nie istnieje obraz o podanym numerze. Proszê najpierw dodaæ obraz do listy.\n ");
				}
				break;
			}
			else
			{
				printf("W spisie nie ma jeszcze obrazów. Proszê najpierw dodaæ obraz do listy.\n");
				break;
			}
		}
		
		case 5:
		{
			printf("\nWybór 5. Edycja obrazu.\n");
			if (ilosc_obrazow >= 1)
			{
				printf("\nKtóry obraz bêdzie przeznaczony do edycji?\n");
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
							printf("Niepoprawna wartoœæ. Proszê podaæ liczbê spoœród wypisanych wy¿ej.\n");
						}
					}
				} while (ktory <= 0);

				if (ktory > 0 && ktory <= ilosc_obrazow)
				{
					int wybor2;
					do
					{
						printf("\nProszê wybraæ jedn¹ z poni¿szych opcji i zatwierdziæ klawiszem enter:\n");
						printf("1. Obrót o 90 stopni.\n2. Obrót o 180 stopni.\n3. Obrót o 270 stopni.\n4. Histogram z zapisem do pliku .CSV.\n5. Progowanie z dowolnym progiem.\n6. Dodawanie szumu typu pieprz i sól.\n7. Rozmycie za pomoc¹ filtru Gaussa.\n8. Cofnij.\n");
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
									printf("Niepoprawna wartoœæ. Proszê podaæ liczbê z przedzia³u od 1 do 8.\n");
								}
							}
						} while (wybor2 <= 0);

						switch (wybor2)
						{
						case 1:
						{
							printf("Wybór 1. Obrót o 90 stopni.\n");
							obrot_o_k90(&lista_obrazow[ktory - 1]);	//czemu wybor - 1, bo gdy np bêdzie to obraz nr 2 to wybor = 2, ale jednoczeœnie zajmuje on miejsce pierwsze w tablicy, bo zerowe zajmuje obraz nr 1
							printf("Dokonano obrotu o 90 stopni.\n");
							break;
						}

						case 2:										//po prostu dwa obroty, wtedy bêdzie 180 stopni
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
							printf("Wybór 7. Rozmycie za pomoc¹ filtru Gaussa.\n");
							filtr_gaussa(&lista_obrazow[ktory - 1]);
							printf("Dokonano rozmycia obrazu za pomoc¹ filtru Gaussa.\n");
							break;
						}

						case 8:
						{
							printf("Wybór 8. Cofnij.\n");
							break;
						}

						default:
						{
							printf("Niestety takiej opcji nie ma w menu wyboru. Proszê podaæ ponownie:\n");
							break;
						}
						}
					} while (wybor2 != 8);
					break;
				}
				else
				{
					printf("W spisie nie istnieje obraz o podanym numerze. Proszê najpierw dodaæ obraz do listy.\n");
					break;
				}
			}
			else
			{
				printf("W spisie nie ma jeszcze obrazów. Proszê najpierw dodaæ obraz do listy.\n");
				break;
			}
		}

		case 6:
		{
			printf("\nWybór: 6. Zakoñczenie pracy w programie.\n");
			//"Pamiêtaj o zwalnianiu pamiêci wszystkich obrazów, jak i samej bazy, przy wychodzeniu z programu"
			if (ilosc_obrazow > 0)	//zwalniamy pamiêæ
			{
				for (int i = ilosc_obrazow - 1; i >= 0; i = i - 1) //iloœæ jest pomniejszona o 1, bo numerowanie w tablicy jest od 0, a iloœæ normalnie od 1
				{
					usun_obraz(&lista_obrazow[i]);
				}
				free(lista_obrazow);
			}
			break;
		}

		default:
		{
			printf("Niestety takiej opcji nie ma w menu wyboru. Proszê podaæ ponownie:\n");
		}
		}
		printf("\n");
	} while (wybor != 6);
	return 0;
}