#include "Jogo.h"

//fazer aqui as funçoes que vão inicializar o Jogo

void ConstrutorJogo(Jogo *j){
	inicializacaoMatriz(j);
	preencheMatriz(j);
}

//receber linhas e colunas se não for mapa pré-definido
void inicializacaoMatriz(Jogo *j){
	int x = 0;
	
	//mudar depois o valor 70
	Mapa auxmapa[70];
	j->mapa = auxmapa;
	for (x = 0; x < 70; x++){
		j->mapa[x] = auxmapa;//ver se isto inicializa bem
		for (int y = 0; y < 70; y++){
			ConstrutorMapa(&(j->mapa[x][y]));
		}
	}
}

//ver como fazer as salas
void preencheMatriz(Jogo *j){
	//enviar para aqui o tamanho do mapa
	//testes 70*70
	srand(time(NULL));//faz com que a funcao rand nao devolva sempre a mesma sequencia
	int tamanho = 70 * 70;
	//preencher primeiro os muros/por muros a volta do mapa
	for (int x = 0; x < 70; x++){
		j->mapa[x][0].muro = 1;
		j->mapa[x][69].muro = 1;
		j->mapa[0][x].muro = 1;
		j->mapa[69][x].muro = 1;
	}

	for (int x = 0; x < 70; x++){
		for (int y = 0; y < 70; y++){
			if (j->mapa[x][y].muro == 0){
				int random = (rand() + 1) % 100;
				if (random <= PercentagemCafeina){
					j->mapa[x][y].cafeina = 1;
				}
				else{
					int random = (rand() + 1) % 100;
					if (random <= PercentagemOrangeBull){
						j->mapa[x][y].orangebull = 1;
					}
					else{
						int random = (rand() + 1) % 100;
						if (random <= PercentagemPedra){
							j->mapa[x][y].pedras = 1;
						}
						else{
							int random = (rand() + 1) % 100;//0 a 100
							if (random <= PercentagemVitaminas){
								j->mapa[x][y].vitamina = 1;
							}
						}
					}
				}
			}
		}
	}


	//aqui colocar em diferentes posicoes os monstros e os jogadores
	int auxX, auxY;
	int tam = 0;

	for (int i = 0; i < tam; i++){
		do{
			auxX = rand() % 70;//0 a 69
			auxY = rand() % 70;//0 a 69
		} while (j->mapa[auxX][auxY].cafeina == 1 || j->mapa[auxX][auxY].muro == 1 || j->mapa[auxX][auxY].orangebull == 1 || j->mapa[auxX][auxY].pedras == 1 || j->mapa[auxX][auxY].vitamina == 1);

		j->jogadores[i].posx = auxX;
		j->jogadores[i].posy = auxY;

		j->mapa[auxX][auxY].jogador = &(j->jogadores[i]);//ver se esta bem / ter acerteza que jogadores esta inicializado
	}

	
}



void actualizaJogo(Jogo *j){
	//quando tem um jogador ao lado dele, ataca
	//quando esta na mesma posicao que um objecto apanha

}

void MovimentoJogador(Mapa **mapa, Jogador *j, int comando){
	//if comando==0 cima// comado==1 baixo // comando==2 esquerda // comando==3 direita

	switch (comando)
	{
	case 0:
		if (mapa[j->posx - 1][j->posy].muro == 0 && mapa[j->posx - 1][j->posy].jogador == NULL){//x-1 é para cima
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