#include "library.h"

#include <iostream>
#include <cstring>
#include <csignal>
#include <netinet/in.h>
#include <arpa/inet.h>


int ServerSocket(int port)
{
    int serverSocket;
    struct sockaddr_in serverAddr;
    // Création du socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }
    // Configuration de l'adresse du serveur
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    // Liaison du socket à l'adresse du serveur
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Erreur lors de la liaison du socket à l'adresse du serveur");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    // Mise en écoute du socket
    if (listen(serverSocket, 5) == -1) {
        perror("Erreur lors de la mise en écoute du socket");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    return serverSocket;
}
int Accept(int sEcoute,char *ipClient)
{
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket;
    // Accepter la connexion entrante
    clientSocket = accept(sEcoute, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
        perror("Erreur lors de l'acceptation de la connexion");
        return -1;
    }
    // Récupérer l'adresse IP du client
    if (ipClient != NULL) {
        strcpy(ipClient, inet_ntoa(clientAddr.sin_addr));
    }
    return clientSocket;
}
int ClientSocket(char* ipServeur,int portServeur)
{
    int clientSocket;
    struct sockaddr_in serverAddr;
    // Création du socket client
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Erreur lors de la création du socket client");
        exit(EXIT_FAILURE);
    }
    // Configuration de l'adresse du serveur
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portServeur);
    if (inet_pton(AF_INET, ipServeur, &serverAddr.sin_addr) <= 0) {
        perror("Adresse IP du serveur invalide");
        exit(EXIT_FAILURE);
    }
    // Connexion au serveur
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Erreur lors de la connexion au serveur");
        exit(EXIT_FAILURE);
    }
    return clientSocket;
}
/*********************************************/
/*Fonction Send du socket */
/*********************************************/
int Send(int sSocket,char* data,int taille)
{
    if (taille > TAILLE_MAX_DATA)
        return -1;
    // Preparation de la charge utile
    char trame[TAILLE_MAX_DATA+2];
    memcpy(trame,data,taille);
    trame[taille] = '#';
    trame[taille+1] = ')';
    // Ecriture sur la socket
    return write(sSocket,trame,taille+2)-2;
}
/*********************************************/
/* Fonction Receive du socket */
/*********************************************/
int Receive(int sSocket,char* data)
{
    bool fini = false;
    int nbLus, i = 0;
    char lu1,lu2;
    while(!fini)
    {
        if ((nbLus = read(sSocket,&lu1,1)) == -1)
            return -1;
        if (nbLus == 0) return i; // connexion fermee par client
        if (lu1 == '#')
        {
            if ((nbLus = read(sSocket,&lu2,1)) == -1)
                return -1;
            if (nbLus == 0) return i; // connexion fermee par client
            if (lu2 == ')') fini = true;
            else
            {
                data[i] = lu1;
                data[i+1] = lu2;
                i += 2;
            }
        }
        else
        {
            data[i] = lu1;
            i++;
        }
    }
    return i;
}

