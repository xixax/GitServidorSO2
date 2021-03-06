#include "Main.h"
#include "MemoriaPartilhada.h"
#include "Rede.h"
#include "Mensagem.h"

#define MAX 256

HANDLE PipeLeitores[N_MAX_LEITORES];
HANDLE hPipe;
HANDLE auxPIPE;
HANDLE hmutex;
BOOL fim = FALSE;
Jogo jogo;//jogo inicializado
//teste
Jogador auxjog;
HANDLE hMapFile;
MemoriaPartilhada *temppartilhada;
MemoriaPartilhada *mp;

DWORD WINAPI Clock(LPVOID param){
	DWORD n;
	int i = 0, x, y;
	while (1){
		WaitForSingleObject(hmutex, INFINITE);
		copiaParaServidor(&jogo, mp);
		for (x = 0; x < 70; x++){
			for (y = 0; y < 70; y++){
				jogo.auxMapa[x * 70 + y].cafeina = jogo.mapa[x * 70 + y].cafeina;
				if (jogo.mapa[x * 70 + y].jogador != NULL){
					jogo.auxMapa[x * 70 + y].jogador.lentidao = jogo.mapa[x * 70 + y].jogador->lentidao;
					jogo.auxMapa[x * 70 + y].jogador.pedras = jogo.mapa[x * 70 + y].jogador->pedras;
					jogo.auxMapa[x * 70 + y].jogador.posx = jogo.mapa[x * 70 + y].jogador->posx;
					jogo.auxMapa[x * 70 + y].jogador.posy = jogo.mapa[x * 70 + y].jogador->posy;
					jogo.auxMapa[x * 70 + y].jogador.vida = jogo.mapa[x * 70 + y].jogador->vida;
				}
				else{
					jogo.auxMapa[x * 70 + y].jogador.vida = -1;
					jogo.auxMapa[x * 70 + y].jogador.lentidao = -1;
					jogo.auxMapa[x * 70 + y].jogador.pedras = -1;
					jogo.auxMapa[x * 70 + y].jogador.posx = -1;
					jogo.auxMapa[x * 70 + y].jogador.posy = -1;
				}
				if (jogo.mapa[x * 70 + y].monstro.vida > -1){
					_tprintf(TEXT("\nMonstro\nPosx:%d\nPosy:%d\nTIPO:%d\n"), jogo.mapa[x * 70 + y].monstro.posx, jogo.mapa[x * 70 + y].monstro.posy, jogo.mapa[x * 70 + y].monstro.tipo);
				}
				jogo.auxMapa[x * 70 + y].monstro.clonado = mp[(x)* 70 + y].monstro.clonado;
				jogo.auxMapa[x * 70 + y].monstro.lentidao = mp[(x)* 70 + y].monstro.lentidao;
				jogo.auxMapa[x * 70 + y].monstro.N = mp[(x)* 70 + y].monstro.N;
				jogo.auxMapa[x * 70 + y].monstro.posx = mp[(x)* 70 + y].monstro.posx;
				jogo.auxMapa[x * 70 + y].monstro.posy = mp[(x)* 70 + y].monstro.posy;
				jogo.auxMapa[x * 70 + y].monstro.sentido = mp[(x)* 70 + y].monstro.sentido;
				jogo.auxMapa[x * 70 + y].monstro.tipo = mp[(x)* 70 + y].monstro.tipo;
				jogo.auxMapa[x * 70 + y].monstro.vida = mp[(x)* 70 + y].monstro.vida;
				jogo.auxMapa[x * 70 + y].muro = jogo.mapa[x * 70 + y].muro;
				jogo.auxMapa[x * 70 + y].orangebull = jogo.mapa[x * 70 + y].orangebull;
				jogo.auxMapa[x * 70 + y].pedras = jogo.mapa[x * 70 + y].pedras;
				jogo.auxMapa[x * 70 + y].visibilidade = jogo.mapa[x * 70 + y].visibilidade;
				jogo.auxMapa[x * 70 + y].vitamina = jogo.mapa[x * 70 + y].vitamina;
			}
		}
		for (i = 0; i < total; i++){
			if (jogadores[i] != NULL){
				jogo.jogador = *jogadores[i];
				WriteFile(PipeLeitores[i], (LPCVOID)&jogo, sizeof(jogo), &n, NULL);
			}
		}
		ReleaseMutex(hmutex);
		Sleep((1000 / 15));
	}
}

