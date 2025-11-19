#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "task2.h"

#define MAX_STUDENT_NAME 40
#define MAX_CHAR 256
#define scan_var 6
#define Round 5
#define Power 10

int compara_operator(float valoare_camp, const char *operator, char *valoare_comparata) {
    float valoare_cmp = (float)atof(valoare_comparata);
    if (strcmp(operator, "=") == 0) return (valoare_camp == valoare_cmp);
    if (strcmp(operator, "<") == 0) return (valoare_camp < valoare_cmp);
    if (strcmp(operator, ">") == 0) return (valoare_camp > valoare_cmp);
    if (strcmp(operator, "<=") == 0) return (valoare_camp <= valoare_cmp);
    if (strcmp(operator, ">=") == 0) return (valoare_camp >= valoare_cmp);
    if (strcmp(operator, "!=") == 0) return (valoare_camp != valoare_cmp);
    return 0;
}

int compara_operator_nume(char *valoare_camp, const char *operator, char *valoare_comparata) {
    if (strcmp(operator, "=") == 0) return strcmp(valoare_camp, valoare_comparata) == 0;
    if (strcmp(operator, "<") == 0) return strcmp(valoare_camp, valoare_comparata) < 0;
    if (strcmp(operator, ">") == 0) return strcmp(valoare_camp, valoare_comparata) > 0;
    if (strcmp(operator, "<=") == 0) return strcmp(valoare_camp, valoare_comparata) <= 0;
    if (strcmp(operator, ">=") == 0) return strcmp(valoare_camp, valoare_comparata) >= 0;
    if (strcmp(operator, "!=") == 0) return strcmp(valoare_camp, valoare_comparata) != 0;
    return 0;
}

void recalculeaza_medii(secretariat *db) {
    if (db->nr_studenti == 0) return;

    // Găsim ID-ul maxim pentru a aloca array-uri suficient de mari
    int max_id = 0;
    for (int i = 0; i < db->nr_studenti; i++) {
        if (db->studenti[i].id > max_id) {
            max_id = db->studenti[i].id;
        }
    }

    // Alocăm array-uri indexate după ID
    float *suma_note = calloc(max_id + 1, sizeof(float));
    int *numar_materii = calloc(max_id + 1, sizeof(int));
    if (!suma_note || !numar_materii) {
        free(suma_note);
        free(numar_materii);
        return;
    }

    // Resetăm mediile
    for (int i = 0; i < db->nr_studenti; i++) {
        db->studenti[i].medie_generala = 0.0f;
    }

    // Calculăm mediile folosind ID-ul ca index
    for (int i = 0; i < db->nr_inrolari; i++) {
        int id_student = db->inrolari[i].id_student;
        if (id_student >= 0 && id_student <= max_id) {
            float suma_note_materie = 0.0f;
            for (int j = 0; j < NUMBER_OF_GRADES; j++) {
                suma_note_materie += db->inrolari[i].note[j] * Power * Power;
            }
            suma_note[id_student] += suma_note_materie;
            numar_materii[id_student]++;
        }
    }

    // Atribuim mediile calculate folosind ID-ul studentului
    for (int i = 0; i < db->nr_studenti; i++) {
        int id = db->studenti[i].id;
        if (id >= 0 && id <= max_id && numar_materii[id] > 0) {
            float medie = (float)(suma_note[id] * Power) / (float)(numar_materii[id]);
            if (medie < 0.0f) medie = 0.0f;

            int zecimala_importanta = (int)(medie) % Power;
            medie = medie - (float)(zecimala_importanta);
            medie /= Power;

            if (zecimala_importanta >= Round) {
                db->studenti[i].medie_generala = (medie + 1) / Power / Power;
            } else {
                db->studenti[i].medie_generala = (float)(medie) / Power / Power;
            }
        }
    }

    free(suma_note);
    free(numar_materii);
}

