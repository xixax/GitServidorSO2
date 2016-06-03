#include "Main.h"

typedef struct Monstro{
	int tipo;  //0-distraido(anda ao calhas), 1-Bully

	int vida;  //comeca a 10, se chegar a 16 separa se e fica com 8
	int lentidao;//distraido:3 e bully:7

	//posicao
	int posx;
	int posy;
}Monstro;