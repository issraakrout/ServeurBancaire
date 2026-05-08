# 📚 RÉSUMÉ TECHNIQUE - SYSTÈME BANCAIRE DISTRIBUÉ

## 🎓 CONCEPTS COUVERTS PAR LE PROJET

---

## 1️⃣ SYNCHRONISATION (Mutex)

### Problème
Sans protection, quand deux threads accèdent au même compte :

```c
// SANS MUTEX - MAUVAIS ! ❌
Thread 1: lire balance = 1000
Thread 2: lire balance = 1000
Thread 1: écrire balance = 1000 + 500 = 1500
Thread 2: écrire balance = 1000 + 300 = 1300  ← PERTE DE 500€ !
```

### Solution
Utiliser un mutex pour bloquer l'accès :

```c
// AVEC MUTEX - BON ! ✅
pthread_mutex_lock(&account_mutex);    // Bloquer

// Seul ce thread peut accéder
balance = 1000;
balance += 500;  // 1500

pthread_mutex_unlock(&account_mutex);  // Libérer

// Prochain thread :
pthread_mutex_lock(&account_mutex);
balance = 1500;  // Lecture correcte !
balance += 300;  // 1800
pthread_mutex_unlock(&account_mutex);
```

### Implémentation dans le Code

**server.c ligne ~200 :**
```c
pthread_mutex_t account_mutex = PTHREAD_MUTEX_INITIALIZER;

// Utilisation dans handle_deposit() :
void handle_deposit(int account_id, double amount, char* response) {
    pthread_mutex_lock(&account_mutex);      // 🔒 LOCK
    
    accounts[account_id].balance += amount;  // Modification sûre
    
    pthread_mutex_unlock(&account_mutex);    // 🔓 UNLOCK
}
```

### Point Clé pour le Professeur
> "Le mutex garantit que **jamais deux threads ne modifient le même compte en même temps**. C'est la base de la programmation concurrente sûre."

---

## 2️⃣ GESTION DES THREADS

### Création de Threads

**server.c ligne ~300 :**
```c
pthread_t thread_id;

// Créer un nouveau thread pour chaque client
pthread_create(&thread_id, NULL, handle_client, (void*)data);

// Détacher le thread (se libère automatiquement)
pthread_detach(thread_id);
```

**Qu'est-ce qu'il se passe :**
1. Client A se connecte → `pthread_create()` crée Thread #1
2. Client B se connecte → `pthread_create()` crée Thread #2
3. Thread #1 et #2 exécutent `handle_client()` **en parallèle**
4. Chaque thread dispose sa propre pile d'exécution
5. Les threads **PARTAGENT** la structure `accounts[]`

### Structure d'un Thread Serveur

```c
void* handle_client(void* arg) {
    ClientData* data = (ClientData*)arg;
    int client_socket = data->socket_fd;
    
    // Chaque thread a ses propres variables locales
    char buffer[256];
    
    // Mais partage la structure globale
    accounts[id].balance  // ← PARTAGÉE
    
    // Boucle infinie pour ce client
    while (1) {
        recv(client_socket, ...);  // Recevoir une commande
        // Traiter la commande
        send(client_socket, ...);  // Envoyer réponse
    }
    
    close(client_socket);
    return NULL;
}
```

### Synchronisation des Threads

```
Serveur démarre
    ↓
Écoute les connexions (ligne: accept())
    ↓
Client 1 connecté → Créer Thread #1 → Détacher
    ↓
Retour à accept() IMMÉDIATEMENT
    ↓
Client 2 connecté → Créer Thread #2 → Détacher
    ↓
Thread #1 et Thread #2 travaillent EN PARALLÈLE
    ↓
Serveur continue à écouter (boucle while infinie)
```

### Point Clé pour le Professeur
> "Avec `pthread_create()` et `pthread_detach()`, le serveur peut gérer **plusieurs clients simultanément**. Chaque client a son propre thread, mais ils partagent les mêmes données (comptes)."

---

## 3️⃣ COMMUNICATION PAR SOCKETS

### Architecture Réseau

```
┌─────────────────────────────────────────────┐
│              COUCHE TCP/IP                  │
└─────────────────────────────────────────────┘
              ↓              ↓
         ┌────────┐   ┌──────────┐
         │ Socket │   │  Socket  │
         │serveur │   │  client  │
         └────────┘   └──────────┘
              ↓              ↓
         127.0.0.1:5000  127.0.0.1:XXXX
```

### Côté Serveur

**server.c ligne ~100 :**

