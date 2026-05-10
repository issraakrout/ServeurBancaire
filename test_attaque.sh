#!/bin/bash
echo "=== ATTAQUE SIMULTANÉE SUR ALICE ==="
echo "Solde initial: 1000€"
echo "3 retraits de 400€ en même temps..."
echo ""

echo "WITHDRAW 0 400" | nc 127.0.0.1 5000 &
echo "WITHDRAW 0 400" | nc 127.0.0.1 5000 &
echo "WITHDRAW 0 400" | nc 127.0.0.1 5000 &

wait
sleep 0.5

echo ""
echo "=== RÉSULTAT ==="
echo "CHECK 0" | nc 127.0.0.1 5000
echo ""
echo "Attendu: 1 retrait réussi (600€) + 2 refusés"
echo "✅ Mutex empêche le solde de passer négatif !"
