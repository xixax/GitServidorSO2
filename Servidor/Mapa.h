#include "Main.h"
#include "Jogador.h"

typedef struct Mapa{
	int visibilidade;//0 invisivel/1 visivel

	int muro;//0 se este bloco n�o for muro/ 1 se este bloco for muro

	Jogador *jogador;
	//monstro

	//objectos
	int pedras;//unico que pode ser mais que 1
	int vitamina;//max 1
	int orangebull;//max 1
	int cafeina;//max 1
}Mapa;

void ConstrutorMapa(Mapa *m);

//fun�oes de constru��o do mapa