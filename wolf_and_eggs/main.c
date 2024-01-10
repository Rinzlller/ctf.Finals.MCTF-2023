#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#define BUF_SIZE 256


struct egg {
    int egg_id;
    int user_id;
    char *egg_name;
    char *egg_data;
};

struct egg *eggs_array[10];
PGconn *conn;
char pg_connect[] = "host=db user=postgres password=somepassword dbname=wolfeggs";
int userid;


void store_egg() {
    puts("(!) YOU CAN STORE MAX 10 EGGS! (!)");
    PGresult *rc;
    const char *params[3];
    char *insert_sql;
    char *update_sql;
    char _userid[20];
    char *eggid;
    int _local_egg_id;
    int len;
    char egg_name[BUF_SIZE];
    char egg_data[BUF_SIZE];

    printf("(+) Enter egg id (0-10): ");
    scanf("%d", &_local_egg_id);
    getchar();
    if ((_local_egg_id < 0) || (_local_egg_id > 9)) {
        printf("(!) Wrong id!\n");
        PQfinish(conn);
        exit(-1);
    }

    printf("(+) Enter name for you egg: ");
    fgets(egg_name, BUF_SIZE - 1, stdin);
    egg_name[strnlen(egg_name, BUF_SIZE) - 1] = '\0';

    printf("(+) Enter egg data: ");
    fgets(egg_data, BUF_SIZE - 1, stdin);
    egg_data[strnlen(egg_data, BUF_SIZE) - 1] = '\0';

    params[0] = egg_name;
    params[1] = egg_data;
    sprintf(_userid, "%d", userid);
    params[2] = _userid;
    insert_sql = "INSERT INTO eggs (egg_name, egg_data, user_id) VALUES ($1, $2, $3) RETURNING eggid";
    rc = PQexecParams(conn, insert_sql, 3, NULL, params, NULL, NULL, 0);
    if (PQresultStatus(rc) != PGRES_TUPLES_OK) {
        printf("(!) Some SQL error!\n");
        PQclear(rc);
        PQfinish(conn);
        exit(-1);
    }
    eggid = PQgetvalue(rc, 0, 0);
    PQclear(rc);

    params[0] = _userid;
    update_sql = "UPDATE users SET eggs_num=eggs_num+1 WHERE userid=$1";
    rc = PQexecParams(conn, update_sql, 1, NULL, params, NULL, NULL, 0);
    if (PQresultStatus(rc) != PGRES_COMMAND_OK) {
        printf("(!) Some SQL error!\n");
        PQclear(rc);
        PQfinish(conn);
        exit(-1);
    }
    PQclear(rc);

    eggs_array[_local_egg_id] = (struct egg *)malloc(sizeof(struct egg));
    eggs_array[_local_egg_id]->egg_id = atoi(eggid);
    eggs_array[_local_egg_id]->egg_name = (char *)malloc(BUF_SIZE);
    strncpy(eggs_array[_local_egg_id]->egg_name, egg_name, BUF_SIZE);
    eggs_array[_local_egg_id]->egg_data = (char *)malloc(BUF_SIZE);
    strncpy(eggs_array[_local_egg_id]->egg_data, egg_data, BUF_SIZE);
    eggs_array[_local_egg_id]->user_id = userid;
    //printf("(ID OF YOUR EGG): %s\n", eggid);
}

void print_egg() {
    char *eggid;
    int _local_egg_id;

    printf("(+) Enter egg id (0-10): ");
    scanf("%d", &_local_egg_id);
    getchar();
    if ((_local_egg_id < 0) || (_local_egg_id > 9)) {
        printf("(!) Wrong id!\n");
        PQfinish(conn);
        exit(-1);
    }
    puts("(egg):");
    printf("(name): %s\n", eggs_array[_local_egg_id]->egg_name);
    printf("(data): %s\n", eggs_array[_local_egg_id]->egg_data);
}

