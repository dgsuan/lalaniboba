#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <limits.h>

#define NUM_CURRENCIES 5
#define NUM_WALLETS (NUM_CURRENCIES + 1)
#define DATA_FILE "account_data.txt"
#define HISTORY_FILE "transactions.txt"
#define PIN_LENGTH 4
#define BIRTHDAY_LENGTH 10
#define MIN_AGE 18
#define MIN_CONTACT_DIGITS 10
#define MAX_CONTACT_DIGITS 15
#define MAX_EMAIL_LENGTH 100
#define DATA_FORMAT_VERSION 3
#define PERSONAL_INFO_VERSION 2
#define ADMIN_NAME "admin"
#define ADMIN_PIN "2580"
#define ADMIN_LOGIN -2
#define MAX_BALANCE 999999999999.99

struct Account {
    int accountNumber;
    char name[100];
    char pin[PIN_LENGTH + 1];
    double wallets[NUM_WALLETS];
    char birthday[BIRTHDAY_LENGTH + 1];
    char contactNumber[MAX_CONTACT_DIGITS + 2];
    char email[MAX_EMAIL_LENGTH + 1];
};

struct Node {
    struct Account data;
    struct Node *next;
};

struct ExchangeRate {
    char code[4];
    char name[40];
    double rate;
};

int readLineFrom(FILE *fp, char *buf, int size);
void readLine(char *buf, int size);
int parseInt(char *text, int *value);
int readInt(int *value);
int readChoice();
int askYesNo();
void showLoginMenu();
void showMainMenu();
void showAdminMenu();
int isAdminName(char *name);
int isValidPin(char *pin);
int isWeakPin(char *pin);
int isLeapYear(int year);
int isValidBirthday(char *birthday);
int getAge(char *birthday);
int isValidContactNumber(char *contactNumber);
int isValidEmail(char *email);
int promptNewPin(char pin[]);
int promptBirthday(char birthday[]);
int promptContactNumber(char contactNumber[]);
int promptEmail(char email[]);
struct Node *findAccountByName(struct Node *head, char *name);
struct Node *findAccountByNumber(struct Node *head, int accountNumber);
int registerNewAccount(struct Node **head, int *numAccounts, int *nextAccountNumber);
int loginAccount(struct Node *head, struct Node **activeAccount);
void freeAccountList(struct Node *head);
char *getWalletCode(struct ExchangeRate rates[], int wallet);
double roundToCentavos(double value);
void initializeExchangeRates(struct ExchangeRate rates[]);
void saveDataToFile(struct Node *head, int numAccounts, int nextAccountNumber, struct ExchangeRate rates[]);
void loadDataFromFile(struct Node **head, int *numAccounts, int *nextAccountNumber, struct ExchangeRate rates[]);
void showComingSoon(char *feature);