void criaLigacoes(int argc, LPTSTR argv[]){
	DWORD n;
	HANDLE hThread;
	//create process
	TCHAR buf[TAM],executavel[MAX] = TEXT("C:\\Users\\ASUS\\Documents\\Ambiente de Trabalho\\Joao\\universidade3ano\\2semestre\\SO2\\Trabalho Pr�tico\\Monstro\\Monstro\\Debug\\Monstro.exe");
	TCHAR argumentos[MAX] = TEXT("0 3 0");//tipo,N,clonado
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	TCHAR argumentosDistraido[MAX] = TEXT("0 3 0");//tipo,N,clonado

	//poe variavel mapa a null
	jogo.mapa = NULL;

#ifdef UNICODE 
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif
	//inicializacao de um mutex
	hmutex = CreateMutex(NULL,FALSE,TEXT("Mutex"));
	if (hmutex == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		return 1;
	}
	
	//memoria partilhada
	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 70*70*sizeof(MemoriaPartilhada), "TrabalhoSO");
	//nome do mapfile qualquer
	if (hMapFile == NULL)
		exit(-1);

	
	mp = (MemoriaPartilhada *)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 70*70*sizeof(MemoriaPartilhada));

	if (mp == NULL)
		exit(-1);

	temppartilhada =malloc(70*70*sizeof(MemoriaPartilhada));

	CopyMemory(mp, temppartilhada, 70*70*sizeof(MemoriaPartilhada));

	free(temppartilhada);
	/////////////////////
	
	//Invocar a thread que inscreve novos leitores
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecebeLeitores, NULL, 0, NULL);

	//Esperar a thread recebeLeitores terminar
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	exit(0);//n�o terminar aqui
}


DWORD WINAPI RecebeLeitores(LPVOID param){
	HANDLE hPipe;
	HANDLE ThreadsAtendeCliente[N_MAX_LEITORES];
	//nao permitir mais do que o limite de jogadores, mas tambem nao precisa de acabar
	while (!fim && total < N_MAX_LEITORES){
		_tprintf(TEXT("[SERVIDOR] Vou passar � cria��o de uma c�pia do pipe '%s' ... (CreateNamedPipe)\n"), PIPE_NAME1);
		//outbound servidor->cliente
		PipeLeitores[total] = CreateNamedPipe(PIPE_NAME1, PIPE_ACCESS_OUTBOUND, PIPE_WAIT | PIPE_TYPE_MESSAGE
			| PIPE_READMODE_MESSAGE, N_MAX_LEITORES, TAM * sizeof(TCHAR), TAM * sizeof(TCHAR),
			1000, NULL);
		if (PipeLeitores[total] == INVALID_HANDLE_VALUE){
			_tperror(TEXT("[ERRO] na liga��o ao leitor!"));
			exit(-1);
		}
		//inbound servidor<-cliente
		hPipe = CreateNamedPipe(PIPE_NAME2, PIPE_ACCESS_INBOUND, PIPE_WAIT | PIPE_TYPE_MESSAGE
			| PIPE_READMODE_MESSAGE, N_MAX_LEITORES, TAM * sizeof(TCHAR), TAM * sizeof(TCHAR),
			1000, NULL);
		if (hPipe == INVALID_HANDLE_VALUE){
			_tperror(TEXT("[ERRO] na liga��o ao leitor!"));
			exit(-1);
		}

		_tprintf(TEXT("[SERVIDOR] Esperar liga��o de um leitor... (ConnectNamedPipe)\n"));
		if (!ConnectNamedPipe(PipeLeitores[total], NULL)){
			_tperror(TEXT("[ERRO] na liga��o ao leitor!"));
			exit(-1);
		}

		//ver maneira de fazer isto melhor
		auxPIPE = PipeLeitores[total];
		//guardar o array, para o waitmultipleobject
		ThreadsAtendeCliente[total]=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AtendeCliente, (LPVOID)hPipe, 0, NULL);
		total++;
	}

	//espera por todas as threads acabarem
	WaitForMultipleObjects(total, ThreadsAtendeCliente, TRUE, INFINITE);

	//desliga cada named pipe
	for (int i = 0; i < total; i++){
		DisconnectNamedPipe(PipeLeitores[i]);
		_tprintf(TEXT("[SERVIDOR] Vou desligar o pipe... (CloseHandle)\n"));
		CloseHandle(PipeLeitores[i]);
	}
	return 0;
}



