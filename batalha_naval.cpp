#include <iostream>
#include <string>
#include <stdlib.h>
#include <locale>
#include <time.h>
#include <windows.h>

using namespace std;

//pontuações
#define P_SCORE 10      //afundar barco pequeno
#define M_SCORE 30      //afundar barco médio
#define G_SCORE 100     //afundar barco grande

//dificuldade padrão
#define BOARDSIZE       10  // tamanho do tabuleiro N x N //este não será alterado durante a execução
#define P_BOATDEFAULT   15  //barcos pequenos
#define M_BOATDEFAULT   7   //barcos médios
#define G_BOATDEFAULT   3   //barcos grandes
#define TOWINDEFAULT    5   //necessários para ganhar
#define MAXATPDEFAULT   10  //chances de errar
#define DEFAULTMODE     "NORMAL"

//dificuldade fácil
#define P_EASY          15
#define M_EASY          10
#define G_EASY          5
#define TOWIN_EASY      3
#define MAXATP_EASY     12
#define MODEEASY        "FÁCIL"

//dificuldade difícil
#define P_HARD          8
#define M_HARD          3
#define G_HARD          1
#define TOWIN_HARD      5
#define MAXATP_HARD     10
#define MODEHARD        "DIFÍCIL"


//configurações iniciais de dificuldade
int P_BOATSAMT = P_BOATDEFAULT;         //quantidade de barcos pequenos
int M_BOATSAMT = M_BOATDEFAULT;         //quantidade de barcos médios
int G_BOATSAMT = G_BOATDEFAULT;         //quantidade de barcos grandes
int TOBEBEATEN = TOWINDEFAULT;          //barcos que precisam ser afundados para ganhar
int MAXATP     = MAXATPDEFAULT;         //número inicial de chances
string modeselected = DEFAULTMODE;      //dificuldade padrão (com o número de barcos acima)

//funções
int      intro();   //tela incial
void     about();   //informações do programa
void howToPlay();   //instruções
void      play();   //função principal do jogo
void      mode();   //função que altera dificuldade
//------------------------------
void printBoard(char board[BOARDSIZE][BOARDSIZE]);  //função que mostra a mesa
void      boats(char board[BOARDSIZE][BOARDSIZE]);  //função que gera os barcos
void newBoard(char board[BOARDSIZE][BOARDSIZE], char masked[BOARDSIZE][BOARDSIZE]); //função que gera a mesa
bool checkShoot(char board[BOARDSIZE][BOARDSIZE], char masked[BOARDSIZE][BOARDSIZE], int line, int column, int *score, string *msg, int *boats); //processa o tiro


//==============================================
int main(){
    //configs
    setlocale(LC_ALL,"");
    srand((unsigned) time (NULL));

    int option;
    //menu
    do{
        option = intro();

        switch(option){
            case 1: play();
                break;
            case 2: about();
                break;
            case 3: howToPlay();
                break;
            case 4: mode();
                break;
            case 5:
                cout << "\n\nAté mais!\n\n\n\n";
                system("PAUSE");
        }

    }while(option < 1 || option > 5);

    return 1;
}

//-------------------------------------------------
//game
void play(){

    char board[BOARDSIZE][BOARDSIZE];       //mesa secreta
    char masked[BOARDSIZE][BOARDSIZE];      //mesa mascarada
    int line, column;                       //linha e coluna digitada pelo jogador
    int score = 0;                          //pontos
    int attempts = MAXATP;                  //chances
    int boatsBeaten = 0;                    //barcos afundados
    string msg = "Bem-vindo ao jogo!\n";

    newBoard(board, masked);                //preenche a mesa com água e a máscara com *
    boats(board);                           //coloca os barcos em posições sorteadas

    while(attempts != 0 && boatsBeaten < TOBEBEATEN){
        //printBoard(board); //debug mode - mostra a mesa sem máscara.

        //mensagens
        cout << "Pontuação: " << score << "\n";
        cout << "Chances: " << attempts << "\n";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
        cout << msg << "\n\n";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

        printBoard(masked);
        //interação
        cout << "\nDigite uma linha: ";
        cin >> line;
        cout << "\nDigite uma coluna: ";
        cin >> column;

        //processamento dos dados coletados
        //caso posição esteja fora da mesa
        if(line < 0 || line >= BOARDSIZE || column < 0 || column >= BOARDSIZE) msg = "Epa! Posição inválida!\n";
        //caso posição já tenha sido revelada
        else if(masked[line][column] != '*') msg = "Epa! Você já atirou nesta posição!\n";

        else{
            bool failure = checkShoot(board, masked, line, column, &score, &msg, &boatsBeaten);

            masked[line][column] = board[line][column]; //revela posição na máscara

            if(failure) attempts--; //se falhou perde uma chance
        }

        system("CLS");
    }

    //Fim de jogo
    int endgame = 0;

    cout << "Fim de jogo.\n";

    if(boatsBeaten >= TOBEBEATEN) cout << "Você conseguiu afundar " << boatsBeaten << " barcos e ganhou o jogo :D\n";

    else cout << "Você conseguiu afundar " << boatsBeaten << " barcos e perdeu o jogo :C\n";

    cout << "pontos: " << score << "\n";

    printBoard(board);
    //restart
    cout << "\n\nJogar novamente?\n\n1 - SIM  2-NÃO\n\n\n\n";
    cin >> endgame;

    system("CLS");

    if(endgame == 1) main();

    //bye
    cout << "Desenvolvido por Emanoel Rodrigues - 2020\n\n\n\n";
    system("PAUSE");
}