int main() {
    struct Node *head;
    struct Node *activeAccount;
    struct ExchangeRate rates[NUM_CURRENCIES];
    int numAccounts;
    int nextAccountNumber;
    int loginChoice;
    int sessionChoice;
    int loginResult;
    int loggedIn;

    head = NULL;
    numAccounts = 0;
    nextAccountNumber = 1;
    initializeExchangeRates(rates);

    loadDataFromFile(&head, &numAccounts, &nextAccountNumber, rates);

    while (1) {
        showLoginMenu();
        printf("Enter choice: ");
        loginChoice = readChoice();

        switch (loginChoice) {
            case 1:
                loginResult = loginAccount(head, &activeAccount);

                if (loginResult == -1) {
                    printf("\nInvalid account name or PIN.\n");

                    if (numAccounts == 0) {
                        printf("No accounts have been registered yet. You can choose [2] Register New Account.\n");
                    }
                } else if (loginResult == ADMIN_LOGIN) {
                    printf("\nWelcome, Administrator!\n");
                    loggedIn = 1;

                    while (loggedIn == 1) {
                        showAdminMenu();
                        printf("Enter choice: ");
                        sessionChoice = readChoice();

                        switch (sessionChoice) {
                            case 1:
                                showComingSoon("Record Exchange Rates");
                                break;
                            case 2:
                                showComingSoon("View/Update Customer Profile");
                                break;
                            case 3:
                                showComingSoon("View Transaction History");
                                break;
                            case 4:
                                printf("\nLogging out Administrator.\n");
                                loggedIn = 0;
                                break;
                            default:
                                printf("\nInvalid choice. Please select 1 to 4.\n");
                                break;
                        }
                    }
                } else {
                    printf("\nWelcome, %s! (Account No. %05d)\n", activeAccount->data.name,
                           activeAccount->data.accountNumber);
                    loggedIn = 1;

                    while (loggedIn == 1) {
                        showMainMenu();
                        printf("Enter choice: ");
                        sessionChoice = readChoice();

                        switch (sessionChoice) {
                            case 1:
                                showComingSoon("Deposit Amount");
                                break;
                            case 2:
                                showComingSoon("Withdraw Amount");
                                break;
                            case 3:
                                showComingSoon("Check Balance");
                                break;
                            case 4:
                                showComingSoon("Check Currency Exchange");
                                break;
                            case 5:
                                showComingSoon("Convert Currency");
                                break;
                            case 6:
                                showComingSoon("Show Interest Amount");
                                break;
                            case 7:
                                showComingSoon("Send Money");
                                break;
                            case 8:
                                showComingSoon("Close Account");
                                break;
                            case 9:
                                showComingSoon("Transaction History");
                                break;
                            case 10:
                                printf("\nLogging out %s. See you again!\n", activeAccount->data.name);
                                loggedIn = 0;
                                break;
                            default:
                                printf("\nInvalid choice. Please select 1 to 10.\n");
                                break;
                        }
                    }
                }
                break;

            case 2:
                if (registerNewAccount(&head, &numAccounts, &nextAccountNumber) == 0) {
                    saveDataToFile(head, numAccounts, nextAccountNumber, rates);
                }
                break;

            case 3:
                printf("\nThank you for using the Banking System. Goodbye!\n");
                freeAccountList(head);
                exit(0);
                break;

            default:
                printf("\nInvalid choice. Please select 1 to 3.\n");
                break;
        }
    }

    return 0;
}

void showComingSoon(char *feature) {
    printf("\n%s - coming soon!\n", feature);
}

int readLineFrom(FILE *fp, char *buf, int size) {
    int c;
    int length;
    int start;
    int tooLong;
    int result;

    length = 0;
    tooLong = 0;
    result = 1;
    c = fgetc(fp);

    if (c == EOF) {
        buf[0] = '\0';
        result = 0;
    } else {
        while (c != '\n' && c != EOF) {
            if (c == '\r') {
            } else if (length < size - 1) {
                buf[length] = (char)c;
                length++;
            } else {
                tooLong = 1;
            }
            c = fgetc(fp);
        }
        buf[length] = '\0';

        if (tooLong == 1) {
            buf[0] = '\0';
        } else {
            while (length > 0 && (buf[length - 1] == ' ' || buf[length - 1] == '\t')) {
                length--;
                buf[length] = '\0';
            }

            start = 0;
            while (buf[start] == ' ' || buf[start] == '\t') {
                start++;
            }

            if (start > 0) {
                memmove(buf, buf + start, length - start + 1);
            }
        }
    }

    return result;
}

void readLine(char *buf, int size) {
    fflush(stdout);

    if (readLineFrom(stdin, buf, size) == 0) {
        printf("\nInput ended. Goodbye!\n");
        exit(0);
    }
}

int parseInt(char *text, int *value) {
    char *end;
    long long number;
    int ok;

    ok = 0;

    if (text[0] != '\0') {
        errno = 0;
        number = strtoll(text, &end, 10);

        if (*end == '\0' && errno == 0 && number >= INT_MIN && number <= INT_MAX) {
            *value = (int)number;
            ok = 1;
        }
    }

    return ok;
}

int readInt(int *value) {
    char line[32];

    readLine(line, sizeof(line));

    return parseInt(line, value);
}

int readChoice() {
    int value;

    if (readInt(&value) == 0) {
        value = 0;
    }

    return value;
}

int askYesNo() {
    char line[8];
    int i;

    readLine(line, sizeof(line));

    for (i = 0; line[i] != '\0'; i++) {
        if (line[i] >= 'A' && line[i] <= 'Z') {
            line[i] = line[i] + ('a' - 'A');
        }
    }

    return (strcmp(line, "y") == 0 || strcmp(line, "yes") == 0);
}

