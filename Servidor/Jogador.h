#include "Main.h"

typedef struct Jogador{
	int vida;
	int lentidao;
	int pedras;//unico objecto que pode acumular até 15 pedras

	//posicao
	int posx;
	int posy;
}Jogador;

void ConstrutorJogador(Jogador *j);

//funçoes de construçao do jogador e comandos do jogador