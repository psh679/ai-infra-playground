#ifndef CONTACT_H
#define CONTACT_H

#define NAME_LEN 100
#define PHONE_LEN 30
#define EMAIL_LEN 100
#define ADDRESS_LEN 200
#define NOTE_LEN 300

typedef struct {
    int id;
    char name[NAME_LEN];
    char phone[PHONE_LEN];
    char email[EMAIL_LEN];
    char address[ADDRESS_LEN];
    char note[NOTE_LEN];
} Contact;

void contact_print(const Contact *contact);
int contact_add(Contact contacts[], int *count, int capacity,
                const Contact *contact);
void contact_list(const Contact contacts[], int count);
Contact *contact_find_by_id(Contact contacts[], int count, int id);
void contact_input(Contact *contact);
int get_next_id(const Contact contacts[], int count);
int contact_delete(Contact contacts[], int *count, int id);
int contact_update_by_id(Contact contacts[], int count, int id,
                        const Contact *new_contact);
int contact_search_by_name(const Contact contacts[], int count, const char *keyword);
int contact_search_by_phone(const Contact contacts[], int count, const char *keyword);
void contact_sort_by_name(Contact contacts[], int count);

#endif