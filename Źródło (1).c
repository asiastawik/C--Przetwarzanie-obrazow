//trzeba zamienic wys z szer! 
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>	//standardowe wej�cie/wyj�cie
#include <stdlib.h>	//standardowe funkcje, rand(), malloc(), realloc(), itd.
#include <string.h> //do napis�w char
#include <locale.h>	//do funkcji setlocale()
#include <time.h>	//do funkcji obs�ugi czasu

struct obraz				//struktura, w tym przypadku jako zmienna globalna (poza main)
{							//jest to tylko deklaracja struktury, nie zmiennej strukturalnej
	char standard[2];		//standard musi by� P2
	int szerokosc_obrazu;	//ca�kowita liczba dodatnia, kolumny
	int wysokosc_obrazu;	//ca�kowita liczba dodatnia, wiersze
	int glebia_szarosci;	//maksymalna warto�� szaro�ci (g��bia szaro�ci) � od 1 do 65535
	int** ilosc_pikseli;	//przyjmuj�ce warto�� od 0 do maksymalnej warto�ci szaro�ci
							//wska�nik na wska�nik, bo potrzebna nam tablica dwuwymiarowa
	char nazwa_obrazu[20];	//powinno wystarczy�
};

void pobierz(FILE* plik)
{
	char piksel;
	char komentarz[50];				//zak�adam, �e komentarz mo�e mie� maksymalnie 50 znak�w
	do								//wykonuj dop�ki nie napotkamy na koniec linii, czy spacj�, czy tabulator (bia�y znak)
	{
		fscanf(plik, "%c", &piksel); //%c, bo char
	} while (piksel == 32 || piksel == 10 || piksel == 9);  //a� do napotkania bia�ego znaku: 32 to spacja, 10 koniec linii 9 to tabulacja, || - lub

	if (piksel == '#')				//# - komentarz; obowi�zuj� zawsze do ko�ca linii
	{
		fgets(komentarz, 50, plik); //funkcja fgets odczytuje znaki z pliku dop�ki nie odczyta podanej ilo�ci, nie napotka ko�ca linii lub ko�ca pliku; zapisuje znaki do podanej tablicy; przestawia pozycje w pliku za ostatni odczytany znak
		pobierz(plik);				//je�eli natrafimy na # to czytamy a� do ko�ca linii i to pomijamy, a potem wracamy do pocz�tku, czyli dalszej obs�ugi pliku, zastosowa�am rekurencj�
	}
	else							//je�li nie to czytaj warto�� kolejnego piksela
		fseek(plik, -1, SEEK_CUR);	//funkcja przesuwa pozycj� w pliku o podan� liczb� znak�w(int) (-1) wzgl�dem aktualnego miejsca (SEEK_CUR), czyli o 1 w prawo
}									//po ka�dym wywolaniu pobierz(plik) przechodzimy do kolejnego znaku w tym pliku, dzi�ki funkcji fseek i to jest ci�gle zapami�tywane