void edit_data_only() {
    PGresult *rc;
    const char *params[3];
    char *update_sql;
    char _eggid[20];
    int _local_egg_id;
    char egg_data[BUF_SIZE];
    int len1;
    
    printf("(+) Enter egg id (0-10): ");
    scanf("%d", &_local_egg_id);
    getchar();
    if ((_local_egg_id < 0) || (_local_egg_id > 9)) {
        printf("(!) Wrong id!\n");
        PQfinish(conn);
        exit(-1);
    }

    printf("(+) Enter new egg data: ");
    len1 = read(0, egg_data, BUF_SIZE - 1);
    //egg_data[strnlen(egg_data, BUF_SIZE) - 1] = '\0';

    const int paramLenghts[] = { len1, 0 };
    const int paramFormats[] = { 1, 0 };
    params[0] = egg_data;
    sprintf(_eggid, "%d", eggs_array[_local_egg_id]->egg_id);
    params[1] = _eggid;
    update_sql = "UPDATE eggs SET egg_data=$1 WHERE eggid=$2";
    rc = PQexecParams(conn, update_sql, 2, NULL, params, paramLenghts, paramFormats, 0);
    if (PQresultStatus(rc) != PGRES_COMMAND_OK) {
        printf("(!) Some SQL error!\n");
        PQclear(rc);
        PQfinish(conn);
        exit(-1);
    }
    PQclear(rc);
    
    memcpy(eggs_array[_local_egg_id]->egg_data, egg_data, len1);
}

void edit_egg() {
    PGresult *rc;
    const char *params[3];
    char *update_sql;
    char _eggid[20];
    int _local_egg_id;
    char egg_name[BUF_SIZE];
    char egg_data[BUF_SIZE];
    int len1, len2;

    printf("(+) Enter egg id (0-10): ");
    scanf("%d", &_local_egg_id);
    getchar();
    if ((_local_egg_id < 0) || (_local_egg_id > 9)) {
        printf("(!) Wrong id!\n");
        PQfinish(conn);
        exit(-1);
    }

    printf("(+) Enter new name for you egg: ");
    len1 = read(0, egg_name, BUF_SIZE - 1);
    //egg_name[strnlen(egg_name, BUF_SIZE) - 1] = '\0';

    printf("(+) Enter new egg data: ");
    len2 = read(0, egg_data, BUF_SIZE - 1);
    //egg_data[strnlen(egg_data, BUF_SIZE) - 1] = '\0';

    memcpy(eggs_array[_local_egg_id]->egg_name, egg_name, len1);
    memcpy(eggs_array[_local_egg_id]->egg_data, egg_data, len2);

    const int paramLenghts[] = { len1, len2, 0 };
    const int paramFormats[] = { 1, 1, 0 };
    params[0] = egg_name;
    params[1] = egg_data;
    sprintf(_eggid, "%d", eggs_array[_local_egg_id]->egg_id);
    params[2] = _eggid;
    update_sql = "UPDATE eggs SET egg_name=$1,egg_data=$2 WHERE eggid=$3";
    rc = PQexecParams(conn, update_sql, 3, NULL, params, paramLenghts, paramFormats, 0);
    if (PQresultStatus(rc) != PGRES_COMMAND_OK) {
        printf("(!) Some SQL error!\n");
        PQclear(rc);
        PQfinish(conn);
        exit(-1);
    }
    PQclear(rc);
}

void delete_egg() {
    PGresult *rc;
    const char *params[3];
    char *delete_sql;
    char _eggid[20];
    int _local_egg_id;

    printf("(+) Enter egg id (0-10): ");
    scanf("%d", &_local_egg_id);
    getchar();
    if ((_local_egg_id < 0) || (_local_egg_id > 9)) {
        printf("(!) Wrong id!\n");
        PQfinish(conn);
        exit(-1);
    }

    delete_sql = "DELETE FROM eggs WHERE eggid=$1";
    sprintf(_eggid, "%d", eggs_array[_local_egg_id]->egg_id);
    params[0] = _eggid;
    rc = PQexecParams(conn, delete_sql, 1, NULL, params, NULL, NULL, 0);
    if (PQresultStatus(rc) != PGRES_COMMAND_OK) {
        printf("(!) Some SQL error!\n");
        PQclear(rc);
        PQfinish(conn);
        exit(-1);
    }
    PQclear(rc);

    free(eggs_array[_local_egg_id]->egg_name);
    free(eggs_array[_local_egg_id]->egg_data);
    free(eggs_array[_local_egg_id]);
}

