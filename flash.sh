#!/bin/bash
# Script de flash automatique du firmware sur le Pi Pico

set -e  # Arrêter en cas d'erreur

echo "🔍 Recherche du Pi Pico en mode bootloader..."

# Détection automatique du point de montage
PICO_MOUNT=$(ls -d /media/$USER/RPI-RP2* 2>/dev/null | head -n1)

if [ -z "$PICO_MOUNT" ]; then
    echo "❌ Erreur: Pi Pico non détecté en mode bootloader"
    echo ""
    echo "📌 Pour mettre le Pico en mode bootloader :"
    echo "   - Méthode 1: Maintenir BOOT + appuyer sur RESET + relâcher BOOT"
    echo "   - Méthode 2: Double-tap rapide sur RESET"
    exit 1
fi

echo "✅ Pi Pico détecté : $PICO_MOUNT"

if [ ! -f "firmware.uf2" ]; then
    echo "❌ Erreur: firmware.uf2 introuvable"
    echo "   Compilez d'abord avec : ./build.sh"
    exit 1
fi

echo "📤 Copie du firmware..."
# Utilise cat > pour éviter les problèmes de permissions avec cp
cat firmware.uf2 > "$PICO_MOUNT/firmware.uf2"
sync
echo "✓ Copie réussie"

echo "✅ Firmware flashé avec succès !"
echo "🔄 Le clavier va redémarrer automatiquement..."
echo "⏳ Attendre 5 secondes..."
sleep 5
echo "✅ Flash terminé !"