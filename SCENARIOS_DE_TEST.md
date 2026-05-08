# 🧪 SCÉNARIOS DE TEST - SYSTÈME BANCAIRE DISTRIBUÉ

## 📋 Vue d'ensemble

Ce document contient des **scénarios de test complets** pour démontrer :
- ✅ **Synchronisation** : mutex protègent les données
- ✅ **Threads** : plusieurs clients simultanés
- ✅ **Sockets** : communication TCP/IP
- ✅ **Concurrence** : sans corruption de données

---

## 🎬 SCÉNARIO 1 : CONSULTATIONS SIMULTANÉES

**Objectif** : Montrer que plusieurs clients peuvent consulter sans conflit

### Exécution

**Terminal 1** (Serveur)
```bash
./server
# Affiche les 3 comptes initiaux
```

**Terminal 2** (Client 1)
```bash
./client
>>> Entrez une commande: CHECK 0
📥 Réponse: Solde du compte 0 (Alice): 1000.00€

>>> Entrez une commande: CHECK 1
📥 Réponse: Solde du compte 1 (Bob): 1500.00€

>>> Entrez une commande: CHECK 2
📥 Réponse: Solde du compte 2 (Charlie): 2000.00€
```

**Terminal 3** (Client 2) - En même temps !
```bash
./client
>>> Entrez une commande: CHECK 0
📥 Réponse: Solde du compte 0 (Alice): 1000.00€

>>> Entrez une commande: CHECK 1
📥 Réponse: Solde du compte 1 (Bob): 1500.00€
```