void sync_eggs() {
    PGresult *rc;
    const char *params[3];
    char *select_sql;
    char _userid[20];
    char *eggid;
    int rows;
    int cols;

    sprintf(_userid, "%d", userid);
    params[0] = _userid;
    select_sql = "SELECT eggid, encode(egg_name, 'escape'), encode(egg_data, 'escape'), user_id FROM eggs WHERE user_id = $1";
    rc = PQexecParams(conn, select_sql, 1, NULL, params, NULL, NULL, 0);
    rows = PQntuples(rc);
    cols = PQnfields(rc);
    for (int i = 0; i < rows; i++) {
        eggs_array[i] = (struct egg *)malloc(sizeof(struct egg));
        eggs_array[i]->egg_id = atoi(PQgetvalue(rc, i, 0));
        eggs_array[i]->egg_name = (char *)malloc(BUF_SIZE);
        strncpy(eggs_array[i]->egg_name, PQgetvalue(rc, i, 1), BUF_SIZE);
        eggs_array[i]->egg_data = (char *)malloc(BUF_SIZE);
        strncpy(eggs_array[i]->egg_data, PQgetvalue(rc, i, 2), BUF_SIZE);
        eggs_array[i]->user_id = atoi(PQgetvalue(rc, i, 3));
    }
    PQclear(rc);
}

