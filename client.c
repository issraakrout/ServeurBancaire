/**
 * CLIENT BANCAIRE - Programmation Systèmes et Réseaux
 * Se connecte au serveur et effectue des opérations bancaires
 * Compile: gcc -o client client.c
 * Exécute: ./client
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Configuration
#define SERVER_IP "127.0.0.1"  // localhost
#define SERVER_PORT 5000
#define BUFFER_SIZE 256

// ============== AFFICHAGE DU MENU ==============

void display_menu() {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║      💳 MENU DES OPÉRATIONS          ║\n");
    printf("╠════════════════════════════════════════╣\n");
    printf("║ 1. CHECK <compte>                      ║\n");
    printf("║    Exemple: CHECK 0                    ║\n");
    printf("║    → Consulter le solde du compte 0    ║\n");
    printf("║                                        ║\n");
    printf("║ 2. DEPOSIT <compte> <montant>          ║\n");
    printf("║    Exemple: DEPOSIT 1 100              ║\n");
    printf("║    → Déposer 100€ sur le compte 1      ║\n");
    printf("║                                        ║\n");
    printf("║ 3. WITHDRAW <compte> <montant>         ║\n");
    printf("║    Exemple: WITHDRAW 0 50              ║\n");
    printf("║    → Retirer 50€ du compte 0           ║\n");
    printf("║                                        ║\n");
    printf("║ 4. EXIT                                ║\n");
    printf("║    → Quitter l'application             ║\n");
    printf("║                                        ║\n");
    printf("║ Comptes disponibles: 0 (Alice)         ║\n");
    printf("║                     1 (Bob)            ║\n");
    printf("║                     2 (Charlie)        ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
}

// ============== FONCTION PRINCIPALE ==============

int main(int argc, char* argv[]) {
    int socket_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char command[BUFFER_SIZE];
    
    printf("╔════════════════════════════════════════╗\n");
    printf("║    🏦 CLIENT BANCAIRE - CONNEXION      ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    
    // ÉTAPE 1: Créer le socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("Erreur: impossible de créer le socket");
        exit(1);
    }
    printf("✓ Socket créé\n");
    
    // ÉTAPE 2: Configurer l'adresse du serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_aton(SERVER_IP, &server_addr.sin_addr);
    
    // ÉTAPE 3: Se connecter au serveur
    if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("❌ ERREUR: Impossible de se connecter au serveur");
        printf("   Assurez-vous que le serveur est lancé sur %s:%d\n", 
               SERVER_IP, SERVER_PORT);
        exit(1);
    }
    
    printf("✓ Connecté au serveur sur %s:%d\n\n", SERVER_IP, SERVER_PORT);
    
    display_menu();
    
    // ÉTAPE 4: Boucle de communication
    while (1) {
        printf(">>> Entrez une commande: ");
        fgets(command, sizeof(command), stdin);
        
        // Supprimer le '\n' à la fin
        command[strcspn(command, "\n")] = 0;
        
        // Vérifier si la commande est vide
        if (strlen(command) == 0) {
            continue;
        }
        
        // Envoyer la commande au serveur
        if (send(socket_fd, command, strlen(command), 0) < 0) {
            perror("Erreur: envoi échoué");
            break;
        }
        
        printf("📤 Envoyé au serveur: %s\n", command);
        
        // Recevoir la réponse du serveur
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(socket_fd, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received <= 0) {
            printf("❌ Serveur déconnecté\n");
            break;
        }
        
        buffer[bytes_received] = '\0';
        
        printf("📥 Réponse du serveur:\n");
        printf("   ✓ %s\n\n", buffer);
        
        // Si l'utilisateur a entré EXIT
        if (strcmp(command, "EXIT") == 0) {
            printf("Au revoir! 👋\n");
            break;
        }
    }
    
    // ÉTAPE 5: Fermer la connexion
    close(socket_fd);
    printf("Déconnexion...\n");
    
    return 0;
}
