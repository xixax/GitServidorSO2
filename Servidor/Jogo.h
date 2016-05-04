#include "Mapa.h"
int total;
Jogador *jogadores;//todos os jogadores - mais facil para modificar os parametros depois

typedef struct Jogos{
	Mapa **mapa;
	Jogador jogador;//utilizado para enviar o jogador do cliente
	//monstro
	// _TCHARbuf[256];
}Jogo;

//fase de testes o mapa vai ser 70*70
void ConstrutorJogo(Jogo *j);
void inicializacaoMatriz(Jogo *j);
void preencheMatriz(Jogo *j);
void actualizaJogo(Jogo *j);
void MovimentoJogador(Mapa **mapa, Jogador *j, int comando);


//funcao de inicializacao da matriz que vai ser o mapa e do relogio
//relogio ainda por descobrir como fazer - talvez por thread do cliente a dormir por x tempo