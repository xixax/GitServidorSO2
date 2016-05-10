#include "Main.h"
#include "Jogo.h"
#include "Rede.h"
#include "Mensagem.h"


#define PIPE_NAME1 TEXT("\\\\.\\pipe\\teste1")//Escreve
#define PIPE_NAME2 TEXT("\\\\.\\pipe\\teste2")//Le
#define REGISTRY_KEY TEXT("Software\\SO2Trabalho\\")//necessário para o registry
#define N_MAX_LEITORES 10
#define TAM 256

HANDLE PipeLeitores[N_MAX_LEITORES];
HANDLE hPipe;
HANDLE auxPIPE;
HANDLE hmutex;
BOOL fim = FALSE;

void criaLigacoes(int argc, LPTSTR argv[]){
	DWORD n;
	HANDLE hThread;
	TCHAR buf[TAM];

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

	//Invocar a thread que inscreve novos leitores
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecebeLeitores, NULL, 0, NULL);

	//Esperar a thread recebeLeitores terminar
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	exit(0);//não terminar aqui
}


DWORD WINAPI RecebeLeitores(LPVOID param){
	HANDLE hPipe;
	Jogador j;
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

		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AtendeCliente, (LPVOID)hPipe, 0, NULL);
		total++;
	}

	//espera por todas as threads acabarem
	WaitForMultipleObjects(total,PipeLeitores, TRUE, INFINITE);

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

	//Jogo
	Jogo jogo;

	//Dados Cliente
	Jogador jogador;
	ConstrutorJogador(&jogador);

	//Registry Key

	//Fase 1 - o servidor aguardo o login do jogador
	while (1){
		//ler do pipe do cliente
		ret = ReadFile(pipeRecebe, (LPVOID)&msg, sizeof(msg), &n, NULL);
		if (n > 0){
			if (msg.comando == 4){//faz login
				TCHAR key[TAM] = REGISTRY_KEY, password[TAM];
				HKEY hKey;
				DWORD size;
				wcscat_s(key,sizeof(key),msg.Username);
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
				wcscat_s(keyName,sizeof(keyName),msg.Username);

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

		jogo.mapa = NULL;

		//Fase 2
		while (1){
			//ler do pipe do cliente
			ret = ReadFile(pipeRecebe, (LPVOID)&msg, sizeof(msg), &n, NULL);
			if (n > 0){
				if (msg.comando == 6){//criar jogo
					if (jogo.mapa != NULL){
						ConstrutorJogo(&jogo);
						//adicionar não trocar completamente
						jogadores[totalnojogo++] = jogador;//ver se esta bem
						WriteFile(pipeEnvia, (LPCVOID)&jogo, sizeof(jogo), &n, NULL);//envia para o cliente
					}
					else{
						WriteFile(pipeEnvia, (LPCVOID)&jogo, sizeof(jogo), &n, NULL);//envia para o cliente
					}
				}
				if (msg.comando == 7){//juntar a jogo
					if (jogo.mapa != NULL){
						//jogo.jogador = jogador;
						jogadores[totalnojogo++] = jogador;//ver se esta bem
						WriteFile(pipeEnvia, (LPCVOID)&jogo, sizeof(jogo), &n, NULL);//envia para o cliente
					}
					else{
						WriteFile(pipeEnvia, (LPCVOID)&jogo, sizeof(jogo), &n, NULL);//envia para o cliente
					}
				}
				if (msg.comando == 8){
					break;
				}
			}
		}


		//Fase 3- o jogo em si, nao sao aceites novos jogadores
		for (int i = 0; i < total; i++){//envia para todos o jogo 
			WriteFile(PipeLeitores[i], (LPCVOID)&jogo, sizeof(jogo), &n, NULL);
		}
		while (1){
			//ler do pipe do cliente
			ret = ReadFile(pipeRecebe, (LPVOID)&msg, sizeof(msg), &n, NULL);
			if (n > 0){
				//aqui se põe o semáforo
				WaitForSingleObject(hmutex,INFINITE);
				//faz açao
				MovimentoJogador(jogo.mapa, &jogador, msg.comando);
				//aqui faz actualiza jogo
				actualizaJogo(&jogo);
				for (int i = 0; i < total; i++){//envia para todos o jogo 
					WriteFile(PipeLeitores[i], (LPCVOID)&jogo, sizeof(jogo), &n, NULL);
				}
				//falta por aqui o clock
				//1/15 - 1 segundo são 15 instantes
				//Sleep(1000 * (jogador.lentidao / 15)); - isto é o timer concluido, depois experimentar
				
				//release mutex
				ReleaseMutex(hmutex);
			}
		}
	}
	return 0;
}