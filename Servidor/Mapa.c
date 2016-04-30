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
}

////funçoes de construção do mapa e relógio