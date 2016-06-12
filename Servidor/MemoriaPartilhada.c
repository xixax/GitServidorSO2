#include "MemoriaPartilhada.h"

void copiaParaMonstro(Jogo *j, MemoriaPartilhada *mp){
	int x, y;
	for (x = 0; x < 70; x++){
		for (y = 0; y < 70; y++){
			int aux3 = x* 70 + y;
			int aux = j->mapa[50].cafeina;
			mp[(x)* 70 + y].hmutex=CreateMutex(NULL, FALSE, TEXT("MutexMonstro"));
			WaitForSingleObject(mp[(x)*70+y].hmutex, INFINITE);
			mp[x*70+y].cafeina=j->mapa[(x)* 70 + y].cafeina;
			//copia de jogador
			if (j->mapa[(x)* 70 + y].jogador != NULL){
				mp[x* 70 + y].jogador.lentidao = j->mapa[(x)* 70 + y].jogador->lentidao;
				mp[x* 70 + y].jogador.pedras = j->mapa[(x)* 70 + y].jogador->pedras;
				mp[x* 70 + y].jogador.posx = j->mapa[(x)* 70 + y].jogador->posx;
				mp[x* 70 + y].jogador.posy = j->mapa[(x)* 70 + y].jogador->posy;
				mp[x* 70 + y].jogador.vida = j->mapa[(x)* 70 + y].jogador->vida;
			}
			else{//nao tem jogador
				mp[x* 70 + y].jogador.lentidao = -1;
				mp[x* 70 + y].jogador.pedras = -1;
				mp[x* 70 + y].jogador.posx = -1;
				mp[x* 70 + y].jogador.posy = -1;
				mp[x* 70 + y].jogador.vida = -1;
			}
			if (j->jogocomecou == 0){
				mp[(x)* 70 + y].monstro.clonado = j->mapa[(x)* 70 + y].monstro.clonado;
				mp[(x)* 70 + y].monstro.lentidao = j->mapa[(x)* 70 + y].monstro.lentidao;
				mp[(x)* 70 + y].monstro.N = j->mapa[(x)* 70 + y].monstro.N;
				mp[(x)* 70 + y].monstro.posx = j->mapa[(x)* 70 + y].monstro.posx;
				mp[(x)* 70 + y].monstro.posy = j->mapa[(x)* 70 + y].monstro.posy;
				mp[(x)* 70 + y].monstro.sentido = j->mapa[(x)* 70 + y].monstro.sentido;
				mp[(x)* 70 + y].monstro.tipo = j->mapa[(x)* 70 + y].monstro.tipo;
				mp[(x)* 70 + y].monstro.vida = j->mapa[(x)* 70 + y].monstro.vida;
			}
			mp[x* 70 + y].muro = j->mapa[(x)* 70 + y].muro;
			mp[x* 70 + y].orangebull = j->mapa[(x)* 70 + y].orangebull;
			mp[x* 70 + y].pedras = j->mapa[(x)* 70 + y].pedras;
			mp[x* 70 + y].visibilidade = j->mapa[(x)* 70 + y].visibilidade;
			mp[x* 70 + y].vitamina = j->mapa[(x)* 70 + y].vitamina;
			ReleaseMutex(mp->hmutex);
		}
	}

}
void copiaParaServidor(Jogo *j, MemoriaPartilhada *mp,HANDLE *hmutex){
	int x, y;
	WaitForSingleObject(*hmutex,INFINITE);
	for (x = 0; x < 70; x++){
		for (y = 0; y < 70; y++){
			WaitForSingleObject(mp[(x)* 70 + y].hmutex, INFINITE);
			j->mapa[(x)* 70 + y].cafeina = mp[(x)* 70 + y].cafeina;
			//preenchimento jogador
			if (j->mapa[(x)* 70 + y].jogador != NULL){
				if (mp[(x)* 70 + y].jogador.vida != -1){
					j->mapa[(x)* 70 + y].jogador->vida = mp[(x)* 70 + y].jogador.vida;
				}
				_tprintf(TEXT("\nJOGADOR\nVIDA:%d\n"), mp[(x)* 70 + y].jogador.vida);
			}
			//preenchimento monstro
			j->mapa[(x)* 70 + y].monstro.clonado = mp[(x)* 70 + y].monstro.clonado;
			j->mapa[(x)* 70 + y].monstro.lentidao = mp[(x)* 70 + y].monstro.lentidao;
			j->mapa[(x)* 70 + y].monstro.N = mp[(x)* 70 + y].monstro.N;
			j->mapa[(x)* 70 + y].monstro.posx = mp[(x)* 70 + y].monstro.posx;
			j->mapa[(x)* 70 + y].monstro.posy = mp[(x)* 70 + y].monstro.posy;
			j->mapa[(x)* 70 + y].monstro.sentido = mp[(x)* 70 + y].monstro.sentido;
			j->mapa[(x)* 70 + y].monstro.tipo = mp[(x)* 70 + y].monstro.tipo;
			j->mapa[(x)* 70 + y].monstro.vida = mp[(x)* 70 + y].monstro.vida;
			j->mapa[(x)* 70 + y].muro = mp[(x)* 70 + y].muro;
			j->mapa[(x)* 70 + y].orangebull = mp[(x)* 70 + y].orangebull;
			j->mapa[(x)* 70 + y].pedras = mp[(x)* 70 + y].pedras;
			j->mapa[(x)* 70 + y].visibilidade = mp[(x)* 70 + y].visibilidade;
			j->mapa[(x)* 70 + y].vitamina = mp[(x)* 70 + y].vitamina;

			ReleaseMutex(mp->hmutex);
		}
	}
	ReleaseMutex(*hmutex);
}