void showLoginMenu() {
    printf("\n========================================\n");
    printf("        BANKING SYSTEM - LOGIN\n");
    printf("========================================\n");
    printf("[1] Login\n");
    printf("[2] Register New Account\n");
    printf("[3] Exit Program\n");
    printf("========================================\n");
}

void showMainMenu() {
    printf("\n========================================\n");
    printf("Select Transaction:\n");
    printf("[1] Deposit Amount\n");
    printf("[2] Withdraw Amount\n");
    printf("[3] Check Balance\n");
    printf("[4] Check Currency Exchange\n");
    printf("[5] Convert Currency\n");
    printf("[6] Show Interest Amount\n");
    printf("[7] Send Money\n");
    printf("[8] Close Account\n");
    printf("[9] Transaction History\n");
    printf("[10] Logout\n");
    printf("========================================\n");
}

void showAdminMenu() {
    printf("\n========================================\n");
    printf("        ADMINISTRATOR MENU\n");
    printf("========================================\n");
    printf("[1] Record Exchange Rates\n");
    printf("[2] View/Update Customer Profile\n");
    printf("[3] View Transaction History\n");
    printf("[4] Logout\n");
    printf("========================================\n");
}

int isAdminName(char *name) {
    int same;
    int i;
    char letter;

    same = 1;

    if (strlen(name) != strlen(ADMIN_NAME)) {
        same = 0;
    } else {
        for (i = 0; name[i] != '\0'; i++) {
            letter = name[i];

            if (letter >= 'A' && letter <= 'Z') {
                letter = letter + ('a' - 'A');
            }

            if (letter != ADMIN_NAME[i]) {
                same = 0;
            }
        }
    }

    return same;
}

int isValidPin(char *pin) {
    int valid;
    int i;

    valid = 1;

    if (strlen(pin) != PIN_LENGTH) {
        valid = 0;
    } else {
        for (i = 0; i < PIN_LENGTH; i++) {
            if (pin[i] < '0' || pin[i] > '9') {
                valid = 0;
            }
        }
    }

    return valid;
}

int isWeakPin(char *pin) {
    int allSame;
    int ascending;
    int descending;
    int i;

    allSame = 1;
    ascending = 1;
    descending = 1;

    for (i = 1; i < PIN_LENGTH; i++) {
        if (pin[i] != pin[i - 1]) {
            allSame = 0;
        }
        if (pin[i] != pin[i - 1] + 1) {
            ascending = 0;
        }
        if (pin[i] != pin[i - 1] - 1) {
            descending = 0;
        }
    }

    return (allSame == 1 || ascending == 1 || descending == 1);
}

int isLeapYear(int year) {
    return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
}

int isValidBirthday(char *birthday) {
    int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int valid;
    int i;
    int month, day, year;
    int maxDay;
    time_t now;
    struct tm *localTime;
    int currentYear, currentMonth, currentDay;

    valid = 1;

    if (strlen(birthday) != BIRTHDAY_LENGTH) {
        valid = 0;
    } else {
        for (i = 0; i < BIRTHDAY_LENGTH; i++) {
            if (i == 2 || i == 5) {
                if (birthday[i] != '/') {
                    valid = 0;
                }
            } else if (birthday[i] < '0' || birthday[i] > '9') {
                valid = 0;
            }
        }
    }

    if (valid == 1) {
        month = (birthday[0] - '0') * 10 + (birthday[1] - '0');
        day   = (birthday[3] - '0') * 10 + (birthday[4] - '0');
        year  = (birthday[6] - '0') * 1000 + (birthday[7] - '0') * 100 +
                (birthday[8] - '0') * 10 + (birthday[9] - '0');

        if (year < 1900 || month < 1 || month > 12) {
            valid = 0;
        } else {
            maxDay = daysInMonth[month - 1];
            if (month == 2 && isLeapYear(year) == 1) {
                maxDay = 29;
            }
            if (day < 1 || day > maxDay) {
                valid = 0;
            }
        }
    }

    if (valid == 1) {
        time(&now);
        localTime = localtime(&now);
        currentYear = localTime->tm_year + 1900;
        currentMonth = localTime->tm_mon + 1;
        currentDay = localTime->tm_mday;

        if (year > currentYear ||
            (year == currentYear && month > currentMonth) ||
            (year == currentYear && month == currentMonth && day > currentDay)) {
            valid = 0;
        }
    }

    return valid;
}

