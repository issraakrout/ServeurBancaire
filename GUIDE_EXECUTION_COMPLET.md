# 📖 GUIDE D'EXÉCUTION - SYSTÈME BANCAIRE DISTRIBUÉ

## 🎯 OBJECTIF

Tester un système bancaire avec :
- **1 Serveur** : gère les comptes et les transactions
- **2 Clients** : deux guichets bancaires qui opèrent simultanément
- **Synchronisation** : sans corruption de données même avec accès simultanés

---

## 💻 CONFIGURATION REQUISE

- **Système** : Linux (Ubuntu/Debian) ou macOS
- **Compilateur** : gcc (déjà installé sur la plupart des systèmes)
- **Librairies** : POSIX threads (pthread) - incluse par défaut
- **Kali Linux** : Non nécessaire ! Fonctionne sur n'importe quel Linux
- **Windows** : Utiliser WSL2 (Windows Subsystem for Linux)

**Vérifier votre système :**
```bash
gcc --version
# Devrait afficher: gcc (Debian 10.2.1...) ou similaire
```

---

## 📂 STRUCTURE DES FICHIERS

Après téléchargement, vous devez avoir :

```
projet_bancaire/
├── server.c              # Code du serveur (400 lignes)
├── client.c              # Code du client (150 lignes)
├── Makefile              # Fichier de compilation
└── README.md ou ce guide
```

---

## ⚙️ ÉTAPE 1 : PRÉPARATION (2 min)

### Option 1 : Sur Ubuntu/Debian
```bash
# Ouvrir un terminal
cd ~
mkdir -p projet_bancaire
cd projet_bancaire

# Créer les fichiers (copier/coller le code fourni)
nano server.c     # Coller le code du serveur, Ctrl+X, Y, Enter
nano client.c     # Coller le code du client, Ctrl+X, Y, Enter
nano Makefile     # Coller le Makefile, Ctrl+X, Y, Enter
```

### Option 2 : Sur macOS
```bash
cd ~
mkdir -p projet_bancaire
cd projet_bancaire

# Créer avec un éditeur
vim server.c      # ou gedit, VSCode, etc.
vim client.c
vim Makefile
```

### Option 3 : Avec VSCode (Recommandé ⭐)
```bash
1. Ouvrir VSCode
2. Fichier → Ouvrir le dossier → /home/user/projet_bancaire
3. Créer 3 nouveaux fichiers (Ctrl+N)
4. Coller le contenu
5. Enregistrer (Ctrl+S)
```

---

## 🔨 ÉTAPE 2 : COMPILATION (1 min)

```bash
cd ~/projet_bancaire

# Compiler tous les fichiers
make

# Output attendu:
# gcc -pthread -Wall -std=c99 -o server server.c
# ✓ Serveur compilé avec succès!
# gcc -o client client.c
# ✓ Client compilé avec succès!
```

**Résultat :**
- ✓ `server` (exécutable du serveur)
- ✓ `client` (exécutable du client)

### En cas d'erreur de compilation

**Erreur: "command not found: make"**
```bash
# Installer make
sudo apt-get install build-essential  # Ubuntu/Debian
brew install make                      # macOS
```

**Erreur: "fatal error: pthread.h: No such file"**
```bash
# Installer les headers pthread
sudo apt-get install libpthread-dev  # Ubuntu/Debian
```

---

## 🚀 ÉTAPE 3 : LANCER LE SERVEUR (Terminal 1)

```bash
cd ~/projet_bancaire
./server
```

**Output attendu :**
```
╔════════════════════════════════════════╗
║   🏦 SERVEUR BANCAIRE - EN DÉMARRAGE   ║
╚════════════════════════════════════════╝

✓ Socket serveur créé
✓ Socket lié au port 5000
✓ Serveur en écoute...

💡 Attendant les connexions sur 127.0.0.1:5000

=== ÉTAT DES COMPTES ===
Compte 0 (Alice): 1000.00€
Compte 1 (Bob): 1500.00€
Compte 2 (Charlie): 2000.00€
========================

```

