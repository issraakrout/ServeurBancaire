/**
 * SERVEUR BANCAIRE - Programmation Systèmes et Réseaux
 * Gère les comptes et les transactions de multiples clients
 * Compile: gcc -pthread -o server server.c
 * Exécute: ./server
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// ============== STRUCTURES DE DONNÉES ==============

typedef struct {
    int account_id;
    char account_name[50];
    double balance;
} Account;

typedef struct {
    int socket_fd;
    int client_id;
} ClientData;

// ============== VARIABLES GLOBALES ==============

// Base de données des comptes bancaires
#define NUM_ACCOUNTS 3
Account accounts[NUM_ACCOUNTS] = {
    {0, "Alice", 1000.0},
    {1, "Bob", 1500.0},
    {2, "Charlie", 2000.0}
};

// Mutex pour synchronisation des accès
pthread_mutex_t account_mutex = PTHREAD_MUTEX_INITIALIZER;

// Compteur de clients
int client_counter = 0;

// ============== FONCTIONS UTILITAIRES ==============

/**
 * Affiche tous les comptes (pour debug)
 */
void display_all_accounts() {
    printf("\n=== ÉTAT DES COMPTES ===\n");
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf("Compte %d (%s): %.2f€\n", 
               accounts[i].account_id, 
               accounts[i].account_name, 
               accounts[i].balance);
    }
    printf("========================\n\n");
}

/**
 * Traite une opération CHECK (consultation de solde)
 */
void handle_check(int account_id, char* response) {
    if (account_id < 0 || account_id >= NUM_ACCOUNTS) {
        sprintf(response, "ERREUR: Compte invalide");
        return;
    }
    
    pthread_mutex_lock(&account_mutex);  // 🔒 SYNCHRONISATION
    double balance = accounts[account_id].balance;
    pthread_mutex_unlock(&account_mutex); // 🔓
    
    sprintf(response, "Solde du compte %d (%s): %.2f€", 
            account_id, 
            accounts[account_id].account_name, 
            balance);
}

/**
 * Traite une opération DEPOSIT (dépôt)
 */
void handle_deposit(int account_id, double amount, char* response) {
    if (account_id < 0 || account_id >= NUM_ACCOUNTS) {
        sprintf(response, "ERREUR: Compte invalide");
        return;
    }
    
    if (amount <= 0) {
        sprintf(response, "ERREUR: Montant invalide");
        return;
    }
    
    pthread_mutex_lock(&account_mutex);  // 🔒 SYNCHRONISATION
    accounts[account_id].balance += amount;
    double new_balance = accounts[account_id].balance;
    pthread_mutex_unlock(&account_mutex); // 🔓
    
    sprintf(response, "Dépôt de %.2f€ réussi. Nouveau solde: %.2f€", 
            amount, new_balance);
    
    printf("[SERVEUR] Dépôt: Compte %d +%.2f€ (nouveau solde: %.2f€)\n",
           account_id, amount, new_balance);
}

/**
 * Traite une opération WITHDRAW (retrait)
 */
void handle_withdraw(int account_id, double amount, char* response) {
    if (account_id < 0 || account_id >= NUM_ACCOUNTS) {
        sprintf(response, "ERREUR: Compte invalide");
        return;
    }
    
    if (amount <= 0) {
        sprintf(response, "ERREUR: Montant invalide");
        return;
    }
    
    pthread_mutex_lock(&account_mutex);  // 🔒 SYNCHRONISATION
    
    // Vérifier le solde
    if (accounts[account_id].balance < amount) {
        pthread_mutex_unlock(&account_mutex); // 🔓
        sprintf(response, "ERREUR: Solde insuffisant. Solde actuel: %.2f€", 
                accounts[account_id].balance);
        return;
    }
    
    // Effectuer le retrait
    accounts[account_id].balance -= amount;
    double new_balance = accounts[account_id].balance;
    pthread_mutex_unlock(&account_mutex); // 🔓
    
    sprintf(response, "Retrait de %.2f€ réussi. Nouveau solde: %.2f€", 
            amount, new_balance);
    
    printf("[SERVEUR] Retrait: Compte %d -%.2f€ (nouveau solde: %.2f€)\n",
           account_id, amount, new_balance);
}

