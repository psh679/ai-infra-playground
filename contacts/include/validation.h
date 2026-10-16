#ifndef VALIDATION_H
#define VALIDATION_H
#include "contact.h"
int validate_name(const char *name);
int validate_phone(const char *phone);
int validate_email(const char *email);
int validate_address(const char *address);
int validate_note(const char *note);
int validate_contact(const Contact *contact);
#endif