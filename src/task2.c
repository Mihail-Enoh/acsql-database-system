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
    if (strcmp(operator, "=") == 0) {
        if (valoare_camp == valoare_cmp)
            return 1;
        return 0;
    }
    if (strcmp(operator, "<") == 0) {
        if (valoare_camp < valoare_cmp)
            return 1;
        return 0;
    }
    if (strcmp(operator, ">") == 0) {
        if (valoare_camp > valoare_cmp)
            return 1;
        return 0;
    }
    if (strcmp(operator, "<=") == 0) {
        if (valoare_camp <= valoare_cmp)
            return 1;
        return 0;
    }
    if (strcmp(operator, ">=") == 0) {
        if (valoare_camp >= valoare_cmp)
            return 1;
        return 0;
    }
    if (strcmp(operator, "!=") == 0) {
        if (valoare_camp != valoare_cmp)
            return 1;
        return 0;
    }
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
void proceseaza_interogare(secretariat *db, char *comanda) {
    int caz_rar = 0;
    int caz_special = 0;
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
                    if (compara_operator((float)db->studenti[i].id, operator, valoare)) {
                        poate_afisa = 1;
                    }
                } else if (strstr(camp_where, "nume")) {
                    if (compara_operator_nume(db->studenti[i].nume, operator, valoare)) {
                        poate_afisa = 1;
                    }
                } else if (strstr(camp_where, "an_studiu")) {
                    if (compara_operator((float)db->studenti[i].an_studiu, operator, valoare)) {
                        poate_afisa = 1;
                    }
                } else if (strstr(camp_where, "statut")) {
                    if (strstr(operator, "!=")) {
                        if (db->studenti[i].statut != valoare[0]) {
                            poate_afisa = 1;
                        }
                    } else if (strstr(operator, "=")) {
                        if (db->studenti[i].statut != valoare[0]) {
                            poate_afisa = 1;
                        }
                    } else {
                        printf("Operator necunoscut.\n");
                    }
                } else if (strstr(camp_where, "medie_generala")) {
                    if (compara_operator(db->studenti[i].medie_generala, operator, valoare)) {
                        poate_afisa = 1;
                    }
                }
                if (strstr(camp_and, "id")) {
                    if (compara_operator((float)db->studenti[i].id, operator2, valoare2)) {
                        poate_afisa_complex = 1;
                    }
                } else if (strstr(camp_and, "nume")) {
                    if (compara_operator_nume(db->studenti[i].nume, operator2, valoare2)) {
                        poate_afisa_complex = 1;
                    }
                } else if (strstr(camp_and, "an_studiu")) {
                    if (compara_operator((float)db->studenti[i].an_studiu, operator2, valoare2)) {
                        poate_afisa_complex = 1;
                    }
                } else if (strstr(camp_and, "statut")) {
                    if (strstr(operator2, "!=")) {
                        if (db->studenti[i].statut != valoare2[0]) {
                            poate_afisa_complex = 1;
                        }
                    } else if (strstr(operator2, "=")) {
                        if (db->studenti[i].statut == valoare2[0]) {
                            poate_afisa_complex = 1;
                        }
                    } else {
                        printf("Operator necunoscut.\n");
                    }
                } else if (strstr(camp_and, "medie_generala")) {
                    if (compara_operator(db->studenti[i].medie_generala, operator2, valoare2)) {
                        poate_afisa_complex = 1;
                    }
                }
            } else if (caz_rar == 1 && caz_special == 0) {
                if (strstr(camp_where, "id")) {
                    if (compara_operator((float)db->studenti[i].id, operator, valoare)) {
                        poate_afisa = 1;
                    }
                } else if (strstr(camp_where, "nume")) {
                    if (compara_operator_nume(db->studenti[i].nume, operator, valoare)) {
                        poate_afisa = 1;
                    }
                } else if (strstr(camp_where, "an_studiu")) {
                    if (compara_operator((float)db->studenti[i].an_studiu, operator, valoare)) {
                        poate_afisa = 1;
                    }
                } else if (strstr(camp_where, "statut")) {
                    if (strstr(operator, "!=")) {
                        if (db->studenti[i].statut != valoare[0]) {
                            poate_afisa = 1;
                        }
                    } else if (strstr(operator, "==")) {
                        if (db->studenti[i].statut != valoare[0]) {
                            poate_afisa = 1;
                        }
                    } else {
                        printf("Operator necunoscut.\n");
                    }
                } else if (strstr(camp_where, "medie_generala")) {
                    if (compara_operator(db->studenti[i].medie_generala, operator, valoare)) {
                        poate_afisa = 1;
                    }
                }
            }
            if ((poate_afisa == 1 && poate_afisa_complex == 1) || (poate_afisa == 1 && caz_rar == 1)
            || caz_special == 1) {
                if (strstr(campuri, "*")) {
                    printf("%d %s %d %c %.2f",
                        db->studenti[i].id,
                        db->studenti[i].nume,
                        db->studenti[i].an_studiu,
                        db->studenti[i].statut,
                        db->studenti[i].medie_generala);
                } else if (strstr(campuri, ", ")) {
                    snprintf(camp_temporar, MAX_CHAR, "%s", campuri);
                    char *camp = strtok((char *)camp_temporar, " ");
                    while (camp) {
                        if (strstr(camp, "id")) {
                            printf("%d", db->studenti[i].id);
                        } else if (strstr(camp, "nume")) {
                            printf("%s", db->studenti[i].nume);
                        } else if (strstr(camp, "an_studiu")) {
                            printf("%d", db->studenti[i].an_studiu);
                        } else if (strstr(camp, "statut")) {
                            printf("%c", db->studenti[i].statut);
                        } else if (strstr(camp, "medie_generala")) {
                            printf("%.2f", db->studenti[i].medie_generala);
                        }
                        camp = strtok(NULL, ", ");
                        if (camp != NULL) {
                            printf(" ");
                        }
                    }
                } else {
                    if (strstr(campuri, "id")) {
                        printf("%d", db->studenti[i].id);
                    } else if (strstr(campuri, "nume")) {
                        printf("%s", db->studenti[i].nume);
                    } else if (strstr(campuri, "an_studiu")) {
                        printf("%d", db->studenti[i].an_studiu);
                    } else if (strstr(campuri, "statut")) {
                        printf("%c", db->studenti[i].statut);
                    } else if (strstr(campuri, "medie_generala")) {
                        printf("%.2f", db->studenti[i].medie_generala);
                    }
                }
                printf("\n");
            }
        }
    } else if (strstr(tabel, "materii")) {
        for (int i = 0; i < db->nr_materii; i++) {
            int poate_afisa = 0, poate_afisa_complex = 0;
            if (caz_rar == 0 && caz_special == 0) {
                if (strstr(camp_where, "id")) {
                    if (compara_operator((float)db->materii[i].id, operator, valoare)) {
                        poate_afisa = 1;
                    }
                } else if (strstr(camp_where, "nume")) {
                    if (compara_operator_nume(db->materii[i].nume, operator, valoare)) {
                        poate_afisa = 1;
                    }
                } else if (strstr(camp_where, "nume_titular")) {
                    if (compara_operator_nume(db->materii[i].nume_titular, operator, valoare)) {
                        poate_afisa = 1;
                    }
                }
                if (strstr(camp_and, "id")) {
                    if (compara_operator((float)db->materii[i].id, operator2, valoare2)) {
                        poate_afisa_complex = 1;
                    }
                } else if (strstr(camp_and, "nume")) {
                    if (compara_operator_nume(db->materii[i].nume, operator2, valoare2)) {
                        poate_afisa_complex = 1;
                    }
                } else if (strstr(camp_and, "nume_titular")) {
                    if (compara_operator_nume(db->materii[i].nume_titular, operator2, valoare2)) {
                        poate_afisa_complex = 1;
                    }
                }
            } else if (caz_rar == 1 && caz_special == 0) {
                if (strstr(camp_where, "id")) {
                    if (compara_operator((float)db->materii[i].id, operator, valoare)) {
                        poate_afisa = 1;
                    }
                } else if (strstr(camp_where, "nume")) {
                    if (compara_operator_nume(db->materii[i].nume, operator, valoare)) {
                        poate_afisa = 1;
                    }
                } else if (strstr(camp_where, "nume_titular")) {
                    if (compara_operator_nume(db->materii[i].nume_titular, operator, valoare)) {
                        poate_afisa = 1;
                    }
                }
            }
            if ((poate_afisa == 1 && poate_afisa_complex == 1) || (poate_afisa == 1 && caz_rar == 1)
            || caz_special == 1) {
                if (strstr(campuri, "*")) {
                    printf("%d %s %s",
                        db->materii[i].id,
                        db->materii[i].nume,
                        db->materii[i].nume_titular);
                } else if (strstr(campuri, ", ")) {
                    snprintf(camp_temporar, MAX_CHAR, "%s", campuri);
                    char *camp = strtok((char *)camp_temporar, " ");
                    while (camp) {
                        if (strstr(camp, "id")) {
                            printf("%d", db->materii[i].id);
                        } else if (strstr(camp, "nume")) {
                            printf("%s", db->materii[i].nume);
                        } else if (strstr(camp, "nume_titular")) {
                            printf("%s", db->materii[i].nume_titular);
                        }
                        camp = strtok(NULL, ", ");
                        if (camp != NULL) {
                            printf(" ");
                        }
                    }
                } else {
                    if (strstr(campuri, "id")) {
                        printf("%d", db->materii[i].id);
                    } else if (strstr(campuri, "nume_titular")) {
                        printf("%s", db->materii[i].nume_titular);
                    } else if (strstr(campuri, "nume")) {
                        printf("%s", db->materii[i].nume);
                    }
                }
                printf("\n");
            }
        }
    } else if (strstr(tabel, "inrolari")) {
        for (int i = 0; i < db->nr_inrolari; i++) {
            int poate_afisa = 0, poate_afisa_complex = 0;
            if (caz_rar == 0 && caz_special == 0) {
                if (strstr(camp_where, "id_student")) {
                    if (compara_operator((float)db->inrolari[i].id_student, operator, valoare)) {
                        poate_afisa = 1;
                    }
                } else if (strstr(camp_where, "id_materie")) {
                    if (compara_operator((float)db->inrolari[i].id_materie, operator, valoare)) {
                        poate_afisa = 1;
                    }
                } else if (strstr(camp_where, "note")) {
                    if (compara_operator(db->inrolari[i].note[0]+db->inrolari[i].note[1]+db->inrolari[i].note[2],
                    operator, valoare)) {
                        poate_afisa = 1;
                    }
                }
                if (strstr(camp_and, "id_student")) {
                    if (compara_operator((float)db->inrolari[i].id_student, operator2, valoare2)) {
                        poate_afisa_complex = 1;
                    }
                } else if (strstr(camp_and, "id_materie")) {
                    if (compara_operator((float)db->inrolari[i].id_materie, operator2, valoare2)) {
                        poate_afisa_complex = 1;
                    }
                } else if (strstr(camp_and, "note")) {
                    if (compara_operator(db->inrolari[i].note[0]+db->inrolari[i].note[1]+db->inrolari[i].note[2],
                    operator2, valoare2)) {
                        poate_afisa_complex = 1;
                    }
                }
            } else if (caz_rar == 1 && caz_special == 0) {
                if (strstr(camp_where, "id_student")) {
                    if (compara_operator((float)db->inrolari[i].id_student, operator, valoare)) {
                        poate_afisa = 1;
                    }
                } else if (strstr(camp_where, "id_materie")) {
                    if (compara_operator((float)db->inrolari[i].id_materie, operator, valoare)) {
                        poate_afisa = 1;
                    }
                } else if (strstr(camp_where, "note")) {
                    if (compara_operator(db->inrolari[i].note[0]+db->inrolari[i].note[1]+db->inrolari[i].note[2],
                    operator, valoare)) {
                        poate_afisa = 1;
                    }
                }
            }
            if ((poate_afisa == 1 && poate_afisa_complex == 1) || (poate_afisa == 1 && caz_rar == 1)
            || caz_special == 1) {
                if (strstr(campuri, "*")) {
                    printf("%d %d %.2f %.2f %.2f",
                        db->inrolari[i].id_student,
                        db->inrolari[i].id_materie,
                        db->inrolari[i].note[0],
                        db->inrolari[i].note[1],
                        db->inrolari[i].note[2]);
                } else if (strstr(campuri, ", ")) {
                    snprintf(camp_temporar, MAX_CHAR, "%s", campuri);
                    char *camp = strtok((char *)camp_temporar, " ");
                    while (camp) {
                        if (strstr(camp, "id_student")) {
                            printf("%d", db->inrolari[i].id_student);
                        } else if (strstr(camp, "id_materie")) {
                            printf("%d", db->inrolari[i].id_materie);
                        } else if (strstr(camp, "note")) {
                        printf("%.2f %.2f %.2f",
                        db->inrolari[i].note[0],
                        db->inrolari[i].note[1],
                        db->inrolari[i].note[2]);
                        }
                        camp = strtok(NULL, ", ");
                        if (camp != NULL) {
                            printf(" ");
                        }
                    }
                } else {
                    if (strstr(campuri, "id_student")) {
                        printf("%d", db->inrolari[i].id_student);
                    } else if (strstr(campuri, "id_materie")) {
                        printf("%d", db->inrolari[i].id_materie);
                    } else if (strstr(campuri, "note")) {
                        printf("%.2f %.2f %.2f",
                        db->inrolari[i].note[0],
                        db->inrolari[i].note[1],
                        db->inrolari[i].note[2]);
                    }
                }
                printf("\n");
            }
        }
    } else {
        printf("Tabelul %s nu exista.\n", tabel);
    }
    free(campuri);
    free(tabel);
    free(camp_where);
    free(operator);
    free(valoare);
    free(camp_temporar);
    free(camp_and);
    free(operator2);
    free(valoare2);
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
        printf("Eroare la alocarea memoriei\n");
        free(tabel); free(camp); free(valoare);
        free(camp_where); free(operator); free(valoare_where);
        return;
    }
    if (strstr(comanda, "AND") != NULL) {
        caz_special = 1;
    }

    if (strstr(comanda, "UPDATE") != NULL) {
        if (sscanf(comanda, "UPDATE %255s SET %255[^=]=%255[^W] WHERE %255s %255s %255[^;];",
                tabel, camp, valoare, camp_where, operator, valoare_where) != scan_var) {
            printf("Comandă incorectă: %s\n", comanda);
            free(tabel); free(camp); free(valoare);
            free(camp_where); free(operator); free(valoare_where);
            return;
        }
    } else {
        printf("Comandă incorectă: %s\n", comanda);
        free(tabel); free(camp); free(valoare);
        free(camp_where); free(operator); free(valoare_where);
        return;
    }
    if (caz_special == 1) {
        char valoare_where_copy[MAX_CHAR];
        strncpy(valoare_where_copy, valoare_where, MAX_CHAR);
        char camp_and_copy[MAX_CHAR] = {0};
        char operator2_copy[MAX_CHAR] = {0};
        char valoare_and_copy[MAX_CHAR] = {0};
        char *aux = strtok(valoare_where_copy, " ");
        if (aux) {
            strncpy(valoare_where, aux, MAX_CHAR);
        }
        aux = strtok(NULL, " ");
        aux = strtok(NULL, " ");
        if (aux) {
            strncpy(camp_and_copy, aux, MAX_CHAR);
        }
        aux = strtok(NULL, " ");
        if (aux) {
            strncpy(operator2_copy, aux, MAX_CHAR);
        }
        aux = strtok(NULL, " ");
        if (aux) {
            strncpy(valoare_and_copy, aux, MAX_CHAR);
        }
        strncpy(camp_and, camp_and_copy, MAX_CHAR);
        strncpy(operator2, operator2_copy, MAX_CHAR);
        strncpy(valoare_and, valoare_and_copy, MAX_CHAR);
    }
    if (strstr(camp, "note")) {
        char *temp_val = strdup(valoare);
        temp_val[strlen(temp_val) - 1] = '\0';
        if (temp_val) {
            nota_1 = (float)atof(strtok(temp_val, " "));
            nota_2 = (float)atof(strtok(NULL, " "));
            nota_3 = (float)atof(strtok(NULL, " "));
            free(temp_val);
        }
    } else if (strstr(camp, "nume")) {
        char *temp_val = strdup(valoare);
        temp_val[strlen(temp_val) - 2] = '\0';
        strncpy(valoare, temp_val + 2, MAX_CHAR);
        free(temp_val);
    } else if (strstr(camp, "statut")) {
        valoare[0] = valoare[1];
        valoare[strlen(valoare) - 1] = '\0';
    }
    if (strstr(tabel, "studenti")) {
        for (int i = 0; i < db->nr_studenti; i++) {
            int poate_modifica = 0, poate_modifica_complex = 0;
            if (strstr(camp_where, "id") &&
                compara_operator((float)db->studenti[i].id, operator, valoare_where)) {
                poate_modifica = 1;
            } else if (strstr(camp_where, "nume") &&
                compara_operator_nume(db->studenti[i].nume, operator, valoare_where)) {
                poate_modifica = 1;
            } else if (strstr(camp_where, "an_studiu") &&
                compara_operator((float)db->studenti[i].an_studiu, operator, valoare_where)) {
                poate_modifica = 1;
            } else if (strstr(camp_where, "statut")) {
                if (strstr("!=", operator)) {
                    if (strstr("=", operator)) {
                        if (db->studenti[i].statut == valoare_where[0]) {
                            poate_modifica = 1;
                        }
                    } else {
                        if (db->studenti[i].statut != valoare_where[0]) {
                            poate_modifica = 1;
                        }
                    }
                } else {
                    printf("Operator necunoscut:%s.\n", operator);
                }
            } else if (strstr(camp_where, "medie_generala") &&
                compara_operator(db->studenti[i].medie_generala, operator, valoare_where)) {
                poate_modifica = 1;
            }
            if (poate_modifica == 1 && caz_special == 1) {
                if (strstr(camp_and, "id") &&
                    compara_operator((float)db->studenti[i].id, operator2, valoare_and)) {
                    poate_modifica_complex = 1;
                } else if (strstr(camp_and, "nume") &&
                    compara_operator_nume(db->studenti[i].nume, operator2, valoare_and)) {
                    poate_modifica_complex = 1;
                } else if (strstr(camp_and, "an_studiu") &&
                    compara_operator((float)db->studenti[i].an_studiu, operator2, valoare_and)) {
                    poate_modifica_complex = 1;
                } else if (strstr(camp_and, "statut")) {
                    if (strstr("!=", operator2)) {
                        if (strstr("=", operator2)) {
                            if (db->studenti[i].statut == valoare_where[0]) {
                                poate_modifica = 1;
                            }
                        } else {
                            if (db->studenti[i].statut != valoare_where[0]) {
                                poate_modifica = 1;
                            }
                        }
                    } else {
                        printf("Operator2 necunoscut:%s.\n", operator2);
                    }
                } else if (strstr(camp_and, "medie_generala")
                && compara_operator(db->studenti[i].medie_generala, operator2, valoare_and)) {
                    poate_modifica_complex = 1;
                }
            }
            if ((poate_modifica == 1 && poate_modifica_complex == 1 && caz_special == 1) ||
            (poate_modifica == 1 && caz_special == 0)) {
                if (strstr(camp, "nume")) {
                    strncpy(db->studenti[i].nume, valoare, MAX_STUDENT_NAME - 1);
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
                int poate_modifica = 0, poate_modifica_complex = 0;
                if (strstr(camp_where, "id") &&
                    compara_operator((float)db->materii[i].id, operator, valoare_where)) {
                    poate_modifica = 1;
                } else if (strstr(camp_where, "nume") &&
                    compara_operator_nume(db->materii[i].nume, operator, valoare_where)) {
                    poate_modifica = 1;
                } else if (strstr(camp_where, "nume_titular") &&
                    compara_operator_nume(db->materii[i].nume_titular, operator, valoare_where)) {
                    poate_modifica = 1;
                }
                if (poate_modifica == 1 && caz_special == 1) {
                    if (strstr(camp_and, "id") &&
                        compara_operator((float)db->materii[i].id, operator2, valoare_and)) {
                        poate_modifica_complex = 1;
                    } else if (strstr(camp_and, "nume") &&
                        compara_operator_nume(db->materii[i].nume, operator2, valoare_and)) {
                        poate_modifica_complex = 1;
                    } else if (strstr(camp_and, "nume_titular") &&
                        compara_operator_nume(db->materii[i].nume_titular, operator2, valoare_and)) {
                        poate_modifica_complex = 1;
                    }
                }
                if ((poate_modifica == 1 && poate_modifica_complex == 1 && caz_special == 1) ||
                (poate_modifica == 1 && caz_special == 0)) {
                    if (strstr(camp, "nume")) {
                        strncpy(db->materii[i].nume, valoare, MAX_CHAR - 1);
                    } else if (strstr(camp, "nume_titular")) {
                        strncpy(db->materii[i].nume_titular, valoare, MAX_CHAR - 1);
                    }
                }
            }
    } else if (strstr(tabel, "inrolari")) {
            for (int i = 0; i < db->nr_inrolari; i++) {
                int poate_modifica = 0, poate_modifica_complex = 0;
                if (strstr(camp_where, "id_student") &&
                    compara_operator((float)db->inrolari[i].id_student, operator, valoare_where)) {
                    poate_modifica = 1;
                } else if (strstr(camp_where, "id_materie") &&
                    compara_operator((float)db->inrolari[i].id_materie, operator, valoare_where)) {
                    poate_modifica = 1;
                }
                if (poate_modifica == 1 && caz_special == 1) {
                    if (strstr(camp_and, "id_student") &&
                        compara_operator((float)db->inrolari[i].id_student, operator2, valoare_and)) {
                        poate_modifica_complex = 1;
                    } else if (strstr(camp_and, "id_materie") &&
                        compara_operator((float)db->inrolari[i].id_materie, operator2, valoare_and)) {
                        poate_modifica_complex = 1;
                    }
                }
                if ((poate_modifica == 1 && poate_modifica_complex == 1 && caz_special == 1) ||
                (caz_special == 0 && poate_modifica == 1)) {
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
    } else {
        printf("Tabelul %s nu există.\n", tabel);
    }
    if (modificari_la_inrolare == 1 && db->nr_studenti > 0) {
        for (int i = 0; i < db->nr_studenti; i++) {
            db->studenti[i].medie_generala = 0.0f;
        }
        float medie = 0.0f;
        int *numar_materii = calloc(db->nr_studenti, sizeof(int));
        if (numar_materii == NULL) {
            printf("Eroare alocare memorie pentru numar_materii\n");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < db->nr_inrolari; i++) {
            int id_student = db->inrolari[i].id_student;
            if (id_student < 0 || id_student >= db->nr_studenti || db->studenti[id_student].nume == NULL) {
                continue;
            }
            float suma_note_materie = 0.0f;
            for (int j = 0; j < NUMBER_OF_GRADES; j++) {
                suma_note_materie += db->inrolari[i].note[j] * Power * Power;
            }
            db->studenti[id_student].medie_generala += suma_note_materie;
            numar_materii[id_student]++;
        }
        for (int i = 0; i < db->nr_studenti; i++) {
            if (numar_materii[i] > 0) {
                medie = (float)(db->studenti[i].medie_generala * Power) / (float)(numar_materii[i]);
                if (medie < 0.0f) {
                    medie = 0.0f;
                }
                int zecimala_importanta = (int)(medie) % Power;
                medie -= (float)(zecimala_importanta);
                medie /= Power;
                if (zecimala_importanta >= Round) {
                    db->studenti[i].medie_generala = (medie + 1) / Power / Power;
                } else {
                    db->studenti[i].medie_generala = (float)(medie) / Power / Power;
                }
            }
        }
        free(numar_materii);
    }
    free(tabel);
    free(camp);
    free(valoare);
    free(camp_where);
    free(operator);
    free(valoare_where);
    free(camp_and);
    free(operator2);
    free(valoare_and);
}
void proceseaza_delete(secretariat *db, char *comanda) {
    if (strstr(comanda, "DELETE") == NULL) {
        printf("Comandă incorectă: %s\n", comanda);
        return;
    }
    int caz_special = 0;
    if (strstr(comanda, "AND") != NULL) {
        caz_special = 1;
    }
    char *tabel = calloc(MAX_CHAR, sizeof(char));
    char *camp_where = calloc(MAX_CHAR, sizeof(char));
    char *operator = calloc(MAX_CHAR, sizeof(char));
    char *valoare_where = calloc(MAX_CHAR, sizeof(char));
    char *camp_and = calloc(MAX_CHAR, sizeof(char));
    char *operator2 = calloc(MAX_CHAR, sizeof(char));
    char *valoare_and = calloc(MAX_CHAR, sizeof(char));
    float nota_1 = 0, nota_2 = 0, nota_3 = 0;
    int modificari_la_inrolare = 0;
    if (!tabel || !camp_where || !operator || !valoare_where) {
        printf("Eroare la alocarea memoriei\n");
        free(tabel); free(camp_where); free(operator); free(valoare_where);
        return;
    }
    if (sscanf(comanda, "DELETE FROM %255[^ ] WHERE %255s %255s %255[^;];",
                tabel, camp_where, operator, valoare_where) != scan_var -1 -1) {
            printf("Comandă incorectă: %s\n", comanda);
            free(tabel); free(camp_where); free(operator); free(valoare_where);
            return;
    }
    if (caz_special == 1) {
        char valoare_where_copy[MAX_CHAR];
        strncpy(valoare_where_copy, valoare_where, MAX_CHAR);
        char camp_and_copy[MAX_CHAR] = {0};
        char operator2_copy[MAX_CHAR] = {0};
        char valoare_and_copy[MAX_CHAR] = {0};
        char *aux = strtok(valoare_where_copy, " ");
        if (aux) {
            strncpy(valoare_where, aux, MAX_CHAR);
        }
        aux = strtok(NULL, " ");
        aux = strtok(NULL, " ");
        if (aux) {
            strncpy(camp_and_copy, aux, MAX_CHAR);
        }
        aux = strtok(NULL, " ");
        if (aux) {
            strncpy(operator2_copy, aux, MAX_CHAR);
        }
        aux = strtok(NULL, " ");
        if (aux) {
            strncpy(valoare_and_copy, aux, MAX_CHAR);
        }
        strncpy(camp_and, camp_and_copy, MAX_CHAR);
        strncpy(operator2, operator2_copy, MAX_CHAR);
        strncpy(valoare_and, valoare_and_copy, MAX_CHAR);
    }
    if (strstr(camp_where, "note")) {
        char *temp_val = strdup(valoare_where);
        temp_val[strlen(temp_val) - 1] = '\0';
        if (temp_val) {
            nota_1 = (float)atof(strtok(temp_val, " "));
            nota_2 = (float)atof(strtok(NULL, " "));
            nota_3 = (float)atof(strtok(NULL, " "));
            free(temp_val);
        }
    } else if (strstr(camp_and, "note")) {
        char *temp_val = strdup(valoare_and);
        temp_val[strlen(temp_val) - 1] = '\0';
        if (temp_val) {
            nota_1 = (float)atof(strtok(temp_val, " "));
            nota_2 = (float)atof(strtok(NULL, " "));
            nota_3 = (float)atof(strtok(NULL, " "));
            free(temp_val);
        }
    }

    if (strstr(tabel, "studenti")) {
        for (int i = 0; i < db->nr_studenti; i++) {
            int poate_sterge = 0, poate_sterge_complex = 0;
            if (strstr(camp_where, "id") &&
                compara_operator((float)db->studenti[i].id, operator, valoare_where)) {
                poate_sterge = 1;
            } else if (strstr(camp_where, "nume") &&
                compara_operator_nume(db->studenti[i].nume, operator, valoare_where)) {
                poate_sterge = 1;
            } else if (strstr(camp_where, "an_studiu") &&
                compara_operator((float)db->studenti[i].an_studiu, operator, valoare_where)) {
                poate_sterge = 1;
            } else if (strstr(camp_where, "statut")) {
                if (strcmp(operator, "=") == 0) {
                    if (db->studenti[i].statut == valoare_where[0]) {
                        poate_sterge = 1;
                    }
                } else if (strcmp(operator, "!=") == 0) {
                    if (db->studenti[i].statut != valoare_where[0]) {
                        poate_sterge = 1;
                    }
                } else {
                    printf("Operator necunoscut.\n");
                }
            } else if (strstr(camp_where, "medie_generala") &&
                compara_operator(db->studenti[i].medie_generala, operator, valoare_where)) {
                poate_sterge = 1;
            }
            if (caz_special == 1) {
                if (strstr(camp_and, "id") &&
                    compara_operator((float)db->studenti[i].id, operator2, valoare_and)) {
                    poate_sterge_complex = 1;
                } else if (strstr(camp_and, "nume") &&
                    compara_operator_nume(db->studenti[i].nume, operator2, valoare_and)) {
                    poate_sterge_complex = 1;
                } else if (strstr(camp_and, "an_studiu") &&
                    compara_operator((float)db->studenti[i].an_studiu, operator2, valoare_and)) {
                    poate_sterge_complex = 1;
                } else if (strstr(camp_and, "statut")) {
                    if (strcmp(operator2, "=") == 0) {
                        if (db->studenti[i].statut == valoare_and[0]) {
                            poate_sterge_complex = 1;
                        }
                    } else if (strcmp(operator2, "!=") == 0) {
                        if (db->studenti[i].statut != valoare_and[0]) {
                            poate_sterge_complex = 1;
                        }
                    } else {
                        printf("Operator necunoscut.\n");
                    }
                } else if (strstr(camp_and, "medie_generala") &&
                    compara_operator(db->studenti[i].medie_generala, operator2, valoare_and)) {
                    poate_sterge_complex = 1;
                }
            }
            if ((poate_sterge == 1 && caz_special == 0) ||
            (poate_sterge_complex == 1 && caz_special == 1 && poate_sterge == 1)) {
                db->nr_studenti--;
                for (int j = i; j < db->nr_studenti; j++) {
                    db->studenti[j] = db->studenti[j + 1];
                }
                i--;
            }
        }
    } else if (strstr(tabel, "materii")) {
        for (int i = 0; i < db->nr_materii; i++) {
            int poate_sterge = 0, poate_sterge_complex = 0;
            if (strstr(camp_where, "id") &&
                compara_operator((float)db->materii[i].id, operator, valoare_where)) {
                poate_sterge = 1;
            } else if (strstr(camp_where, "nume") &&
                compara_operator_nume(db->materii[i].nume, operator, valoare_where)) {
                poate_sterge = 1;
            } else if (strstr(camp_where, "nume_titular") &&
                compara_operator_nume(db->materii[i].nume_titular, operator, valoare_where)) {
                poate_sterge = 1;
            }
            if (caz_special == 1 && poate_sterge == 1) {
                if (strstr(camp_and, "id") &&
                    compara_operator((float)db->materii[i].id, operator2, valoare_and)) {
                    poate_sterge_complex = 1;
                } else if (strstr(camp_and, "nume") &&
                    compara_operator_nume(db->materii[i].nume, operator2, valoare_and)) {
                    poate_sterge_complex = 1;
                } else if (strstr(camp_and, "nume_titular") &&
                    compara_operator_nume(db->materii[i].nume_titular, operator2, valoare_and)) {
                    poate_sterge_complex = 1;
                }
            }
            if ((poate_sterge_complex == 1 && caz_special == 1 && poate_sterge == 1) ||
            (poate_sterge == 1 && caz_special == 0)) {
                if (db->materii[i].nume != NULL) {
                    free(db->materii[i].nume);
                    db->materii[i].nume = NULL;
                }
                if (db->materii[i].nume_titular != NULL) {
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
            int poate_sterge = 0, poate_sterge_complex = 0;
            if (strstr(camp_where, "id_student") &&
                compara_operator((float)db->inrolari[i].id_student, operator, valoare_where)) {
                poate_sterge = 1;
            } else if (strstr(camp_where, "id_materie") &&
                compara_operator((float)db->inrolari[i].id_materie, operator, valoare_where)) {
                poate_sterge = 1;
            } else if (strstr(camp_where, "note")) {
                char nota_1_str[MAX_CHAR], nota_2_str[MAX_CHAR], nota_3_str[MAX_CHAR];
                snprintf(nota_1_str, sizeof(nota_1_str), "%.2f", nota_1);
                snprintf(nota_2_str, sizeof(nota_2_str), "%.2f", nota_2);
                snprintf(nota_3_str, sizeof(nota_3_str), "%.2f", nota_3);
                if (compara_operator(db->inrolari[i].note[0], operator, nota_1_str) &&
                    compara_operator(db->inrolari[i].note[1], operator, nota_2_str) &&
                    compara_operator(db->inrolari[i].note[2], operator, nota_3_str)) {
                    poate_sterge = 1;
                }
            }
            if (caz_special == 1 && poate_sterge == 1) {
                if (strstr(camp_and, "id_student") &&
                    compara_operator((float)db->inrolari[i].id_student, operator2, valoare_and)) {
                    poate_sterge_complex = 1;
                } else if (strstr(camp_and, "id_materie") &&
                    compara_operator((float)db->inrolari[i].id_materie, operator2, valoare_and)) {
                    poate_sterge_complex = 1;
                } else if (strstr(camp_and, "note")) {
                    char nota_1_str[MAX_CHAR], nota_2_str[MAX_CHAR], nota_3_str[MAX_CHAR];
                    snprintf(nota_1_str, sizeof(nota_1_str), "%.2f", nota_1);
                    snprintf(nota_2_str, sizeof(nota_2_str), "%.2f", nota_2);
                    snprintf(nota_3_str, sizeof(nota_3_str), "%.2f", nota_3);
                    if (compara_operator(db->inrolari[i].note[0], operator2, nota_1_str) &&
                        compara_operator(db->inrolari[i].note[1], operator2, nota_2_str) &&
                        compara_operator(db->inrolari[i].note[2], operator2, nota_3_str)) {
                        poate_sterge = 1;
                    }
                }
            }
            if ((poate_sterge == 1 && caz_special == 0) ||
            (poate_sterge == 1 && poate_sterge_complex == 1 && caz_special == 1)) {
                modificari_la_inrolare = 1;
                for (int j = i; j < db->nr_inrolari - 1; j++) {
                    db->inrolari[j] = db->inrolari[j + 1];
                }
                db->nr_inrolari--;
                i--;
            }
        }
    } else {
        printf("Tabel necunoscut.\n");
    }
        if (modificari_la_inrolare == 1 && db->nr_studenti > 0) {
            for (int i = 0; i < db->nr_studenti; i++) {
                db->studenti[i].medie_generala = 0.0f;
            }
            float medie = 0.0f;
            int *numar_materii = calloc(db->nr_studenti, sizeof(int));
            if (numar_materii == NULL) {
                printf("Eroare alocare memorie pentru numar_materii\n");
                exit(EXIT_FAILURE);
            }
            for (int i = 0; i < db->nr_inrolari; i++) {
                int id_student = db->inrolari[i].id_student;
                if (id_student < 0 || id_student >= db->nr_studenti || db->studenti[id_student].nume == NULL) {
                    continue;
                }
                float suma_note_materie = 0.0f;
                for (int j = 0; j < NUMBER_OF_GRADES; j++) {
                    suma_note_materie += db->inrolari[i].note[j] * Power * Power;
                }
                db->studenti[id_student].medie_generala += suma_note_materie;
                numar_materii[id_student]++;
            }
            for (int i = 0; i < db->nr_studenti; i++) {
                if (numar_materii[i] > 0) {
                    medie = (float)(db->studenti[i].medie_generala * Power) / (float)(numar_materii[i]);
                    if (medie < 0.0f) {
                        medie = 0.0f;
                    }
                    int zecimala_importanta = (int)(medie) % Power;
                    medie -= (float)(zecimala_importanta);
                    medie /= Power;
                    if (zecimala_importanta >= Round) {
                        db->studenti[i].medie_generala = (medie + 1) / Power / Power;
                    } else {
                        db->studenti[i].medie_generala = (float)(medie) / Power / Power;
                    }
                }
            }
            free(numar_materii);
        }

    free(tabel);
    free(camp_where);
    free(operator);
    free(valoare_where);
    free(camp_and);
    free(operator2);
    free(valoare_and);
}
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Eroare: Nu a fost specificat niciun fișier de intrare.\n");
        return 1;
    }
    char *delimitator_fisier = strstr(argv[1], ":");
    if (delimitator_fisier) {
        *delimitator_fisier = '\0';
    }
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
    return 0;
}
      /*          } else if (strstr(campuri, "note")) {
                    printf("%.2f %.2f %.2f",
                        db->inrolari[i].note[0],
                        db->inrolari[i].note[1],
                        db->inrolari[i].note[2]);
                }
                } else if (strstr(campuri, "*")) {
                printf("%d %d %.2f %.2f %.2f",
                    db->inrolari[i].id_student,
                    db->inrolari[i].id_materie,
                    db->inrolari[i].note[0],
                    db->inrolari[i].note[1],
                    db->inrolari[i].note[2]);
                } else if (strstr(campuri, ",")) {
                    char *camp = strtok(campuri, ", ");
                    while (camp != NULL) {
                        if (strstr(camp, "id_student")) {
                            printf("%d", db->inrolari[i].id_student);
                        } else if (strstr(camp, "id_materie")) {
                            printf("%d", db->inrolari[i].id_materie);
                        } else if (strstr(camp, "note")) {
                            printf("%.2f %.2f %.2f",
                            db->inrolari[i].note[0],
                            db->inrolari[i].note[1],
                            db->inrolari[i].note[2]);
                        }
                        camp = strtok(NULL, ", ");
                        if (camp != NULL) {
                            printf(" ");
                        }
                    }
                } else {
                    if (strstr(campuri, "id_student")) {
                        printf("%d", db->inrolari[i].id_student);
                    } else if (strstr(campuri, "id_materie")) {
                        printf("%d", db->inrolari[i].id_materie);
                    } else if (strstr(campuri, "note")) {
                        printf("%.2f %.2f %.2f",
                        db->inrolari[i].note[0],
                        db->inrolari[i].note[1],
                        db->inrolari[i].note[2]);
                    }
                }
                printf("\n");
            }
        }
    } else {
        printf("Tabel necunoscut.\n");
        */
