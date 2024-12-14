# syntax=docker/dockerfile:1
FROM ghcr.io/inti-cmnb/kicad8_auto_full:latest

RUN apt update \
    && apt install -y unzip wget \
    && rm -rf /var/lib/apt/lists/*

# Add marbastlib 3D models
ARG models_dir8=/root/.local/share/kicad/8.0/3rdparty/3dmodels

RUN wget https://github.com/ebastler/marbastlib/releases/download/2024.08.18/KiCAD-PCM-2024.08.18.zip -O ~/marbastlib.zip \
    && unzip ~/marbastlib.zip -d ~/marbastlib \
    && rm ~/marbastlib.zip \
    && mkdir -p ${models_dir8}/com_github_ebastler_marbastlib \
    && cp ~/marbastlib/3dmodels/* ${models_dir8}/com_github_ebastler_marbastlib \
    && rm -rf ~/marbastlib
