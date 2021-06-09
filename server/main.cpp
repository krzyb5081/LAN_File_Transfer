#include <iostream>
#include <winsock2.h>
#include <stdio.h>
#include <fstream>

using namespace std;
//*****SERWER*****//
int main()
{
    ////niezbedne dla klienta i serwera
    /////*****konfiguracja****/////////
    char recvBuffer[1024];
    int iloscBajtow = 0;
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if(result != NO_ERROR) {
        cout << "WSAStartup() sie nie udalo" << endl;
    }

    SOCKET mainSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(mainSocket == SOCKET_ERROR) {
        cout << "Stworzenie socketu sie nie udalo" << endl;

    }

    sockaddr_in service;
    memset(& service, 0, sizeof(service));
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = htonl(INADDR_ANY);
    service.sin_port = htons(54543);

    //serwer
    if(bind(mainSocket, (SOCKADDR *) & service, sizeof(service)) == SOCKET_ERROR) {
        cout << "bind sie nie udal" << endl;
        closesocket(mainSocket);
        return 1;
    }

    if(listen(mainSocket,1) == SOCKET_ERROR) {
        cout << "Blad z nasluchiwaniem socketa" << endl;
    }

    SOCKET acceptSocket = SOCKET_ERROR;
    cout << "Oczekiwanie na polaczenie sie klienta" << endl;

    char cale[100]={0};
    hostent *lphe;
    gethostname(cale, 100);
    lphe = gethostbyname(cale);
    cout << "Adres serwera: " << inet_ntoa(*(in_addr*)*lphe->h_addr_list) << endl;
    //o chuj chodzi z gwiazdkami

    while(acceptSocket == SOCKET_ERROR) {
        acceptSocket = accept(mainSocket, NULL, NULL);
    }
    mainSocket = acceptSocket;

    cout << "Polaczono z klientem" << endl;
    ///////////////////////////////////
    ////****koniec konfiguracji****////
    ///////////////////////////////////

    //wymiana danych

    //recv: nazwa pliku
    sprintf(recvBuffer, "\0");
    if((iloscBajtow = recv(mainSocket,recvBuffer,1024,0)) == -1){
        cout << "blad w odbieraniu danych" << endl;
        while(1);
    }
    recvBuffer[iloscBajtow] = '\0';

    string nazwaPliku = recvBuffer;
    cout << "Nazwa pobieranego pliku: " << nazwaPliku << endl;


    //send: chce czy nie chce tak/nie
    if(send(mainSocket,"tak",3,0) == -1){
        cout << "blad w wysylaniu danych" << endl;
        while(1);
    }

    //recv: wielkosc pliku
    sprintf(recvBuffer, "\0");
    if((iloscBajtow = recv(mainSocket,recvBuffer,1024,0)) == -1){
        cout << "blad w odbieraniu danych" << endl;
        while(1);
    }
    recvBuffer[iloscBajtow] = '\0';


    string wielkoscPlikuS = recvBuffer;
    unsigned long long wielkoscPliku = atoll(wielkoscPlikuS.c_str());//tutaj bylo atio
    cout << "Wielkosc pobieranego pliku: " << wielkoscPlikuS << endl;


    std::fstream fileWriter;
    fileWriter.open(nazwaPliku.c_str(), std::fstream::out | std::fstream::binary);

    unsigned long long bytesDownloaded = 0;
    do {
        //recv: zaawartosc pliku
        sprintf(recvBuffer, "\0");
        if((iloscBajtow = recv(mainSocket,recvBuffer,1024,0)) == -1){
            cout << "blad w odbieraniu danych" << endl;
            while(1);
        }
        recvBuffer[iloscBajtow] = '\0';

        bytesDownloaded += iloscBajtow;

        fileWriter.write(recvBuffer, iloscBajtow);

        if((bytesDownloaded*100)%wielkoscPliku==0){
            cout << "Pobrano " << (bytesDownloaded*100)/wielkoscPliku << "% - " << bytesDownloaded << " / " << wielkoscPliku << "b" << endl;
        }


    } while(bytesDownloaded < wielkoscPliku);

    cout << "Plik pobrany" << endl;
    fileWriter.close();
    closesocket(mainSocket);





while(1){};
return 0;
}