void dodawanie_obrazu(struct obraz* obraz)
{
	FILE* plik;									//utworzenie struktury FILE* (uchwytu, identyfikatora) pliku, do kt�rego b�dziemy si� odwo�ywa�
	do {
		printf("Prosz� poda� nazw� obrazu, kt�r� chcesz doda�:\n8x8\nangel\nbird\ncells\neuro\nfigury\nflir\ngears\ngradient\nmerc\nplane\robo\nteddy\ntest\nzurawie\nzurawie-mydlane\n\n");
		printf("Chc� doda�: ");
		scanf("%s", obraz->nazwa_obrazu);		//aby przekaza� zmienn� strukturaln� do funkcji gdzie mia�aby by� zmodyfikowana, nale�y to zrobi� przez wska�nik

		strcat(obraz->nazwa_obrazu, ".pgm");	//funkcja strcat scala dwa �a�cuchy znak�w w jeden; w naszym przypadku nazw� obrazu i rozszerzenie .pgm
		plik = fopen(obraz->nazwa_obrazu, "r");	//"r" � otwiera plik do odczytu, nazwa pliku � tablica znak�w char*; w tym miejscu mo�na poda� pe�n� �cie�k� pliku lub sam� nazw� pliku -> wtedy lokalizuje tam, gdzie jest program

		if (!plik)								//sprawdzamy czy plik istnieje
		{
			printf("\nB��d. Niestety nie uda�o si� otworzy� tego pliku. Zapis pliku nie b�dzie mo�liwy.\n");
		}
	} while (!plik);							//wykonuj dop�ki plik nie istnieje, tzn. je�li istnieje to zako�cz p�tl�!!!

	//"Poza pierwsz� lini�, mog� si� one pojawi� w ka�dym miejscu pliku." - dlatego przy odczycie standardu (pierwsza linia) nie zwracam uwagi na komentarze
	fscanf(plik, "%s", obraz->standard); //%s do struktur, fscanf czyta pojedynczy znak z pliku
	if (strcmp(obraz->standard, "P2") != 0) //funkcja strcmp por�wnuje napisy; zwraca warto�� zero, gdy jeden napis jest taki sam jak drugi
	{
		printf("Przepraszamy, ale niestety standard tego pliku jest niezgodny z wymaganym do odczytu.");
		exit(0);								//zako�czenie programu
	}

	//oczytujemy kolejn� lini�, czyli wymiary
	pobierz(plik);	//obsluga komentarzy
	fscanf(plik, "%d %d", &obraz->wysokosc_obrazu, &obraz->szerokosc_obrazu); //dwa razy %d bo dwie liczby

	//dynamiczna alokacja dwuwymiarowej macierzy dla pikseli obrazu + zabezpieczenie przez utrat� pami�ci
	obraz->ilosc_pikseli = malloc(obraz->szerokosc_obrazu * sizeof(int*));			//najpierw kolumny; alokujemy pamie� na liczb� kolumn w danej tablicy pikseli - st�d wska�nik na int
	//przechowywa� b�dziemy wska�niki (do odpowiednich tablic jednowymiarowych przechowuj�cych wiersze), dlatego jako argument funkcji sizeof() podajemy int *

	if (obraz->ilosc_pikseli != NULL)												//je�li mamy tyle wolnej pami�ci to wchodzimy do p�tli
	{
		for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
		{
			obraz->ilosc_pikseli[i] = malloc(obraz->wysokosc_obrazu * sizeof(int)); //teraz dynamicznie alokujemy wiersze; alokujemy dynamicznie na poszczeg�lne pola w danej kolumnie
		}
	}
	else																			//je�li nie ma pami�ci to zwalniamy j�
	{
		for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
		{
			free(obraz->ilosc_pikseli[i]);											//zwolnienie z wierszy
		}
		free(obraz->ilosc_pikseli);													//zwolnienie ca�ej tablicy, pierwotnej tablicy wska�nik�w do wierszy
		printf("\nB��d alokacji, przerwanie dzia�ania.\n");
		exit(0);																	//zako�czenie programu
	}

	//odczytujemy kolejn� lini�, czyli g��bi� szaro�ci
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
	fclose(plik);				//zamkni�cie pliku czy�ci bufory danych powi�zane z nim, zwalnia uchwyt oraz pozwala na otwieranie pliku na zewn�trz lub jego usuni�cie;
}

void zapisz_obraz(struct obraz* obraz)
{
	FILE* plik;					//utworzenie struktury FILE* (uchwytu, identyfikatora) pliku, do kt�rego b�dziemy si� odwo�ywa�

	printf("\nProsz� poda� now� nazw� obrazu: ");
	char nowy_obraz[20];
	scanf("%s", nowy_obraz);	//&s odczytuje sekwencje znak�w nie b�d�cych bia�ymi znakami, argument powinien by� wska�nikiem (czyli tablic�) na char
	printf("\n");
	strcat(nowy_obraz, ".pgm");		//funkcja strcat scala dwa �a�cuchy znak�w w jeden; w tym przypadku now� nazw� obrazu i rozszerzenie .pgm
									//otwarcie/utworzenie pliku oraz przypisanie go do utworzonego �uchwytu�. Funkcja fopen() przyjmuje dwa parametry : nazw� pliku i tryb obs�ugi pliku
	plik = fopen(nowy_obraz, "w");	//"w" � otwiera/tworzy plik do zapisu, usuwa jego star� tre��

	//teraz zapisujemy warto�ci, zmienione w edycji lub nie
	fprintf(plik, "%s\n", obraz->standard);										//zapisujemy w pliku w pierwszej linii standard, \n - nowa linia, bia�y znak
	fprintf(plik, "%d %d\n", obraz->wysokosc_obrazu, obraz->szerokosc_obrazu);	//w drugiej linii zapisujemy wymiary obrazu
	fprintf(plik, "%d\n", obraz->glebia_szarosci);								//w trzeciej zapisujemy g��bi� szaro�ci

	for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)						//zapis pikseli, najpierw kolumny
	{
		for (int j = 0; j < obraz->wysokosc_obrazu; j = j + 1)					//teraz wiersze
		{
			fprintf(plik, "%d ", obraz->ilosc_pikseli[i][j]);					//liczba + spacja
		}
		fprintf(plik, "\n");													//wstawiamy do pliku now� lini�
	}
	printf("Obraz pomy�lnie zapisano do pliku.\n");
}