int getAge(char *birthday) {
    int month, day, year;
    int age;
    time_t now;
    struct tm *localTime;

    month = (birthday[0] - '0') * 10 + (birthday[1] - '0');
    day   = (birthday[3] - '0') * 10 + (birthday[4] - '0');
    year  = (birthday[6] - '0') * 1000 + (birthday[7] - '0') * 100 +
            (birthday[8] - '0') * 10 + (birthday[9] - '0');

    time(&now);
    localTime = localtime(&now);

    age = (localTime->tm_year + 1900) - year;

    if ((localTime->tm_mon + 1) < month ||
        ((localTime->tm_mon + 1) == month && localTime->tm_mday < day)) {
        age = age - 1;
    }

    return age;
}

int isValidContactNumber(char *contactNumber) {
    int valid;
    int start;
    int i;
    int digitCount;

    valid = 1;
    start = 0;

    if (contactNumber[0] == '+') {
        start = 1;
    }

    digitCount = (int)strlen(contactNumber) - start;

    if (digitCount < MIN_CONTACT_DIGITS || digitCount > MAX_CONTACT_DIGITS) {
        valid = 0;
    } else {
        for (i = start; contactNumber[i] != '\0'; i++) {
            if (contactNumber[i] < '0' || contactNumber[i] > '9') {
                valid = 0;
            }
        }
    }

    return valid;
}

int isValidEmail(char *email) {
    int valid;
    int i;
    int atCount;
    int atPosition;
    char *domain;
    char *lastDot;

    valid = 1;
    atCount = 0;
    atPosition = -1;

    if (strlen(email) < 6 || strlen(email) > MAX_EMAIL_LENGTH) {
        valid = 0;
    } else {
        if (strchr(email, ' ') != NULL) {
            valid = 0;
        }

        for (i = 0; email[i] != '\0'; i++) {
            if (email[i] == '@') {
                atCount++;
                atPosition = i;
            }
        }
    }

    if (valid == 1 && (atCount != 1 || atPosition < 1)) {
        valid = 0;
    }

    if (valid == 1) {
        domain = email + atPosition + 1;
        lastDot = strrchr(domain, '.');

        if (lastDot == NULL || domain[0] == '.' || strlen(lastDot + 1) < 2 ||
            strstr(email, "..") != NULL) {
            valid = 0;
        }
    }

    return valid;
}

int promptNewPin(char pin[]) {
    char input[32];
    int done;
    int result;

    done = 0;
    result = 0;

    while (done == 0) {
        printf("Create a 4-digit PIN: ");

        readLine(input, sizeof(input));

        if (isValidPin(input) == 0) {
            printf("Invalid PIN. PIN must be exactly 4 digits.\n\n");
        } else if (isWeakPin(input) == 1) {
            printf("That PIN is too easy to guess. Please avoid repeated digits (0000, 1111, ..., 9999)\n");
            printf("and simple sequences (1234, 4321, ...).\n\n");
        } else {
            strcpy(pin, input);
            done = 1;
        }
    }

    return result;
}

int promptBirthday(char birthday[]) {
    char input[64];
    int done;
    int result;

    done = 0;
    result = 0;

    while (done == 0) {
        printf("Birthday (MM/DD/YYYY): ");

        readLine(input, sizeof(input));

        if (isValidBirthday(input) == 0) {
            printf("Invalid birthday. Use MM/DD/YYYY with a real date that is not in the future.\n\n");
        } else if (getAge(input) < MIN_AGE) {
            printf("\nYou must be at least %d years old to open an account.\n", MIN_AGE);
            result = -1;
            done = 1;
        } else {
            strcpy(birthday, input);
            done = 1;
        }
    }

    return result;
}

int promptContactNumber(char contactNumber[]) {
    char input[64];
    int done;
    int result;

    done = 0;
    result = 0;

    while (done == 0) {
        printf("Contact Number: ");

        readLine(input, sizeof(input));

        if (isValidContactNumber(input) == 0) {
            printf("Invalid contact number.\n\n",
                   MIN_CONTACT_DIGITS, MAX_CONTACT_DIGITS);
        } else {
            strcpy(contactNumber, input);
            done = 1;
        }
    }

    return result;
}