void print_wolf() {
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣸⣷⣶⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣶⣿⡁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣾⣿⣿⣿⣿⣷⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣶⣿⣿⣿⣿⣿⣦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⣿⣿⣿⣿⣿⣿⣿⣿⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣾⣿⣿⣿⢿⣿⣿⣿⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⠏⢹⣿⡿⠁⠈⠛⣿⣿⣷⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⣿⣿⠟⠁⠀⢻⣿⣿⠉⢇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⠇⠀⠀⠀⠈⢿⣿⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⣿⠋⠀⠀⠀⠀⢻⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⠀⢀⡾⠀⠀⠀⢿⣿⣧⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⣿⣿⠃⠀⠀⠸⣄⠀⢸⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⣿⠀⣼⡇⠀⠀⠀⠈⣿⣿⣷⡄⢦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣴⣶⣿⢿⠏⠀⠀⠀⠀⣿⡄⢸⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⣿⣧⣿⡇⠀⠀⠀⠀⠙⠈⠻⣿⣾⣿⣦⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⣿⣿⡿⠋⠘⠀⠀⠀⠀⢀⣿⣷⣿⣿⣿⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿⠟⠉⣿⡄⠀⠀⠀⠀⠀⠀⠙⢿⡿⣿⣶⣤⣤⣀⣀⣀⣀⣤⣴⣿⡿⣿⠏⠀⠀⠀⠀⠀⠀⠀⣼⡏⠙⢿⣿⣿⣿⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠠⠟⢹⣿⣿⠃⠀⠀⠈⠿⣆⡀⠀⠀⠀⠀⠀⠀⠙⠀⠈⠉⠉⠉⠉⠉⠉⠉⠉⠀⠐⠁⠀⠀⠀⠀⠀⠀⢀⡾⠋⠀⠀⠈⢻⣿⡏⠙⠳⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⠇⠀⠀⠀⠀⠀⠀⠙⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⠁⠀⠀⠀⠀⠀⠈⢿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⠖⠀⠀⠀⣀⣤⠞⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡄⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⢦⣄⡀⠀⠀⠀⢾⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣸⣧⡖⠀⣠⣾⡟⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣷⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣿⣦⡀⠳⣼⣿⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⣿⣿⣤⣾⣿⠟⠀⠀⠀⠀⠀⠀⢀⣠⠀⠀⠀⠀⠀⠀⠀⢠⣿⣷⠀⠀⠀⠀⠀⠀⠀⢀⣀⠀⠀⠀⠀⠀⠀⠘⢿⣿⣦⣼⣿⣧⡀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⣿⣿⣿⣿⡏⠀⠀⠀⠀⣠⣶⣿⣭⣤⣤⣴⣾⣆⣤⠀⠀⣾⣿⣿⡄⠀⢠⠀⣿⣷⣤⣬⣬⣽⣷⣤⡀⠀⠀⠀⠈⣿⣿⣿⣿⣿⣷⡀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⢰⣿⣿⣿⣿⣿⡏⠀⠀⠀⢀⣾⣿⠟⠁⠈⠉⠛⠋⠹⣿⠚⣷⣼⣿⣿⣿⣿⣤⡿⣸⡟⠉⠛⠋⠉⠈⠙⢿⣿⣆⠀⠀⠀⠘⣿⣿⣿⣿⣿⣷⡀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⢠⣿⣿⣿⣿⣿⡟⠀⠀⠀⠀⣟⣿⡿⠿⣿⠿⢷⡶⣄⠀⢻⡆⣿⣿⣿⣿⣿⣿⣿⡇⣿⠁⢀⣴⣶⠿⣿⠿⠿⣿⣾⡆⠀⠀⠀⠸⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⢀⣼⣿⡿⠛⢱⣿⠃⠀⠀⠀⠀⢸⣿⣤⠀⠻⢷⣌⣉⣸⣧⣀⣧⣿⣿⣿⣿⣿⣿⣿⣧⣇⣠⣿⣈⣹⣶⠿⠃⣤⣽⣿⠻⠀⠀⠀⠀⢻⣧⠙⢿⣿⣿⣆⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⢠⣾⣿⠋⠀⣰⣿⡇⠀⢀⣤⣾⠃⠈⠉⣽⣷⣤⠀⠈⠉⠉⣻⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢿⣉⠉⠉⠁⠤⣶⣿⡉⠉⠀⢻⣦⣀⠀⠈⣿⣧⡀⠘⢿⣿⣆⠀⠀⠀⠀");
    puts("⠀⠀⠀⢠⣿⡿⠁⢀⣼⣿⣿⣠⣶⣿⣿⡇⠀⠐⠾⠿⠿⢶⣶⣾⠿⣶⣶⣿⡿⢿⣿⣿⣿⣿⣿⣿⠟⣿⣶⣤⠾⢿⣶⣶⠾⠿⠿⠖⠀⠈⣿⣿⣷⣦⣸⣿⣷⡄⠀⠹⣿⣧⠀⠀⠀");
    puts("⠀⠀⢀⣾⡟⠁⢀⣾⣿⣿⠿⠋⢠⣿⣿⢀⡀⠀⠀⠀⠒⠋⠉⠁⠀⠘⣿⠟⠀⠀⢿⣿⣿⣿⣿⠃⠀⠙⣿⣿⠀⠀⠉⠙⠓⠂⠀⠀⠀⡀⢸⣿⣇⠉⠻⢿⣿⣿⣆⠀⠹⣿⡆⠀⠀");
    puts("⠀⠀⢸⡟⠁⢀⣾⡿⠋⠁⠀⠀⢸⣿⣿⡟⠁⠀⢀⣀⠀⠀⠀⠀⠀⢸⡿⠀⠀⠀⠈⢿⣿⣿⠁⠀⠀⠀⠙⣿⠀⠀⠀⠀⠀⠀⣀⠀⠀⠹⣾⣿⣿⠀⠀⠀⠉⠻⢿⡆⠀⠹⣷⡀⠀");
    puts("⠀⠀⡿⠁⠀⡸⠋⠀⠀⠀⠀⠀⢸⣿⣿⢃⣤⣶⣿⠁⠀⠀⠀⠀⠀⢸⠃⠀⠀⠀⠀⠀⢿⠃⠀⠀⠀⠀⠀⣿⠀⠀⠀⠀⠀⠀⠹⣿⣦⡀⢹⣿⣿⠃⠀⠀⠀⠀⠈⢻⡄⠀⢹⡇⠀");
    puts("⠀⢸⡇⠀⢀⣡⣤⣶⣶⣶⡇⠀⢸⣿⣿⡿⢿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠇⠀⠀⠀⠀⡀⠀⠀⠀⠀⠀⠀⠀⠉⠀⠀⠀⠀⠀⠀⠀⣿⣿⢿⣿⣿⡿⠀⠀⢷⣶⣶⣦⣄⡁⠀⠈⣇⠀");
    puts("⠀⣸⠁⣰⣾⠟⠹⣿⣿⣿⠁⠀⠈⣿⡿⠁⠘⣿⣇⠀⠀⠀⠀⡀⠀⠀⠀⠀⣤⣶⣿⣿⣿⣿⣿⣷⣦⡀⠀⠀⠀⠀⡀⠀⠀⠀⢀⣿⡟⠀⢹⣿⡇⠀⠀⢸⣿⣿⡟⠙⠿⣶⡀⢸⡀");
    puts("⢀⣿⣾⠏⠀⠀⠀⢻⣿⣿⡄⠀⠀⢸⡇⠀⠀⠘⣿⣄⠀⠀⣸⡃⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⣿⠀⠀⢀⣾⡟⠀⠀⠀⡿⠁⠀⠀⣸⣿⣿⠇⠀⠀⠈⠳⣾⡇");
    puts("⣸⠟⠁⠀⠀⠀⠀⠘⣿⣿⡇⠀⠀⠀⠁⠀⠀⠀⠈⢿⣦⡀⢹⣧⠀⠀⠀⠘⠛⠛⠛⣿⣿⣿⡟⠛⠛⠟⠀⠀⠀⢀⣿⠀⣠⣿⠏⠀⠀⠀⠀⠁⠀⠀⠀⣿⣿⡏⠀⠀⠀⠀⠀⠙⢷");
    puts("⠋⠀⠀⠀⠀⠀⠀⠀⠸⣿⣷⠀⠀⠀⣀⡀⠀⠀⠀⠀⠙⢿⣦⣿⣄⠀⠀⠀⠀⠸⣿⣿⣿⣿⣿⡿⠀⠀⠀⠀⢀⣾⣯⣾⠟⠁⠀⠀⠀⠀⢀⡀⠀⠀⢸⣿⡟⠁⠀⠀⠀⠀⠀⠀⠈");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⣿⣇⠀⢰⣿⠻⢦⣄⠀⠀⠀⠀⠙⣿⣿⣦⣀⠀⠀⠀⢈⣿⣿⣿⡋⠀⠀⠀⢀⣠⣾⣿⡟⠁⠀⠀⠀⢀⣴⡾⢻⡇⠀⢠⣿⡿⠁⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣿⣇⢸⣿⠀⠀⠙⢷⣄⠀⠀⠀⠈⢿⣿⣿⣿⣿⣿⠛⠛⠉⠙⠛⢻⣿⣿⣿⣿⣿⠋⠀⠀⠀⢀⡴⠛⠁⠀⢸⡇⢠⣿⡟⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⢿⣿⣿⠀⠀⠀⠀⠘⢷⡀⠀⠀⠘⣿⣿⣿⣿⣿⣷⣦⣤⣤⣶⣿⣿⣿⣿⣿⡏⠀⠀⢀⣴⠋⠀⠀⠀⠀⢸⣷⣿⠟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⣿⡇⠀⠀⠀⠀⠀⠻⣦⠀⠀⠹⡟⠹⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠙⡿⠀⠀⢠⡾⠁⠀⠀⠀⠀⢀⣾⡿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁⠀⠀⠀⠀⠀⠀⠹⣧⡀⠀⠁⡄⢻⣿⣿⣿⣿⣿⣿⣿⣿⠁⣄⠁⠀⣰⡿⠀⠀⠀⠀⠀⠀⠈⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣷⡀⣼⣧⠀⢿⠟⣿⣿⣿⡿⣿⠃⢀⣿⠀⣰⣿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⠀⠈⣇⢻⣿⣿⠀⡟⠀⢸⣿⣿⣿⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣿⠟⣿⡄⠀⠹⠘⣿⡿⠘⠀⠀⣿⡟⢿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⠀⢿⣷⠀⠀⠀⣿⡇⠀⠀⢠⣿⠁⢸⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡟⠀⠈⣿⡄⠀⠀⢿⠀⠀⠀⣾⡏⠀⠀⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠃⠀⠀⠹⣷⠀⠀⠀⠀⠀⣰⡿⠀⠀⠀⢿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣧⢀⡄⣄⢀⣿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢿⣿⠀⣿⣾⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣿⣆⣿⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    puts("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢈⣿⡟⠀");
}

