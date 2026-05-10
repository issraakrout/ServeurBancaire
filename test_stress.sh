#!/bin/bash
echo "=== TEST DE STRESS : 100 OPÉRATIONS ==="
echo "Dépôts et retraits automatiques sur tous les comptes"
echo ""

for i in $(seq 1 20); do
    echo "DEPOSIT 0 10"  | nc 127.0.0.1 5000 &
    echo "DEPOSIT 1 10"  | nc 127.0.0.1 5000 &
    echo "DEPOSIT 2 10"  | nc 127.0.0.1 5000 &
    echo "WITHDRAW 0 10" | nc 127.0.0.1 5000 &
    echo "WITHDRAW 1 10" | nc 127.0.0.1 5000 &
done

wait
sleep 1

echo ""
echo "=== SOLDES FINAUX ==="
echo "CHECK 0" | nc 127.0.0.1 5000
echo "CHECK 1" | nc 127.0.0.1 5000
echo "CHECK 2" | nc 127.0.0.1 5000
echo ""
echo "✅ Si soldes corrects → zéro corruption après 100 opérations !"