// ============== GESTION DES CLIENTS ==============

/**
 * Fonction exécutée par chaque thread client
 * Gère la communication avec UN client
 */
void* handle_client(void* arg) {
    ClientData* data = (ClientData*)arg;
    int client_socket = data->socket_fd;
    int client_id = data->client_id;
    
    char buffer[256];
    char response[256];
    
    printf("\n[THREAD #%d] Client connecté\n", client_id);
    
    // Boucle de communication avec le client
    while (1) {
        // Recevoir la commande du client
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received <= 0) {
            printf("[THREAD #%d] Client déconnecté\n", client_id);
            break;
        }
        
        buffer[bytes_received] = '\0';
        
        printf("[THREAD #%d] Reçu: %s\n", client_id, buffer);
        
        // Parser et traiter la commande
        int account_id;
        double amount;
        
        if (sscanf(buffer, "CHECK %d", &account_id) == 1) {
            handle_check(account_id, response);
        }
        else if (sscanf(buffer, "DEPOSIT %d %lf", &account_id, &amount) == 2) {
            handle_deposit(account_id, amount, response);
        }
        else if (sscanf(buffer, "WITHDRAW %d %lf", &account_id, &amount) == 2) {
            handle_withdraw(account_id, amount, response);
        }
        else if (strcmp(buffer, "EXIT") == 0) {
            strcpy(response, "Au revoir!");
        }
        else {
            strcpy(response, "Commande inconnue. Utilisez: CHECK, DEPOSIT, WITHDRAW, EXIT");
        }
        
        // Envoyer la réponse au client
        send(client_socket, response, strlen(response), 0);
        
        printf("[THREAD #%d] Envoyé: %s\n\n", client_id, response);
        
        // Afficher l'état des comptes
        display_all_accounts();
        
        // Si client quitte
        if (strcmp(buffer, "EXIT") == 0) {
            break;
        }
    }
    
    close(client_socket);
    free(data);
    return NULL;
}

// ============== FONCTION PRINCIPALE ==============

int main(int argc, char* argv[]) {
    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    int port = 5000;
    
    printf("╔════════════════════════════════════════╗\n");
    printf("║   🏦 SERVEUR BANCAIRE - EN DÉMARRAGE   ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    
    // ÉTAPE 1: Créer le socket serveur
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Erreur: impossible de créer le socket");
        exit(1);
    }
    printf("✓ Socket serveur créé\n");
    
    // Permettre la réutilisation du port
    int reuse = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    
    // ÉTAPE 2: Lier le socket à un port
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur: impossible de lier le socket");
        exit(1);
    }
    printf("✓ Socket lié au port %d\n", port);
    
    // ÉTAPE 3: Mettre le serveur en écoute
    if (listen(server_socket, 5) < 0) {
        perror("Erreur: listen() a échoué");
        exit(1);
    }
    printf("✓ Serveur en écoute...\n\n");
    printf("💡 Attendant les connexions sur 127.0.0.1:%d\n\n", port);
    
    display_all_accounts();
    
    // ÉTAPE 4: Accepter les clients
    while (1) {
        client_addr_len = sizeof(client_addr);
        
        // Accept bloque jusqu'à qu'un client se connecte
        int client_socket = accept(server_socket, 
                                   (struct sockaddr*)&client_addr,
                                   &client_addr_len);
        
        if (client_socket < 0) {
            perror("Erreur: accept() a échoué");
            continue;
        }
        
        // Incrémenter le compteur de clients
        client_counter++;
        
        // Créer une structure avec les infos du client
        ClientData* data = (ClientData*)malloc(sizeof(ClientData));
        data->socket_fd = client_socket;
        data->client_id = client_counter;
        
        printf("📱 Nouvelle connexion reçue (Client #%d)\n", client_counter);
        printf("   Adresse IP: %s\n\n", inet_ntoa(client_addr.sin_addr));
        
        // ÉTAPE 5: Créer un thread pour gérer ce client
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, (void*)data) != 0) {
            perror("Erreur: pthread_create() a échoué");
            free(data);
            close(client_socket);
            continue;
        }
        
        // Détacher le thread (il se libérera automatiquement)
        pthread_detach(thread_id);
    }
    
    close(server_socket);
    return 0;
}
