#include "Jogador.h"
#include "Mapa.h"

void ConstrutorJogador(Jogador *j){
	//construir jogador
	j->lentidao = LentidaoJogador;
	j->vida = VidaJogador;
	j->pedras = 0;
}

void MovimentoJogador(Mapa **mapa,Jogador *j, int comando){
	//if comando==0 cima// comado==1 baixo // comando==2 esquerda // comando==3 direita

	switch (comando)
	{
	case 0:
		if (mapa[j->posx-1][j->posy].muro == 0 && mapa[j->posx-1][j->posy].jogador == NULL){//x-1 é para cima
			//transfere jogador
			mapa[j->posx - 1][j->posy].jogador = mapa[j->posx][j->posy].jogador;
			mapa[j->posx][j->posy].jogador = NULL;
			j->posx = j->posx - 1;//ver se e preciso fazer no mapa tambem
		}
		break;
	case 1:
		if (mapa[j->posx + 1][j->posy].muro == 0 && mapa[j->posx + 1][j->posy].jogador == NULL){//x+1 é para baixo
			//transfere jogador
			mapa[j->posx + 1][j->posy].jogador = mapa[j->posx][j->posy].jogador;
			mapa[j->posx][j->posy].jogador = NULL;
			j->posx = j->posx + 1;//ver se e preciso fazer no mapa tambem
		}
		break;
	case 2:
		if (mapa[j->posx][j->posy - 1].muro == 0 && mapa[j->posx][j->posy - 1].jogador == NULL){//y-1 é para a esquerda
			//transfere jogador
			mapa[j->posx][j->posy - 1].jogador = mapa[j->posx][j->posy].jogador;
			mapa[j->posx][j->posy].jogador = NULL;
			j->posy = j->posy - 1;//ver se e preciso fazer no mapa tambem
		}
		break;
	case 3:
		if (mapa[j->posx][j->posy + 1].muro == 0 && mapa[j->posx][j->posy + 1].jogador == NULL){//y+1 é para cima
			//transfere jogador
			mapa[j->posx][j->posy + 1].jogador = mapa[j->posx][j->posy].jogador;
			mapa[j->posx][j->posy].jogador = NULL;
			j->posy = j->posy + 1;//ver se e preciso fazer no mapa tambem
		}
		break;
	default:	
		break;
	}

}