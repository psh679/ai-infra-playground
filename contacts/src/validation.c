#include "validation.h"

#include <ctype.h>
#include <string.h>
#include <stdio.h>

static void trim_in_place(char *s)
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

static int is_valid_phone_char(char ch)
{
    return isdigit((unsigned char)ch) ||
           ch == '+' || ch == '-' ||
           ch == '(' || ch == ')' ||
           ch == ' ' || ch == '.';
}

int validate_name(const char *name)
{
    char temp[NAME_LEN];
    size_t i;
    int has_alpha = 0;

    if (name == NULL) {
        return 0;
    }

    snprintf(temp, sizeof(temp), "%s", name);
    trim_in_place(temp);

    if (temp[0] == '\0') {
        return 0;
    }

    for (i = 0; temp[i] != '\0'; i++) {
        if (isalpha((unsigned char)temp[i])) {
            has_alpha = 1;
        }
    }

    return has_alpha ? 1 : 0;
}

int validate_phone(const char *phone)
{
    size_t i;
    char temp[PHONE_LEN];

    if (phone == NULL) {
        return 0;
    }

    snprintf(temp, sizeof(temp), "%s", phone);
    trim_in_place(temp);

    if (temp[0] == '\0') {
        return 0;
    }

    for (i = 0; temp[i] != '\0'; i++) {
        if (!is_valid_phone_char(temp[i])) {
            return 0;
        }
    }

    return 1;
}

int validate_email(const char *email)
{
    char temp[EMAIL_LEN];
    size_t at_count = 0;
    size_t dot_count = 0;
    size_t i;

    if (email == NULL) {
        return 0;
    }

    snprintf(temp, sizeof(temp), "%s", email);
    trim_in_place(temp);

    if (temp[0] == '\0') {
        return 0;
    }

    for (i = 0; temp[i] != '\0'; i++) {
        if (temp[i] == '@') {
            at_count++;
        }
        if (temp[i] == '.') {
            dot_count++;
        }
    }

    if (at_count != 1) {
        return 0;
    }

    if (dot_count == 0) {
        return 0;
    }

    /* simple check: ensure '@' appears before final dot */
    if (strchr(temp, '@') == NULL) {
        return 0;
    }

    if (strchr(temp, '.') == NULL) {
        return 0;
    }

    return 1;
}

int validate_address(const char *address)
{
    char temp[ADDRESS_LEN];

    if (address == NULL) {
        return 0;
    }

    snprintf(temp, sizeof(temp), "%s", address);
    trim_in_place(temp);

    return 1;  /* allow empty address */
}

int validate_note(const char *note)
{
    char temp[NOTE_LEN];

    if (note == NULL) {
        return 0;
    }

    snprintf(temp, sizeof(temp), "%s", note);
    trim_in_place(temp);

    return 1;  /* allow empty note */
}

int validate_contact(const Contact *contact)
{
    if (contact == NULL) {
        return 0;
    }

    if (!validate_name(contact->name)) {
        return 0;
    }

    if (!validate_phone(contact->phone)) {
        return 0;
    }

    if (!validate_email(contact->email)) {
        return 0;
    }

    if (!validate_address(contact->address)) {
        return 0;
    }

    if (!validate_note(contact->note)) {
        return 0;
    }

    return 1;
}