//-------------------------------------------------

int intro(){
    int opt;

    //menu
    cout << "Bem-Vindo à Batalha Naval 1.2\n";
    cout << "Você está na dificuldade " << modeselected;
    cout << "\n_______________________________\n\n";
    cout << "1 - JOGAR\n";
    cout << "2 - SOBRE\n";
    cout << "3 - COMO JOGAR\n";
    cout << "4 - ALTERAR DIFICULDADE\n";
    cout << "5 - SAIR \n";
    cout << "\nESCOLHA A OPÇÃO DESEJADA\nE PRESSIONE ENTER\n";

    cin >> opt;

    system("CLS");
    return opt;
}

//------------------------------------------------
//preenche as mesas recém criadas
void newBoard(char board[BOARDSIZE][BOARDSIZE], char masked[BOARDSIZE][BOARDSIZE]){
    for(int line = 0; line < BOARDSIZE; line++){
        for(int column = 0; column < BOARDSIZE; column++){
            board[line][column] = 'A';
            masked[line][column] = '*';
        }
    }
}

//------------------------------------------------
//imprime a mesa
void printBoard(char board[BOARDSIZE][BOARDSIZE]){

    cout << "    ";

    for(int i = 0; i < BOARDSIZE; i++){
        cout << i << " ";
    }
    cout << "\n    ";
    for(int i = 0; i < BOARDSIZE; i++){
        cout << "| ";
    }
    cout << "\n";

    for(int line = 0; line < BOARDSIZE; line++){
        cout << line << " - ";
        for(int column = 0; column < BOARDSIZE; column++){
            if(board[line][column] == 'A'){
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
                cout << board[line][column] << " ";
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
            else if(board[line][column] == 'P'){
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
                cout << board[line][column] << " ";
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
            else if(board[line][column] == 'M'){
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
                cout << board[line][column] << " ";
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
            else if(board[line][column] == 'G'){
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
                cout << board[line][column] << " ";
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }

            else cout << board[line][column] << " ";
        }
        cout << "\n";
    }
}

//------------------------------------------------
//sorteia os barcos
void boats(char board[BOARDSIZE][BOARDSIZE]){

    int L, C;

    //barcos pequenos
    for(int i = 0; i < P_BOATSAMT; i++){
        L = rand() % BOARDSIZE;
        C = rand() % BOARDSIZE;

        if(board[L][C] != 'A') i--; //caso posição seja diferente de água (ou seja, tenha barco), a função sorteia mais 1x
        else board[L][C] = 'P';
    }

    //barcos médios
    for(int i = 0; i < M_BOATSAMT; i++){
        L = rand() % BOARDSIZE;
        C = rand() % BOARDSIZE;

        if(board[L][C] != 'A' || board[L][C-1] != 'A' || board[L][C+1] != 'A') i--; //caso a posição seguinte ou anterior esteja ocupada
        else{
            board[L][C] = 'M';
            if(C == BOARDSIZE-1) board[L][C-1] = 'M'; //se a posição sorteada for na borda da mesa, a outra metade do barco fica uma posição atrás
            else board[L][C+1] = 'M'; //se não, a outra metade fica uma posição a frente
        }
    }

    //barcos grandes
    for(int i = 0; i < G_BOATSAMT; i++){
        L = rand() % BOARDSIZE;
        C = rand() % BOARDSIZE;

        if(board[L][C] != 'A' || board[L][C-1] != 'A' || board[L][C-2] != 'A' || board[L][C+1] != 'A' || board[L][C+2] != 'A') i--;
        else{
            board[L][C] = 'G';
            if(C >= BOARDSIZE-2){
                board[L][C-1] = 'G';
                board[L][C-2] = 'G';
            }
            else{
                board[L][C+1] = 'G';
                board[L][C+2] = 'G';
            }
        }
    }
}

//------------------------------------------------
//verifica o tiro
bool checkShoot(char board[BOARDSIZE][BOARDSIZE],char masked[BOARDSIZE][BOARDSIZE], int L, int C, int *score, string *msg, int *boats){

    switch(board[L][C]){
        case 'P':
            *score = *score + P_SCORE;
            *msg = "Afundou um barco pequeno\n";
            *boats = *boats + 1;
            return false;
            break;

        case 'M':
            //há um pequeno erro aqui, se houverem 2 barcos médios um do lado do outro, serão contabilizados 3
            //as duas metades do primeiro, metade do primeiro e metade do segundo e as duas metades do segundo
            //infelizmente não sei como corrigir
            if(masked[L][C+1] == 'M' || masked[L][C-1] == 'M'){
                *msg = "Afundou um barco médio\n";
                *score = *score + M_SCORE;
                *boats = *boats + 1;
            }
            else{
                *msg = "Acertou metade de um barco médio\n";
            }
            return false;
            break;

        case 'G':
            //o erro anterior se repete aqui, no caso 2 barcos viram 4
            if((masked[L][C+1] == 'G' && masked[L][C+2] == 'G') ||
                (masked[L][C-1] == 'G' && masked[L][C-2] == 'G') ||
                (masked[L][C+1] == 'G' && masked[L][C-1] == 'G')){

                *msg = "Afundou um barco grande\n";
                *score = *score + G_SCORE;
                *boats = *boats + 1;
            }
            else{
                *msg = "Acertou um barco grande, mas não afundou\n";
            }

            return false;
            break;

        default:
            *msg = "Acertou na água :C\n";
            return true;
    }
}

//------------------------------------------------

void mode(){
    int MODE;

    cout << "1 - FÁCIL   2 - NORMAL   3 - DIFÍCIL\n";
    cout << "Em qual nível deseja jogar? ";
    cin >> MODE;

    switch(MODE){
    case 1:
        modeselected    = MODEEASY;
        P_BOATSAMT      = M_EASY;
        G_BOATSAMT      = G_EASY;
        TOBEBEATEN      = TOWIN_EASY;
        MAXATP          = MAXATP_EASY;
        break;
    case 3:
        modeselected    = MODEHARD;
        P_BOATSAMT      = P_HARD;
        M_BOATSAMT      = M_HARD;
        G_BOATSAMT      = G_HARD;
        TOBEBEATEN      = TOWIN_HARD;
        MAXATP          = MAXATP_HARD;
        break;
    default:    //caso coloque 2 ou qualquer outra coisa, volta pro modo padrão
        modeselected    = DEFAULTMODE;
        P_BOATSAMT      = P_BOATDEFAULT;
        M_BOATSAMT      = M_BOATDEFAULT;
        G_BOATSAMT      = G_BOATDEFAULT;
        TOBEBEATEN      = TOWINDEFAULT;
        MAXATP          = MAXATPDEFAULT;
    }

    system("CLS");
    main();
}

//------------------------------------------------

void howToPlay(){
    cout << "Você está na dificuldade " << modeselected;
    cout << "\n\n           COMO FUNCIONA:\n\n";
    cout << "- Escolha aleatoriamente uma posição e tente afundar um barco.\n";
    cout << "- Você ganhará o jogo caso afunde "<< TOBEBEATEN << " barcos inteiros.\n";
    cout << "- Você começa com " << MAXATP << " chances. Caso erre o tiro, perderá uma chance.\n";
    cout << "- Há no total " << P_BOATSAMT + M_BOATSAMT + G_BOATSAMT << " barcos espalhados em " << BOARDSIZE * BOARDSIZE << " posições possívels.\n";
    cout << "- Barcos pequenos ocupam 1 posição. Médios ocupam 2 posições. \nE grandes ocupam 3 posições.\n\n\n\n";

    system("PAUSE");

    system("CLS");
    main();
}

//------------------------------------------------

void about(){
    cout << "Desenvolvido por Emanoel Rodrigues\n";
    cout << "Data da build: 02/12/2020\n\n\n";
    cout << "Curso: Linguagem C e C++ - Iniciante Ao Avançado por OneDay Code.\n\n";

    system("PAUSE");
    system("CLS");
    main();
}

//==================================================
