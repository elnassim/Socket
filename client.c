#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

typedef struct {
    char username[20];
    int password;
} Login;

typedef struct {
    char nom[50];
    char prenom[50];
    char gsm[30];
    char email[50];
    char rue[100];
    char ville[50];
    char pays[50];
} contact;

void addContact(SOCKET client) {
    contact nvC;

    // Demander les informations du contact à l'utilisateur
    printf("Nom du contact : ");
    scanf("%s", nvC.nom);
    printf("Prenom du contact : ");
    scanf("%s", nvC.prenom);
    printf("GSM du contact : ");
    scanf("%s", nvC.gsm);
    printf("Email du contact : ");
    scanf("%s", nvC.email);
    printf("Adresse - Nom de la rue : ");   
    scanf("%s", nvC.rue);
    printf("Adresse - Ville : ");
    scanf("%s", nvC.ville);
    printf("Adresse - Pays : ");
    scanf("%s", nvC.pays);
    printf("Contact saisi:  %s  %s  %s  %s  %s  %s  %s\n", nvC.nom, nvC.prenom, nvC.gsm, nvC.email, nvC.rue, nvC.ville, nvC.pays);

    // Envoyer les informations du contact au serveur
    send(client, (char *)&nvC, sizeof(contact), 0);

    // Attente de la réponse du serveur
    char reponse[100];
    recv(client, reponse, sizeof(reponse), 0);

    // Affichage de la réponse du serveur
    printf("Réponse du serveur : %s\n", reponse);
}

void Rechercher(SOCKET server) {
    char email[30]; // Email du contact à rechercher

    // Saisir l'email du contact à rechercher
    printf("Veuillez saisir l'email du contact que vous voulez rechercher:");
    scanf("%s", email);

    // Envoyer l'email au serveur
    if (send(server, email, sizeof(email), 0) == SOCKET_ERROR) {
        printf("Erreur lors de l'envoi de l'email au serveur.\n");
        return;
    }

    int contactCount;
    if (recv(server, (char*)&contactCount, sizeof(int), 0) == SOCKET_ERROR) {
        printf("Erreur lors de la réception du nombre de c.\n");
        return;
    }


    if (contactCount == 0) {
        printf("Le contact avec l'email '%s' n'a pas été trouvé.\n", email);
        return;
    }

    // Allouer de la mémoire pour stocker les contactes
    contact *c = malloc(sizeof(contact) * contactCount);
    if (c == NULL) {
        printf("Erreur d'allocation de mémoire pour les c.\n");
        return;
    }

    // Recevoir les contactes du serveur
    if (recv(server, (char*)c, sizeof(contact) * contactCount, 0) == SOCKET_ERROR) {
        printf("Erreur lors de la réception des c.\n");
        free(c);
        return;
    }



    int i = 0;
    // Vérifier si l'email a été trouvé
    if (strlen(c[0].email) == 0) {
        printf("Le contact avec l'email '%s' n'a pas été trouvé.\n", email);
    } else {
        while(i < contactCount)
        {
            printf("Contact trouvé :\n");
            printf("Nom : %s\n", c[i].nom);
            printf("Prenom : %s\n", c[i].prenom);
            printf("GSM : %s\n", c[i].gsm);
            printf("Email : %s\n", c[i].email);
            printf("Adresse :\n");
            printf("  Rue : %s\n", c[i].rue);
            printf("  Ville : %s\n", c[i].ville);
            printf("  Pays : %s\n", c[i].pays);
            printf("\n");
            i++;
        }
    }
}

void supprimerContact(SOCKET server) {
                   
    char emailToDelete[50];
    printf("Entrez l'email du contact à supprimer : ");
    scanf("%s", emailToDelete);
    // Envoyer l'email du contact à supprimer au serveur
    if (send(server, emailToDelete, sizeof(emailToDelete), 0) == SOCKET_ERROR) {
        printf("Erreur lors de l'envoi de la demande de suppression de contact.\n");
        return;
    }

    // Attendre la réponse du serveur
    char response[100];
    if (recv(server, response, sizeof(response), 0) == SOCKET_ERROR) {
        printf("Erreur lors de la réception de la réponse du serveur.\n");
        return;
    }

    // Afficher la réponse du serveur
    printf("Réponse du serveur : %s\n", response);
}