void proceseaza_interogare(secretariat *db, char *comanda) {
    int caz_rar = 0, caz_special = 0;
    char *campuri = calloc(MAX_CHAR, sizeof(char));
    char *tabel = calloc(MAX_CHAR, sizeof(char));
    char *camp_where = calloc(MAX_CHAR, sizeof(char));
    char *operator = calloc(MAX_CHAR, sizeof(char));
    char *valoare = calloc(MAX_CHAR, sizeof(char));
    char *camp_temporar = calloc(MAX_CHAR, sizeof(char));
    char *camp_and = calloc(MAX_CHAR, sizeof(char));
    char *operator2 = calloc(MAX_CHAR, sizeof(char));
    char *valoare2 = calloc(MAX_CHAR, sizeof(char));

    if (!campuri || !tabel || !camp_where || !operator || !valoare ||
        !camp_temporar || !camp_and || !operator2 || !valoare2) {
        perror("Eroare malloc");
        exit(EXIT_FAILURE);
    }

    if (strstr(comanda, "AND") != NULL) {
        sscanf(comanda, "SELECT %4095[^F] FROM %4095[^W] WHERE %4095[^ ] %s %4095[^A] AND %4095[^ ] %s %4095[^;];",
               campuri, tabel, camp_where, operator, valoare, camp_and, operator2, valoare2);
    } else if (strstr(comanda, "WHERE") != NULL) {
        sscanf(comanda, "SELECT %4095[^F] FROM %4095[^W] WHERE %4095[^ ] %s %4095[^;];",
               campuri, tabel, camp_where, operator, valoare);
        caz_rar = 1;
    } else if (strstr(comanda, "FROM") != NULL) {
        sscanf(comanda, "SELECT %4095[^F] FROM %4095[^;];", campuri, tabel);
        caz_special = 1;
    } else {
        printf("Comanda necunoscută.\n");
        free(campuri); free(tabel); free(camp_where); free(operator);
        free(valoare); free(camp_temporar); free(camp_and); free(operator2); free(valoare2);
        return;
    }

    if (strstr(tabel, "studenti")) {
        for (int i = 0; i < db->nr_studenti; i++) {
            int poate_afisa = 0, poate_afisa_complex = 0;

            if (caz_rar == 0 && caz_special == 0) {
                if (strstr(camp_where, "id")) {
                    poate_afisa = compara_operator((float)db->studenti[i].id, operator, valoare);
                } else if (strstr(camp_where, "nume")) {
                    poate_afisa = compara_operator_nume(db->studenti[i].nume, operator, valoare);
                } else if (strstr(camp_where, "an_studiu")) {
                    poate_afisa = compara_operator((float)db->studenti[i].an_studiu, operator, valoare);
                } else if (strstr(camp_where, "statut")) {
                    if (strcmp(operator, "=") == 0) poate_afisa = (db->studenti[i].statut == valoare[0]);
                    else if (strcmp(operator, "!=") == 0) poate_afisa = (db->studenti[i].statut != valoare[0]);
                } else if (strstr(camp_where, "medie_generala")) {
                    poate_afisa = compara_operator(db->studenti[i].medie_generala, operator, valoare);
                }

                if (strstr(camp_and, "id")) {
                    poate_afisa_complex = compara_operator((float)db->studenti[i].id, operator2, valoare2);
                } else if (strstr(camp_and, "nume")) {
                    poate_afisa_complex = compara_operator_nume(db->studenti[i].nume, operator2, valoare2);
                } else if (strstr(camp_and, "an_studiu")) {
                    poate_afisa_complex = compara_operator((float)db->studenti[i].an_studiu, operator2, valoare2);
                } else if (strstr(camp_and, "statut")) {
                    if (strcmp(operator2, "=") == 0) {
                        poate_afisa_complex = (db->studenti[i].statut == valoare2[0]);
                    } else if (strcmp(operator2, "!=") == 0) {
                        poate_afisa_complex = (db->studenti[i].statut != valoare2[0]);
                    }
                } else if (strstr(camp_and, "medie_generala")) {
                    poate_afisa_complex = compara_operator(db->studenti[i].medie_generala, operator2, valoare2);
                }
            } else if (caz_rar == 1 && caz_special == 0) {
                if (strstr(camp_where, "id")) {
                    poate_afisa = compara_operator((float)db->studenti[i].id, operator, valoare);
                } else if (strstr(camp_where, "nume")) {
                    poate_afisa = compara_operator_nume(db->studenti[i].nume, operator, valoare);
                } else if (strstr(camp_where, "an_studiu")) {
                    poate_afisa = compara_operator((float)db->studenti[i].an_studiu, operator, valoare);
                } else if (strstr(camp_where, "statut")) {
                    if (strcmp(operator, "=") == 0) {
                        poate_afisa = (db->studenti[i].statut == valoare[0]);
                    } else if (strcmp(operator, "!=") == 0) {
                        poate_afisa = (db->studenti[i].statut != valoare[0]);
                    }
                } else if (strstr(camp_where, "medie_generala")) {
                    poate_afisa = compara_operator(db->studenti[i].medie_generala, operator, valoare);
                }
            }

            if ((poate_afisa && poate_afisa_complex) || (poate_afisa && caz_rar) || caz_special) {
                if (strstr(campuri, "*")) {
                    printf("%d %s %d %c %.2f\n",
                        db->studenti[i].id, db->studenti[i].nume,
                        db->studenti[i].an_studiu, db->studenti[i].statut,
                        db->studenti[i].medie_generala);
                } else if (strstr(campuri, ", ")) {
                    snprintf(camp_temporar, MAX_CHAR, "%s", campuri);
                    char *camp = strtok(camp_temporar, " ");
                    int first = 1;
                    while (camp) {
                        if (!first) printf(" ");
                        first = 0;
                        if (strstr(camp, "id")) printf("%d", db->studenti[i].id);
                        else if (strstr(camp, "nume")) printf("%s", db->studenti[i].nume);
                        else if (strstr(camp, "an_studiu")) printf("%d", db->studenti[i].an_studiu);
                        else if (strstr(camp, "statut")) printf("%c", db->studenti[i].statut);
                        else if (strstr(camp, "medie_generala")) printf("%.2f", db->studenti[i].medie_generala);
                        camp = strtok(NULL, ", ");
                    }
                    printf("\n");
                } else {
                    if (strstr(campuri, "id")) printf("%d", db->studenti[i].id);
                    else if (strstr(campuri, "nume")) printf("%s", db->studenti[i].nume);
                    else if (strstr(campuri, "an_studiu")) printf("%d", db->studenti[i].an_studiu);
                    else if (strstr(campuri, "statut")) printf("%c", db->studenti[i].statut);
                    else if (strstr(campuri, "medie_generala")) printf("%.2f", db->studenti[i].medie_generala);
                    printf("\n");
                }
            }
        }
    } else if (strstr(tabel, "materii")) {
        for (int i = 0; i < db->nr_materii; i++) {
            int poate_afisa = 0, poate_afisa_complex = 0;

            if (caz_rar == 0 && caz_special == 0) {
                if (strstr(camp_where, "id")) {
                    poate_afisa = compara_operator((float)db->materii[i].id, operator, valoare);
                } else if (strstr(camp_where, "nume_titular")) {
                    poate_afisa = compara_operator_nume(db->materii[i].nume_titular, operator, valoare);
                } else if (strstr(camp_where, "nume")) {
                    poate_afisa = compara_operator_nume(db->materii[i].nume, operator, valoare);
                }
                if (strstr(camp_and, "id")) {
                    poate_afisa_complex = compara_operator((float)db->materii[i].id, operator2, valoare2);
                } else if (strstr(camp_and, "nume_titular")) {
                    poate_afisa_complex = compara_operator_nume(db->materii[i].nume_titular, operator2, valoare2);
                } else if (strstr(camp_and, "nume")) {
                    poate_afisa_complex = compara_operator_nume(db->materii[i].nume, operator2, valoare2);
                }
            } else if (caz_rar == 1 && caz_special == 0) {
                if (strstr(camp_where, "id")) {
                    poate_afisa = compara_operator((float)db->materii[i].id, operator, valoare);
                } else if (strstr(camp_where, "nume_titular")) {
                    poate_afisa = compara_operator_nume(db->materii[i].nume_titular, operator, valoare);
                } else if (strstr(camp_where, "nume")) {
                    poate_afisa = compara_operator_nume(db->materii[i].nume, operator, valoare);
                }
            }

            if ((poate_afisa && poate_afisa_complex) || (poate_afisa && caz_rar) || caz_special) {
                if (strstr(campuri, "*")) {
                    printf("%d %s %s\n", db->materii[i].id, db->materii[i].nume, db->materii[i].nume_titular);
                } else if (strstr(campuri, ", ")) {
                    snprintf(camp_temporar, MAX_CHAR, "%s", campuri);
                    char *camp = strtok(camp_temporar, " ");
                    int first = 1;
                    while (camp) {
                        if (!first) printf(" ");
                        first = 0;
                        if (strstr(camp, "id")) printf("%d", db->materii[i].id);
                        else if (strstr(camp, "nume_titular")) printf("%s", db->materii[i].nume_titular);
                        else if (strstr(camp, "nume")) printf("%s", db->materii[i].nume);
                        camp = strtok(NULL, ", ");
                    }
                    printf("\n");
                } else {
                    if (strstr(campuri, "id")) printf("%d", db->materii[i].id);
                    else if (strstr(campuri, "nume_titular")) printf("%s", db->materii[i].nume_titular);
                    else if (strstr(campuri, "nume")) printf("%s", db->materii[i].nume);
                    printf("\n");
                }
            }
        }
    } else if (strstr(tabel, "inrolari")) {
        for (int i = 0; i < db->nr_inrolari; i++) {
            int poate_afisa = 0, poate_afisa_complex = 0;

            if (caz_rar == 0 && caz_special == 0) {
                if (strstr(camp_where, "id_student")) {
                    poate_afisa = compara_operator((float)db->inrolari[i].id_student, operator, valoare);
                } else if (strstr(camp_where, "id_materie")) {
                    poate_afisa = compara_operator((float)db->inrolari[i].id_materie, operator, valoare);
                } else if (strstr(camp_where, "note")) {
                    poate_afisa = compara_operator(db->inrolari[i].note[0] +
                        db->inrolari[i].note[1] + db->inrolari[i].note[2], operator, valoare);
                }
                if (strstr(camp_and, "id_student")) {
                    poate_afisa_complex = compara_operator((float)db->inrolari[i].id_student, operator2, valoare2);
                } else if (strstr(camp_and, "id_materie")) {
                    poate_afisa_complex = compara_operator((float)db->inrolari[i].id_materie, operator2, valoare2);
                } else if (strstr(camp_and, "note")) {
                    poate_afisa_complex = compara_operator(db->inrolari[i].note[0] +
                        db->inrolari[i].note[1] + db->inrolari[i].note[2], operator2, valoare2);
                }
            } else if (caz_rar == 1 && caz_special == 0) {
                if (strstr(camp_where, "id_student")) {
                    poate_afisa = compara_operator((float)db->inrolari[i].id_student, operator, valoare);
                } else if (strstr(camp_where, "id_materie")) {
                    poate_afisa = compara_operator((float)db->inrolari[i].id_materie, operator, valoare);
                } else if (strstr(camp_where, "note")) {
                    poate_afisa = compara_operator(db->inrolari[i].note[0] +
                        db->inrolari[i].note[1] + db->inrolari[i].note[2], operator, valoare);
                }
            }
            if ((poate_afisa && poate_afisa_complex) || (poate_afisa && caz_rar) || caz_special) {
                if (strstr(campuri, "*")) {
                    printf("%d %d %.2f %.2f %.2f\n", db->inrolari[i].id_student, db->inrolari[i].id_materie,
                        db->inrolari[i].note[0], db->inrolari[i].note[1], db->inrolari[i].note[2]);
                } else if (strstr(campuri, ", ")) {
                    snprintf(camp_temporar, MAX_CHAR, "%s", campuri);
                    char *camp = strtok(camp_temporar, " ");
                    int first = 1;
                    while (camp) {
                        if (!first) printf(" ");
                        first = 0;
                        if (strstr(camp, "id_student")) {
                            printf("%d", db->inrolari[i].id_student);
                        } else if (strstr(camp, "id_materie")) {
                            printf("%d", db->inrolari[i].id_materie);
                        } else if (strstr(camp, "note")) {
                            printf("%.2f %.2f %.2f", db->inrolari[i].note[0],
                                db->inrolari[i].note[1], db->inrolari[i].note[2]);
                        }
                        camp = strtok(NULL, ", ");
                    }
                    printf("\n");
                } else {
                    if (strstr(campuri, "id_student")) {
                        printf("%d", db->inrolari[i].id_student);
                    } else if (strstr(campuri, "id_materie")) {
                        printf("%d", db->inrolari[i].id_materie);
                    } else if (strstr(campuri, "note")) {
                        printf("%.2f %.2f %.2f", db->inrolari[i].note[0],
                            db->inrolari[i].note[1], db->inrolari[i].note[2]);
                    }
                    printf("\n");
                }
            }
        }
    }

    free(campuri); free(tabel); free(camp_where); free(operator);
    free(valoare); free(camp_temporar); free(camp_and); free(operator2); free(valoare2);
}

