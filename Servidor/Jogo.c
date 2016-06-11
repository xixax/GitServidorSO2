#include "Jogo.h"



void ConstrutorJogo(Jogo *j){
	inicializacaoMatriz(j);
	preencheMatriz(j);
	j->jogocomecou = 0;
	
	for (int i = 0; i < total; i++){//poe o vector jogadores todos para null
		jogadores[i] = NULL;
	}
}

//receber linhas e colunas se não for mapa pré-definido
void inicializacaoMatriz(Jogo *j){
	//mudar depois o valor 70
	
	j->mapa = malloc(sizeof(Mapa) *70 * 70);
	for (int x = 0; x < 70; x++){
		for (int y = 0; y < 70; y++){
			//ConstrutorMapa(&auxMapa[x*70 + y]);
			//Mapa auxmapa;
			ConstrutorMapa(&j->mapa[x * 70 + y]);
			//j->mapa[x * 70 + y] = auxmapa;
			//inicializacao monstro, para nao por mais um for
		}
	}
}


//ver como fazer as salas
//ACEDER AOS DADOS x*colunas+y
void preencheMatriz(Jogo *j){
	//enviar para aqui o tamanho do mapa
	//testes 70*70
	srand(time(NULL));//faz com que a funcao rand nao devolva sempre a mesma sequencia
	int tamanho = 70 * 70;
	//preencher primeiro os muros/por muros a volta do mapa
	for (int x = 0; x < 70; x++){
		j->mapa[x * 70 + 0].muro = 1;
		j->mapa[x * 70 + 69].muro = 1;
		j->mapa[0 * 70 + x].muro = 1;
		j->mapa[69 * 70 + x].muro = 1;
	}
	for (int x = 10; x < 70;x=x+10){
		for (int y = 0; y < 70; y++){
			j->mapa[x*70+y].muro = 1;
			j->mapa[y*70+x].muro = 1;
			if (y % 5 == 0){
				j->mapa[x*70+y].muro = 2;//por portas nas paredes
				j->mapa[y*70+x].muro = 2;//por portas nas paredes
			}
		}
	}
	//////////////////////////////////////////////////////////

	for (int x = 0; x < 70; x++){
		for (int y = 0; y < 70; y++){
			if (j->mapa[x*70+y].muro == 0){
				int random = (rand() + 1) % 100;
				if (random <= PercentagemCafeina){
					j->mapa[x*70+y].cafeina = 1;
				}
				else{
					int random = (rand() + 1) % 100;
					if (random <= PercentagemOrangeBull){
						j->mapa[x*70+y].orangebull = 1;
					}
					else{
						int random = (rand() + 1) % 100;
						if (random <= PercentagemPedra){
							j->mapa[x*70+y].pedras = 1;
						}
						else{
							int random = (rand() + 1) % 100;//0 a 100
							if (random <= PercentagemVitaminas){
								j->mapa[x*70+y].vitamina = 1;
							}
						}
					}
				}
			}
		}
	}
}

void adicionaJogadoresMapa(Jogo *j){
	//fazer funcao para isto
	//aqui colocar em diferentes posicoes os monstros e os jogadores
	int auxX, auxY;

	for (int i = 0; i < totalnojogo; i++){
		do{
			auxX = rand() % 70;//0 a 69
			auxY = rand() % 70;//0 a 69
			Mapa aux = j->mapa[auxX * 70 + auxY];
		} while (j->mapa[auxX*70+auxY].cafeina == 1 || j->mapa[auxX*70+auxY].muro == 1 || j->mapa[auxX*70+auxY].muro == 2 || j->mapa[auxX*70+auxY].orangebull == 1 || j->mapa[auxX*70+auxY].pedras == 1 || j->mapa[auxX*70+auxY].vitamina == 1 || j->mapa[auxX*70+auxY].jogador != NULL);

		jogadores[i]->posx = auxX;
		jogadores[i]->posy = auxY;

		j->mapa[auxX*70+auxY].jogador = &(jogadores[i]);//ver se esta bem / ter acerteza que jogadores esta inicializado
	}
}