void usun_obraz(struct obraz* obraz)
{
	for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
	{
		free(obraz->ilosc_pikseli[i]);										//zwolnienie z wierszy
	}
	free(obraz->ilosc_pikseli);												//zwolnienie ca�ej tablicy, pierwotnej tablicy wska�nik�w do wierszy
	printf("Obraz usuni�to.\n");
}

void obrot_o_k90(struct obraz* obraz)
{
	int** tablica_tymczasowa;
	tablica_tymczasowa = malloc(obraz->szerokosc_obrazu * sizeof(int*));			//kolumny; przechowywa� b�dziemy wska�niki (do odpowiednich tablic jednowymiarowych przechowuj�cych wiersze), dlatego jako argument funkcji sizeof() podajemy int *
	if (tablica_tymczasowa != NULL)													//je�li mamy tyle wolnej pami�ci to wchodzimy do p�tli
	{
		for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
		{
			tablica_tymczasowa[i] = malloc(obraz->wysokosc_obrazu * sizeof(int));	//teraz dynamicznie alokujemy wiersze; alokujemy dynamicznie na poszczeg�lne pola w danej kolumnie
		}
	}
	else																			//je�li nie ma pami�ci to zwalniamy j�
	{
		for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
		{
			free(tablica_tymczasowa[i]);											//zwolnienie z wierszy
		}
		free(tablica_tymczasowa);													//zwolnienie ca�ej tablicy, pierwotnej tablicy wska�nik�w do wierszy
		printf("B��d alokacji, przerwanie dzia�ania.\n");
		exit(0);	//zako�czenie programu
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
		obraz->ilosc_pikseli = malloc(obraz->szerokosc_obrazu * sizeof(int*));			//najpierw kolumny; alokujemy pamie� na liczb� kolumn w danej tablicy pikseli - st�d wska�nik na int
		//przechowywa� b�dziemy wska�niki (do odpowiednich tablic jednowymiarowych przechowuj�cych wiersze), dlatego jako argument funkcji sizeof() podajemy int *

		if (obraz->ilosc_pikseli != NULL)												//je�li mamy tyle wolnej pami�ci to wchodzimy do p�tli
		{
			for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
			{
				obraz->ilosc_pikseli[i] = malloc(obraz->wysokosc_obrazu * sizeof(int)); //teraz dynamicznie alokujemy wiersze; alokujemy dynamicznie na poszczeg�lne pola w danej kolumnie
			}
		}
		else																			//je�li nie ma pami�ci to zwalniamy j�
		{
			for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
			{
				free(obraz->ilosc_pikseli[i]);											//zwolnienie z wierszy
			}
			free(obraz->ilosc_pikseli);													//zwolnienie ca�ej tablicy, pierwotnej tablicy wska�nik�w do wierszy
			printf("B��d alokacji, przerwanie dzia�ania.\n");
			exit(0);																	//zako�czenie programu
		}
	}

	//obr�t w lewo
	for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
	{
		for (int j = 0; j < obraz->wysokosc_obrazu; j = j + 1)
		{
			obraz->ilosc_pikseli[i][j] = tablica_tymczasowa[obraz->wysokosc_obrazu - 1 - j][i];
			/*
			dlaczego tak? bo na przyk�ad gdy mamy tablic� 3x2, dostaniemy tablic� 2x3 i teraz dla i=0, j=0 w nowej tablicy chcemy
			pobra� warto�� z [1][0] ze starej, wi�c bierzemy [(wysokosc = 2) - 1 - (j = 0)][i = 0], czyli [1][0] jak chcieli�my
			*/
		}
	}

	//zwalniamy tablic� tymczasow�
	for (int i = 0; i < obraz->wysokosc_obrazu; i = i + 1)
	{
		free(tablica_tymczasowa[i]);
	}
	free(tablica_tymczasowa);
}

