#!/bin/bash
echo "=== TEST DÉPÔTS SIMULTANÉS ==="
echo "Solde initial Alice: 1000€"
echo "Envoi simultané: +500€ et +300€"
echo ""

# Envoie les 2 commandes EN MÊME TEMPS avec &
echo "DEPOSIT 0 500" | nc 127.0.0.1 5000 &
echo "DEPOSIT 0 300" | nc 127.0.0.1 5000 &

# Attend que les 2 terminent
wait

sleep 0.5

# Vérifie le résultat final
echo ""
echo "=== VÉRIFICATION RÉSULTAT ==="
echo "CHECK 0" | nc 127.0.0.1 5000
echo ""
echo "Résultat attendu: 1800€ (1000+500+300)"
echo "✅ Si 1800€ → mutex fonctionne parfaitement !"
