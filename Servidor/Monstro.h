#include "Main.h"

typedef struct Monstro{
	int tipo;  //0-distraido(anda ao calhas), 1-Bully

	int vida;  //comeca a 10, se chegar a 16 separa se e fica com 8
	int lentidao;//distraido:3 e bully:7

	int N;//saber quanto é que o distraido anda ate poder virar de direcao, recebida por argumentos
	int sentido;//0-cima 1-baixo 2-esquerda 3-direita
	int clonado;//0 para original, 1 para clone

	//posicao
	int posx;
	int posy;
}Monstro;