void histogram_zapis(struct obraz* obraz)		//histogram m�wi nam ile pikseli danego koloru znajduje si� na obrazie
{
	FILE* plik;
	printf("Prosz� poda� nazw� pliku:\n");
	char nazwa_pliku[20];
	scanf("%s", nazwa_pliku);					//&s odczytuje sekwencje znak�w nie b�d�cych bia�ymi znakami, argument powinien by� wska�nikiem (czyli tablic�) na char
	strcat(nazwa_pliku, ".csv");				//funkcja strcat scala dwa �a�cuchy znak�w w jeden; w tym przypadku now� nazw� obrazu i rozszerzenie .csv
	//otwarcie/utworzenie pliku oraz przypisanie go do utworzonego �uchwytu�. Funkcja fopen() przyjmuje dwa parametry : nazw� pliku i tryb obs�ugi pliku
	plik = fopen(nazwa_pliku, "w");				//"w" � otwiera/tworzy plik do zapisu, usuwa jego star� tre��

	//"Histogram tworzy si� przez zsumowanie wszystkich pikseli dla ka�dego odcienia szaro�ci(np. do tablicy)"
	for (int piksel = 0; piksel < obraz->glebia_szarosci; piksel = piksel + 1)
	{
		int odcien_szarosci = 0;				//ile ich jest, nie odcie�
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
	fclose(plik);											//zamkni�cie pliku czy�ci bufory danych powi�zane z nim, zwalnia uchwyt oraz pozwala na otwieranie pliku na zewn�trz lub jego usuni�cie;
}

void progowanie(struct obraz* obraz)	//"Polega on na rozdziale obrazu na dwie cz�ci � bia�� i czarn�, wzgl�dem poziomu szaro�ci pikseli."
{
	int poziom_szarosci;
	do
	{
		printf("Zakres: [0; %d]", obraz->glebia_szarosci);
		printf("\nProsz� poda� pr�g z zakresu.\n");
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
				printf("Niepoprawna warto��. Prosz� poda� warto�� progu z zakresu.\n");
			}
		}
	} while (poziom_szarosci < 0 || poziom_szarosci > obraz->glebia_szarosci);

	//rozdzia� obrazu na dwie cz�ci � bia�� i czarn�, wzgl�dem poziomu szaro�ci pikseli
	for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
	{
		for (int j = 0; j < obraz->wysokosc_obrazu; j = j + 1)
		{
			if (obraz->ilosc_pikseli[i][j] <= poziom_szarosci)	//je�li dany piksel (jego g�ebia szaro�ci) jest mniejszy lub r�wny obranemu poziomowi to przyjmujemy go jako bia�y - 0
			{
				obraz->ilosc_pikseli[i][j] = 0;
			}
			else												//je�li g��bia szaro�ci jest wi�ksza od obranego poziomu to piksel przyjmuje warto�� max czyli warto�� g��bi szaro�ci
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
		printf("\nProsz� poda� maksymaln� warto�� procentow� zaszumionych pikseli:\n");
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
				printf("Niepoprawna warto��. Warto�� procentowa mo�e wynosi� od 0 do 100%.\n");
			}
		}
	} while (procent_pikseli < 0 || procent_pikseli >= 100);
	*/
	
	//losowanie automatyczne:
	procent_pikseli = rand() % 101 / 10;								//wylosowanie liczby z zakresu od 1 do 100, dzielone przez 10 - aby nie by�o za du�o
																		//po wielu pr�bach dzielenie przez 10 by�o optymalnym rozwi�zaniem
	for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
	{
		for (int j = 0; j < obraz->wysokosc_obrazu; j = j + 1)
		{
			int szum = rand() % 401;									//im wi�kszy zakres tym mniejsza szansa na zaszumienie danego piksela, zakres od 1 do 400 dawa� optymalne rozwi�zanie
			/*
			wylosowanie liczby z zakresu od 1 do 400, okre�la, kt�ry akurat piksel b�dzie zaszumiony;
			gdy np. wylosuje si� liczba 346 i jest ona du�o wi�ksza od wylosowanych pikseli (kt�rych jest np. 20) to
			danego piksela nie zaszumiamy, a gdy wylosuje si� np. liczba 20 to dany piksel zostanie zaszumiony
			*/
			if (szum <= procent_pikseli)
			{
				if (szum <= 2 * procent_pikseli / 3)					//zak�adamy, �e 2/3 zaszumionych pikseli b�dzie bia�a, a reszta czarna
					obraz->ilosc_pikseli[i][j] = 0;						//bia�y piksel

				else
					obraz->ilosc_pikseli[i][j] = obraz->glebia_szarosci; //czarny piksel
			}
		}
	}
}

