#include "Mapa.h"

void ConstrutorMapa(Mapa *m){
	//construcao
	m->cafeina = 0;
	m->orangebull = 0;
	m->pedras = 0;
	m->vitamina = 0;

	m->visibilidade = 0;

	m->muro = 0;

	m->jogador = NULL;
	m->monstro.clonado = -1;
	m->monstro.lentidao = -1;
	m->monstro.N = -1;
	m->monstro.posx = -1;
	m->monstro.posy = -1;
	m->monstro.sentido = -1;
	m->monstro.tipo = -1;
	m->monstro.vida = -1;
}

////funçoes de construção do mapa e relógio