void afficherTousContacts(SOCKET server) {
    contact c;

    // Envoyer le choix au serveur pour afficher tous les contacts
    int choice = 5;
    send(server, (char *)&choice, sizeof(int), 0);

    // Attendre la réponse du serveur et afficher chaque contact reçu
    while (recv(server, (char*)&c, sizeof(contact), 0) != SOCKET_ERROR) {
        printf("Contact :\n");
        printf("Nom : %s\n", c.nom);
        printf("Prenom : %s\n", c.prenom);
        printf("GSM : %s\n", c.gsm);
        printf("Email : %s\n", c.email);
        printf("Adresse :\n");
        printf("  Rue : %s\n", c.rue);
        printf("  Ville : %s\n", c.ville);
        printf("  Pays : %s\n\n", c.pays);
    }
}


void adminMenu(SOCKET client) {
    int choice;
    do {
        printf("\n*******Menu******\n");
        printf("1-Ajouter un contact\n");
        printf("2-Rechercher un contact\n");
        printf("3-Supprimer un contact\n");
        printf("4-Modifier un contact\n");
        printf("5-Afficher tous les c\n");
        printf("6-Quitter\n");
        printf("Entrer votre choix: ");
        scanf("%d", &choice);
        // Envoyer le choix au serveur
        send(client, (char *)&choice, sizeof(int), 0);
        // Traiter la réponse du serveur si nécessaire
        switch (choice) {
            case 1:
                // Ajouter un contact
                addContact(client);
                break;
            case 2 :
                Rechercher(client);
                break;
            case 3:

                // Appeler la fonction pour supprimer le contact
               supprimerContact(client);
               break;
            case 4:

            case 5:
            afficherTousContacts(client);
            // Ajouter d'autres cas pour les autres options du menu
            default:
                printf("Choix invalide.\n");
                break;
        }
    } while (choice != 6);
}

void userMenu(SOCKET client) {
    int choice;
    do {
        printf("\n******Menu*******\n");
        printf("1-Rechercher un contact\n");
        printf("2-Afficher tous les c\n");
        printf("3-Quitter\n");
        printf("Entrer votre choix: ");
        scanf("%d", &choice);
        // Envoyer le choix au serveur
        send(client, (char *)&choice, sizeof(int), 0);
        // Traiter la réponse du serveur si nécessaire
        switch (choice) {
            case 1:
                // Rechercher un contact
                Rechercher(client);
                break;
            case 2:
                // Afficher tous les contacts
                afficherTousContacts(client);
                break;
            case 3:
                // Quitter
                printf("scored by MANOLAS \n");
                break;
            default:
                printf("Choix invalide.\n");
                break;
        }
    } while (choice != 3);
}

#define PORT 50000
#define MAX_LOGIN_ATTEMPTS 3

int main() {
    WSADATA wsa;
    SOCKET client, server;
    struct sockaddr_in serverAddr;
    Login loginInfo;
    int userType;
    int essai = 0; // compteur de tentatives

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock.\n");
        return 1;
    }

    // Create socket
    if ((client = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Failed to create socket.\n");
        WSACleanup();
        return 1;
    }

    // Server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(client, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Failed to connect to server.\n");
        closesocket(client);
        WSACleanup();
        return 1;
    }
    printf("Connection established.\n");

    do {
        // Input login credentials
        printf("Enter your username: ");
        scanf("%s", loginInfo.username);
        printf("Enter your password: ");
        scanf("%d", &loginInfo.password);

        // Send login info to server
        send(client, (char *)&loginInfo, sizeof(Login), 0);

        // Receive user type from server
        recv(client, (char *)&userType, sizeof(int), 0);

        // Check user type and display appropriate message
        switch (userType) {
            case 1:
                printf("You are an administrator.\n");
                adminMenu(client);
                break;
            case 2:
                printf("You are a guest user.\n");
                 userMenu(client);
                break;
            case 0:
                printf("Invalid username or password. Please try again.\n");
                essai++;
                break;
            default:
                printf("Unknown response from server.\n");
                break;
        }

        // If attempts exceed 3, stop the program
        if (essai >= MAX_LOGIN_ATTEMPTS) {
            printf("Maximum login attempts reached. Exiting.\n");
            closesocket(client);
            WSACleanup();
            return 1;
        }
    } while (userType == 0); // Continue looping until a valid user is logged in

    // Close socket
    closesocket(client);
    WSACleanup();

    return 0;
}