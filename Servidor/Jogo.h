#include "Mapa.h"
int total;

typedef struct Jogos{
	Mapa **mapa;
	Jogador *jogadores;//todos os jogadores - mais facil para modificar os parametros depois
	//monstro
	//_TCHAR buf[256];
}Jogo;

//fase de testes o mapa vai ser 70*70
void ConstrutorJogo(Jogo *j);


//funcao de inicializacao da matriz que vai ser o mapa e do relogio
//relogio ainda por descobrir como fazer