void filtr_gaussa(struct obraz* obraz)
{
	int** tablica_tymczasowa;
	tablica_tymczasowa = malloc(obraz->szerokosc_obrazu * sizeof(int*));	//kolumny; przechowywa� b�dziemy wska�niki (do odpowiednich tablic jednowymiarowych przechowuj�cych wiersze), dlatego jako argument funkcji sizeof() podajemy int *
	if (tablica_tymczasowa != NULL)											//je�li mamy tyle wolnej pami�ci to wchodzimy do p�tli
	{
		for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
		{
			tablica_tymczasowa[i] = malloc(obraz->wysokosc_obrazu * sizeof(int));//teraz dynamicznie alokujemy wiersze; alokujemy dynamicznie na poszczeg�lne pola w danej kolumnie
		}
	}
	else																		//je�li nie ma pami�ci to zwalniamy j�
	{
		for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
		{
			free(tablica_tymczasowa[i]);										//zwolnienie z wierszy
		}
		free(tablica_tymczasowa);												//zwolnienie ca�ej tablicy, pierwotnej tablicy wska�nik�w do wierszy
		printf("B��d alokacji, przerwanie dzia�ania.\n");
		exit(0);	//zako�czenie programu
	}

	//przepisujemy obraz do tymczasowej
	for (int i = 0; i < obraz->szerokosc_obrazu; i = i + 1)
	{
		for (int j = 0; j < obraz->wysokosc_obrazu; j = j + 1)
		{
			tablica_tymczasowa[i][j] = obraz->ilosc_pikseli[i][j];
		}
	}

	//podobnny do filtra �redniej ruchomej z programu 1

	int pomocnicza[9];	//pomocnicza tablica
	/*
	instrukcja: "warto�ci pikseli pod mask� s� mno�one przez odpowiednie wagi i sumowane, nast�pnie dzielone przez sum� wag"
	*/

	for (int i = 1; i < obraz->szerokosc_obrazu - 1; i = i + 1)
	{
		for (int j = 1; j < obraz->wysokosc_obrazu - 1; j = j + 1)
		{
			//wagi podane by�y w instrukcji
			//ni�ej wszystkie mo�liwo�ci konfiguracji - okno 3x3
			pomocnicza[0] = 1 * tablica_tymczasowa[i - 1][j - 1];
			pomocnicza[1] = 2 * tablica_tymczasowa[i - 1][j];
			pomocnicza[2] = 1 * tablica_tymczasowa[i - 1][j + 1];
			pomocnicza[3] = 2 * tablica_tymczasowa[i][j - 1];
			pomocnicza[4] = 4 * tablica_tymczasowa[i][j];
			pomocnicza[5] = 2 * tablica_tymczasowa[i][j + 1];
			pomocnicza[6] = 1 * tablica_tymczasowa[i + 1][j - 1];
			pomocnicza[7] = 2 * tablica_tymczasowa[i + 1][j];
			pomocnicza[8] = 1 * tablica_tymczasowa[i + 1][j + 1];

			int srednia = (pomocnicza[0] + pomocnicza[1] + pomocnicza[2] + pomocnicza[3] + pomocnicza[4] + pomocnicza[5] + pomocnicza[6] + pomocnicza[7] + pomocnicza[8]) / (1 + 2 + 1 + 2 + 4 + 2 + 1 + 2 + 1);	//dzielone przez sum� wag
			obraz->ilosc_pikseli[i][j] = srednia;
		}
	}

	//zwalniamy tablic� tymczasow�
	for (int i = 0; i < obraz->wysokosc_obrazu; i = i + 1)
	{
		free(tablica_tymczasowa[i]);
	}
	free(tablica_tymczasowa);
}

