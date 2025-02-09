#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

char board[3][3] = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}};

void drawBoard() 
{
    system("cls");
    cout << "     |     |     " << endl;
    cout << "  " << board[0][0] << "  |  " << board[0][1] << "  |  " << board[0][2] << endl;
    cout << "_____|_____|_____" << endl;
    cout << "     |     |     " << endl;
    cout << "  " << board[1][0] << "  |  " << board[1][1] << "  |  " << board[1][2] << endl;
    cout << "_____|_____|_____" << endl;
    cout << "     |     |     " << endl;
    cout << "  " << board[2][0] << "  |  " << board[2][1] << "  |  " << board[2][2] << endl;
    cout << "     |     |     " << endl;
}

bool checkWin(char symbol) {
    for(int i = 0; i < 3; i++) {
        if(board[i][0] == symbol && board[i][1] == symbol && board[i][2] == symbol) return true;
        if(board[0][i] == symbol && board[1][i] == symbol && board[2][i] == symbol) return true;
    }
    if(board[0][0] == symbol && board[1][1] == symbol && board[2][2] == symbol) return true;
    if(board[0][2] == symbol && board[1][1] == symbol && board[2][0] == symbol) return true;
    return false;
}

bool isBoardFull() {
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            if(board[i][j] == ' ') return false;
    return true;
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(CP_UTF8);
       
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Ошибка инициализации WSA" << endl;
        return 1;
    }

    string choice;
    cout << "Создать игру (h) или Подключиться (c)? ";
    cin >> choice;

    if(choice == "h") {
        SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(12345);
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
        listen(listenSocket, 1);

        cout << "Ожидание второго игрока..." << endl;
        SOCKET clientSocket = accept(listenSocket, NULL, NULL);
        
        char symbol = 'X';
        bool myTurn = true;

        while(true) {
            drawBoard();
            
            if(myTurn) {
                int row, col;
                cout << "Ваш ход (строка[1-3] столбец[1-3]): ";
                cin >> row >> col;
                row--; col--;
                
                if(row >= 0 && row < 3 && col >= 0 && col < 3 && board[row][col] == ' ') {
                    board[row][col] = symbol;
                    string move = to_string(row) + to_string(col);
                    send(clientSocket, move.c_str(), move.length(), 0);
                    myTurn = false;
                } else {
                    cout << "Неверный ход! Попробуйте снова." << endl;
                    Sleep(1000);
                }
            } else {
                cout << "Ожидание хода противника..." << endl;
                char buffer[2];
                recv(clientSocket, buffer, 2, 0);
                int row = buffer[0] - '0';
                int col = buffer[1] - '0';
                board[row][col] = 'O';
                myTurn = true;
            }

            if(checkWin('X')) {
                drawBoard();
                cout << "Крестики победили!" << endl;
                break;
            }
            if(checkWin('O')) {
                drawBoard();
                cout << "Нолики победили!" << endl;
                break;
            }
            if(isBoardFull()) {
                drawBoard();
                cout << "Ничья!" << endl;
                break;
            }
        }
        closesocket(clientSocket);
        closesocket(listenSocket);
    }
    else if(choice == "c") {
        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(12345);
        
        string ip;
        cout << "Введите IP-адрес хоста: ";
        cin >> ip;
        inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

        connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
        
        char symbol = 'O';
        bool myTurn = false;

        while(true) {
            drawBoard();
            
            if(myTurn) {
                int row, col;
                cout << "Ваш ход (строка[1-3] столбец[1-3]): ";
                cin >> row >> col;
                row--; col--;
                
                if(row >= 0 && row < 3 && col >= 0 && col < 3 && board[row][col] == ' ') {
                    board[row][col] = symbol;
                    string move = to_string(row) + to_string(col);
                    send(clientSocket, move.c_str(), move.length(), 0);
                    myTurn = false;
                } else {
                    cout << "Неверный ход! Попробуйте снова." << endl;
                    Sleep(1000);
                }
            } else {
                cout << "Ожидание хода противника..." << endl;
                char buffer[2];
                recv(clientSocket, buffer, 2, 0);
                int row = buffer[0] - '0';
                int col = buffer[1] - '0';
                board[row][col] = 'X';
                myTurn = true;
            }

            if(checkWin('X')) {
                drawBoard();
                cout << "Крестики победили!" << endl;
                break;
            }
            if(checkWin('O')) {
                drawBoard();
                cout << "Нолики победили!" << endl;
                break;
            }
            if(isBoardFull()) {
                drawBoard();
                cout << "Ничья!" << endl;
                break;
            }
        }
        closesocket(clientSocket);
    }

    WSACleanup();
    cout << "\nНажмите Enter для выхода...";
    cin.ignore();
    cin.get();
    return 0;
}