### Résultat Attendu
✅ Les deux clients reçoivent les mêmes soldes
✅ Le serveur gère les deux demandes sans conflit
✅ Dans le terminal serveur: deux threads (#1 et #2) travaillent en parallèle

---

## 🎬 SCÉNARIO 2 : DÉPÔT SIMULTANÉ (Synchronisation !)

**Objectif** : Montrer que les dépôts simultanés sont protégés par mutex

### Exécution

**Terminal 1** (Serveur)
```
État initial:
Compte 0 (Alice): 1000.00€
```

**Terminal 2** (Client 1) - Dépôt 1
```bash
>>> Entrez une commande: DEPOSIT 0 500
📥 Réponse: Dépôt de 500.00€ réussi. Nouveau solde: 1500.00€
```

**Terminal 3** (Client 2) - Dépôt 2 (au MÊME MOMENT)
```bash
>>> Entrez une commande: DEPOSIT 0 300
📥 Réponse: Dépôt de 300.00€ réussi. Nouveau solde: 1800.00€
```

### Résultat Attendu
✅ Solde final = 1000 + 500 + 300 = 1800€
✅ SANS MUTEX : résultat aléatoire (1300€, 1500€, etc.)
✅ AVEC MUTEX : résultat TOUJOURS correct (1800€)

**Dans Terminal 1 :**
```
[THREAD #1] Reçu: DEPOSIT 0 500
[THREAD #1] Envoyé: Dépôt de 500.00€ réussi. Nouveau solde: 1500.00€
[Serveur] Dépôt: Compte 0 +500.00€ (nouveau solde: 1500.00€)

[THREAD #2] Reçu: DEPOSIT 0 300
[THREAD #2] Envoyé: Dépôt de 300.00€ réussi. Nouveau solde: 1800.00€
[Serveur] Dépôt: Compte 0 +300.00€ (nouveau solde: 1800.00€)

=== ÉTAT DES COMPTES ===
Compte 0 (Alice): 1800.00€  ← CORRECT !
```

---

## 🎬 SCÉNARIO 3 : RETRAIT AVEC SOLDE INSUFFISANT

**Objectif** : Montrer la gestion d'erreur

### Exécution

**Terminal 1** (Serveur)
```
Compte 1 (Bob): 1500.00€
```

**Terminal 2** (Client 1)
```bash
>>> Entrez une commande: WITHDRAW 1 2000
📥 Réponse: ERREUR: Solde insuffisant. Solde actuel: 1500.00€
```

### Résultat Attendu
✅ Le retrait est refusé
✅ Le solde reste inchangé à 1500€
✅ Un message d'erreur clair est envoyé

---

## 🎬 SCÉNARIO 4 : TRANSFERT SIMULÉ (Retrait + Dépôt)

**Objectif** : Simuler un virement de Bob vers Alice

### Exécution

**État initial :**
```
Compte 0 (Alice): 1000.00€
Compte 1 (Bob): 1500.00€
```

**Terminal 2** (Client 1 - Retrait de Bob)
```bash
>>> Entrez une commande: WITHDRAW 1 300
📥 Réponse: Retrait de 300.00€ réussi. Nouveau solde: 1200.00€
```

**Terminal 3** (Client 2 - Dépôt d'Alice) - Immédiatement après
```bash
>>> Entrez une commande: DEPOSIT 0 300
📥 Réponse: Dépôt de 300.00€ réussi. Nouveau solde: 1300.00€
```

### Résultat Attendu
```
Compte 0 (Alice): 1300.00€  ← +300€ reçus
Compte 1 (Bob): 1200.00€    ← -300€ envoyés
```

✅ Transfert réussi sans corruption de données

---

## 🎬 SCÉNARIO 5 : PLUSIEURS TRANSACTIONS RAPIDES

**Objectif** : Tester la capacité à gérer plusieurs opérations rapides

### Exécution

**Terminal 2** (Client 1)
```bash
>>> CHECK 0
📥 Solde du compte 0 (Alice): 1300.00€

>>> DEPOSIT 0 100
📥 Dépôt de 100.00€ réussi. Nouveau solde: 1400.00€

>>> WITHDRAW 0 50
📥 Retrait de 50.00€ réussi. Nouveau solde: 1350.00€

>>> CHECK 0
📥 Solde du compte 0 (Alice): 1350.00€
```

**Terminal 3** (Client 2) - Pendant ce temps
```bash
>>> CHECK 1
📥 Solde du compte 1 (Bob): 1200.00€

>>> WITHDRAW 1 100
📥 Retrait de 100.00€ réussi. Nouveau solde: 1100.00€

>>> CHECK 1
📥 Solde du compte 1 (Bob): 1100.00€
```

### Résultat Attendu
✅ Tous les calculs sont corrects
✅ Aucune corruption de données
✅ Les threads coexistent sans problème

---

## 🎬 SCÉNARIO 6 : DÉCONNEXION PROPRE

**Objectif** : Montrer l'arrêt gracieux

### Exécution

**Terminal 2** (Client 1)
```bash
>>> EXIT
📤 Envoyé au serveur: EXIT
📥 Réponse du serveur: Au revoir!
Au revoir! 👋
```

**Terminal 1** (Serveur)
```
[THREAD #1] Client déconnecté
```

✅ La déconnexion se fait proprement
✅ Le thread est libéré correctement
✅ Le serveur continue à écouter les autres clients

---

## 🔬 SCÉNARIO 7 : TEST DE STRESS (Avancé)

**Objectif** : Montrer la stabilité sous charge

**Terminal 2** (Client 1)
```bash
>>> DEPOSIT 0 100
>>> DEPOSIT 0 100
>>> DEPOSIT 0 100
>>> DEPOSIT 0 100
>>> DEPOSIT 0 100
>>> CHECK 0
```

**Terminal 3** (Client 2) - Pendant ce temps
```bash
>>> WITHDRAW 0 50
>>> WITHDRAW 0 50
>>> WITHDRAW 0 50
>>> WITHDRAW 0 50
>>> CHECK 0
```

### Résultat Attendu
```
Compte 0 Final: 1000 + (5×100) - (4×50) = 1400€
```

✅ Même avec beaucoup d'opérations simultanées
✅ Le résultat final est TOUJOURS correct
✅ Cela démontre l'efficacité de la synchronisation

---

## 📊 RÉSUMÉ DES TESTS

| Scénario | Concept | Résultat Attendu |
|----------|---------|-----------------|
| 1 | Threads (Lectures) | Aucun conflit |
| 2 | Synchronisation (Écritures) | Mutex protègent les données |
| 3 | Gestion d'erreur | Refus et message d'erreur |
| 4 | Sockets (Communication) | Échange réussi |
| 5 | Concurrence | Pas de race condition |
| 6 | Déconnexion | Propre et sûre |
| 7 | Stress | Stabilité complète |

---

## 🎯 POINTS À METTRE EN ÉVIDENCE

### Pour le Professeur

1. **Synchronisation**
   ```
   "Sans mutex: résultats imprévisibles
    Avec mutex: résultats TOUJOURS corrects
    Démo: Scénario 2 (dépôts simultanés)"
   ```

2. **Threads**
   ```
   "Chaque client = 1 thread serveur
    Gestion transparente des connexions multiples
    Démo: Scénario 4 (transfert simultané)"
   ```

3. **Sockets**
   ```
   "Communication TCP/IP fiable
    Échange de commandes et réponses
    Démo: Scénario 5 (plusieurs transactions)"
   ```

4. **Concept Créatif**
   ```
   "Système bancaire réaliste
    Opérations réelles: dépôt, retrait, consultation
    Gestion d'erreurs intégrée
    Démo: Scénario 3 (retrait insuffisant)"
   ```

---

## 💡 CONSEILS POUR LA DÉMONSTRATION

1. **Préparez 3 terminaux** à l'avance
2. **Testez d'abord seul** avant la présentation
3. **Utilisez des montants mémorables** (100€, 300€, etc.)
4. **Montrez le code** en particulier les mutexes
5. **Expliquez chaque étape** au professeur
6. **Faites des opérations LENTES** (pause entre 2) pour bien voir les threads

---

## 🚀 EXÉCUTION RAPIDE D'UN SCÉNARIO COMPLET

```bash
# Terminal 1
./server

# Terminal 2
./client
>>> CHECK 0
>>> DEPOSIT 0 100
>>> WITHDRAW 0 50
>>> EXIT

# Terminal 3
./client
>>> CHECK 0
>>> DEPOSIT 0 100
>>> CHECK 0
>>> EXIT
```

**Résultat final attendu :**
```
Compte 0: 1000 + 100 + 100 - 50 = 1150€ ✅
```