int main()
{
	int ilosc_obrazow = 0;
	struct obraz* lista_obrazow = NULL; //deklaracja zmiennej strukturalnej, jest to tablica, w kt�rej b�dziemy przechowywa� struktury
										//wska�nik, aby mo�na by�o �atwo edytowa� ewentualne zmiany
										//to jest w�a�nie nasza dynamiczna baza do przechowywania w pami�ci i obs�ugi obraz�w
										//NULL, bo na poczatku po odpaleniu programu nie ma zadnego obrazu na liscie, jest ona pusta
	setlocale(LC_ALL, "polish_poland");	//do polskich znak�w
	srand(time(NULL));					//liczby pseudolosowe - losowanie

	int ktory;							//kt�ry obraz jest aktywny?
	int wybor;
	printf("				MENU WYBORU\nProsz� wybra� jedn� z poni�szych opcji i zatwierdzi� klawiszem enter:\n");
	do
	{
		printf("1. Dodanie obrazu.\n2. Wy�wietlenie listy obraz�w.\n3. Zapis wprowadzonych zmian.\n4. Usuni�cie obrazu z listy.\n5. Edycja obrazu.\n6. Zako�czenie pracy programu.\n");

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
					printf("Niepoprawna warto��. Prosz� poda� liczb� z przedzia�u od 1 do 6.\n");
				}
			}
		} while (wybor <= 0);

		switch (wybor)
		{
		case 1:
		{
			printf("\nWyb�r 1. Dodanie obrazu.\n");
			ilosc_obrazow = ilosc_obrazow + 1;														//zwi�kszamy liczb� obraz�w o 1
			//"Chc�c alokowa�, realokowa� lub zwalnia� pami�� w osobnej funkcji, musisz przekaza� adres do wska�nika, na kt�rym pami�� b�dzie alokowana.Czyli: dla tablic jednowymiarowych przekazuje si� wska�nik na wska�nik" - instrukcja
			struct obraz* lista_obrazow_tymczasowa;													//realokujemy pami��, bo wczytujemy obraz, wi�c potrzebna nam ta pami��
			lista_obrazow_tymczasowa = realloc(lista_obrazow, ilosc_obrazow * sizeof(struct obraz));//nowy przydzia� pami�ci, powi�ksza si� obszar tablicy, pierwszy parametr to adres pocz�tku tablicy utworzonej za pomoc� funkcji malloc(), drugi to nowy rozmiar pami�ci w bajtach
			if (lista_obrazow_tymczasowa != NULL)													//bardzo wa�ne zabezpieczenie przed utrat� danych
			{
				lista_obrazow = lista_obrazow_tymczasowa;
			}
			else
			{
				printf("Nie uda�o si� powi�kszy� tablicy.\n");
				lista_obrazow = malloc(1 * sizeof(double));											//nie uda�o si� powi�kszy�, a wi�c tablica ma jeden element
			}
			dodawanie_obrazu(&lista_obrazow[ilosc_obrazow - 1]);									//wywo�anie funkcji
			break;																					//ilo�� jest pomniejszona o 1, bo numerowanie w tablicy jest od 0, a ilo�� normalnie od 1
		}																							//do funkcji przekazuj� wska�nik na tablic�, wi�c �eby wyj�� warto�� trzeba da� &

		case 2:
		{
			if (ilosc_obrazow >= 1)																	//tylko je�li w og�le mamy jaki� obraz to wykonujemy
			{
				printf("\nWyb�r 2. Wy�wietlenie lisy obraz�w:\n");
				for (int i = 0; i < ilosc_obrazow; i = i + 1)
				{
					printf("%d. %s\n", i + 1, lista_obrazow[i].nazwa_obrazu);						//%d do numeracji obrazu, %s do napis�w; i + 1 bo gdy i jest 0 to numer obrazu to 1, a jednoczesnie pierwszy element to zerowy numer w tablicy
				}
			}

			else
				printf("Brak obraz�w do wy�wietlenia. Prosz� najpierw wybra� opcj� 1 i doda� obrazy.\n");
			break;
		}
	
		case 3:
		{
			if (ilosc_obrazow >= 1)
			{
				printf("\nWyb�r 3. Zapis wprowadzonych zmian.\nKt�ry obraz chcesz zapisa�?\n");
				for (int i = 0; i < ilosc_obrazow; i = i + 1)					//wypisujemy obrazy, kt�re posiadamy
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
							printf("Niepoprawna warto��. Prosz� poda� liczb� spo�r�d wypisanych wy�ej.\n");
						}
					}
				} while (ktory <= 0);

				if (ktory > 0 && ktory <= ilosc_obrazow)						//musi by� wi�kszy od 0 i mniejszy od posiadanej ilo�ci
				{
					zapisz_obraz(&lista_obrazow[ktory - 1]);					//ilo�� jest pomniejszona o 1, bo numerowanie w tablicy jest od 0, a ilo�� normalnie od 1
					break;
				}
				else
				{
					printf("W spisie nie istnieje obraz o podanym numerze. Prosz� najpierw doda� obraz do listy.\n");
					break;
				}
			}
			else
			{
				printf("W spisie nie ma jeszcze obraz�w. Prosz� najpierw doda� obraz do listy\n");
				break;
			}
		}
		
		case 4:
		{
			if (ilosc_obrazow >= 1)
			{
				printf("\nWyb�r 4. Usuni�cie obrazu z listy.\nKt�ry obraz chcesz usun��?\n");
				for (int i = 0; i < ilosc_obrazow; i = i + 1)					//wypisujemy obrazy, kt�re posiadamy
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
							printf("Niepoprawna warto��. Prosz� poda� liczb� spo�r�d wypisanych wy�ej.\n");
						}
					}
				} while (ktory <= 0);

				if (ktory > 0 && ktory <= ilosc_obrazow)
				{
					if (ktory == ilosc_obrazow)							//gdy jest tylko jeden obraz i jeden usuwamy
					{
						usun_obraz(&lista_obrazow[ilosc_obrazow - 1]);	//ilo�� jest pomniejszona o 1, bo numerowanie w tablicy jest od 0, a ilo�� normalnie od 1
						ilosc_obrazow = ilosc_obrazow - 1;				//zmniejszamy liczb� obraz�w, bo jeden usun�li�my

						//zabezpieczenie
						struct obraz* lista_obrazow_tymczasowa;													//realokujemy pami��, bo wczytujemy obraz, wi�c potrzebna nam ta pami��
						lista_obrazow_tymczasowa = realloc(lista_obrazow, ilosc_obrazow * sizeof(struct obraz));//nowy przydzia� pami�ci, powi�ksza si� obszar tablicy, pierwszy parametr to adres pocz�tku tablicy utworzonej za pomoc� funkcji malloc(), drugi to nowy rozmiar pami�ci w bajtach
						if (lista_obrazow_tymczasowa != NULL)													//bardzo wa�ne zabezpieczenie przed utrat� danych
						{
							lista_obrazow = lista_obrazow_tymczasowa;
						}
						else
						{
							lista_obrazow = malloc(1 * sizeof(double));					//nie uda�o si� powi�kszy�, a wi�c tablica ma jeden element
						}
					}
					else																//gdy jest wi�cej ni� jeden obraz
					{
						struct obraz zamieniony;
						zamieniony = lista_obrazow[ilosc_obrazow - 1];					//za zamienionego dajemy ostatni obraz z listy
						lista_obrazow[ilosc_obrazow - 1] = lista_obrazow[ktory - 1];	//za ostatni dajemy ten, kt�ry chcemy usun��
						lista_obrazow[ktory - 1] = zamieniony;							//na miejsce usuni�tego dajemy zamienionego (tego, kt�ry by� wcze�niej jako ostatni)

						usun_obraz(&lista_obrazow[ilosc_obrazow - 1]);					//ilo�� jest pomniejszona o 1, bo numerowanie w tablicy jest od 0, a ilo�� normalnie od 1
						ilosc_obrazow = ilosc_obrazow - 1;								//zmniejszamy liczb� obraz�w, bo jeden usun�li�my

						//zabezpieczenie
						struct obraz* lista_obrazow_tymczasowa;													//realokujemy pami��, bo wczytujemy obraz, wi�c potrzebna nam ta pami��
						lista_obrazow_tymczasowa = realloc(lista_obrazow, ilosc_obrazow * sizeof(struct obraz));//nowy przydzia� pami�ci, powi�ksza si� obszar tablicy, pierwszy parametr to adres pocz�tku tablicy utworzonej za pomoc� funkcji malloc(), drugi to nowy rozmiar pami�ci w bajtach
						if (lista_obrazow_tymczasowa != NULL)													//bardzo wa�ne zabezpieczenie przed utrat� danych
						{
							lista_obrazow = lista_obrazow_tymczasowa;
						}
						else
						{
							lista_obrazow = malloc(1 * sizeof(double));											//nie uda�o si� powi�kszy�, a wi�c tablica ma jeden element
						}
					}
				}
				else
				{
					printf("W spisie nie istnieje obraz o podanym numerze. Prosz� najpierw doda� obraz do listy.\n ");
				}
				break;
			}
			else
			{
				printf("W spisie nie ma jeszcze obraz�w. Prosz� najpierw doda� obraz do listy.\n");
				break;
			}
		}
		
		case 5:
		{
			printf("\nWyb�r 5. Edycja obrazu.\n");
			if (ilosc_obrazow >= 1)
			{
				printf("\nKt�ry obraz b�dzie przeznaczony do edycji?\n");
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
							printf("Niepoprawna warto��. Prosz� poda� liczb� spo�r�d wypisanych wy�ej.\n");
						}
					}
				} while (ktory <= 0);

				if (ktory > 0 && ktory <= ilosc_obrazow)
				{
					int wybor2;
					do
					{
						printf("\nProsz� wybra� jedn� z poni�szych opcji i zatwierdzi� klawiszem enter:\n");
						printf("1. Obr�t o 90 stopni.\n2. Obr�t o 180 stopni.\n3. Obr�t o 270 stopni.\n4. Histogram z zapisem do pliku .CSV.\n5. Progowanie z dowolnym progiem.\n6. Dodawanie szumu typu pieprz i s�l.\n7. Rozmycie za pomoc� filtru Gaussa.\n8. Cofnij.\n");
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
									printf("Niepoprawna warto��. Prosz� poda� liczb� z przedzia�u od 1 do 8.\n");
								}
							}
						} while (wybor2 <= 0);

						switch (wybor2)
						{
						case 1:
						{
							printf("Wyb�r 1. Obr�t o 90 stopni.\n");
							obrot_o_k90(&lista_obrazow[ktory - 1]);	//czemu wybor - 1, bo gdy np b�dzie to obraz nr 2 to wybor = 2, ale jednocze�nie zajmuje on miejsce pierwsze w tablicy, bo zerowe zajmuje obraz nr 1
							printf("Dokonano obrotu o 90 stopni.\n");
							break;
						}

						case 2:										//po prostu dwa obroty, wtedy b�dzie 180 stopni
						{
							printf("Wyb�r 2. Obr�t o 180 stopni.\n");
							obrot_o_k90(&lista_obrazow[ktory - 1]);
							obrot_o_k90(&lista_obrazow[ktory - 1]);
							printf("Dokonano obrotu o 180 stopni.\n");
							break;
						}

						case 3:										//3 obroty
						{
							printf("Wyb�r 3. Obr�t o 270 stopni.\n");
							obrot_o_k90(&lista_obrazow[ktory - 1]);
							obrot_o_k90(&lista_obrazow[ktory - 1]);
							obrot_o_k90(&lista_obrazow[ktory - 1]);
							printf("Dokonano obrotu o 270 stopni.\n");
							break;
						}

						case 4:
						{
							printf("Wyb�r 4. Histogram z zapisem do pliku .CSV.\n");
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
							printf("Wyb�r 6. Dodawanie szumu typu pieprz i s�l.\n");
							szum_pieprz_sol(&lista_obrazow[ktory - 1]);
							printf("Dokonano zaszumienia typu pieprz i s�l.\n");
							break;
						}

						case 7:
						{
							printf("Wyb�r 7. Rozmycie za pomoc� filtru Gaussa.\n");
							filtr_gaussa(&lista_obrazow[ktory - 1]);
							printf("Dokonano rozmycia obrazu za pomoc� filtru Gaussa.\n");
							break;
						}

						case 8:
						{
							printf("Wyb�r 8. Cofnij.\n");
							break;
						}

						default:
						{
							printf("Niestety takiej opcji nie ma w menu wyboru. Prosz� poda� ponownie:\n");
							break;
						}
						}
					} while (wybor2 != 8);
					break;
				}
				else
				{
					printf("W spisie nie istnieje obraz o podanym numerze. Prosz� najpierw doda� obraz do listy.\n");
					break;
				}
			}
			else
			{
				printf("W spisie nie ma jeszcze obraz�w. Prosz� najpierw doda� obraz do listy.\n");
				break;
			}
		}

		case 6:
		{
			printf("\nWyb�r: 6. Zako�czenie pracy w programie.\n");
			//"Pami�taj o zwalnianiu pami�ci wszystkich obraz�w, jak i samej bazy, przy wychodzeniu z programu"
			if (ilosc_obrazow > 0)	//zwalniamy pami��
			{
				for (int i = ilosc_obrazow - 1; i >= 0; i = i - 1) //ilo�� jest pomniejszona o 1, bo numerowanie w tablicy jest od 0, a ilo�� normalnie od 1
				{
					usun_obraz(&lista_obrazow[i]);
				}
				free(lista_obrazow);
			}
			break;
		}

		default:
		{
			printf("Niestety takiej opcji nie ma w menu wyboru. Prosz� poda� ponownie:\n");
		}
		}
		printf("\n");
	} while (wybor != 6);
	return 0;
}