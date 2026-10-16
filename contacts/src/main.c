#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "contact.h"
#include "storage.h"
#include "validation.h"

#define MAX_CONTACTS 100
#define DB_PATH "data/contacts.db"

static int read_choice(void)
{
    char buf[64];
    char *p;

    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        return -1;
    }

    p = buf;
    while (*p && isspace((unsigned char)*p)) {
        p++;
    }

    return atoi(p);
}

static void print_menu(void)
{
    printf("\n===== Contact Manager (SQLite) =====\n");
    printf("1. Add contact\n");
    printf("2. List contacts\n");
    printf("3. Save to DB\n");
    printf("4. Reload from DB\n");
    printf("5. Update contact\n");
    printf("6. Delete contact\n");
    printf("7. Search by name\n");
    printf("8. Search by phone\n");
    printf("9. Sort by name\n");
    printf("0. Exit\n");
    printf("==================================\n");
    printf("Choose: ");
}

static void input_contact_for_update(Contact *c)
{
    char buf[512];

    if (!c) return;

    printf("New Name: ");
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        buf[0] = '\0';
    }
    buf[strcspn(buf, "\n")] = '\0';
    snprintf(c->name, sizeof(c->name), "%s", buf);

    printf("New Phone: ");
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        buf[0] = '\0';
    }
    buf[strcspn(buf, "\n")] = '\0';
    snprintf(c->phone, sizeof(c->phone), "%s", buf);

    printf("New Email: ");
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        buf[0] = '\0';
    }
    buf[strcspn(buf, "\n")] = '\0';
    snprintf(c->email, sizeof(c->email), "%s", buf);

    printf("New Address: ");
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        buf[0] = '\0';
    }
    buf[strcspn(buf, "\n")] = '\0';
    snprintf(c->address, sizeof(c->address), "%s", buf);

    printf("New Note: ");
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        buf[0] = '\0';
    }
    buf[strcspn(buf, "\n")] = '\0';
    snprintf(c->note, sizeof(c->note), "%s", buf);
}

static int validate_and_add(Contact contacts[], int *count, const Contact *c)
{
    if (!c || !validate_contact(c)) {
        printf("Invalid contact data.\n");
        printf("Rules: name non-empty, phone digits/+-() . allowed, email must contain @ and .\n");
        return -1;
    }

    if (contact_add(contacts, count, MAX_CONTACTS, c) != 0) {
        printf("Contact list is full.\n");
        return -1;
    }

    return 0;
}

static int validate_and_update(Contact contacts[], int count, int id, const Contact *updated)
{
    if (!updated || !validate_contact(updated)) {
        printf("Invalid updated contact data.\n");
        printf("Rules: name non-empty, phone digits/+-() . allowed, email must contain @ and .\n");
        return -1;
    }

    if (contact_update_by_id(contacts, count, id, updated) != 0) {
        printf("Contact ID %d not found.\n", id);
        return -1;
    }

    return 0;
}

static void print_help(void)
{
    printf("Available commands:\n");
    printf("  add       Add one contact interactively\n");
    printf("  list      List all contacts\n");
    printf("  delete    Delete a contact by ID\n");
    printf("  search    Search by name\n");
    printf("  phone     Search by phone\n");
    printf("  update    Update a contact by ID\n");
    printf("  help      Show help\n");
}