DWORD WINAPI AtendeCliente(LPVOID param){
	HANDLE pipeRecebe = (HANDLE)param;
	HANDLE pipeEnvia = auxPIPE;
	Mensagem msg;
	int n;
	int ret;
	//Dados Cliente
	Jogador jogador;
	Jogo erroJogo;//para mensagens de erro
	erroJogo.mapa = NULL;
	//informacao para processos
	TCHAR buf[TAM], executavel[MAX] = TEXT("C:\\Users\\ASUS\\Documents\\Ambiente de Trabalho\\Joao\\universidade3ano\\2semestre\\SO2\\Trabalho Pr�tico\\Monstro\\Monstro\\Debug\\Monstro.exe");
	TCHAR argumentosDistraido[MAX] = TEXT("0 3 0");//tipo,N,clonado
	TCHAR argumentosBully[MAX] = TEXT("1 0 0");//tipo,N,clonado
	PROCESS_INFORMATION pi;
	STARTUPINFO si;

	//////////////////////////

	TCHAR Username[TAM];

	////recordes
	int RecordeActual=0;
	TCHAR NomeJogador[TAM];
	//
	ConstrutorJogador(&jogador);

	//Registry Key

	//Fase 1 - o servidor aguardo o login do jogador
	//ler do pipe do cliente
	do{
		ret = ReadFile(pipeRecebe, (LPVOID)&msg, sizeof(msg), &n, NULL);
		if (n > 0){
			if (msg.comando == 4){//faz login
				TCHAR key[TAM] = REGISTRY_KEY, password[TAM];
				HKEY hKey;
				DWORD size;
				wcscpy(Username, msg.Username);
				wcscat(key, Username);
				if (RegOpenKeyEx(HKEY_CURRENT_USER, key, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
				{
					RegQueryValueEx(hKey, TEXT("Password"), NULL, NULL, (LPBYTE)password, &size);
					if (_tcscmp(password, msg.Password) == 0){
						msg.sucesso = 1;
						wcscpy(NomeJogador, Username);
					}
					else{
						msg.sucesso = 0;
					}
				}
				else{
					msg.sucesso = 0;
				}
				WriteFile(pipeEnvia, (LPCVOID)&msg, sizeof(msg), &n, NULL);//envia para o cliente
				if (msg.sucesso == 1){
					break;//se login foi sucesso passa para proxima fase
				}
			}
			if (msg.comando == 5){//faz registo
				HKEY key;
				DWORD keyDword;
				TCHAR keyName[TAM] = REGISTRY_KEY;
				DWORD size;
				wcscpy(Username, msg.Username);
				wcscat_s(keyName, sizeof(keyName), msg.Username);

				if (RegOpenKeyEx(HKEY_CURRENT_USER, keyName, 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS){//verifica se j� tem um cliente igual
					msg.sucesso = 0;
				}
				else{
					if (RegCreateKeyEx(HKEY_CURRENT_USER, keyName, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &keyDword) != ERROR_SUCCESS) {//cria o Registry
						msg.sucesso = 0;
					}
					else{
						RegSetValueEx(key, TEXT("Nome"), 0, REG_SZ, (LPBYTE)msg.Username, _tcslen(msg.Username)*sizeof(TCHAR));
						RegSetValueEx(key, TEXT("Password"), 0, REG_SZ, (LPBYTE)msg.Password, _tcslen(msg.Password)*sizeof(TCHAR));
						RegSetValueEx(key, TEXT("1"), 0, REG_SZ, (LPBYTE)"0", _tcslen("0")*sizeof(TCHAR));
						RegSetValueEx(key, TEXT("2"), 0, REG_SZ, (LPBYTE)"0", _tcslen("0")*sizeof(TCHAR));
						RegSetValueEx(key, TEXT("3"), 0, REG_SZ, (LPBYTE)"0", _tcslen("0")*sizeof(TCHAR));
						RegSetValueEx(key, TEXT("4"), 0, REG_SZ, (LPBYTE)"0", _tcslen("0")*sizeof(TCHAR));
						RegSetValueEx(key, TEXT("5"), 0, REG_SZ, (LPBYTE)"0", _tcslen("0")*sizeof(TCHAR));
						wcscpy(NomeJogador, keyName);
						msg.sucesso = 1;
					}
				}
				WriteFile(pipeEnvia, (LPCVOID)&msg, sizeof(msg), &n, NULL);//envia para o cliente
			}
		}
	} while (msg.sucesso != 1);


	//Fase 2
	int flg = 0;//esta flag e para saber se ele j� esta a espera do inicio do jogo ou nao
	while (1){
		//ler do pipe do cliente
		ret = ReadFile(pipeRecebe, (LPVOID)&msg, sizeof(msg), &n, NULL);
		//_tprintf(TEXT("[CLIENTE]:AQUI!!!!!! %d!\n"), jogo.jogocomecou);
		if (n > 0){
			if (flg == 0){
				if (msg.comando == 6){//criar jogo
					if (jogo.mapa == NULL){
						ConstrutorJogo(&jogo);
						//adicionar n�o trocar completamente
						for (int i = 0; i < total; i++){
							if (PipeLeitores[i] == pipeEnvia){
								jogadores[i] = &jogador;//ver se esta bem
								totalnojogo++;
							}
						}
						flg = 1;
						WriteFile(pipeEnvia, (LPCVOID)&jogo, sizeof(jogo), &n, NULL);//envia para o cliente
					}
					else{
						WriteFile(pipeEnvia, (LPCVOID)&erroJogo, sizeof(erroJogo), &n, NULL);//envia para o cliente
					}
				}
				if (msg.comando == 7){//juntar a jogo
					if (jogo.mapa != NULL && jogo.jogocomecou == 0){
						//jogo.jogador = jogador;
						for (int i = 0; i < total; i++){
							if (PipeLeitores[i] == pipeEnvia){
								jogadores[i] = &jogador;//ver se esta bem
								totalnojogo++;
							}
						}
						flg = 1;
						WriteFile(pipeEnvia, (LPCVOID)&jogo, sizeof(jogo), &n, NULL);//envia para o cliente
					}
					else{
						erroJogo.jogocomecou = 1;
						erroJogo.jogador = jogador;
						_tprintf(TEXT("[CLIENTE]:AQUI2!!!!!! %d %s !\n"), erroJogo.jogocomecou, erroJogo.mapa);
						WriteFile(pipeEnvia, (LPCVOID)&erroJogo, sizeof(erroJogo), &n, NULL);//envia para o cliente
					}
				}
			}
			if (msg.comando == 8){
				break;
			}
			if (msg.comando == 10){
				TCHAR key[TAM] = REGISTRY_KEY;
				HKEY hKey;
				DWORD size;
				wcscat(key, Username);
				if (RegOpenKeyEx(HKEY_CURRENT_USER, key, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
				{
					msg.sucesso = 1;
					RegQueryValueEx(hKey, TEXT("1"), NULL, NULL, (LPBYTE)msg.recorde1, &size);
					RegQueryValueEx(hKey, TEXT("2"), NULL, NULL, (LPBYTE)msg.recorde2, &size);
					RegQueryValueEx(hKey, TEXT("3"), NULL, NULL, (LPBYTE)msg.recorde3, &size);
					RegQueryValueEx(hKey, TEXT("4"), NULL, NULL, (LPBYTE)msg.recorde4, &size);
					RegQueryValueEx(hKey, TEXT("5"), NULL, NULL, (LPBYTE)msg.recorde5, &size);
					//msg.recorde1 = atoi(recorde1); msg.recorde2 = atoi(recorde2); msg.recorde3 = atoi(recorde3); msg.recorde4 = atoi(recorde4); msg.recorde5 = atoi(recorde5);
					WriteFile(pipeEnvia, (LPCVOID)&msg, sizeof(msg), &n, NULL);//envia para o cliente
				}
				else{
					msg.sucesso = 0;
					WriteFile(pipeEnvia, (LPCVOID)&msg, sizeof(msg), &n, NULL);//envia para o cliente
				}
			}
		}
	}


	/////////por isto numa funcao, avisa jogadores do inicio do jogo, dispoe os jogadores no mapa, e envia o jogo
	if (jogo.jogocomecou == 0){
		for (int i = 0; i < total; i++){//envia para todos a informacao que o jogo vai comecar
			if (jogadores[i] != NULL){
				msg.comando = 8;
				WriteFile(PipeLeitores[i], (LPCVOID)&msg, sizeof(msg), &n, NULL);
			}
		}
		//adiciona os jogadores ao mapa
		adicionaJogadoresMapa(&jogo);
		copiaParaMonstro(&jogo, mp);
		jogo.jogocomecou = 1;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Clock, (LPVOID)NULL, 0, NULL);
		//activa os monstros
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		if (CreateProcess(executavel, argumentosDistraido, NULL, NULL, 0, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi) == 0){
			_tprintf(TEXT("\nOcorreu um erro ao iniciar o Monstro!!!!\n\n"));
		}
		if (CreateProcess(executavel, argumentosBully, NULL, NULL, 0, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi) == 0){
			_tprintf(TEXT("\nOcorreu um erro ao iniciar o Monstro!!!!\n\n"));
		}
	}

	//Fase 3- o jogo em si, nao sao aceites novos jogadores
	//envia o jogo para todos

	_tprintf(TEXT("\n\nJogador\nVida:%d\nLentidao:%d\nPedras:%d\nPosx:%d\nPosy:%d\n\n"), jogador.vida, jogador.lentidao, jogador.pedras, jogador.posx, jogador.posy);
	while (jogador.vida > 0 || msg.comando != 9){//fazer espera comando de saida, no final guarda os pontos que vao ser contados na thread clock
		//ler do pipe do cliente
		ret = ReadFile(pipeRecebe, (LPVOID)&msg, sizeof(msg), &n, NULL);
		if (n > 0){
			//aqui se p�e o sem�foro
			WaitForSingleObject(hmutex, INFINITE);
			//faz a�ao
			MovimentoJogador(jogo.mapa, &jogador, msg.comando);
			//aqui faz actualiza jogo
			actualizaJogo(&jogo);
			//release mutex
			ReleaseMutex(hmutex);
			copiaParaMonstro(&jogo, mp);
			RecordeActual++;
		}
	}

	//Aqui escrever o contador para os recordes nos registry
	TCHAR key[TAM] = REGISTRY_KEY, recorde[TAM];
	HKEY hKey;
	DWORD size;
	wcscat(key, NomeJogador);
	if (RegOpenKeyEx(HKEY_CURRENT_USER, key, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		RegQueryValueEx(hKey, TEXT("1"), NULL, NULL, (LPBYTE)recorde, &size);
		if (_tcscmp(RecordeActual, recorde) > 0){
			RegSetValueEx(key, TEXT("1"), 0, REG_SZ, (LPBYTE)RecordeActual, sizeof(RecordeActual));
		}
		else{
			RegQueryValueEx(hKey, TEXT("2"), NULL, NULL, (LPBYTE)recorde, &size);
			if (_tcscmp(RecordeActual, recorde) > 0){
				RegSetValueEx(key, TEXT("2"), 0, REG_SZ, (LPBYTE)RecordeActual, sizeof(RecordeActual));
			}
			else{
				RegQueryValueEx(hKey, TEXT("3"), NULL, NULL, (LPBYTE)recorde, &size);
				if (_tcscmp(RecordeActual, recorde) > 0){
					RegSetValueEx(key, TEXT("3"), 0, REG_SZ, (LPBYTE)RecordeActual, sizeof(RecordeActual));
				}
				else{
					RegQueryValueEx(hKey, TEXT("4"), NULL, NULL, (LPBYTE)recorde, &size);
					if (_tcscmp(RecordeActual, recorde) > 0){
						RegSetValueEx(key, TEXT("4"), 0, REG_SZ, (LPBYTE)RecordeActual, sizeof(RecordeActual));
					}
					else{
						RegQueryValueEx(hKey, TEXT("5"), NULL, NULL, (LPBYTE)recorde, &size);
						if (_tcscmp(RecordeActual, recorde) > 0){
							RegSetValueEx(key, TEXT("5"), 0, REG_SZ, (LPBYTE)RecordeActual, sizeof(RecordeActual));
						}
					}
				}
			}
		}
	}
	return 0;
}