#include <iostream>
#include <winsock2.h>
#include <stdio.h>
#include <fstream>

using namespace std;
//*****KLIENNT*****//

//int wyslij(SOCKET & socket, const char * buffer, int bytesy, int flagi);
//int odbierz(SOCKET & socket, con)
int main()
{

    ////niezbedne dla klienta i serwera
    ////****konfiguracja****///////////
    char recvBuffer[1024];
    int iloscBajtow = 0;
    string nazwaPliku = "";
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if(result != NO_ERROR) {
        cout << "WSAStartup() sie nie udalo" << endl;
        while(1);
    }

    SOCKET mainSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(mainSocket == SOCKET_ERROR) {
        cout << "Stworzenie socketu sie nie udalo" << endl;
        while(1);
    }

    cout << "Numer IP komputera na ktory wysylasz plik\n(liczby oddzielane kropkami bez spacji): " << endl;
    string ipSerwera = "";
    getline(cin,ipSerwera);

    sockaddr_in service;
    memset(& service, 0, sizeof(service));
    service.sin_family = AF_INET;
    service.sin_port = htons(54543);
    service.sin_addr.s_addr = inet_addr(ipSerwera.c_str());

    if(service.sin_addr.s_addr == -1){
        cout << "nie udalo sie zrobic ajpi" << endl;
    }

    //klient
    while(connect(mainSocket, (SOCKADDR *) &service, sizeof(service)) == SOCKET_ERROR) {
        cout << "Trwa laczenie z serwerem : error :" << WSAGetLastError() << endl;
        //while(1);
    }

    cout << "Polaczono z serwerem" << endl;

    ///////////////////////////////////
    ////****koniec konfiguracji****////
    ///////////////////////////////////


    //wymiana danych


    cout << "Nazwa pliku: " << endl;
    getline(cin,nazwaPliku);

    //send: nazwa pliku
    if(send(mainSocket,nazwaPliku.c_str(),nazwaPliku.length(),0) == -1) {
        cout << "blad w wysylaniu danych" << endl;
        while(1);
    }

    //recv: chce czy nie chce tak/nie
    sprintf(recvBuffer,"\0");
    if((iloscBajtow = recv(mainSocket,recvBuffer,1024,0)) == -1) {
        cout << "blad w odbieraniu danych" << endl;
        while(1);
    }
    recvBuffer[iloscBajtow] = '\0';

    string odpowiedz = recvBuffer;
    if(!(odpowiedz.compare("tak")==0)) {
        cout << "serwer nie chce pliku" << endl;
        while(1);
    }

    //sprawdzanie wielkosci pliku i czy istnieje
    std::fstream fileSizeReader;
    fileSizeReader.open(nazwaPliku.c_str(), std::fstream::in | std::fstream::binary | std::fstream::ate);
    unsigned long long wielkoscPliku = fileSizeReader.tellg();
    if (wielkoscPliku == -1) {
        cout << "Nie mozna odczytac pliku" << endl;
        while(1);

    }
    fileSizeReader.close();

    //zamiana liczby na ciag znakow
    char * wielkoscPlikuChar = new char[1024];
    sprintf(wielkoscPlikuChar,"%llu\0",wielkoscPliku);

    cout << "Wielkosc pliku: " << wielkoscPlikuChar << endl;

    //send: wielkosc pliku
    if(send(mainSocket,wielkoscPlikuChar,strlen(wielkoscPlikuChar),0) == -1) {
        cout << "blad w wysylaniu danych" << endl;
        while(1);
    }

    std::fstream fileReader;
    fileReader.open(nazwaPliku.c_str(), std::fstream::in | std::fstream::binary);

    char buffer[1024];
    do {
        fileReader.read(buffer, sizeof(buffer));

        //send: zawartosc pliku
        if(send(mainSocket,buffer,fileReader.gcount(),0) == -1) {
            cout << "blad w wysylaniu danych" << endl;
            while(1);
        }
nie dzialaja procenty jak trzeba
        if((fileReader.gcount()*100)%wielkoscPliku==0){
            cout << "Wyslano " << (fileReader.gcount()*100)/wielkoscPliku << "% - " << fileReader.gcount() << " / " << wielkoscPliku << "b" << endl;
        }

    } while(fileReader.gcount()!=0);

    cout << "plik wyslany" << endl;

    fileReader.close();
    closesocket(mainSocket);



    //klient wysyla drugiemu klientowi wiadomosc o tym ze chce wyslac plik
    //przydadza sie sockety datagramowe


    //https://stackoverflow.com/questions/4252593/how-do-i-make-my-server-application-use-my-ip-address-instead-of-localhost


while(1);
return 0;
}
