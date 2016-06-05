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

void criaLigacoes(int argc, LPTSTR argv[]){
	DWORD n;
	HANDLE hThread;
	//create process
	TCHAR buf[TAM],executavel[MAX] = TEXT("C:\\Users\\ASUS\\Documents\\Ambiente de Trabalho\\Joao\\universidade3ano\\2semestre\\SO2\\Trabalho Prático\\Monstro\\Monstro\\Debug\\Monstro.exe");
	TCHAR argumentos[MAX] = TEXT("0 3 0");//tipo,N,clonado
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	

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

	// memoria partilhada
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
	exit(0);//não terminar aqui
}


DWORD WINAPI RecebeLeitores(LPVOID param){
	HANDLE hPipe;
	HANDLE ThreadsAtendeCliente[N_MAX_LEITORES];
	//nao permitir mais do que o limite de jogadores, mas tambem nao precisa de acabar
	while (!fim && total < N_MAX_LEITORES){
		_tprintf(TEXT("[SERVIDOR] Vou passar à criação de uma cópia do pipe '%s' ... (CreateNamedPipe)\n"), PIPE_NAME1);
		//outbound servidor->cliente
		PipeLeitores[total] = CreateNamedPipe(PIPE_NAME1, PIPE_ACCESS_OUTBOUND, PIPE_WAIT | PIPE_TYPE_MESSAGE
			| PIPE_READMODE_MESSAGE, N_MAX_LEITORES, TAM * sizeof(TCHAR), TAM * sizeof(TCHAR),
			1000, NULL);
		if (PipeLeitores[total] == INVALID_HANDLE_VALUE){
			_tperror(TEXT("[ERRO] na ligação ao leitor!"));
			exit(-1);
		}
		//inbound servidor<-cliente
		hPipe = CreateNamedPipe(PIPE_NAME2, PIPE_ACCESS_INBOUND, PIPE_WAIT | PIPE_TYPE_MESSAGE
			| PIPE_READMODE_MESSAGE, N_MAX_LEITORES, TAM * sizeof(TCHAR), TAM * sizeof(TCHAR),
			1000, NULL);
		if (hPipe == INVALID_HANDLE_VALUE){
			_tperror(TEXT("[ERRO] na ligação ao leitor!"));
			exit(-1);
		}

		_tprintf(TEXT("[SERVIDOR] Esperar ligação de um leitor... (ConnectNamedPipe)\n"));
		if (!ConnectNamedPipe(PipeLeitores[total], NULL)){
			_tperror(TEXT("[ERRO] na ligação ao leitor!"));
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

DWORD WINAPI Clock(LPVOID param){
	DWORD n;
	int i = 0;
	while (1){
		copiaParaServidor(&jogo, mp);
		for (i = 0; i < total; i++){
			if (jogadores[i] != NULL){
				jogo.jogador=*jogadores[i];
				WriteFile(PipeLeitores[i], (LPCVOID)&jogo, sizeof(jogo), &n, NULL);
			}
		}
		Sleep((1000/15));
	}
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
	TCHAR buf[TAM], executavel[MAX] = TEXT("C:\\Users\\ASUS\\Documents\\Ambiente de Trabalho\\Joao\\universidade3ano\\2semestre\\SO2\\Trabalho Prático\\Monstro\\Monstro\\Debug\\Monstro.exe");
	TCHAR argumentosDistraido[MAX] = TEXT("0 3 0");//tipo,N,clonado
	TCHAR argumentosBully[MAX] = TEXT("1 0 0");//tipo,N,clonado
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	//////////////////////////
	ConstrutorJogador(&jogador);

	//Registry Key

	//Fase 1 - o servidor aguardo o login do jogador
	while (1){
		//ler do pipe do cliente
		do{
			ret = ReadFile(pipeRecebe, (LPVOID)&msg, sizeof(msg), &n, NULL);
			if (n > 0){
				if (msg.comando == 4){//faz login
					TCHAR key[TAM] = REGISTRY_KEY, password[TAM];
					HKEY hKey;
					DWORD size;
					TCHAR Username[TAM];
					wcscpy(Username, msg.Username);
					wcscat(key, Username);
					if (RegOpenKeyEx(HKEY_CURRENT_USER, key, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
					{
						RegQueryValueEx(hKey, TEXT("Password"), NULL, NULL, (LPBYTE)password, &size);
						if (_tcscmp(password, msg.Password) == 0){
							msg.sucesso = 1;
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
					wcscat_s(keyName, sizeof(keyName), msg.Username);

					if (RegOpenKeyEx(HKEY_CURRENT_USER, keyName, 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS){//verifica se já tem um cliente igual
						msg.sucesso = 0;
					}
					else{
						if (RegCreateKeyEx(HKEY_CURRENT_USER, keyName, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &keyDword) != ERROR_SUCCESS) {//cria o Registry
							msg.sucesso = 0;
						}
						else{
							RegSetValueEx(key, TEXT("Nome"), 0, REG_SZ, (LPBYTE)msg.Username, _tcslen(msg.Username)*sizeof(TCHAR));
							RegSetValueEx(key, TEXT("Password"), 0, REG_SZ, (LPBYTE)msg.Password, _tcslen(msg.Password)*sizeof(TCHAR));
							msg.sucesso = 1;
						}
					}
					WriteFile(pipeEnvia, (LPCVOID)&msg, sizeof(msg), &n, NULL);//envia para o cliente
				}
			}
		} while (msg.sucesso != 1);


		//Fase 2
		int flg = 0;//esta flag e para saber se ele já esta a espera do inicio do jogo ou nao
		while (1){
			//ler do pipe do cliente
			ret = ReadFile(pipeRecebe, (LPVOID)&msg, sizeof(msg), &n, NULL);
			//_tprintf(TEXT("[CLIENTE]:AQUI!!!!!! %d!\n"), jogo.jogocomecou);
			if (n > 0){
				if (flg == 0){
					if (msg.comando == 6){//criar jogo
						if (jogo.mapa == NULL){
							ConstrutorJogo(&jogo);
							//adicionar não trocar completamente
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
			}
		}


		/////////por isto numa funcao, avisa jogadores do inicio do jogo, dispoe os jogadores no mapa, e envia o jogo
		if (jogo.jogocomecou == 0){
			jogo.jogocomecou = 1;
			for (int i = 0; i < total; i++){//envia para todos a informacao que o jogo vai comecar
				if (jogadores[i] != NULL){
					msg.comando = 8;
					WriteFile(PipeLeitores[i], (LPCVOID)&msg, sizeof(msg), &n, NULL);
				}
			}
			//adiciona os jogadores ao mapa
			adicionaJogadoresMapa(&jogo);
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Clock, (LPVOID)NULL, 0, NULL);
			//activa os monstros
			copiaParaMonstro(&jogo, mp);
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
		WaitForSingleObject(hmutex, INFINITE);//acho que nao esta a funcionar
		jogo.jogador = jogador;
		_tprintf(TEXT("\n\nJogador\nVida:%d\nLentidao:%d\nPedras:%d\nPosx:%d\nPosy:%d\n\n"), jogo.jogador.vida, jogo.jogador.lentidao, jogo.jogador.pedras, jogo.jogador.posx, jogo.jogador.posy);
		WriteFile(pipeEnvia, (LPCVOID)&jogo, sizeof(jogo), &n, NULL);
		ReleaseMutex(hmutex);
		while (1){//fazer espera comando de saida, no final guarda os pontos que vao ser contados na thread clock
			//ler do pipe do cliente
			ret = ReadFile(pipeRecebe, (LPVOID)&msg, sizeof(msg), &n, NULL);
			if (n > 0){
				//aqui se põe o semáforo
				WaitForSingleObject(hmutex,INFINITE);
				//faz açao
				MovimentoJogador(jogo.mapa, &jogador, msg.comando);
				//aqui faz actualiza jogo
				actualizaJogo(&jogo);
				WriteFile(pipeEnvia, (LPCVOID)&jogo, sizeof(jogo), &n, NULL);
				//release mutex
				ReleaseMutex(hmutex);
				copiaParaMonstro(&jogo, mp);
			}
		}
	}
	return 0;
}