int promptEmail(char email[]) {
    char input[MAX_EMAIL_LENGTH + 1];
    int done;
    int result;

    done = 0;
    result = 0;

    while (done == 0) {
        printf("Email Address: ");

        readLine(input, sizeof(input));

        if (isValidEmail(input) == 0) {
            printf("Invalid email address.\n\n");
        } else {
            strcpy(email, input);
            done = 1;
        }
    }

    return result;
}

struct Node *findAccountByName(struct Node *head, char *name) {
    struct Node *current;
    struct Node *found;

    found = NULL;

    for (current = head; current != NULL; current = current->next) {
        if (strcmp(current->data.name, name) == 0) {
            found = current;
        }
    }

    return found;
}

struct Node *findAccountByNumber(struct Node *head, int accountNumber) {
    struct Node *current;
    struct Node *found;

    found = NULL;

    for (current = head; current != NULL; current = current->next) {
        if (current->data.accountNumber == accountNumber) {
            found = current;
        }
    }

    return found;
}

void freeAccountList(struct Node *head) {
    struct Node *next;

    while (head != NULL) {
        next = head->next;
        free(head);
        head = next;
    }
}

int registerNewAccount(struct Node **head, int *numAccounts, int *nextAccountNumber) {
    struct Account newAccount;
    struct Node *newNode;
    struct Node *tail;
    int w;
    int result;

    printf("\nRegister New Account\n");
    printf("Account Name: ");
    readLine(newAccount.name, sizeof(newAccount.name));

    if (newAccount.name[0] == '\0') {
        printf("\nInvalid account name. Use 1 to 99 characters.\n");
        result = -1;
    } else if (isAdminName(newAccount.name) == 1) {
        printf("\nThat account name is reserved. Please try again with a different name.\n");
        result = -1;
    } else if (findAccountByName(*head, newAccount.name) != NULL) {
        printf("\nAn account with that name already exists. Please try again.\n");
        result = -1;
    } else if (promptNewPin(newAccount.pin) == -1 ||
               promptBirthday(newAccount.birthday) == -1 ||
               promptContactNumber(newAccount.contactNumber) == -1 ||
               promptEmail(newAccount.email) == -1) {
        printf("\nRegistration cancelled.\n");
        result = -1;
    } else {
        newNode = (struct Node *)malloc(sizeof(struct Node));

        if (newNode == NULL) {
            printf("\nThe system is out of memory. Cannot register a new account.\n");
            result = -1;
        } else {
            newAccount.accountNumber = *nextAccountNumber;
            *nextAccountNumber = *nextAccountNumber + 1;
            for (w = 0; w < NUM_WALLETS; w++) {
                newAccount.wallets[w] = 0.00;
            }

            newNode->data = newAccount;
            newNode->next = NULL;

            if (*head == NULL) {
                *head = newNode;
            } else {
                tail = *head;
                while (tail->next != NULL) {
                    tail = tail->next;
                }
                tail->next = newNode;
            }

            printf("\nAccount \"%s\" registered successfully!\n", newAccount.name);
            printf("Your account number is: %05d\n", newAccount.accountNumber);
            printf("Birthday       : %s\n", newAccount.birthday);
            printf("Contact Number : %s\n", newAccount.contactNumber);
            printf("Email Address  : %s\n", newAccount.email);

            *numAccounts = *numAccounts + 1;
            result = 0;
        }
    }

    return result;
}

int loginAccount(struct Node *head, struct Node **activeAccount) {
    char name[100];
    char pin[32];
    struct Node *found;
    int result;

    printf("\nLogin\n");
    printf("Account Name: ");
    readLine(name, sizeof(name));
    printf("PIN: ");
    readLine(pin, sizeof(pin));

    if (isAdminName(name) == 1) {
        if (strcmp(pin, ADMIN_PIN) == 0) {
            result = ADMIN_LOGIN;
        } else {
            result = -1;
        }
    } else {
        found = findAccountByName(head, name);

        if (found == NULL) {
            result = -1;
        } else if (strcmp(found->data.pin, pin) != 0) {
            result = -1;
        } else {
            *activeAccount = found;
            result = 0;
        }
    }

    return result;
}

char *getWalletCode(struct ExchangeRate rates[], int wallet) {
    char *code;

    if (wallet == 0) {
        code = "PHP";
    } else {
        code = rates[wallet - 1].code;
    }

    return code;
}

double roundToCentavos(double value) {
    return round(value * 100.0) / 100.0;
}