**Le serveur attend maintenant les clients...**

---

## 💳 ÉTAPE 4 : LANCER LE CLIENT 1 (Terminal 2)

```bash
cd ~/projet_bancaire
./client
```

**Output :**
```
╔════════════════════════════════════════╗
║    🏦 CLIENT BANCAIRE - CONNEXION      ║
╚════════════════════════════════════════╝

✓ Socket créé
✓ Connecté au serveur sur 127.0.0.1:5000

╔════════════════════════════════════════╗
║      💳 MENU DES OPÉRATIONS          ║
║ ...
╚════════════════════════════════════════╝

>>> Entrez une commande:
```

**Taper une commande :**
```bash
>>> Entrez une commande: CHECK 0

📤 Envoyé au serveur: CHECK 0
📥 Réponse du serveur:
   ✓ Solde du compte 0 (Alice): 1000.00€
```

---

## 💳 ÉTAPE 5 : LANCER LE CLIENT 2 (Terminal 3)

```bash
# Ouvrir un 3ème terminal
cd ~/projet_bancaire
./client
```

Maintenant vous avez :
- **Terminal 1** : Serveur en écoute
- **Terminal 2** : Client 1 prêt pour les opérations
- **Terminal 3** : Client 2 prêt pour les opérations

---

## 🧪 ÉTAPE 6 : TESTER LA CONCURRENCE

### Scénario de Test 1 : Dépôts Simultanés

**Terminal 2 (Client 1) :**
```bash
>>> Entrez une commande: DEPOSIT 0 500
📤 Envoyé au serveur: DEPOSIT 0 500
📥 Réponse du serveur:
   ✓ Dépôt de 500.00€ réussi. Nouveau solde: 1500.00€
```

**Terminal 3 (Client 2) - Simultanément :**
```bash
>>> Entrez une commande: CHECK 0
📤 Envoyé au serveur: CHECK 0
📥 Réponse du serveur:
   ✓ Solde du compte 0 (Alice): 1500.00€
```

### Scénario de Test 2 : Retrait Insuffisant

**Terminal 2 :**
```bash
>>> Entrez une commande: WITHDRAW 0 2000
📤 Envoyé au serveur: WITHDRAW 0 2000
📥 Réponse du serveur:
   ✓ ERREUR: Solde insuffisant. Solde actuel: 1500.00€
```

### Scénario de Test 3 : Transfert Simulé

**Terminal 2 (Retrait de Bob) :**
```bash
>>> Entrez une commande: WITHDRAW 1 300
📤 Envoyé au serveur: WITHDRAW 1 300
📥 Réponse du serveur:
   ✓ Retrait de 300.00€ réussi. Nouveau solde: 1200.00€
```

