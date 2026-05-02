# Surveillance de réseau public basée sur ESP32-CAM

> **选择你的语言/Choose your language/言語を選択してください/Choisissez votre langue**:
> - [中文](README.md)
> - [English](README-EN.md)
> - [日本語](README-JP.md)
> - Français(maintenant)

> **Remarque** : Ce document est traduit automatiquement. Pour la version originale en chinois, consultez [README.md](README.md).

Surveillance de réseau public basée sur ESP32-CAM, adaptée aux véhicules télécommandés à longue distance.

---

## Introduction

Ce projet est basé sur le CameraWebServer d'Espressif Systems et met en œuvre une surveillance de réseau public via le transfert de serveur cloud. Il peut être utilisé pour des véhicules télécommandés, des machines agricoles et d'autres équipements. En utilisant le compact ESP32-CAM et le streaming via le protocole UDP, il atteint une légèreté et une faible latence. Les broches inutilisées de l'ESP32 permettent un développement ultérieur de fonctionnalités supplémentaires. En raison de mes compétences limitées, ce projet a été développé avec l'aide de l'IA.

> **Remarque** : Ce projet utilise un protocole de transmission non chiffré et n'est pas recommandé pour des scénarios nécessitant de la confidentialité ou impliquant la vie privée. Pour une surveillance domestique, vous pouvez utiliser directement le CameraWebServer d'Espressif Systems pour un déploiement dans un réseau local.

**Veuillez respecter les lois locales et interdire strictement toute utilisation illégale.**

---

## Licence

- Le projet dans son ensemble adopte la [Licence MIT](LICENSE).
- Le fichier [app_httpd.cpp](app_httpd.cpp) provient d'Espressif Systems et adopte la [Licence Apache 2.0](LICENSE-APACHE-2.0).

---

## Instructions de déploiement

### Modification et téléchargement du code ESP32-CAM

1. **Cloner le projet**
   - Renommez le dossier en "Public-network-monitoring-based-on-ESP32-CAM".
   - Ouvrez le fichier `Public-network-monitoring-based-on-ESP32-CAM.ino` avec Arduino.

2. **Modifier le code selon votre situation et les commentaires**
   - Décommentez le modèle de votre carte de développement dans le fichier [board_config.h](board_config.h). Assurez-vous que les paramètres sont corrects pour éviter d'endommager la carte.
   - La ligne 97 définit la résolution, par défaut QVGA (320×240).
   - La ligne 151 définit le taux de rafraîchissement. Calcul : `1000 / valeur = FPS`. Par exemple, si réglé sur 33 (par défaut), cela correspond à environ 30 FPS.
     - En utilisant 320x240 à 30fps, la consommation de bande passante du serveur est d'environ 1,1mbps en descendant et 1,8mbps en montant.

3. **Compiler et téléverser**

---

### Configuration du serveur

1. **Téléverser le fichier `server.py` sur le serveur**

2. **Installer aiohttp et démarrer le serveur**
   - Installer avec pip :
     ```bash
     pip install aiohttp
     python server.py
     ```
   - **Il est recommandé d'utiliser screen pour s'assurer que le serveur continue de fonctionner après déconnexion**
     - Terminez le processus avec `Ctrl+C`.
     - Créez une session screen :
       ```bash
       screen
       ```
     - Démarrez le serveur :
       ```bash
       python server.py
       ```
     - Détachez la session screen :
       Utilisez le raccourci `Ctrl+A+D` (maintenez Ctrl, appuyez sur A, puis sur D).

   - **Si l'installation échoue avec le message suivant** :
     ```
     × This environment is externally managed
     ╰─> To install Python packages system-wide, try apt install python3-xyz...
     ```
     - Assurez-vous que le module venv est installé :
       ```bash
       sudo apt update
       sudo apt install python3-full python3-venv -y
       ```
     - Créez un environnement virtuel :
       ```bash
       python3 -m venv ~/my_aiohttp_env
       ```
     - Activez l'environnement virtuel :
       ```bash
       source ~/my_aiohttp_env/bin/activate
       ```
     - Installez aiohttp :
       ```bash
       pip install aiohttp
       ```
     - Obtenez le chemin de l'environnement virtuel :
       ```bash
       which python3
       ```
     - Créez un fichier `run.sh` :
       ```bash
       nano run.sh
       ```
       Entrez le contenu suivant :
       ```bash
       #!/bin/bash
       PYTHON_PATH="/chemin/vers/votre/environnement/virtuel/bin/python3"
       YOUR_SCRIPT="nom_du_script.py"
       cd "$(dirname "$0")"
       exec "$PYTHON_PATH" "$YOUR_SCRIPT"
       ```
     - Désactivez l'environnement virtuel :
       ```bash
       deactivate
       ```
     - Démarrez le serveur :
       ```bash
       sh run.sh
       ```

3. **Ouvrir les ports 8888 et 8081**

---

## Connexion

1. **Tester la connexion**
   - Entrez l'adresse suivante dans votre navigateur :
     ```
     http://[IP-de-votre-serveur]:8081/stream/[votre-camId]
     ```
   - Vous devriez voir la vidéo capturée par l'ESP32-CAM.

2. **Connecter avec des logiciels comme motionEye**
   - Les méthodes spécifiques sont omises ici.