void proceseaza_update(secretariat *db, char *comanda) {
    char *tabel = calloc(MAX_CHAR, sizeof(char));
    char *camp = calloc(MAX_CHAR, sizeof(char));
    char *valoare = calloc(MAX_CHAR, sizeof(char));
    char *camp_where = calloc(MAX_CHAR, sizeof(char));
    char *operator = calloc(MAX_CHAR, sizeof(char));
    char *valoare_where = calloc(MAX_CHAR, sizeof(char));
    char *camp_and = calloc(MAX_CHAR, sizeof(char));
    char *operator2 = calloc(MAX_CHAR, sizeof(char));
    char *valoare_and = calloc(MAX_CHAR, sizeof(char));
    float nota_1 = 0, nota_2 = 0, nota_3 = 0;
    int caz_special = 0, modificari_la_inrolare = 0;

    if (!tabel || !camp || !valoare || !camp_where || !operator || !valoare_where) {
        free(tabel); free(camp); free(valoare); free(camp_where); free(operator); free(valoare_where);
        return;
    }

    if (strstr(comanda, "AND") != NULL) caz_special = 1;

    if (sscanf(comanda, "UPDATE %255s SET %255[^=]=%255[^W] WHERE %255s %255s %255[^;];",
            tabel, camp, valoare, camp_where, operator, valoare_where) != scan_var) {
        free(tabel); free(camp); free(valoare); free(camp_where); free(operator); free(valoare_where);
        return;
    }

    if (caz_special) {
        char valoare_where_copy[MAX_CHAR];
        strncpy(valoare_where_copy, valoare_where, MAX_CHAR - 1);
        char *aux = strtok(valoare_where_copy, " ");
        if (aux) strncpy(valoare_where, aux, MAX_CHAR - 1);
        aux = strtok(NULL, " ");  // AND
        aux = strtok(NULL, " ");
        if (aux) strncpy(camp_and, aux, MAX_CHAR - 1);
        aux = strtok(NULL, " ");
        if (aux) strncpy(operator2, aux, MAX_CHAR - 1);
        aux = strtok(NULL, " ");
        if (aux) strncpy(valoare_and, aux, MAX_CHAR - 1);
    }

    if (strstr(camp, "note")) {
        char *temp_val = strdup(valoare);
        temp_val[strlen(temp_val) - 1] = '\0';
        nota_1 = (float)atof(strtok(temp_val, " "));
        nota_2 = (float)atof(strtok(NULL, " "));
        nota_3 = (float)atof(strtok(NULL, " "));
        free(temp_val);
    } else if (strstr(camp, "nume") && !strstr(camp, "nume_titular")) {
        char *temp_val = strdup(valoare);
        temp_val[strlen(temp_val) - 2] = '\0';
        strncpy(valoare, temp_val + 2, MAX_CHAR - 1);
        free(temp_val);
    } else if (strstr(camp, "statut")) {
        valoare[0] = valoare[1];
        valoare[1] = '\0';
    }

    if (strstr(tabel, "studenti")) {
        for (int i = 0; i < db->nr_studenti; i++) {
            int poate_modifica = 0, poate_modifica_greu = 0;

            if (strstr(camp_where, "id")) {
                poate_modifica = compara_operator((float)db->studenti[i].id, operator, valoare_where);
            } else if (strstr(camp_where, "nume")) {
                poate_modifica = compara_operator_nume(db->studenti[i].nume, operator, valoare_where);
            } else if (strstr(camp_where, "an_studiu")) {
                poate_modifica = compara_operator((float)db->studenti[i].an_studiu, operator, valoare_where);
            } else if (strstr(camp_where, "statut")) {
                if (strcmp(operator, "=") == 0) {
                    poate_modifica = (db->studenti[i].statut == valoare_where[0]);
                } else if (strcmp(operator, "!=") == 0) {
                    poate_modifica = (db->studenti[i].statut != valoare_where[0]);
                }
            } else if (strstr(camp_where, "medie_generala")) {
                poate_modifica = compara_operator(db->studenti[i].medie_generala, operator, valoare_where);
            }

            if (poate_modifica && caz_special) {
                if (strstr(camp_and, "id")) {
                    poate_modifica_greu = compara_operator((float)db->studenti[i].id, operator2, valoare_and);
                } else if (strstr(camp_and, "nume")) {
                    poate_modifica_greu = compara_operator_nume(db->studenti[i].nume, operator2, valoare_and);
                } else if (strstr(camp_and, "an_studiu")) {
                    poate_modifica_greu = compara_operator((float)db->studenti[i].an_studiu, operator2, valoare_and);
                } else if (strstr(camp_and, "statut")) {
                    if (strcmp(operator2, "=") == 0) {
                        poate_modifica_greu = (db->studenti[i].statut == valoare_and[0]);
                    } else if (strcmp(operator2, "!=") == 0) {
                        poate_modifica_greu = (db->studenti[i].statut != valoare_and[0]);
                    }
                } else if (strstr(camp_and, "medie_generala")) {
                    poate_modifica_greu = compara_operator(db->studenti[i].medie_generala, operator2, valoare_and);
                }
            }

            if ((poate_modifica && poate_modifica_greu && caz_special) || (poate_modifica && !caz_special)) {
                if (strstr(camp, "nume") && !strstr(camp, "nume_titular")) {
                    strncpy(db->studenti[i].nume, valoare, MAX_STUDENT_NAME - 1);
                } else if (strstr(camp, "id")) {
                    db->studenti[i].id = atoi(valoare);
                } else if (strstr(camp, "an_studiu")) {
                    db->studenti[i].an_studiu = atoi(valoare);
                } else if (strstr(camp, "statut")) {
                    db->studenti[i].statut = valoare[0];
                } else if (strstr(camp, "medie_generala")) {
                    db->studenti[i].medie_generala = (float)atof(valoare);
                }
            }
        }
    } else if (strstr(tabel, "materii")) {
        for (int i = 0; i < db->nr_materii; i++) {
            int poate_modifica = 0, poate_modifica_greu = 0;

            if (strstr(camp_where, "id")) {
                poate_modifica = compara_operator((float)db->materii[i].id, operator, valoare_where);
            } else if (strstr(camp_where, "nume_titular")) {
                poate_modifica = compara_operator_nume(db->materii[i].nume_titular, operator, valoare_where);
            } else if (strstr(camp_where, "nume")) {
                poate_modifica = compara_operator_nume(db->materii[i].nume, operator, valoare_where);
            }

            if (poate_modifica && caz_special) {
                if (strstr(camp_and, "id")) {
                    poate_modifica_greu = compara_operator((float)db->materii[i].id, operator2, valoare_and);
                } else if (strstr(camp_and, "nume_titular")) {
                    poate_modifica_greu = compara_operator_nume(db->materii[i].nume_titular, operator2, valoare_and);
                } else if (strstr(camp_and, "nume")) {
                    poate_modifica_greu = compara_operator_nume(db->materii[i].nume, operator2, valoare_and);
                }
            }

            if ((poate_modifica && poate_modifica_greu && caz_special) || (poate_modifica && !caz_special)) {
                if (strstr(camp, "nume_titular")) strncpy(db->materii[i].nume_titular, valoare, MAX_CHAR - 1);
                else if (strstr(camp, "nume")) strncpy(db->materii[i].nume, valoare, MAX_CHAR - 1);
            }
        }
    } else if (strstr(tabel, "inrolari")) {
        for (int i = 0; i < db->nr_inrolari; i++) {
            int poate_modifica = 0, poate_modifica_greu = 0;

            if (strstr(camp_where, "id_student")) {
                poate_modifica = compara_operator((float)db->inrolari[i].id_student, operator, valoare_where);
            } else if (strstr(camp_where, "id_materie")) {
                poate_modifica = compara_operator((float)db->inrolari[i].id_materie, operator, valoare_where);
            } else if (strstr(camp_where, "note")) {
                poate_modifica = compara_operator(db->inrolari[i].note[0] +
                    db->inrolari[i].note[1] + db->inrolari[i].note[2], operator, valoare_where);
            }
            if (poate_modifica && caz_special) {
                if (strstr(camp_and, "id_student")) {
                    poate_modifica_greu = compara_operator((float)db->inrolari[i].id_student, operator2, valoare_and);
                } else if (strstr(camp_and, "id_materie")) {
                    poate_modifica_greu = compara_operator((float)db->inrolari[i].id_materie, operator2, valoare_and);
                } else if (strstr(camp_and, "note")) {
                    poate_modifica_greu = compara_operator(db->inrolari[i].note[0] +
                        db->inrolari[i].note[1] + db->inrolari[i].note[2], operator2, valoare_and);
                }
            }
            if ((poate_modifica && poate_modifica_greu && caz_special) || (poate_modifica && !caz_special)) {
                modificari_la_inrolare = 1;
                if (strstr(camp, "id_student")) {
                    db->inrolari[i].id_student = atoi(valoare);
                } else if (strstr(camp, "id_materie")) {
                    db->inrolari[i].id_materie = atoi(valoare);
                } else if (strstr(camp, "note")) {
                    db->inrolari[i].note[0] = nota_1;
                    db->inrolari[i].note[1] = nota_2;
                    db->inrolari[i].note[2] = nota_3;
                }
            }
        }
    }

    if (modificari_la_inrolare) {
        recalculeaza_medii(db);
    }

    free(tabel); free(camp); free(valoare); free(camp_where);
    free(operator); free(valoare_where); free(camp_and);
    free(operator2); free(valoare_and);
}