static int cli_add(Contact contacts[], int *count)
{
    Contact c;

    memset(&c, 0, sizeof(c));
    c.id = get_next_id(contacts, *count);

    printf("Name: ");
    char name[NAME_LEN];
    if (fgets(name, sizeof(name), stdin) == NULL) {
        return -1;
    }
    name[strcspn(name, "\n")] = '\0';
    snprintf(c.name, sizeof(c.name), "%s", name);

    printf("Phone: ");
    char phone[PHONE_LEN];
    if (fgets(phone, sizeof(phone), stdin) == NULL) {
        return -1;
    }
    phone[strcspn(phone, "\n")] = '\0';
    snprintf(c.phone, sizeof(c.phone), "%s", phone);

    printf("Email: ");
    char email[EMAIL_LEN];
    if (fgets(email, sizeof(email), stdin) == NULL) {
        return -1;
    }
    email[strcspn(email, "\n")] = '\0';
    snprintf(c.email, sizeof(c.email), "%s", email);

    printf("Address: ");
    char address[ADDRESS_LEN];
    if (fgets(address, sizeof(address), stdin) == NULL) {
        return -1;
    }
    address[strcspn(address, "\n")] = '\0';
    snprintf(c.address, sizeof(c.address), "%s", address);

    printf("Note: ");
    char note[NOTE_LEN];
    if (fgets(note, sizeof(note), stdin) == NULL) {
        return -1;
    }
    note[strcspn(note, "\n")] = '\0';
    snprintf(c.note, sizeof(c.note), "%s", note);

    if (validate_and_add(contacts, count, &c) != 0) {
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    Contact contacts[MAX_CONTACTS];
    int count = 0;
    int choice = 0;

    /* Create DB path if folder exists */
    const char *db_path = DB_PATH;

    /* initialize DB */
    if (contacts_db_init(db_path) != 0) {
        fprintf(stderr, "Failed to initialize database: %s\n", db_path);
        return 1;
    }

    /* CLI mode */
    if (argc > 1) {
        if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            print_help();
            return 0;
        }

        if (strcmp(argv[1], "add") == 0) {
            if (cli_add(contacts, &count) != 0) {
                fprintf(stderr, "Failed to add contact.\n");
                return 1;
            }

            if (contacts_db_save(db_path, contacts, count) != 0) {
                fprintf(stderr, "Failed to save in database.\n");
                return 1;
            }

            printf("Contact saved to %s\n", db_path);
            return 0;
        }

        if (strcmp(argv[1], "list") == 0) {
            if (contacts_db_load(db_path, contacts, &count, MAX_CONTACTS) != 0) {
                fprintf(stderr, "Failed to load database.\n");
                return 1;
            }

            printf("All contacts (%d):\n", count);
            contact_list(contacts, count);
            return 0;
        }

        if (strcmp(argv[1], "search") == 0) {
            if (argc < 3) {
                fprintf(stderr, "Usage: %s search <keyword>\n", argv[0]);
                return 1;
            }

            if (contacts_db_load(db_path, contacts, &count, MAX_CONTACTS) != 0) {
                fprintf(stderr, "Failed to load database.\n");
                return 1;
            }

            contact_search_by_name(contacts, count, argv[2]);
            return 0;
        }

        if (strcmp(argv[1], "phone") == 0) {
            if (argc < 3) {
                fprintf(stderr, "Usage: %s phone <keyword>\n", argv[0]);
                return 1;
            }

            if (contacts_db_load(db_path, contacts, &count, MAX_CONTACTS) != 0) {
                fprintf(stderr, "Failed to load database.\n");
                return 1;
            }

            contact_search_by_phone(contacts, count, argv[2]);
            return 0;
        }

        if (strcmp(argv[1], "delete") == 0) {
            int id;

            if (argc < 3) {
                fprintf(stderr, "Usage: %s delete <id>\n", argv[0]);
                return 1;
            }

            id = atoi(argv[2]);

            if (contacts_db_load(db_path, contacts, &count, MAX_CONTACTS) != 0) {
                fprintf(stderr, "Failed to load database.\n");
                return 1;
            }

            if (contact_delete(contacts, &count, id) != 0) {
                fprintf(stderr, "Contact ID %d not found.\n", id);
                return 1;
            }

            if (contacts_db_save(db_path, contacts, count) != 0) {
                fprintf(stderr, "Failed to save database after delete.\n");
                return 1;
            }

            printf("Deleted contact ID %d\n", id);
            return 0;
        }

        if (strcmp(argv[1], "update") == 0) {
            int id;
            Contact updated;

            if (argc < 3) {
                fprintf(stderr, "Usage: %s update <id>\n", argv[0]);
                return 1;
            }

            id = atoi(argv[2]);

            if (contacts_db_load(db_path, contacts, &count, MAX_CONTACTS) != 0) {
                fprintf(stderr, "Failed to load database.\n");
                return 1;
            }

            if (contact_find_by_id(contacts, count, id) == NULL) {
                fprintf(stderr, "Contact ID %d not found.\n", id);
                return 1;
            }

            memset(&updated, 0, sizeof(updated));
            updated.id = id;

            printf("Please enter new values:\n");
            input_contact_for_update(&updated);

            if (validate_and_update(contacts, count, id, &updated) != 0) {
                fprintf(stderr, "Update failed.\n");
                return 1;
            }

            if (contacts_db_save(db_path, contacts, count) != 0) {
                fprintf(stderr, "Failed to save updated data.\n");
                return 1;
            }

            printf("Updated contact ID %d\n", id);
            return 0;
        }

        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        print_help();
        return 1;
    }

    /* interactive menu */
    for (;;) {
        print_menu();

        choice = read_choice();
        if (choice < 0) {
            printf("Input error. Exiting.\n");
            break;
        }

        if (choice == 0) {
            printf("Saving to SQLite DB before exit...\n");
            if (contacts_db_save(db_path, contacts, count) != 0) {
                fprintf(stderr, "Failed to save contacts to %s\n", db_path);
            } else {
                printf("Saved %d contacts to %s\n", count, db_path);
            }
            printf("Bye.\n");
            break;
        }

        if (choice == 1) {
            Contact c;

            if (count >= MAX_CONTACTS) {
                printf("Contact list is full.\n");
                continue;
            }

            memset(&c, 0, sizeof(c));
            c.id = get_next_id(contacts, count);

            printf("Adding contact with id=%d\n", c.id);
            contact_input(&c);

            if (validate_and_add(contacts, &count, &c) == 0) {
                printf("Contact added successfully.\n");
                if (contacts_db_save(db_path, contacts, count) != 0) {
                    fprintf(stderr, "Failed to save contacts to %s\n", db_path);
                } else {
                    printf("Saved to %s\n", db_path);
                }
            }
        } else if (choice == 2) {
            if (contacts_db_load(db_path, contacts, &count, MAX_CONTACTS) != 0) {
                fprintf(stderr, "Failed to load contacts from %s\n", db_path);
                continue;
            }
            printf("All contacts (%d):\n", count);
            contact_list(contacts, count);
        } else if (choice == 3) {
            if (contacts_db_save(db_path, contacts, count) != 0) {
                fprintf(stderr, "Failed to save contacts to %s\n", db_path);
            } else {
                printf("Saved %d contacts to %s\n", count, db_path);
            }
        } else if (choice == 4) {
            int old_count = count;
            if (contacts_db_load(db_path, contacts, &count, MAX_CONTACTS) != 0) {
                fprintf(stderr, "Failed to reload %s\n", db_path);
            } else {
                printf("Reloaded contacts: %d -> %d\n", old_count, count);
            }
        } else if (choice == 5) {
            int id;
            Contact updated;

            printf("Enter contact ID to update: ");
            scanf("%d", &id);
            getchar();

            if (contact_find_by_id(contacts, count, id) == NULL) {
                printf("Contact ID %d not found.\n", id);
                continue;
            }

            memset(&updated, 0, sizeof(updated));
            updated.id = id;

            printf("Please enter new values:\n");
            input_contact_for_update(&updated);

            if (validate_and_update(contacts, count, id, &updated) == 0) {
                printf("Contact ID %d updated.\n", id);
                if (contacts_db_save(db_path, contacts, count) != 0) {
                    fprintf(stderr, "Failed to save contacts to %s\n", db_path);
                } else {
                    printf("Saved to %s\n", db_path);
                }
            }
        } else if (choice == 6) {
            int id;
            printf("Enter contact ID to delete: ");
            scanf("%d", &id);
            getchar();

            if (contact_delete(contacts, &count, id) != 0) {
                printf("Contact ID %d not found.\n", id);
            } else {
                printf("Contact ID %d deleted.\n", id);
                if (contacts_db_save(db_path, contacts, count) != 0) {
                    fprintf(stderr, "Failed to save contacts to %s\n", db_path);
                } else {
                    printf("Saved to %s\n", db_path);
                }
            }
        } else if (choice == 7) {
            char keyword[NAME_LEN];
            printf("Enter name keyword to search: ");
            if (fgets(keyword, sizeof(keyword), stdin) == NULL) {
                keyword[0] = '\0';
            }
            keyword[strcspn(keyword, "\n")] = '\0';

            if (contacts_db_load(db_path, contacts, &count, MAX_CONTACTS) != 0) {
                fprintf(stderr, "Failed to load contacts from %s\n", db_path);
                continue;
            }

            contact_search_by_name(contacts, count, keyword);
        } else if (choice == 8) {
            char keyword[PHONE_LEN];
            printf("Enter phone keyword to search: ");
            if (fgets(keyword, sizeof(keyword), stdin) == NULL) {
                keyword[0] = '\0';
            }
            keyword[strcspn(keyword, "\n")] = '\0';

            if (contacts_db_load(db_path, contacts, &count, MAX_CONTACTS) != 0) {
                fprintf(stderr, "Failed to load contacts from %s\n", db_path);
                continue;
            }

            contact_search_by_phone(contacts, count, keyword);
        } else if (choice == 9) {
            if (contacts_db_load(db_path, contacts, &count, MAX_CONTACTS) != 0) {
                fprintf(stderr, "Failed to load contacts from %s\n", db_path);
                continue;
            }

            contact_sort_by_name(contacts, count);
            printf("Contacts sorted by name:\n");
            contact_list(contacts, count);

            if (contacts_db_save(db_path, contacts, count) != 0) {
                fprintf(stderr, "Failed to save sorted contacts to %s\n", db_path);
            } else {
                printf("Saved sorted contacts to %s\n", db_path);
            }
        } else {
            printf("Unknown option: %d\n", choice);
        }
    }

    return 0;
}