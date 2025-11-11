/**
 * @file bitstream.h
 * @brief Manipulation des flux binaires.
 *
 * Fournit des outils pour lire et écrire des bits dans des buffers binaires.
 */
#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stddef.h>
#include <limits.h>

typedef struct {
    unsigned char* ptr;
    size_t capa;
} BitStream;

/**
 * @brief Écrit des bits dans un flux binaire.
 *
 * Cette fonction écrit un certain nombre de bits à partir d'une source
 * dans un flux binaire représenté par une structure `BitStream`.
 *
 * @param curr Le pointeur vers le flux binaire où écrire les bits.
 * @param src La valeur source contenant les bits à écrire.
 * @param nbit Le nombre de bits à écrire (au maximum 8).
 * @return Le nombre de bits effectivement écrits.
 */
size_t pushbits(BitStream* curr, unsigned char src, size_t nbit);

/**
 * @brief Lit des bits depuis un flux binaire.
 *
 * Cette fonction extrait un certain nombre de bits depuis un flux binaire
 * et les stocke dans une destination.
 *
 * @param curr Le pointeur vers le flux binaire d'où lire les bits.
 * @param dest Un pointeur vers la variable où stocker les bits lus.
 * @param nbit Le nombre de bits à lire (au maximum 8).
 * @return Le nombre de bits effectivement lus.
 */
size_t pullbits(BitStream* curr, unsigned char* dest, size_t nbit);

#endif // BITSTREAM_H
