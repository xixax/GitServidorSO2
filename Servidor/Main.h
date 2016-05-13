#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

//numero maximo de jogadores
#define N_MAX_LEITORES 10

//Tam default
#define TAM 256

//Named pipes
#define PIPE_NAME1 TEXT("\\\\.\\pipe\\teste1")//Escreve
#define PIPE_NAME2 TEXT("\\\\.\\pipe\\teste2")//Le

//Registry
#define REGISTRY_KEY TEXT("Software\\SO2Trabalho\\")//necessário para o registry

//Visibilidade
#define Visibilidade 10;

//lentidao
#define LentidaoJogador 5
#define LentidaoMonstroDistraido 3
#define LentidaoMonstroBully 7

//vida
#define VidaJogador 10
#define VidaMonstroDistraido 5
#define VidaMonstroBully 5

//objectos
#define VidaVitaminas 1 //dá 1 de vida
#define VidaOrangeBull 3//dá 3 de vida
#define TiraLentidaoCafeina 2//tira 2 de lentidao

//Raridade dos objectos
#define PercentagemVitaminas 5 //percentagem
#define PercentagemOrangeBull 3 //percentagem
#define PercentagemCafeina 1 //percentagem
#define PercentagemPedra 11 //percentagem