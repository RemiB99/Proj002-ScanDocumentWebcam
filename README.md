# Proj002 : ScanDocumentWebcam

Rémi Bouvier - M2 CMI Info - Université Savoie Mont Blanc

## Description

Ce programme a pour but de pouvoir "scanner" un document en le plaçant devant la Webcam.
L'utilisateur place le document qu'il souhaite scanner devant sa webcam, et obtient en sortie le document présenté (sans les autres éléments parasites capturés par la webcam).

## Installation

Pour fonctionner, ce programme nécessite d'avoir une version d'OpenCV fonctionnelle installée sur son système.
Voir  [Installation OpenCV](https://docs.opencv.org/4.x/df/d65/tutorial_table_of_content_introduction.html) pour plus de détails.

Afin d'utiliser OpenCV, il est également nécessaire d'avoir un compilateur C++ fonctionnel ainsi que l'outil de configuration de construction CMake
Sous Linux :
```bash
sudo apt install -y g++
```
et
```bash
sudo apt install -y cmake
```

## Utilisation

Pour compiler et lancer le programme, dans le répertoire courant :
```bash
cmake -DCMAKE_BUILD_TYPE=Release
```
```bash
make
```
et 
```bash
./scan <methode-extraction-contours>
```
où méthode-extraction-contours est la méthode d'extraction de contours choisie parmi "Canny" ou "Watershed".

Une fois le programme lancé, 2 fenêtres apparaissent :
- Une fenêtre nommée "scan"
- Une fenêtre nomée "scanned image"

La fenêtre "scan" donne une visualisation du flux vidéo capturé par la caméra.
Le rectangle rouge représente la zone où l'utilisateur doit placer son document pour le scanner.
Les traits de différentes couleurs qui apparaissent quand l'utilisateur place son document dans le rectangle représentent les contours détectés par l'algorithme de détection de contours.

La fenêtre "scanned image" contient un carré noir au lancement du programme et affiche ensuite l'image scannée une fois que l'utilisateur a placé son document dans le carré rouge.

## Améliorations possibles

- Lissage temporel pour améliorer la captation de l'image
- Application de filtres supplémentaires sur l'image de sortie pour améliorer sa qualité
- Image fixe en sortie, qui peut ensuite être enregistrée dans les dossiers de l'utilisateur
- Choix de différents formats d'images (A3, A4, A5)
...