void initializeExchangeRates(struct ExchangeRate rates[]) {
    strcpy(rates[0].code, "USD");
    strcpy(rates[0].name, "United States Dollar (USD)");
    rates[0].rate = 0;

    strcpy(rates[1].code, "JPY");
    strcpy(rates[1].name, "Japanese Yen (JPY)");
    rates[1].rate = 0;

    strcpy(rates[2].code, "GBP");
    strcpy(rates[2].name, "British Pound Sterling (GBP)");
    rates[2].rate = 0;

    strcpy(rates[3].code, "EUR");
    strcpy(rates[3].name, "Euro (EUR)");
    rates[3].rate = 0;

    strcpy(rates[4].code, "CNY");
    strcpy(rates[4].name, "Chinese Yuan Renminbi (CNY)");
    rates[4].rate = 0;
}

void saveDataToFile(struct Node *head, int numAccounts, int nextAccountNumber, struct ExchangeRate rates[]) {
    FILE *filePointer;
    struct Node *current;
    int i;
    int w;

    filePointer = fopen(DATA_FILE, "w");

    if (filePointer == NULL) {
        printf("\nError: Unable to save data to file.\n");
    } else {
        fprintf(filePointer, "%d %d %d\n", numAccounts, nextAccountNumber, DATA_FORMAT_VERSION);

        for (current = head; current != NULL; current = current->next) {
            fprintf(filePointer, "%d\n", current->data.accountNumber);
            fprintf(filePointer, "%s\n", current->data.name);
            fprintf(filePointer, "%s\n", current->data.pin);
            for (w = 0; w < NUM_WALLETS; w++) {
                fprintf(filePointer, "%.2f%c", current->data.wallets[w], (w == NUM_WALLETS - 1) ? '\n' : ' ');
            }
            fprintf(filePointer, "%s\n", current->data.birthday);
            fprintf(filePointer, "%s\n", current->data.contactNumber);
            fprintf(filePointer, "%s\n", current->data.email);
        }

        for (i = 0; i < NUM_CURRENCIES; i++) {
            fprintf(filePointer, "%.4f\n", rates[i].rate);
        }

        fclose(filePointer);
    }
}

