#include "storage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sqlite3.h>

static int ensure_table(sqlite3 *db)
{
    const char *sql =
        "CREATE TABLE IF NOT EXISTS contacts ("
        "id INTEGER PRIMARY KEY,"
        "name TEXT NOT NULL,"
        "phone TEXT NOT NULL,"
        "email TEXT NOT NULL,"
        "address TEXT,"
        "note TEXT"
        ");";
    return sqlite3_exec(db, sql, NULL, NULL, NULL);
}

int contacts_db_init(const char *db_path)
{
    sqlite3 *db;
    int rc;

    if (db_path == NULL) {
        return -1;
    }

    rc = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "sqlite3_open failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    rc = ensure_table(db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "ensure_table failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    sqlite3_close(db);
    return 0;
}

int contacts_db_load(const char *db_path, Contact contacts[], int *count, int capacity)
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc;
    int index = 0;

    if (db_path == NULL || contacts == NULL || count == NULL) {
        return -1;
    }

    *count = 0;

    rc = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "sqlite3_open failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    const char *sql =
        "SELECT id, name, phone, email, address, note FROM contacts ORDER BY id ASC;";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "prepare failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        Contact c;
        memset(&c, 0, sizeof(c));

        c.id = sqlite3_column_int(stmt, 0);
        snprintf(c.name, sizeof(c.name), "%s", (const char *)sqlite3_column_text(stmt, 1));
        snprintf(c.phone, sizeof(c.phone), "%s", (const char *)sqlite3_column_text(stmt, 2));
        snprintf(c.email, sizeof(c.email), "%s", (const char *)sqlite3_column_text(stmt, 3));
        snprintf(c.address, sizeof(c.address), "%s", (const char *)sqlite3_column_text(stmt, 4));
        snprintf(c.note, sizeof(c.note), "%s", (const char *)sqlite3_column_text(stmt, 5));

        if (index >= capacity) {
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return -1;
        }

        contacts[index++] = c;
    }

    *count = index;

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        return -1;
    }

    return 0;
}

int contacts_db_save(const char *db_path, const Contact contacts[], int count)
{
    sqlite3 *db;
    int rc;

    if (db_path == NULL) {
        return -1;
    }

    rc = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "sqlite3_open failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    rc = ensure_table(db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "ensure_table failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);

    sqlite3_exec(db, "DELETE FROM contacts;", NULL, NULL, NULL);

    sqlite3_stmt *stmt = NULL;
    const char *sql =
        "INSERT INTO contacts (id, name, phone, email, address, note) "
        "VALUES (?, ?, ?, ?, ?, ?);";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "prepare failed: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        sqlite3_close(db);
        return -1;
    }

    for (int i = 0; i < count; i++) {
        sqlite3_bind_int(stmt, 1, contacts[i].id);
        sqlite3_bind_text(stmt, 2, contacts[i].name, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, contacts[i].phone, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, contacts[i].email, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, contacts[i].address, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, contacts[i].note, -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            fprintf(stderr, "insert failed: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
            sqlite3_close(db);
            return -1;
        }

        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);

    sqlite3_close(db);
    return 0;
}

int contacts_db_add(const char *db_path, const Contact *contact)
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc;

    if (db_path == NULL || contact == NULL) {
        return -1;
    }

    rc = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "sqlite3_open failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    rc = ensure_table(db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "ensure_table failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    const char *sql =
        "INSERT INTO contacts (id, name, phone, email, address, note) "
        "VALUES (?, ?, ?, ?, ?, ?);";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "prepare failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    sqlite3_bind_int(stmt, 1, contact->id);
    sqlite3_bind_text(stmt, 2, contact->name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, contact->phone, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, contact->email, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, contact->address, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, contact->note, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return (rc == SQLITE_DONE) ? 0 : -1;
}

int contacts_db_update(const char *db_path, int id, const Contact *updated)
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc;

    if (db_path == NULL || updated == NULL) {
        return -1;
    }

    rc = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "sqlite3_open failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    const char *sql =
        "UPDATE contacts SET name=?, phone=?, email=?, address=?, note=? WHERE id=?;";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "prepare failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    sqlite3_bind_text(stmt, 1, updated->name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, updated->phone, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, updated->email, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, updated->address, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, updated->note, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, id);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return (rc == SQLITE_DONE) ? 0 : -1;
}

int contacts_db_delete(const char *db_path, int id)
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc;

    if (db_path == NULL) {
        return -1;
    }

    rc = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "sqlite3_open failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    const char *sql = "DELETE FROM contacts WHERE id=?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "prepare failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    sqlite3_bind_int(stmt, 1, id);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return (rc == SQLITE_DONE) ? 0 : -1;
}