**Terminal 3 (Dépôt d'Alice) - Immédiatement :**
```bash
>>> Entrez une commande: DEPOSIT 0 300
📤 Envoyé au serveur: DEPOSIT 0 300
📥 Réponse du serveur:
   ✓ Dépôt de 300.00€ réussi. Nouveau solde: 1800.00€
```

**Résultat :** Argent transféré de Bob à Alice ! ✓

---

## 📊 OBSERVATION SUR LE SERVEUR

**Dans Terminal 1, vous verrez :**

```
[THREAD #1] Reçu: CHECK 0
[THREAD #1] Envoyé: Solde du compte 0 (Alice): 1500.00€

[THREAD #2] Reçu: DEPOSIT 0 300
[THREAD #2] Envoyé: Dépôt de 300.00€ réussi. Nouveau solde: 1800.00€

=== ÉTAT DES COMPTES ===
Compte 0 (Alice): 1800.00€
Compte 1 (Bob): 1200.00€
Compte 2 (Charlie): 2000.00€
========================
```

**Points clés :**
- 🔒 Les mutex protègent les données
- 🧵 Chaque client = 1 thread
- 📡 Communication TCP en temps réel
- ✓ Pas de corruption de données même avec accès simultanés

---

## 🛑 ÉTAPE 7 : ARRÊTER

### Quitter les clients
```bash
>>> Entrez une commande: EXIT
📤 Envoyé au serveur: EXIT
📥 Réponse du serveur:
   ✓ Au revoir!
Au revoir! 👋
```

### Arrêter le serveur
```bash
# Dans Terminal 1
Ctrl+C

# Le serveur s'arrête proprement
```

---

## 🐛 DÉPANNAGE

### Problème: "Connection refused"
```
Erreur: impossible de se connecter au serveur
```
**Solution :** Vérifier que le serveur tourne dans Terminal 1
```bash
# Terminal 1 doit afficher: "Attendant les connexions..."
```

### Problème: "Address already in use"
```
bind: Address already in use
```
**Solution :** Le port 5000 est occupé, attendre 30 secondes ou changer le port
```c
// Dans server.c, ligne 200:
int port = 5001;  // Changer 5000 en 5001
```

### Problème: "No such file or directory"
```
./server: No such file or directory
```
**Solution :** Recompiler
```bash
make clean
make
./server
```

### Problème: Compilation échoue
```bash
gcc: error: unrecognized command line option '-pthread'
```
**Solution :** gcc doit être à jour
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential

# macOS
brew install gcc
```

---

## 📝 COMMANDES DISPONIBLES

| Commande | Exemple | Résultat |
|----------|---------|----------|
| CHECK | `CHECK 0` | Affiche le solde du compte 0 |
| DEPOSIT | `DEPOSIT 1 100` | Ajoute 100€ au compte 1 |
| WITHDRAW | `WITHDRAW 2 50` | Retire 50€ du compte 2 |
| EXIT | `EXIT` | Quitter l'application |

---

## 🎓 CONCEPTS EXPLIQUÉS

### 1️⃣ Threads (`pthread_create`)
```c
pthread_t thread_id;
pthread_create(&thread_id, NULL, handle_client, (void*)data);
```
→ Chaque client connecté crée un nouveau thread serveur

### 2️⃣ Synchronisation (Mutex)
```c
pthread_mutex_lock(&account_mutex);
// Accès exclusif aux données
accounts[id].balance -= amount;
pthread_mutex_unlock(&account_mutex);
```
→ Empêche 2 threads d'accéder aux données simultanément

### 3️⃣ Sockets TCP
```c
socket() → Créer le socket
bind() → Attacher un port
listen() → Écouter les connexions
accept() → Accepter un client
send/recv() → Communiquer
```
→ Communication fiable entre serveur et clients

---

## 📈 RÉSULTATS ATTENDUS

Après les tests, vous devez observer :

✅ Le serveur accepte plusieurs clients
✅ Les clients peuvent opérer simultanément
✅ Les soldes sont correctement synchronisés
✅ Pas d'erreur de corruption de données
✅ Les messages sont affichés dans les bons ordre
✅ EXIT termine correctement la connexion

---

## 🎯 RAPPORT POUR LE PROFESSEUR

**À inclure dans votre présentation :**

1. **Synchronisation** : 
   - Utilisation de mutex pour protéger l'accès aux comptes
   - Démonstration : dépôts simultanés ne corrompent pas le solde

2. **Gestion des Threads** :
   - Chaque client = 1 thread (`pthread_create`)
   - Gestion propre avec `pthread_detach`
   - Démonstration : 2 clients simultanés

3. **Communication par Sockets** :
   - Serveur TCP sur port 5000
   - Échange de messages textes
   - Démonstration : échange de commandes/réponses

4. **Concept Créatif** :
   - Système bancaire réaliste (comptes, soldes, transactions)
   - Opérations bancaires classiques (check, dépôt, retrait)
   - Gestion d'erreurs (solde insuffisant)

5. **Démonstration en Direct** :
   - Montrer 1 serveur + 2 clients opérant simultanément
   - Faire des transactions concurrentes
   - Vérifier les soldes restent cohérents

