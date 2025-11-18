#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STUDENT_NAME 40
#define NUMBER_OF_GRADES 3

#include "task1.h"

#define MAX_CHAR 256
#define Round 5
#define Power 10

secretariat *citeste_secretariat(const char *nume_fisier) {
    // TODO(student): 1.1
    FILE *file = fopen(nume_fisier, "r");
    if (file == NULL) {
        printf("Fisierul nu a putut fi deschis\n");
        exit(1);
    }
    secretariat *database = malloc(sizeof(secretariat));
    database->studenti = NULL;
    database->materii = NULL;
    database->inrolari = NULL;
    database->nr_studenti = 0;
    database->nr_materii = 0;
    database->nr_inrolari = 0;

    if (database == NULL) {
        printf("Eroare la alocarea memoriei\n");
        exit(1);
    }
    int nr_studenti = 0, nr_materii = 0, nr_inrolari = 0;
    float medie = 0.0f;
    char line[MAX_CHAR] = {0}, b[MAX_CHAR] = {0}, d[MAX_CHAR] = {0}, section[MAX_CHAR] = {0};
    snprintf(section, sizeof(section), " ");
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        if (line[0] == '[') {
            snprintf(section, sizeof(section), "%s", line);
        } else {
            if (strcmp(section, "[STUDENTI]") == 0) {
                nr_studenti++;
            } else if (strcmp(section, "[MATERII]") == 0) {
                nr_materii++;
            } else if (strcmp(section, "[INROLARI]") == 0) {
                nr_inrolari++;
            }
        }
    }
    database->nr_studenti = nr_studenti;
    database->nr_materii = nr_materii;
    database->nr_inrolari = nr_inrolari;
    database->studenti = malloc(nr_studenti * sizeof(student));
    for (int i = 0; i < nr_studenti; i++) {
        database->studenti[i].medie_generala = 0.0f;
    }
    database->materii = malloc(nr_materii * sizeof(materie));
    database->inrolari = malloc(nr_inrolari * sizeof(inrolare));
    rewind(file);
    int cnt_student = 0, cnt_materii = 0, cnt_inrolari = 0;
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        if (line[0] == '[') {
            snprintf(section, sizeof(section), "%s", line);
        } else {
            if (strcmp(section, "[STUDENTI]") == 0) {
                sscanf(line, "%d, %[^,], %d, %c", &database->studenti[cnt_student].id,
                database->studenti[cnt_student].nume, &database->studenti[cnt_student].an_studiu,
                &database->studenti[cnt_student].statut);
                cnt_student++;
            } else if (strcmp(section, "[MATERII]") == 0) {
                sscanf(line, "%d, %[^,], %[^\n]", &database->materii[cnt_materii].id, b, d);
                database->materii[cnt_materii].nume = strdup(b);
                database->materii[cnt_materii].nume_titular = strdup(d);
                cnt_materii++;
            } else if (strcmp(section, "[INROLARI]") == 0) {
                sscanf(line, "%d, %d, %f %f %f", &database->inrolari[cnt_inrolari].id_student,
                &database->inrolari[cnt_inrolari].id_materie, &database->inrolari[cnt_inrolari].note[0],
                &database->inrolari[cnt_inrolari].note[1], &database->inrolari[cnt_inrolari].note[2]);
                cnt_inrolari++;
            }
        }
    }
    int *numar_materii = calloc(nr_studenti, sizeof(int));
    for (int i = 0; i < nr_inrolari; i++) {
        int id_student = database->inrolari[i].id_student;
        float suma_note_materie = 0.0f;
        for (int j = 0; j < NUMBER_OF_GRADES; j++) {
            suma_note_materie += database->inrolari[i].note[j] * Power * Power;
        }
        database->studenti[id_student].medie_generala += suma_note_materie;
        numar_materii[id_student]++;
    }
    for (int i = 0; i < nr_studenti; i++) {
        if (numar_materii[i] > 0) {
            medie = (float)(database->studenti[i].medie_generala *Power) / (float)(numar_materii[i]);
            if (medie < 0.0f) {
                medie = 0.0f;
            }
            int zecimala_importanta = (int) (medie) % Power;
            medie = medie - (float)(zecimala_importanta);
            medie /= Power;
            if (zecimala_importanta >= Round) {
                database->studenti[i].medie_generala = (medie + 1) / Power / Power;
            } else {
            database->studenti[i].medie_generala = (float)(medie) / Power / Power;
            }
        }
    }
    free(numar_materii);
    fclose(file);
    return database;
}

void adauga_student(secretariat *s, int id, char *nume, int an_studiu, char statut, float medie_generala) {
    // TODO(student): 1.2
    s->nr_studenti++;
    s->studenti = realloc(s->studenti, s->nr_studenti * sizeof(student));
    s->studenti[s->nr_studenti - 1].id = id;
    snprintf(s->studenti[s->nr_studenti - 1].nume, sizeof(s->studenti[s->nr_studenti - 1].nume), "%s", nume);
    s->studenti[s->nr_studenti - 1].an_studiu = an_studiu;
    s->studenti[s->nr_studenti - 1].statut = statut;
    s->studenti[s->nr_studenti - 1].medie_generala = medie_generala;
}


void elibereaza_secretariat(secretariat **s) {
    if (*s == NULL) return;
    for (int i = 0; i < (*s)->nr_materii; i++) {
        free((*s)->materii[i].nume);
        free((*s)->materii[i].nume_titular);
    }
    free((*s)->materii);
    free((*s)->inrolari);
    free((*s)->studenti);
    free(*s);
    *s = NULL;
}
