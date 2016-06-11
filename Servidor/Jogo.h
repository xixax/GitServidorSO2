#include "AuxMapa.h"

int total;
int totalnojogo;
Jogador *jogadores[10];//todos os jogadores - mais facil para modificar os parametros depois

typedef struct Jogos{
	AuxMapa auxMapa[70 * 70];
	Mapa *mapa;
	Jogador jogador;//utilizado para enviar o jogador do cliente
	AuxMapa auxMapa[70*70];
	//monstro
	// _TCHARbuf[256];
	int jogocomecou;//0 nao e 1 sim
}Jogo;

//fase de testes o mapa vai ser 70*70
void ConstrutorJogo(Jogo *j);
void inicializacaoMatriz(Jogo *j);
void preencheMatriz(Jogo *j);
void actualizaJogo(Jogo *j);
void MovimentoJogador(Mapa **mapa, Jogador *j, int comando);
void adicionaJogadoresMapa(Jogo *j);


//funcao de inicializacao da matriz que vai ser o mapa e do relogio
//relogio ainda por descobrir como fazer - talvez por thread do cliente a dormir por x tempo