void proceseaza_delete(secretariat *db, char *comanda) {
    if (!strstr(comanda, "DELETE")) return;

    int caz_special = 0;
    if (strstr(comanda, "AND")) caz_special = 1;

    char *tabel = calloc(MAX_CHAR, sizeof(char));
    char *camp_where = calloc(MAX_CHAR, sizeof(char));
    char *operator = calloc(MAX_CHAR, sizeof(char));
    char *valoare_where = calloc(MAX_CHAR, sizeof(char));
    char *camp_and = calloc(MAX_CHAR, sizeof(char));
    char *operator2 = calloc(MAX_CHAR, sizeof(char));
    char *valoare_and = calloc(MAX_CHAR, sizeof(char));
    int modificari_la_inrolare = 0;

    if (!tabel || !camp_where || !operator || !valoare_where) {
        free(tabel); free(camp_where); free(operator); free(valoare_where);
        return;
    }

    if (sscanf(comanda, "DELETE FROM %255[^ ] WHERE %255s %255s %255[^;];",
                tabel, camp_where, operator, valoare_where) != 4) {
        free(tabel); free(camp_where); free(operator); free(valoare_where);
        return;
    }

    if (caz_special) {
        char valoare_where_copy[MAX_CHAR];
        strncpy(valoare_where_copy, valoare_where, MAX_CHAR - 1);
        char *aux = strtok(valoare_where_copy, " ");
        if (aux) strncpy(valoare_where, aux, MAX_CHAR - 1);
        aux = strtok(NULL, " ");  // AND
        aux = strtok(NULL, " ");
        if (aux) strncpy(camp_and, aux, MAX_CHAR - 1);
        aux = strtok(NULL, " ");
        if (aux) strncpy(operator2, aux, MAX_CHAR - 1);
        aux = strtok(NULL, " ");
        if (aux) strncpy(valoare_and, aux, MAX_CHAR - 1);
    }

    if (strstr(tabel, "studenti")) {
        for (int i = 0; i < db->nr_studenti; i++) {
            int poate_sterge = 0, poate_sterge_greu = 0;

            if (strstr(camp_where, "id")) {
                poate_sterge = compara_operator((float)db->studenti[i].id, operator, valoare_where);
            } else if (strstr(camp_where, "nume")) {
                poate_sterge = compara_operator_nume(db->studenti[i].nume, operator, valoare_where);
            } else if (strstr(camp_where, "an_studiu")) {
                poate_sterge = compara_operator((float)db->studenti[i].an_studiu, operator, valoare_where);
            } else if (strstr(camp_where, "statut")) {
                if (strcmp(operator, "=") == 0) {
                    poate_sterge = (db->studenti[i].statut == valoare_where[0]);
                } else if (strcmp(operator, "!=") == 0) {
                    poate_sterge = (db->studenti[i].statut != valoare_where[0]);
                }
            } else if (strstr(camp_where, "medie_generala")) {
                poate_sterge = compara_operator(db->studenti[i].medie_generala, operator, valoare_where);
            }

            if (caz_special && poate_sterge) {
                if (strstr(camp_and, "id")) {
                    poate_sterge_greu = compara_operator((float)db->studenti[i].id, operator2, valoare_and);
                } else if (strstr(camp_and, "nume")) {
                    poate_sterge_greu = compara_operator_nume(db->studenti[i].nume, operator2, valoare_and);
                } else if (strstr(camp_and, "an_studiu")) {
                    poate_sterge_greu = compara_operator((float)db->studenti[i].an_studiu, operator2, valoare_and);
                } else if (strstr(camp_and, "statut")) {
                    if (strcmp(operator2, "=") == 0) {
                        poate_sterge_greu = (db->studenti[i].statut == valoare_and[0]);
                    } else if (strcmp(operator2, "!=") == 0) {
                        poate_sterge_greu = (db->studenti[i].statut != valoare_and[0]);
                    }
                } else if (strstr(camp_and, "medie_generala")) {
                    poate_sterge_greu = compara_operator(db->studenti[i].medie_generala, operator2, valoare_and);
                }
            }

            if ((poate_sterge && !caz_special) || (poate_sterge && poate_sterge_greu && caz_special)) {
                int id_sters = db->studenti[i].id;

                // Șterge înrolările asociate acestui student
                for (int j = 0; j < db->nr_inrolari; j++) {
                    if (db->inrolari[j].id_student == id_sters) {
                        for (int k = j; k < db->nr_inrolari - 1; k++) {
                            db->inrolari[k] = db->inrolari[k + 1];
                        }
                        db->nr_inrolari--;
                        j--;
                    }
                }

                // Șterge studentul (ID-urile RĂMÂN NESCHIMBATE - nu se actualizează!)
                db->nr_studenti--;
                for (int j = i; j < db->nr_studenti; j++) {
                    db->studenti[j] = db->studenti[j + 1];  // NU modificăm db->studenti[j].id - rămâne ce a fost!
                }

                modificari_la_inrolare = 1;
                i--;
            }
        }
    } else if (strstr(tabel, "materii")) {
        for (int i = 0; i < db->nr_materii; i++) {
            int poate_sterge = 0, poate_sterge_greu = 0;

            if (strstr(camp_where, "id")) {
                poate_sterge = compara_operator((float)db->materii[i].id, operator, valoare_where);
            } else if (strstr(camp_where, "nume_titular")) {
                poate_sterge = compara_operator_nume(db->materii[i].nume_titular, operator, valoare_where);
            } else if (strstr(camp_where, "nume")) {
                poate_sterge = compara_operator_nume(db->materii[i].nume, operator, valoare_where);
            }

            if (caz_special && poate_sterge) {
                if (strstr(camp_and, "id")) {
                    poate_sterge_greu = compara_operator((float)db->materii[i].id, operator2, valoare_and);
                } else if (strstr(camp_and, "nume_titular")) {
                    poate_sterge_greu = compara_operator_nume(db->materii[i].nume_titular, operator2, valoare_and);
                } else if (strstr(camp_and, "nume")) {
                    poate_sterge_greu = compara_operator_nume(db->materii[i].nume, operator2, valoare_and);
                }
            }

            if ((poate_sterge && !caz_special) || (poate_sterge && poate_sterge_greu && caz_special)) {
                if (db->materii[i].nume) {
                    free(db->materii[i].nume);
                    db->materii[i].nume = NULL;
                }
                if (db->materii[i].nume_titular) {
                    free(db->materii[i].nume_titular);
                    db->materii[i].nume_titular = NULL;
                }
                for (int j = i; j < db->nr_materii - 1; j++) {
                    db->materii[j] = db->materii[j + 1];
                }
                db->nr_materii--;
                i--;
            }
        }
    } else if (strstr(tabel, "inrolari")) {
        for (int i = 0; i < db->nr_inrolari; i++) {
            int poate_sterge = 0, poate_sterge_greu = 0;

            if (strstr(camp_where, "id_student")) {
                poate_sterge = compara_operator((float)db->inrolari[i].id_student, operator, valoare_where);
            } else if (strstr(camp_where, "id_materie")) {
                poate_sterge = compara_operator((float)db->inrolari[i].id_materie, operator, valoare_where);
            } else if (strstr(camp_where, "note")) {
                poate_sterge = compara_operator(db->inrolari[i].note[0] +
                    db->inrolari[i].note[1] + db->inrolari[i].note[2], operator, valoare_where);
            }

            if (caz_special && poate_sterge) {
                if (strstr(camp_and, "id_student")) {
                    poate_sterge_greu = compara_operator((float)db->inrolari[i].id_student, operator2, valoare_and);
                } else if (strstr(camp_and, "id_materie")) {
                    poate_sterge_greu = compara_operator((float)db->inrolari[i].id_materie, operator2, valoare_and);
                } else if (strstr(camp_and, "note")) {
                    poate_sterge_greu = compara_operator(db->inrolari[i].note[0] +
                        db->inrolari[i].note[1] + db->inrolari[i].note[2], operator2, valoare_and);
                }
            }

            if ((poate_sterge && !caz_special) || (poate_sterge && poate_sterge_greu && caz_special)) {
                modificari_la_inrolare = 1;
                for (int j = i; j < db->nr_inrolari - 1; j++) {
                    db->inrolari[j] = db->inrolari[j + 1];
                }
                db->nr_inrolari--;
                i--;
            }
        }
    }

    if (modificari_la_inrolare) {
        recalculeaza_medii(db);
    }

    free(tabel); free(camp_where); free(operator); free(valoare_where);
    free(camp_and); free(operator2); free(valoare_and);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Eroare: Nu a fost specificat niciun fișier de intrare.\n");
        return 1;
    }

    char *delimitator_fisier = strstr(argv[1], ":");
    if (delimitator_fisier) *delimitator_fisier = '\0';

    secretariat *db = citeste_secretariat(argv[1]);
    if (!db) {
        printf("Eroare: Citirea bazei de date a eșuat.\n");
        return 1;
    }

    int numar_comenzi = 0;
    char linie[MAX_CHAR];
    if (fgets(linie, sizeof(linie), stdin) == NULL) {
        printf("Eroare!\n");
        return 1;
    }
    if (sscanf(linie, "%d", &numar_comenzi) != 1) {
        printf("Eroare!\n");
        return 1;
    }

    char comanda[MAX_CHAR];
    for (int i = 0; i < numar_comenzi; i++) {
        if (fgets(comanda, sizeof(comanda), stdin) == NULL) {
            printf("Eroare la citirea comenzii %d.\n", i + 1);
            break;
        }
        comanda[strcspn(comanda, "\n")] = 0;

        if (strstr(comanda, "UPDATE")) {
            proceseaza_update(db, comanda);
        } else if (strstr(comanda, "DELETE")) {
            proceseaza_delete(db, comanda);
        } else if (strstr(comanda, "SELECT")) {
            proceseaza_interogare(db, comanda);
        } else {
            printf("Comanda %d necunoscuta: %s\n", i + 1, comanda);
        }
    }

    elibereaza_secretariat(&db);
    return 0;
}
