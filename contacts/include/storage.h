#ifndef STORAGE_H
#define STORAGE_H

#include "contact.h"

int contacts_db_init(const char *db_path);
int contacts_db_load(const char *db_path, Contact contacts[], int *count, int capacity);
int contacts_db_save(const char *db_path, const Contact contacts[], int count);
int contacts_db_add(const char *db_path, const Contact *contact);
int contacts_db_update(const char *db_path, int id, const Contact *updated);
int contacts_db_delete(const char *db_path, int id);

#endif