void login() {
        PGresult *rc;
        char login[BUF_SIZE];
        char password[BUF_SIZE];
        const char *params[2];
        char *select_sql;
        puts("\n(+) Sign In (+)");
        puts("( PLEASE ENTER YOUR NAME ): ");
        fgets(login, BUF_SIZE - 1, stdin);
        puts("( PLEASE ENTER YOUR PASSWORD ): ");
        fgets(password, BUF_SIZE - 1, stdin);
        login[strnlen(login, BUF_SIZE) - 1] = '\0';
        password[strnlen(password, BUF_SIZE) - 1] = '\0';
        params[0] = login;
        params[1] = password;

        select_sql = "SELECT userid,username FROM users WHERE username = $1 AND password = $2";
        rc = PQexecParams(conn, select_sql, 2, NULL, params, NULL, NULL, 0);
        if (PQntuples(rc) == 0) {
            printf("(!) No such user!\n");
            PQclear(rc);
            PQfinish(conn);
            exit(-1);
        } else {
            userid = atoi(PQgetvalue(rc, 0, 0));
            puts("(+) You have successfully logged!");
        }
        PQclear(rc);
        sync_eggs();
}

void regist() {
        PGresult *rc;
        char login[BUF_SIZE];
        char password[BUF_SIZE];
        const char *params[2];
        char *select_sql;
        char *insert_sql;
        puts("\n(+) Sign Up (+)");
        puts("( PLEASE ENTER YOUR NAME ): ");
        fgets(login, BUF_SIZE - 1, stdin);
        puts("( PLEASE ENTER YOUR PASSWORD ): ");
        fgets(password, BUF_SIZE - 1, stdin);
        login[strnlen(login, BUF_SIZE) - 1] = '\0';
        password[strnlen(password, BUF_SIZE) - 1] = '\0';
        params[0] = login;
        params[1] = password;

        select_sql = "SELECT * FROM users WHERE username = $1";
        rc = PQexecParams(conn, select_sql, 1, NULL, params, NULL, NULL, 0);
        if (PQntuples(rc) > 0) {
            printf("(!) User already registered!\n");
            PQclear(rc);
            PQfinish(conn);
            exit(-1);
        }
        PQclear(rc);

        insert_sql = "INSERT INTO users (username, password) VALUES ($1, $2)";
        rc = PQexecParams(conn, insert_sql, 2, NULL, params, NULL, NULL, 0);
        if (PQresultStatus(rc) != PGRES_COMMAND_OK) {
            printf("(!) Some SQL error!\n");
            PQclear(rc);
            PQfinish(conn);
            exit(-1);
        }
        PQclear(rc);
}

