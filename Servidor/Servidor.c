#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

#define PIPE_NAME1 TEXT("\\\\.\\pipe\\teste1")//Escreve
#define PIPE_NAME2 TEXT("\\\\.\\pipe\\teste2")//Le
#define N_MAX_LEITORES 10
#define TAM 256

HANDLE PipeLeitores[N_MAX_LEITORES];
HANDLE hPipe;
int total;
BOOL fim = FALSE;

DWORD WINAPI RecebeLeitores(LPVOID param);
DWORD WINAPI AtendeCliente(LPVOID param);


int _tmain(int argc, LPTSTR argv[]){
	DWORD n;
	HANDLE hThread;
	TCHAR buf[TAM];

#ifdef UNICODE 
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif
	//Invocar a thread que inscreve novos leitores
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecebeLeitores, NULL, 0, NULL);

	//fazer isto sempre que se muda a struct, escreve para todos os clientes
	do{
		_tprintf(TEXT("[SERVIDOR] Frase: "));
		_fgetts(buf, 256, stdin);
		//Escrever para todos os leitores inscritos
		if (buf[0]!='\n'){
			for (int i = 0; i < total; i++)
				if (!WriteFile(PipeLeitores[i], buf, _tcslen(buf)*sizeof(TCHAR), &n, NULL)) {
				_tperror(TEXT("[ERRO] Escrever no pipe... (WriteFile)\n"));
				//se der erro, temos de por aqui uma segurança, eliminar este cliente
				exit(-1);
				}
		}
		_tprintf(TEXT("[SERVIDOR] Enviei %d bytes aos %d clientes... (WriteFile)\n"), n, total);
	} while (_tcsncmp(buf, TEXT("fim"), 3));
	fim = TRUE;

	//Esperar a thread recebeLeitores terminar
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	exit(0);
}

DWORD WINAPI RecebeLeitores(LPVOID param){
	HANDLE hPipe;
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

		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AtendeCliente, (LPVOID)hPipe, 0, NULL);
		total++;
	}
	//desliga cada named pipe
	for (int i = 0; i < total; i++){
		DisconnectNamedPipe(PipeLeitores[i]);
		_tprintf(TEXT("[SERVIDOR] Vou desligar o pipe... (CloseHandle)\n"));
		CloseHandle(PipeLeitores[i]);
	}
	return 0;
}

DWORD WINAPI AtendeCliente(LPVOID param){
	HANDLE pipe = (HANDLE)param;
	TCHAR buf[TAM];//no trabalho é uma struct
	int n;
	int ret;
	while (1){
		//ler do pipe do cliente
		ret = ReadFile(pipe, buf, TAM, &n, NULL);
			if (n > 0 && buf[0]!='\n'){
			buf[(n / sizeof(TCHAR)) - 1] = '\0'; //pos=255
			//escrever para todos
			_tprintf(TEXT("[SERVIDOR] Recebi %d bytes: '%s'... (ReadFile)\n"), n, buf);
			for (int i = 0; i < total; i++)
				WriteFile(PipeLeitores[i], buf, _tcslen(buf)*sizeof(TCHAR), &n, NULL);
		}
	}
	return 0;
}