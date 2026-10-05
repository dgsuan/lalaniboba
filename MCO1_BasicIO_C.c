/********************
Last names: Bobadilla, Calvez, Casao, Sanico
Language: C
Paradigm(s): Procedural
********************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ACCOUNTS 100
#define ADMIN_NAME "admin"
#define ADMIN_PIN "2580"
#define NUM_CURRENCIES 5
#define NUM_WALLETS 6
#define MAX_BALANCE 999999999999.99
#define MIN_RATE 0.0001

struct Account {
    int accountNumber;
    char name[100];
    char pin[5];
    char contactNumber[16];
    double wallets[NUM_WALLETS];
};

struct ExchangeRate {
    char code[4];
    char name[40];
    double rate;
};

struct Account accounts[MAX_ACCOUNTS];
struct ExchangeRate rates[NUM_CURRENCIES];
int numAccounts = 0;

void readLine(char *buf, int size);
int readChoice();
double readNumber();
double roundToCentavos(double value);
void comingSoon();
void showLoginMenu();
void showMainMenu();
void showAdminMenu();
int isValidPin(char *pin);
int isValidContact(char *contact);
int nameExists(char *name);
int findAccount(char *name, char *pin);
void registerAccount();

void initializeExchangeRates();
char *getWalletCode(int wallet);
char *getWalletName(int wallet);
int selectWallet(struct Account *account);
void depositAmount(struct Account *account);
void withdrawAmount(struct Account *account);
void recordExchangeRate();

int main() {
    char name[100];
    char pin[100];
    int loginChoice;
    int sessionChoice;
    int loggedIn;
    int accountIndex;
    struct Account *activeAccount;

    initializeExchangeRates();

    while (1) {
        showLoginMenu();
        printf("Enter choice: ");
        loginChoice = readChoice();

        switch (loginChoice) {
            case 1:
                printf("\nAccount name: ");
                readLine(name, sizeof(name));
                printf("PIN: ");
                readLine(pin, sizeof(pin));
                loggedIn = 1;
                accountIndex = findAccount(name, pin);

                if (strcmp(name, ADMIN_NAME) == 0 && strcmp(pin, ADMIN_PIN) == 0) {
                    printf("\nWelcome, Administrator!\n");

                    while (loggedIn == 1) {
                        showAdminMenu();
                        printf("Enter choice: ");
                        sessionChoice = readChoice();

                        if (sessionChoice == 1) {
                            recordExchangeRate();
                        } else if (sessionChoice == 2 || sessionChoice == 3) {
                            comingSoon();
                        } else if (sessionChoice == 4) {
                            printf("\nLogging out Administrator.\n");
                            loggedIn = 0;
                        } else {
                            printf("\nInvalid choice. Please select 1 to 4.\n");
                        }
                    }
                } else if (accountIndex == -1) {
                    printf("\nInvalid account name or PIN.\n");
                } else {
                    activeAccount = &accounts[accountIndex];
                    printf("\nWelcome, %s!\n", activeAccount->name);

                    while (loggedIn == 1) {
                        showMainMenu();
                        printf("Enter choice: ");
                        sessionChoice = readChoice();

                        if (sessionChoice == 1) {
                            depositAmount(activeAccount);
                        } else if (sessionChoice == 2) {
                            withdrawAmount(activeAccount);
                        } else if (sessionChoice >= 3 && sessionChoice <= 9) {
                            comingSoon();
                        } else if (sessionChoice == 10) {
                            printf("\nLogging out %s. See you again!\n", activeAccount->name);
                            loggedIn = 0;
                        } else {
                            printf("\nInvalid choice. Please select 1 to 10.\n");
                        }
                    }
                }
                break;

            case 2:
                registerAccount();
                break;

            case 3:
                printf("\nThank you for using the Banking System. Goodbye!\n");
                return 0;

            default:
                printf("\nInvalid choice. Please select 1 to 3.\n");
                break;
        }
    }

    return 0;
}

void readLine(char *buf, int size) {
    fflush(stdout);

    if (fgets(buf, size, stdin) == NULL) {
        printf("\nInput ended. Goodbye!\n");
        exit(0);
    }

    buf[strcspn(buf, "\r\n")] = '\0';
}

int readChoice() {
    char line[32];
    char *end;
    long value;

    readLine(line, sizeof(line));

    value = strtol(line, &end, 10);

    if (end == line || *end != '\0' || value < 0 || value > 1000) {
        return -1;
    }

    return (int)value;
}

double readNumber() {
    char line[64];
    char *end;
    double value;

    readLine(line, sizeof(line));
    value = strtod(line, &end);

    if (end == line || *end != '\0' || value <= 0 || value > MAX_BALANCE) {
        return -1;
    }

    return value;
}

double roundToCentavos(double value) {
    long long centavos;

    centavos = (long long)(value * 100.0 + 0.5);

    return centavos / 100.0;
}

void comingSoon() {
    printf("\nComing Soon\n");
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
    printf("        ADMIN MENU\n");
    printf("========================================\n");
    printf("[1] Record Exchange Rates\n");
    printf("[2] View/Update Customer Profile\n");
    printf("[3] View Transaction History\n");
    printf("[4] Logout\n");
    printf("========================================\n");
}

int isValidPin(char *pin) {
    int i;

    if (strlen(pin) != 4) {
        return 0;
    }

    for (i = 0; i < 4; i++) {
        if (pin[i] < '0' || pin[i] > '9') {
            return 0;
        }
    }

    return 1;
}

int isValidContact(char *contact) {
    int i;
    int length;

    length = strlen(contact);

    if (length < 10 || length > 15) {
        return 0;
    }

    for (i = 0; i < length; i++) {
        if (contact[i] < '0' || contact[i] > '9') {
            return 0;
        }
    }

    return 1;
}

int nameExists(char *name) {
    int i;

    for (i = 0; i < numAccounts; i++) {
        if (strcmp(accounts[i].name, name) == 0) {
            return 1;
        }
    }

    return 0;
}

int findAccount(char *name, char *pin) {
    int i;

    for (i = 0; i < numAccounts; i++) {
        if (strcmp(accounts[i].name, name) == 0 && strcmp(accounts[i].pin, pin) == 0) {
            return i;
        }
    }

    return -1;
}

void registerAccount() {
    char name[100];
    char pin[100];
    char contact[100];
    int w;

    if (numAccounts >= MAX_ACCOUNTS) {
        printf("\nThe system is full. Cannot register a new account.\n");
        return;
    }

    printf("\nRegister New Account\n");
    printf("Account Name: ");
    readLine(name, sizeof(name));

    if (name[0] == '\0') {
        printf("\nInvalid account name.\n");
        return;
    }

    if (strcmp(name, ADMIN_NAME) == 0 || nameExists(name) == 1) {
        printf("\nThat account name is not available.\n");
        return;
    }

    printf("Create a 4-digit PIN: ");
    readLine(pin, sizeof(pin));

    if (isValidPin(pin) == 0) {
        printf("\nInvalid PIN. It must be exactly 4 digits.\n");
        return;
    }

    printf("Contact Number: ");
    readLine(contact, sizeof(contact));

    if (isValidContact(contact) == 0) {
        printf("\nInvalid contact number. Use 10 to 15 digits only.\n");
        return;
    }

    accounts[numAccounts].accountNumber = numAccounts + 1;
    strcpy(accounts[numAccounts].name, name);
    strcpy(accounts[numAccounts].pin, pin);
    strcpy(accounts[numAccounts].contactNumber, contact);

    for (w = 0; w < NUM_WALLETS; w++) {
        accounts[numAccounts].wallets[w] = 0.00;
    }

    printf("\nAccount \"%s\" registered successfully!\n", name);
    printf("Your account number is: %05d\n", accounts[numAccounts].accountNumber);

    numAccounts++;
}

void initializeExchangeRates() {
    strcpy(rates[0].code, "USD");
    strcpy(rates[0].name, "United States Dollar (USD)");
    rates[0].rate = 62.00;

    strcpy(rates[1].code, "JPY");
    strcpy(rates[1].name, "Japanese Yen (JPY)");
    rates[1].rate = 0.40;

    strcpy(rates[2].code, "GBP");
    strcpy(rates[2].name, "British Pound Sterling (GBP)");
    rates[2].rate = 84.00;

    strcpy(rates[3].code, "EUR");
    strcpy(rates[3].name, "Euro (EUR)");
    rates[3].rate = 72.00;

    strcpy(rates[4].code, "CNY");
    strcpy(rates[4].name, "Chinese Yuan Renminbi (CNY)");
    rates[4].rate = 9.00;
}

char *getWalletCode(int wallet) {
    if (wallet == 0) {
        return "PHP";
    }

    return rates[wallet - 1].code;
}

char *getWalletName(int wallet) {
    if (wallet == 0) {
        return "Philippine Peso (PHP)";
    }

    return rates[wallet - 1].name;
}

int selectWallet(struct Account *account) {
    int w;
    int choice;

    printf("\nSelect Currency Wallet:\n");

    for (w = 0; w < NUM_WALLETS; w++) {
        printf("[%d] %-30s Balance: %.2f\n", w + 1, getWalletName(w), account->wallets[w]);
    }

    printf("\nCurrency: ");
    choice = readChoice();

    if (choice < 1 || choice > NUM_WALLETS) {
        printf("\nInvalid currency selection.\n");
        return -1;
    }

    return choice - 1;
}

void depositAmount(struct Account *account) {
    int wallet;
    double amount;

    printf("\nDeposit Amount\n");
    printf("Account Number: %05d\n", account->accountNumber);
    printf("Account Name: %s\n", account->name);

    wallet = selectWallet(account);

    if (wallet == -1) {
        return;
    }

    printf("\nWallet: %s\n", getWalletName(wallet));
    printf("Current Balance: %.2f %s\n", account->wallets[wallet], getWalletCode(wallet));

    printf("\nDeposit Amount: ");
    amount = readNumber();

    if (amount > 0) {
        amount = roundToCentavos(amount);
    }

    if (amount < 0.01) {
        printf("\nInvalid amount. Enter a plain positive number of at least 0.01.\n");
    } else if (account->wallets[wallet] + amount > MAX_BALANCE) {
        printf("\nDeposit not allowed: a wallet cannot hold more than %.2f.\n", MAX_BALANCE);
    } else {
        account->wallets[wallet] = roundToCentavos(account->wallets[wallet] + amount);
        printf("\nDeposit successful!\n");
        printf("Updated Balance: %.2f %s\n", account->wallets[wallet], getWalletCode(wallet));
    }
}

void withdrawAmount(struct Account *account) {
    int wallet;
    double amount;

    printf("\nWithdraw Amount\n");
    printf("Account Number: %05d\n", account->accountNumber);
    printf("Account Name: %s\n", account->name);

    wallet = selectWallet(account);

    if (wallet == -1) {
        return;
    }

    printf("\nWallet: %s\n", getWalletName(wallet));
    printf("Current Balance: %.2f %s\n", account->wallets[wallet], getWalletCode(wallet));

    printf("\nWithdraw Amount: ");
    amount = readNumber();

    if (amount > 0) {
        amount = roundToCentavos(amount);
    }

    if (amount < 0.01) {
        printf("\nInvalid amount. Enter a plain positive number of at least 0.01.\n");
    } else if (amount > account->wallets[wallet]) {
        printf("\nInsufficient balance!\n");
    } else {
        account->wallets[wallet] = roundToCentavos(account->wallets[wallet] - amount);
        printf("\nWithdrawal successful!\n");
        printf("Updated Balance: %.2f %s\n", account->wallets[wallet], getWalletCode(wallet));
    }
}

void recordExchangeRate() {
    int i;
    int choice;
    double rate;

    printf("\nRecord Exchange Rate\n\n");

    for (i = 0; i < NUM_CURRENCIES; i++) {
        printf("[%d] %-30s Current: 1 %s = %.4f PHP\n", i + 1, rates[i].name, rates[i].code, rates[i].rate);
    }

    printf("\nSelect Foreign Currency: ");
    choice = readChoice();

    if (choice < 1 || choice > NUM_CURRENCIES) {
        printf("\nInvalid selection.\n");
        return;
    }

    printf("New Exchange Rate (1 %s = ? PHP): ", rates[choice - 1].code);
    rate = readNumber();

    if (rate > 0) {
        rate = (long long)(rate * 10000.0 + 0.5) / 10000.0;
    }

    if (rate < MIN_RATE) {
        printf("\nInvalid exchange rate. Use a plain number of at least %.4f.\n", MIN_RATE);
        printf("The recorded rate was not changed.\n");
    } else {
        rates[choice - 1].rate = rate;
        printf("\n%s exchange rate recorded: 1 %s = %.4f PHP\n", rates[choice - 1].name, rates[choice - 1].code, rate);
    }
}