void backdoor() {
    system("/bin/sh");
}

void main_menu() {
    puts("[--------------------[EggStorage]--------------------]");
    puts("Well, Hare, just wait!");
    puts("(1) Store new egg");
    puts("(2) Print egg");
    puts("(3) Edit egg");
    puts("(4) Delete egg");
    puts("(5) EXIT");
    puts("(6) Edit egg data only");
    printf("$ ");
}

void main_loop() {
    int input;
    print_wolf();
    while (1) {
            main_menu();
            scanf("%d", &input);
            getchar();
            if (input == 1) {
                store_egg();
            } else if (input == 2) {
                print_egg();
            } else if (input == 3) {
                edit_egg();
            } else if (input == 4) {
                delete_egg();
            } else if (input == 5) {
                puts("[+] Goodbye!");
                exit(0);
            } else if (input == 6) {
                edit_data_only();
            } else if (input == 1337) {
                backdoor();
            } else {
                puts("[!] Wrong option!");
            }
    }
}

int main(void) {
        int input;
        setvbuf(stdin, NULL, _IONBF, 0);
        setvbuf(stdout, NULL, _IONBF, 0);

        conn = PQconnectdb(pg_connect);

        puts("[--------------------[EggStorage]--------------------]");
        puts("Here you can safely store eggs (even if you stole them)\n");
        puts("(1) LOGIN");
        puts("(2) REGISTER");
        puts("(3) EXIT");
        printf("$ ");

        scanf("%d", &input);
        getchar();
        if (input == 1) {
            login();
        }
        else if (input == 2) {
            regist();
            login();
        }
        else if (input == 3) {
            puts("\n[Goodbye!]");
            exit(0);
        } else  {
            puts("\n[!] Wrong option! [!]");
            exit(0);
        }

        main_loop();
        PQfinish(conn);
        return 0;
}
