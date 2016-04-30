#include "Jogador.h"

void ConstrutorJogador(Jogador *j){
	//construir jogador
	j->lentidao = LentidaoJogador;
	j->vida = VidaJogador;
	j->pedras = 0;
}

void MovimentoJogador(Jogador *j,int comando){
	//if comando==0 cima// comado==1 baixo // comando==2 esquerda // comando==3 direita
}