```c
// Étape 1: Créer le socket
int server_socket = socket(AF_INET, SOCK_STREAM, 0);

// Étape 2: Lier à un port
struct sockaddr_in server_addr;
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(5000);           // Port 5000
server_addr.sin_addr.s_addr = INADDR_ANY;     // Toutes les interfaces
bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

// Étape 3: Écouter les connexions
listen(server_socket, 5);  // Queue de 5 connexions max

// Étape 4: Accepter les clients (boucle infinie)
while (1) {
    int client_socket = accept(server_socket, ...);  // 🛑 BLOQUE jusqu'à client
    // Créer thread pour ce client
}
```

### Côté Client

**client.c ligne ~50 :**

```c
// Étape 1: Créer le socket client
int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

// Étape 2: Configurer l'adresse du serveur
struct sockaddr_in server_addr;
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(5000);
inet_aton("127.0.0.1", &server_addr.sin_addr);

// Étape 3: Se connecter
connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

// Étape 4: Boucle d'interaction
while (1) {
    // Lire une commande de l'utilisateur
    fgets(command, sizeof(command), stdin);
    
    // Envoyer la commande au serveur
    send(socket_fd, command, strlen(command), 0);
    
    // Recevoir la réponse du serveur
    recv(socket_fd, buffer, sizeof(buffer), 0);
    
    // Afficher la réponse
    printf("%s\n", buffer);
}
```

### Flux de Communication

```
CLIENT                          SERVEUR
  │                                │
  ├─ socket() ────────────────────┤ socket()
  │                                │
  ├─ connect("127.0.0.1:5000") ──┤ accept()
  │                                │ pthread_create()
  │                                │
  ├─ send("CHECK 0") ────────────>│ recv()
  │                                │ handle_check()
  │<──── recv("Solde: 1000€") ────┤ send()
  │                                │
  ├─ send("DEPOSIT 0 100") ──────>│ recv()
  │                                │ handle_deposit()
  │<── recv("Nouveau: 1100€") ────┤ send()
  │                                │
  ├─ send("EXIT") ───────────────>│ recv()
  │                                │ exit thread
  ├─ close() ─────────────────────┤ close()
  │                                │
```

### Point Clé pour le Professeur
> "Les sockets TCP permettent une **communication fiable et ordonnée** entre serveur et clients. Les données arrivent en ordre et sans perte, contrairement à UDP."

---

## 4️⃣ CONCEPT CRÉATIF : SYSTÈME BANCAIRE

### Fonctionnalités Implémentées

| Feature | Implémentation |
|---------|-----------------|
| Comptes bancaires | Tableau `accounts[3]` avec Alice, Bob, Charlie |
| Solde | Champ `double balance` |
| Consultation | Commande `CHECK id` → affiche solde |
| Dépôt | Commande `DEPOSIT id montant` → solde += montant |
| Retrait | Commande `WITHDRAW id montant` → solde -= montant |
| Erreurs | Vérification du solde avant retrait |
| Persistance (optionnel) | Sauvegarder dans un fichier |

### Structure de Données

```c
typedef struct {
    int account_id;
    char account_name[50];
    double balance;
} Account;

Account accounts[3] = {
    {0, "Alice", 1000.0},
    {1, "Bob", 1500.0},
    {2, "Charlie", 2000.0}
};
```

### Logique d'une Transaction

```c
void handle_withdraw(int account_id, double amount, char* response) {
    // 1. Synchronisation
    pthread_mutex_lock(&account_mutex);
    
    // 2. Validation
    if (accounts[account_id].balance < amount) {
        return "ERREUR: Solde insuffisant";
    }
    
    // 3. Exécution atomique
    accounts[account_id].balance -= amount;
    
    // 4. Libération
    pthread_mutex_unlock(&account_mutex);
    
    // 5. Confirmation
    return "Retrait réussi";
}
```

### Point Clé pour le Professeur
> "Ce concept bancaire n'est pas juste pour le plaisir - il démontre comment les concepts de synchronisation, threads et sockets s'appliquent à un **vrai problème du monde réel**."

---

## 🔧 RÉSUMÉ TECHNIQUE COMPLET

### Fichiers du Projet

| Fichier | Lignes | Rôle |
|---------|--------|------|
| server.c | ~400 | Gère les comptes, accepte clients, crée threads |
| client.c | ~150 | Interface utilisateur, communication TCP |
| Makefile | ~20 | Compilation avec les bonnes options |

### Options de Compilation

```bash
gcc -pthread -Wall -std=c99 -o server server.c
     │
     └─ -pthread : active la librairie POSIX threads
```

### Commandes Disponibles

```
CHECK <compte_id>
    → Affiche le solde du compte
    
DEPOSIT <compte_id> <montant>
    → Ajoute montant au compte
    
WITHDRAW <compte_id> <montant>
    → Retire montant du compte (si solde suffisant)
    
EXIT
    → Déconnecte le client
```

