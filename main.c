#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/task1.h"
#include "src/task1.c"
#include "src/task2.c"

int main() {
    secretariat *db = citeste_secretariat("/home/mihai/tema3pclp/tema3_v9/schelet/tests/db/big.db");
    if (!db) {
        fprintf(stderr, "Eroare: Citirea bazei de date a eșuat.\n");
        return 1;
    }
    int numar_comenzi = 0;
    char linie[MAX_CHAR];
    if (fgets(linie, sizeof(linie), stdin) == NULL) {
        fprintf(stderr, "Eroare!\n");
        return 1;
    }
    if (sscanf(linie, "%d", &numar_comenzi) != 1) {
        fprintf(stderr, "Eroare!\n");
        return 1;
    }
    char comanda[MAX_CHAR];
    for (int i = 0; i < numar_comenzi; i++) {
        if (fgets(comanda, sizeof(comanda), stdin) == NULL) {
            fprintf(stderr, "Eroare la citirea comenzii %d.\n", i + 1);
            break;
        }
        comanda[strcspn(comanda, "\n")] = 0;
        if (strstr(comanda, "UPDATE") != NULL) {
            proceseaza_update(db, comanda);
        } else if (strstr(comanda, "DELETE") != NULL) {
            proceseaza_delete(db, comanda);
        } else if (strstr(comanda, "AND") != NULL || strstr(comanda, "WHERE") != NULL ||
                   (strstr(comanda, "SELECT") != NULL && strstr(comanda, "FROM") != NULL)) {
            proceseaza_interogare(db, comanda);
        } else {
            printf("Comanda %d necunoscuta: %s\n", i + 1, comanda);
        }
    }
    elibereaza_secretariat(&db);
    free(db);
    return 0;
}
/*
2
UPDATE studenti SET medie_generala = 9.0 WHERE id = 0 AND an_studiu = 1;
SELECT nume, medie_generala FROM studenti WHERE id = 0;
*/