void actualizaJogo(Jogo *j){
	//quando tem um jogador ao lado dele, ataca
	//quando esta na mesma posicao que um objecto apanha
	//o tamanho do mapa tem de estar em variaveis globais
	Mapa auxmapa=j->mapa[10*70 +10];
	for (int x = 0; x < totalnojogo; x++){
		if (j->mapa[(jogadores[x]->posx - 1) * 70 + jogadores[x]->posy].jogador != NULL){
			//por condicao se for pedra -2
			if (j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy].jogador->pedras >= 1){
				j->mapa[(jogadores[x]->posx - 1) * 70 + jogadores[x]->posy].jogador->vida = j->mapa[(jogadores[x]->posx - 1) + jogadores[x]->posy].jogador->vida - 2;
				j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy].jogador->pedras = j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy].jogador->pedras - 1;
			}
			else{
				j->mapa[(jogadores[x]->posx - 1) * 70 + jogadores[x]->posy].jogador->vida = j->mapa[(jogadores[x]->posx - 1) + jogadores[x]->posy].jogador->vida - 1;
			}
		}
		if (j->mapa[(jogadores[x]->posx + 1)*70 + jogadores[x]->posy].jogador != NULL){
			//por condicao se for pedra -2
			if (j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy].jogador->pedras >= 1){
				j->mapa[(jogadores[x]->posx + 1) * 70 + jogadores[x]->posy].jogador->vida = j->mapa[(jogadores[x]->posx + 1) * 70 + jogadores[x]->posy].jogador->vida - 2;
				j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy].jogador->pedras = j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy].jogador->pedras - 1;
			}
			else{
				j->mapa[(jogadores[x]->posx + 1) * 70 + jogadores[x]->posy].jogador->vida = j->mapa[(jogadores[x]->posx + 1) * 70 + jogadores[x]->posy].jogador->vida - 1;
			}
		}
		if (j->mapa[(jogadores[x]->posx)*70 + jogadores[x]->posy - 1].jogador != NULL){
			//por condicao se for pedra -2
			if (j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy].jogador->pedras >= 1){
				j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy - 1].jogador->vida = j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy - 1].jogador->vida - 2;
				j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy].jogador->pedras = j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy].jogador->pedras - 1;
			}
			else{
				j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy - 1].jogador->vida = j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy - 1].jogador->vida - 1;
			}
		}
		if (j->mapa[(jogadores[x]->posx)*70 + jogadores[x]->posy + 1].jogador != NULL){
			//por condicao se for pedra -2
			if (j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy].jogador->pedras >= 1){
				j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy + 1].jogador->vida = j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy + 1].jogador->vida - 2;
				j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy].jogador->pedras = j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy].jogador->pedras - 1;
			}
			else{
				j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy + 1].jogador->vida = j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy + 1].jogador->vida - 1;
			}
		}

		if (j->mapa[(jogadores[x]->posx)*70 + jogadores[x]->posy].cafeina == 1){
			if (jogadores[x]->lentidao == LentidaoJogador){
				jogadores[x]->lentidao = jogadores[x]->lentidao - TiraLentidaoCafeina;
				j->mapa[(jogadores[x]->posx)*70 + jogadores[x]->posy].cafeina = 0;
			}
		}
		if (j->mapa[(jogadores[x]->posx)*70 + jogadores[x]->posy].orangebull == 1){
			if (jogadores[x]->vida < VidaJogador * 2 && jogadores[x]->vida + VidaOrangeBull < VidaJogador * 2){
				jogadores[x]->vida = jogadores[x]->vida + VidaOrangeBull;
				j->mapa[(jogadores[x]->posx)*70 + jogadores[x]->posy].orangebull = 0;
			}
		}
		if (j->mapa[(jogadores[x]->posx)*70 + jogadores[x]->posy].pedras >0){
			do{
				jogadores[x]->pedras = jogadores[x]->pedras + 1;//apanha todas e não só 1
				j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy].pedras = j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy].pedras - 1;
			} while (jogadores[x]->pedras < 15 && j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy].pedras>0);
		}
		if (j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy].vitamina == 1){
			if (jogadores[x]->vida < VidaJogador * 2 && jogadores[x]->vida + VidaVitaminas < VidaJogador * 2){
				jogadores[x]->vida = jogadores[x]->vida + VidaVitaminas;
				j->mapa[(jogadores[x]->posx) * 70 + jogadores[x]->posy].vitamina = 0;
			}
		}
	}
}

void MovimentoJogador(Mapa *mapa, Jogador *j, int comando){
	//if comando==0 cima// comado==1 baixo // comando==2 esquerda // comando==3 direita
	Mapa aux;
	switch (comando)
	{
	case 0:
		if (mapa[(j->posx - 1)*70 + j->posy].muro != 1 && mapa[(j->posx - 1)*70+j->posy].jogador == NULL){//x-1 é para cima
			//transfere jogador
			mapa[(j->posx - 1)*70+ j->posy].jogador = mapa[(j->posx)*70+j->posy].jogador;
			mapa[(j->posx)*70+j->posy].jogador = NULL;
			j->posx = j->posx - 1;//ver se e preciso fazer no mapa tambem
		}
		break;
	case 1:
		if (mapa[(j->posx + 1)*70 +j->posy].muro != 1 && mapa[(j->posx + 1)*70 +j->posy].jogador == NULL){//x+1 é para baixo
			//transfere jogador
			mapa[(j->posx + 1)*70 +j->posy].jogador = mapa[(j->posx) *70 +j->posy].jogador;
			mapa[(j->posx)*70 + j->posy].jogador = NULL;
			j->posx = j->posx + 1;//ver se e preciso fazer no mapa tambem
		}
		break;
	case 2:
		if (mapa[(j->posx)*70 +j->posy - 1].muro != 1 && mapa[(j->posx)*70 +j->posy - 1].jogador == NULL){//y-1 é para a esquerda
			//transfere jogador
			mapa[(j->posx)*70 +j->posy - 1].jogador = mapa[(j->posx)*70 +j->posy].jogador;
			mapa[(j->posx)*70 +j->posy].jogador = NULL;
			j->posy = j->posy - 1;//ver se e preciso fazer no mapa tambem
		}
		break;
	case 3:
		aux = mapa[(j->posx) * 70 + j->posy + 1];
		if (mapa[(j->posx)*70 +j->posy + 1].muro != 1 && mapa[(j->posx)*70 +j->posy + 1].jogador == NULL){//y+1 é para cima
			//transfere jogador
			mapa[(j->posx)*70 + j->posy + 1].jogador = mapa[(j->posx)*70 +j->posy].jogador;
			mapa[(j->posx)*70 +j->posy].jogador = NULL;
			j->posy = j->posy + 1;//ver se e preciso fazer no mapa tambem
		}
		break;
	default:
		break;
	}

}