### Flux d'Exécution Global

```
1. Lancer serveur ./server
   ├─ Crée socket
   ├─ Lie au port 5000
   ├─ Entre en boucle accept()
   └─ Attend clients...

2. Lancer client 1 ./client
   ├─ Crée socket
   ├─ Se connecte à 127.0.0.1:5000
   ├─ Serveur : accept() retourne, crée Thread #1
   └─ Client entre en boucle de commandes

3. Lancer client 2 ./client
   ├─ Crée socket
   ├─ Se connecte à 127.0.0.1:5000
   ├─ Serveur : accept() retourne, crée Thread #2
   ├─ Thread #1 et #2 coexistent
   └─ Gérés par le mutex pour les comptes partagés

4. Opérations
   ├─ Client 1 : DEPOSIT 0 100 → Thread #1 exécute
   ├─ Client 2 : WITHDRAW 0 50 → Thread #2 exécute (attend mutex)
   ├─ Mutex assure : pas de corruption de données
   └─ Résultat : 1000 + 100 - 50 = 1050€ ✅

5. Arrêt
   ├─ Client 1 : EXIT → Thread #1 se termine
   ├─ Client 2 : EXIT → Thread #2 se termine
   └─ Serveur : continue à écouter
```

---

## 📊 COMPARAISON : AVEC vs SANS SYNCHRONISATION

### SANS Mutex (❌ Mauvais)
```
Temps  Thread 1              Thread 2
────────────────────────────────────
0      lire balance = 1000
       (1000) ← état partagé
                              lire balance = 1000
1      balance += 500
       écrire 1500 ← état partagé
       
                              balance += 300
                              écrire 1300 ← OUPS !
                              
Résultat: 1300€ au lieu de 1800€ 💥
```

### AVEC Mutex (✅ Bon)
```
Temps  Thread 1                    Thread 2
─────────────────────────────────────────────
0      LOCK (bloque)
       lire balance = 1000
       balance += 500
1      écrire 1500
       UNLOCK (libère)
                                   LOCK (prend)
                                   lire balance = 1500
                                   balance += 300
                                   écrire 1800
                                   UNLOCK
       
Résultat: 1800€ ✅
```

---

## 🎯 CRITÈRES D'ÉVALUATION

### ✅ Synchronisation
- [ ] Utilise `pthread_mutex_t`
- [ ] `pthread_mutex_lock()` avant accès aux données
- [ ] `pthread_mutex_unlock()` après modification
- [ ] Les données ne se corrompent jamais, même avec accès simultanés

### ✅ Gestion des Threads
- [ ] `pthread_create()` pour créer threads clients
- [ ] Chaque client = 1 thread
- [ ] `pthread_detach()` pour libération automatique
- [ ] Plusieurs clients peuvent se connecter simultanément

### ✅ Communication par Sockets
- [ ] `socket()` pour créer les sockets
- [ ] `bind()` pour lier le serveur
- [ ] `listen()` et `accept()` pour écouter/accepter
- [ ] `send()` et `recv()` pour communiquer
- [ ] Protocol TCP fiable sur localhost:5000

### ✅ Concept Créatif
- [ ] Système bancaire fonctionnel
- [ ] Opérations réalistes (check, deposit, withdraw)
- [ ] Gestion d'erreurs (solde insuffisant)
- [ ] Bien documenté et expliqué

---

## 💡 EXPLICATIONS POUR LA PRÉSENTATION

### "Qu'est-ce qu'un thread ?"
> "C'est un flux d'exécution léger qui partage la mémoire avec les autres threads du même processus. Ici, le serveur crée un thread par client pour les gérer en parallèle."

### "Pourquoi un mutex ?"
> "Sans mutex, deux threads pourraient modifier le même compte en même temps, causant une corruption de données. Le mutex garantit qu'un seul thread accède à un moment donné."

### "Pourquoi des sockets ?"
> "Les sockets permettent la communication réseau fiable entre le serveur et les clients. TCP garantit que chaque message arrive intact et dans l'ordre."

### "Pourquoi ce concept bancaire ?"
> "C'est un exemple réaliste qui montre pourquoi la synchronisation et la concurrence sont critiques dans le monde réel. Une banque ne peut pas se permettre de perdre de l'argent !"

---

## 🚀 POINTS FORTS À SOULIGNER

1. **Production-Ready Code** : Code réel, pas juste un jouet
2. **Vrai Problème** : Transactions bancaires = défi de concurrence réel
3. **Tous les Concepts** : Mutex + Threads + Sockets + Erreurs
4. **Facilement Testable** : Facile de voir 2 clients opérer simultanément
5. **Extensible** : Peut ajouter persistance, authentification, etc.

