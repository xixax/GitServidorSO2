#include "Main.h"

typedef struct Mensagem{
	_TCHAR Username[30];
	_TCHAR Password[30];
	int sucesso;
	_TCHAR recorde1[10], recorde2[10], recorde3[10], recorde4[10], recorde5[10];
	int comando;//0-cima 1-baixo 2-esquerda 3-direita 4-Faz Login 5-Faz Registo 6-Criar Jogo 7-Juntar a jogo 8-Começa jogo 9-Termina Jogo
}Mensagem;