void loadDataFromFile(struct Node **head, int *numAccounts, int *nextAccountNumber, struct ExchangeRate rates[]) {
    FILE *filePointer;
    char line[400];
    char *end;
    char *p;
    long recordStart;
    int fieldsRead;
    int fileVersion;
    int legacyFormat;
    int hasPersonalInfo;
    int hasWallets;
    int expectedAccounts;
    int readOk;
    int i;
    int w;
    double oldBalance;
    double value;
    char oldCurrency[4];
    struct Node *newNode;
    struct Node *tail;

    *head = NULL;
    *numAccounts = 0;
    *nextAccountNumber = 1;
    expectedAccounts = 0;
    tail = NULL;

    filePointer = fopen(DATA_FILE, "r");

    if (filePointer == NULL) {

        remove(HISTORY_FILE);
    } else {
        legacyFormat = 0;
        hasPersonalInfo = 0;
        hasWallets = 0;
        fileVersion = 0;

        if (readLineFrom(filePointer, line, sizeof(line)) == 1) {
            fieldsRead = sscanf(line, "%d %d %d", &expectedAccounts, nextAccountNumber, &fileVersion);

            if (fieldsRead == 1) {
                legacyFormat = 1;
            } else if (fieldsRead == 3 && fileVersion >= PERSONAL_INFO_VERSION) {
                hasPersonalInfo = 1;

                if (fileVersion >= DATA_FORMAT_VERSION) {
                    hasWallets = 1;
                }
            } else if (fieldsRead < 1) {
                expectedAccounts = 0;
                *nextAccountNumber = 1;
            }
        }

        if (expectedAccounts < 0) {
            expectedAccounts = 0;
        }

        for (i = 0; i < expectedAccounts; i++) {
            recordStart = ftell(filePointer);

            newNode = (struct Node *)malloc(sizeof(struct Node));

            if (newNode == NULL) {
                printf("\nWarning: the system ran out of memory.\n");
                break;
            }

            newNode->next = NULL;
            readOk = 1;

            for (w = 0; w < NUM_WALLETS; w++) {
                newNode->data.wallets[w] = 0.00;
            }
            strcpy(newNode->data.birthday, "N/A");
            strcpy(newNode->data.contactNumber, "N/A");
            strcpy(newNode->data.email, "N/A");

            if (legacyFormat == 1) {
                newNode->data.accountNumber = i + 1;
            } else if (readLineFrom(filePointer, line, sizeof(line)) == 0 ||
                       parseInt(line, &newNode->data.accountNumber) == 0 ||
                       newNode->data.accountNumber < 1) {
                readOk = 0;
            }

            if (readOk == 1 && (readLineFrom(filePointer, newNode->data.name, sizeof(newNode->data.name)) == 0 ||
                                newNode->data.name[0] == '\0')) {
                readOk = 0;
            }

            if (readOk == 1 && readLineFrom(filePointer, newNode->data.pin, sizeof(newNode->data.pin)) == 0) {
                readOk = 0;
            }

            if (readOk == 0) {

                free(newNode);
                fseek(filePointer, recordStart, SEEK_SET);
                break;
            }

            if (hasWallets == 1) {
                readLineFrom(filePointer, line, sizeof(line));
                p = line;

                for (w = 0; w < NUM_WALLETS; w++) {
                    value = strtod(p, &end);

                    if (end == p) {
                        value = -1;
                    } else {
                        p = end;
                    }

                    newNode->data.wallets[w] = value;
                }
            } else {

                oldBalance = 0.00;
                oldCurrency[0] = '\0';
                readLineFrom(filePointer, line, sizeof(line));
                fieldsRead = sscanf(line, "%lf %3s", &oldBalance, oldCurrency);

                if (fieldsRead == 1 && readLineFrom(filePointer, line, sizeof(line)) == 1) {
                    sscanf(line, "%3s", oldCurrency);
                }

                for (w = 0; w < NUM_WALLETS; w++) {
                    newNode->data.wallets[w] = 0.00;
                }

                newNode->data.wallets[0] = oldBalance;

                for (w = 1; w < NUM_WALLETS; w++) {
                    if (strcmp(rates[w - 1].code, oldCurrency) == 0) {
                        newNode->data.wallets[0] = 0.00;
                        newNode->data.wallets[w] = oldBalance;
                    }
                }
            }

            for (w = 0; w < NUM_WALLETS; w++) {
                if (!(newNode->data.wallets[w] >= 0 && newNode->data.wallets[w] <= MAX_BALANCE)) {
                    printf("\nWarning: invalid balance found for account %05d (%s). It was reset to 0.00.\n",
                           newNode->data.accountNumber, getWalletCode(rates, w));
                    newNode->data.wallets[w] = 0.00;
                } else {
                    newNode->data.wallets[w] = roundToCentavos(newNode->data.wallets[w]);
                }
            }

            if (hasPersonalInfo == 1) {
                readLineFrom(filePointer, newNode->data.birthday, sizeof(newNode->data.birthday));
                readLineFrom(filePointer, newNode->data.contactNumber, sizeof(newNode->data.contactNumber));
                readLineFrom(filePointer, newNode->data.email, sizeof(newNode->data.email));

                if (isValidBirthday(newNode->data.birthday) == 0) {
                    strcpy(newNode->data.birthday, "N/A");
                }
                if (newNode->data.contactNumber[0] == '\0') {
                    strcpy(newNode->data.contactNumber, "N/A");
                }
                if (newNode->data.email[0] == '\0') {
                    strcpy(newNode->data.email, "N/A");
                }
            }

            if (*head == NULL) {
                *head = newNode;
            } else {
                tail->next = newNode;
            }
            tail = newNode;
            *numAccounts = *numAccounts + 1;
        }

        if (*numAccounts != expectedAccounts) {
            printf("\nWarning: the file says %d account(s) but only %d valid record(s) were found.\n",
                   expectedAccounts, *numAccounts);
        }

        if (legacyFormat == 1) {
            *nextAccountNumber = *numAccounts + 1;
        }

        for (i = 0; i < NUM_CURRENCIES; i++) {
            rates[i].rate = 0;

            if (readLineFrom(filePointer, line, sizeof(line)) == 1) {
                value = strtod(line, &end);

                if (end != line && value >= 0 && value <= MAX_BALANCE) {
                    rates[i].rate = value;
                }
            }
        }

        fclose(filePointer);
        printf("\nPrevious account data loaded successfully (%d account(s)).\n", *numAccounts);
    }
}
