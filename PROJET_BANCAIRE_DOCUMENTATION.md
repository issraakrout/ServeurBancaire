# 🏦 PROJET : SYSTÈME BANCAIRE DISTRIBUÉ
## Programmation Systèmes et Réseaux

---

## 📋 RÉSUMÉ DU PROJET

**Concept** : Un système bancaire avec :
- **1 Serveur Central** : gère les comptes et transactions
- **2 Clients (Guichets)** : permettent les dépôts/retraits
- **Synchronisation** : threads pour gérer clients simultanés
- **Communication** : sockets TCP/IP
- **Concurrence** : gestion des accès simultanés aux comptes

---

## 🎯 FONCTIONNALITÉS

### Serveur Bancaire (`server.c`)
- Gère une base de données de comptes en mémoire
- Accepte les connexions multiples (threads)
- Traite les opérations : consultation, dépôt, retrait
- **Synchronisation** : mutex pour éviter les conditions de course

### Clients (`client1.c` et `client2.c`)
- Se connectent au serveur via sockets TCP
- Effectuent des opérations bancaires
- Peuvent se connecter **simultanément**
- Reçoivent les confirmations du serveur

---

## 🏗️ ARCHITECTURE

```
┌─────────────────────────────────────────┐
│         SERVEUR BANCAIRE               │
│  - Gère les comptes (struct Account)   │
│  - Écoute sur port 5000                │
│  - Crée un thread par client           │
│  - Utilise mutex pour synchronisation  │
└──────────────┬──────────────────────────┘
               │
        ┌──────┴──────┐
        │             │
   ┌────▼────┐   ┌───▼─────┐
   │ CLIENT 1 │   │ CLIENT 2 │
   │ Port TCP │   │ Port TCP │
   └──────────┘   └──────────┘
```

---

## 📚 CONCEPTS CLÉS COUVERTS

### 1. **Synchronisation (Threads)**
- Utilisation de `pthread_create()` pour créer threads
- `pthread_mutex_lock/unlock()` pour protéger les données partagées
- `pthread_join()` pour attendre les threads

### 2. **Gestion des Threads**
- Chaque client connecté = 1 thread serveur
- Les threads partagent la même structure de données (comptes)
- Nécessite synchronisation avec mutexes

### 3. **Communication Sockets**
- `socket()` : création du socket
- `bind()` / `listen()` / `accept()` : serveur
- `connect()` : client
- `send()` / `recv()` : échange de données

---

## 📂 STRUCTURE DES FICHIERS

```
projet_bancaire/
├── server.c          # Serveur principal
├── client.c          # Code client (identique pour client1 & client2)
├── Makefile          # Pour compiler
└── README.md         # Instructions
```

---

## 🛠️ ÉTAPES DE RÉALISATION

### ÉTAPE 1 : Préparation (5 min)
1. Créer un dossier `projet_bancaire/`
2. Créer les 3 fichiers : `server.c`, `client.c`, `Makefile`
3. Copier/coller les codes (voir section suivante)

### ÉTAPE 2 : Compilation (2 min)
```bash
make
# Génère : server, client
```

### ÉTAPE 3 : Lancer le serveur (Terminal 1)
```bash
./server
# Output: "Serveur en écoute sur le port 5000..."
```

### ÉTAPE 4 : Lancer Client 1 (Terminal 2)
```bash
./client
# Vous pouvez faire des opérations
```

### ÉTAPE 5 : Lancer Client 2 (Terminal 3)
```bash
./client
# Se connecte simultanément
```

### ÉTAPE 6 : Tester la Concurrence
- Faire des opérations simultanées sur les 2 clients
- Vérifier que les montants se synchronisent
- Exemple : dépôt de 100€ sur client1 + retrait de 50€ sur client2

---

## 💾 DONNÉES TRANSMISES

### Format Client → Serveur
```
operation code retrait/dépôt montant
Exemples:
CHECK 0            (vérifier solde)
DEPOSIT 1 100      (dépôt de 100€ sur compte 1)
WITHDRAW 0 50      (retrait de 50€ sur compte 0)
EXIT               (quitter)
```

### Format Serveur → Client
```
Solde actuel: 1000€
Opération réussie
Erreur: solde insuffisant
```

---

## ⚙️ POINTS TECHNIQUES

### Mutex (Synchronisation)
```c
pthread_mutex_lock(&mutex);      // Bloquer l'accès
// ... modifier les données ...
pthread_mutex_unlock(&mutex);    // Libérer l'accès
```

### Création de Thread
```c
pthread_t thread_id;
pthread_create(&thread_id, NULL, fonction, (void*)argument);
```

### Sockets
```c
int sockfd = socket(AF_INET, SOCK_STREAM, 0);  // Créer
bind(sockfd, ...);                             // Lier au port
listen(sockfd, 5);                             // Écouter
int client = accept(sockfd, ...);              // Accepter client
```

---

## 🚀 AMÉLIORATIONS POSSIBLES (Bonus)

1. **Persistance** : sauvegarder comptes dans un fichier
2. **Authentification** : PIN de compte
3. **Historique** : garder un journal des transactions
4. **Virements** : transferts entre comptes
5. **Multi-devise** : conversion €/$ en temps réel
6. **Base de données** : utiliser SQLite au lieu de la mémoire

---

## ❓ QUESTIONS FRÉQUENTES

**Q: Est-ce que Kali Linux est nécessaire ?**
A: Non ! Kali est une distribution pour test de sécurité. N'importe quel Linux (Ubuntu, Debian) ou macOS fonctionne parfaitement. Windows avec WSL2 aussi.

**Q: Pourquoi mutex et pas juste des threads ?**
A: Si 2 clients retirent en même temps sans mutex, vous pouvez avoir une "condition de course" (race condition) où le solde devient incohérent.

**Q: Peut-on tester sur une seule machine ?**
A: OUI ! Le serveur et clients peuvent tous tourner sur le même PC (localhost:5000).

**Q: À quoi sert le Makefile ?**
A: À compiler automatiquement avec les bonnes options : `gcc -pthread -o server server.c`

---

## 📊 CRITÈRES D'ÉVALUATION ATTENDUS

✅ **Synchronisation** : Les threads partagent les données sans corruption
✅ **Gestion des threads** : Création/suppression propre avec pthread
✅ **Sockets** : Communication TCP/IP fonctionnelle
✅ **Concept créatif** : Système bancaire complet et logique
✅ **Documentation** : Code commenté et explications claires

---

## 🎓 RESSOURCES UTILES

- Man pages : `man socket`, `man pthread_create`, `man mutex`
- Tutorial : https://tldp.org/LDP/tlk/node24.html
- Debugging : utilisez `gdb` pour déboguer les threads

