#include "Main.h"

typedef struct Jogador{
	int vida;
	int lentidao;
	int pedras;//unico objecto que pode acumular at� 15 pedras

	//posicao
	int posx;
	int posy;
}Jogador;

void ConstrutorJogador(Jogador *j);
void MovimentoJogador(Jogador *j, int comando);

//fun�oes de constru�ao do jogador e comandos do jogador