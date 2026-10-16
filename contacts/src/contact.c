#include "contact.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char to_lower_char(char c)
{
    if (c >= 'A' && c <= 'Z') {
        return (char)(c - 'A' + 'a');
    }
    return c;
}

static int contains_ignore_case(const char *haystack, const char *needle)
{
    size_t i, j;
    size_t hay_len, needle_len;

    if (haystack == NULL || needle == NULL) {
        return 0;
    }

    hay_len = strlen(haystack);
    needle_len = strlen(needle);

    if (needle_len == 0) {
        return 1;
    }
    if (needle_len > hay_len) {
        return 0;
    }

    for (i = 0; i <= hay_len - needle_len; i++) {
        int match = 1;
        for (j = 0; j < needle_len; j++) {
            if (to_lower_char(haystack[i + j]) != to_lower_char(needle[j])) {
                match = 0;
                break;
            }
        }
        if (match) {
            return 1;
        }
    }

    return 0;
}

static void trim(char *s)
{
    char *start;
    size_t len;

    if (s == NULL) {
        return;
    }

    start = s;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    if (start != s) {
        memmove(s, start, strlen(start) + 1);
    }

    len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) {
        s[len - 1] = '\0';
        len--;
    }
}

static char *safe_fgets(char *buf, size_t size)
{
    if (fgets(buf, size, stdin) == NULL) {
        return NULL;
    }

    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    }

    return buf;
}

static int compare_by_name(const void *a, const void *b)
{
    const Contact *ca = (const Contact *)a;
    const Contact *cb = (const Contact *)b;
    return strcmp(ca->name, cb->name);
}

void contact_print(const Contact *contact)
{
    if (contact == NULL) {
        return;
    }

    printf("ID: %d\n", contact->id);
    printf("Name: %s\n", contact->name);
    printf("Phone: %s\n", contact->phone);
    printf("Email: %s\n", contact->email);
    printf("Address: %s\n", contact->address);
    printf("Note: %s\n", contact->note);
    printf("-----\n");
}

int contact_add(Contact contacts[], int *count, int capacity,
                const Contact *contact)
{
    if (contacts == NULL || count == NULL || contact == NULL) {
        return -1;
    }

    if (*count >= capacity) {
        return -1;
    }

    contacts[*count] = *contact;
    (*count)++;
    return 0;
}

void contact_list(const Contact contacts[], int count)
{
    if (contacts == NULL) {
        return;
    }

    if (count <= 0) {
        printf("No contacts.\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        contact_print(&contacts[i]);
    }
}

Contact *contact_find_by_id(Contact contacts[], int count, int id)
{
    if (contacts == NULL) {
        return NULL;
    }

    for (int i = 0; i < count; i++) {
        if (contacts[i].id == id) {
            return &contacts[i];
        }
    }

    return NULL;
}

void contact_input(Contact *contact)
{
    char buf[512];

    if (contact == NULL) {
        return;
    }

    printf("Name: ");
    if (safe_fgets(buf, sizeof(buf)) == NULL) {
        buf[0] = '\0';
    }
    trim(buf);
    snprintf(contact->name, sizeof(contact->name), "%s", buf);

    printf("Phone: ");
    if (safe_fgets(buf, sizeof(buf)) == NULL) {
        buf[0] = '\0';
    }
    trim(buf);
    snprintf(contact->phone, sizeof(contact->phone), "%s", buf);

    printf("Email: ");
    if (safe_fgets(buf, sizeof(buf)) == NULL) {
        buf[0] = '\0';
    }
    trim(buf);
    snprintf(contact->email, sizeof(contact->email), "%s", buf);

    printf("Address: ");
    if (safe_fgets(buf, sizeof(buf)) == NULL) {
        buf[0] = '\0';
    }
    trim(buf);
    snprintf(contact->address, sizeof(contact->address), "%s", buf);

    printf("Note: ");
    if (safe_fgets(buf, sizeof(buf)) == NULL) {
        buf[0] = '\0';
    }
    trim(buf);
    snprintf(contact->note, sizeof(contact->note), "%s", buf);
}

int get_next_id(const Contact contacts[], int count)
{
    int max_id = 0;

    if (contacts == NULL || count <= 0) {
        return 1;
    }

    for (int i = 0; i < count; i++) {
        if (contacts[i].id > max_id) {
            max_id = contacts[i].id;
        }
    }

    return max_id + 1;
}

int contact_delete(Contact contacts[], int *count, int id)
{
    int i, found = 0;

    if (contacts == NULL || count == NULL) {
        return -1;
    }

    for (i = 0; i < *count; i++) {
        if (contacts[i].id == id) {
            found = 1;
            break;
        }
    }

    if (!found) {
        return -1;
    }

    for (i = i; i < *count - 1; i++) {
        contacts[i] = contacts[i + 1];
    }

    (*count)--;
    return 0;
}

int contact_update_by_id(Contact contacts[], int count, int id, const Contact *new_contact)
{
    Contact *target;

    if (contacts == NULL || new_contact == NULL) {
        return -1;
    }

    target = contact_find_by_id(contacts, count, id);
    if (target == NULL) {
        return -1;
    }

    *target = *new_contact;
    target->id = id;
    return 0;
}

int contact_search_by_name(const Contact contacts[], int count, const char *keyword)
{
    int matches = 0;

    if (contacts == NULL || keyword == NULL) {
        return 0;
    }

    for (int i = 0; i < count; i++) {
        if (contains_ignore_case(contacts[i].name, keyword)) {
            printf("MATCH #%d:\n", matches + 1);
            contact_print(&contacts[i]);
            matches++;
        }
    }

    if (matches == 0) {
        printf("No matching contacts found for keyword: %s\n", keyword);
    }

    return matches;
}

int contact_search_by_phone(const Contact contacts[], int count, const char *keyword)
{
    int matches = 0;

    if (contacts == NULL || keyword == NULL) {
        return 0;
    }

    for (int i = 0; i < count; i++) {
        if (contains_ignore_case(contacts[i].phone, keyword)) {
            printf("MATCH #%d:\n", matches + 1);
            contact_print(&contacts[i]);
            matches++;
        }
    }

    if (matches == 0) {
        printf("No matching contacts found for phone keyword: %s\n", keyword);
    }

    return matches;
}

void contact_sort_by_name(Contact contacts[], int count)
{
    if (contacts == NULL || count <= 1) {
        return;
    }

    qsort(contacts, (size_t)count, sizeof(Contact), compare_by_name);
}
    