#include "Jogo.h"


typedef struct MemoriaPartilhada{
	int visibilidade;//0 invisivel/1 visivel

	int muro;//0 se este bloco n�o for muro/ 1 se este bloco for muro / 2 se for porta

	Jogador jogador;
	Monstro monstro;

	//objectos
	int pedras;//unico que pode ser mais que 1
	int vitamina;//max 1
	int orangebull;//max 1
	int cafeina;//max 1

	//mutex para nao mexerem todos ao mesmo tempo
	HANDLE hmutex;
}MemoriaPartilhada;

//memoria a ser partilhada
void copiaParaMonstro(Jogo *j,MemoriaPartilhada *mp);
void copiaParaServidor(Jogo *j,MemoriaPartilhada *mp);