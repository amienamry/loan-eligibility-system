#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CUSTOMERS 100
#define MAX_NAME_LEN 50
#define MIN_INCOME 0.0
#define MAX_INCOME 100000.0
#define MIN_COMMITMENT 0.0
#define MAX_COMMITMENT 50000.0
#define MIN_LOAN_INCOME 3000.0
#define MAX_DTI 40.0
#define FILE_NAME "loanapplications.txt"
#define ELIGIBLE 1
#define NOT_ELIGIBLE 0

void clearInputBuffer(void);
int getCustomerID(void);
int getCustomerName(char name[], size_t size);
double getValidatedAmount(const char prompt[], double minimum, double maximum, int *inputOk);
double calculateDTI(double income, double commitments);
int determineEligibility(double income, double dti);
const char *getStatusText(int status);
void displayResult(int id, const char name[], double income, double commitments, double dti, int status);
double calculateAverageDTI(double totalDTI, int count);
void displaySummary(const double dtiRatio[], const int eligibility[], int count);
int saveToFile(const int customerID[], const char customerName[][MAX_NAME_LEN], const double monthlyIncome[], const double commitments[], const double dtiRatio[], const int eligibility[], int count);

/*
 * Discard the remainder of an overlong input line.
 * Parameters: none.
 * Returns: nothing.
 */
void clearInputBuffer(void)
{
    int character;

    while ((character = getchar()) != '\n' && character != EOF) {
    }
}

/*
 * Read one bounded line from standard input.
 * Parameters: buffer is the destination; size is its capacity.
 * Returns: 1 for a complete line, 0 at EOF, or -1 if the line was too long.
 */
static int readLine(char buffer[], size_t size)
{
    size_t length;
    int character;

    if (size == 0 || fgets(buffer, (int)size, stdin) == NULL) {
        return 0;
    }

    length = strcspn(buffer, "\n");
    if (buffer[length] == '\n') {
        buffer[length] = '\0';
    } else {
        character = getchar();
        if (character != '\n' && character != EOF) {
            clearInputBuffer();
            return -1;
        }
    }

    length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\r') {
        buffer[length - 1] = '\0';
    }
    return 1;
}

/*
 * Prompt for a positive customer ID; zero ends data entry.
 * Parameters: none.
 * Returns: the ID, zero for the sentinel, or -1 at end of input.
 */
int getCustomerID(void)
{
    char line[128];

    for (;;) {
        char *end;
        long id;
        int lineStatus;

        printf("Enter Customer ID (0 to stop): ");
        fflush(stdout);
        lineStatus = readLine(line, sizeof(line));
        if (lineStatus == 0) {
            return -1;
        }
        if (lineStatus < 0) {
            printf("  [ERROR] Customer ID input is too long.\n");
            continue;
        }

        errno = 0;
        id = strtol(line, &end, 10);
        while (isspace((unsigned char)*end)) {
            end++;
        }
        if (line == end || errno == ERANGE || *end != '\0' ||
            id < 0 || id > INT_MAX) {
            printf("  [ERROR] Enter a non-negative whole-number ID.\n");
            continue;
        }
        return (int)id;
    }
}

/*
 * Prompt for a non-empty customer name, including spaces.
 * Parameters: name is the destination array; size is its capacity.
 * Returns: 1 if a name was read, or 0 at end of input.
 */
int getCustomerName(char name[], size_t size)
{
    for (;;) {
        size_t index;
        int hasNonSpace = 0;
        int lineStatus;

        printf("Enter Customer Name: ");
        fflush(stdout);
        lineStatus = readLine(name, size);
        if (lineStatus == 0) {
            return 0;
        }
        if (lineStatus < 0) {
            printf("  [ERROR] Name must be no more than %zu characters.\n",
                   size - 1);
            continue;
        }

        for (index = 0; name[index] != '\0'; index++) {
            if (!isspace((unsigned char)name[index])) {
                hasNonSpace = 1;
                break;
            }
        }
        if (hasNonSpace) {
            return 1;
        }
        printf("  [ERROR] Customer name cannot be empty.\n");
    }
}

/*
 * Read and validate a finite amount within the inclusive range.
 * Parameters: prompt, minimum, maximum, and inputOk (set to zero at EOF).
 * Returns: the validated amount, or 0.0 if input ends.
 */
double getValidatedAmount(const char prompt[], double minimum, double maximum,
                          int *inputOk)
{
    char line[128];

    *inputOk = 1;
    for (;;) {
        char *end;
        double amount;
        int lineStatus;

        printf("%s", prompt);
        fflush(stdout);
        lineStatus = readLine(line, sizeof(line));
        if (lineStatus == 0) {
            *inputOk = 0;
            return 0.0;
        }
        if (lineStatus < 0) {
            printf("  [ERROR] Please enter a valid amount.\n");
            continue;
        }

        errno = 0;
        amount = strtod(line, &end);
        while (isspace((unsigned char)*end)) {
            end++;
        }
        if (line == end || errno == ERANGE || *end != '\0' ||
            !isfinite(amount) || amount < minimum || amount > maximum) {
            printf("  [ERROR] Amount must be between RM%.0f and RM%.0f (without RM prefix).\n",
                   minimum, maximum);
            continue;
        }
        return amount;
    }
}

/*
 * Calculate the debt-to-income percentage.
 * Parameters: monthly income and monthly commitments.
 * Returns: DTI percentage, or 0.0 when income is zero.
 */
double calculateDTI(double income, double commitments)
{
    if (income == 0.0) {
        return 0.0;
    }
    return commitments * 100.0 / income;
}

/*
 * Apply the minimum-income and maximum-DTI eligibility rules.
 * Parameters: monthly income and calculated DTI percentage.
 * Returns: ELIGIBLE or NOT_ELIGIBLE.
 */
int determineEligibility(double income, double dti)
{
    if (income > 0.0 && income >= MIN_LOAN_INCOME && dti <= MAX_DTI) {
        return ELIGIBLE;
    }
    return NOT_ELIGIBLE;
}

/*
 * Convert a status flag to display and file text.
 * Parameters: eligibility status flag.
 * Returns: the corresponding status string.
 */
const char *getStatusText(int status)
{
    return status == ELIGIBLE ? "Eligible" : "Not Eligible";
}

/*
 * Display one customer's complete assessment.
 * Parameters: ID, name, income, commitments, DTI, and eligibility status.
 * Returns: nothing.
 */
void displayResult(int id, const char name[], double income,
                   double commitments, double dti, int status)
{
    printf("\n--------------------------------------------------\n");
    printf(" Customer ID      : %d\n", id);
    printf(" Customer Name    : %s\n", name);
    printf(" Monthly Income   : RM %.2f\n", income);
    printf(" Commitments      : RM %.2f\n", commitments);
    if (income == 0.0) {
        printf(" DTI Ratio        : N/A\n");
    } else {
        printf(" DTI Ratio        : %.2f%%\n", dti);
    }
    printf(" Loan Status      : %s\n", getStatusText(status));
    printf("--------------------------------------------------\n");
}

/*
 * Calculate the average DTI with a zero-customer guard.
 * Parameters: total DTI and number of customers.
 * Returns: average DTI, or 0.0 when count is zero.
 */
double calculateAverageDTI(double totalDTI, int count)
{
    if (count <= 0) {
        return 0.0;
    }
    return totalDTI / count;
}

/*
 * Calculate and display the summary from the stored arrays.
 * Parameters: DTI and eligibility arrays, and the number of records.
 * Returns: nothing.
 */
void displaySummary(const double dtiRatio[], const int eligibility[],
                    int count)
{
    int index;
    int eligibleCount = 0;
    int notEligibleCount;
    double totalDTI = 0.0;

    for (index = 0; index < count; index++) {
        if (eligibility[index] == ELIGIBLE) {
            eligibleCount++;
        }
        totalDTI += dtiRatio[index];
    }
    notEligibleCount = count - eligibleCount;

    printf("\n================== SUMMARY ==================\n");
    printf(" Total customers processed : %d\n", count);
    printf(" Total Eligible            : %d\n", eligibleCount);
    printf(" Total Not Eligible        : %d\n", notEligibleCount);
    printf(" Average DTI ratio         : %.2f%%\n",
           calculateAverageDTI(totalDTI, count));
    printf("=============================================\n");
}

/*
 * Write all records as quoted-name CSV, overwriting any previous file.
 * Parameters: all six parallel arrays and the number of records.
 * Returns: 1 on success, or 0 if opening, writing, or closing fails.
 */
int saveToFile(const int customerID[],
               const char customerName[][MAX_NAME_LEN],
               const double monthlyIncome[], const double commitments[],
               const double dtiRatio[], const int eligibility[], int count)
{
    FILE *file = fopen(FILE_NAME, "w");
    int index;
    int success = 1;

    if (file == NULL) {
        perror("Unable to open " FILE_NAME);
        return 0;
    }

    if (fprintf(file, "CustomerID,CustomerName,MonthlyIncome,"
                      "TotalCommitments,DTIRatio,Status\n") < 0) {
        success = 0;
    }

    for (index = 0; success && index < count; index++) {
        size_t nameIndex;

        if (fprintf(file, "%d,\"", customerID[index]) < 0) {
            success = 0;
            break;
        }
        for (nameIndex = 0; customerName[index][nameIndex] != '\0';
             nameIndex++) {
            if (customerName[index][nameIndex] == '"') {
                if (fputc('"', file) == EOF) {
                    success = 0;
                    break;
                }
            }
            if (fputc((unsigned char)customerName[index][nameIndex], file) ==
                EOF) {
                success = 0;
                break;
            }
        }
        if (!success) {
            break;
        }

        if (monthlyIncome[index] == 0.0) {
            if (fprintf(file, "\",%.2f,%.2f,N/A,%s\n",
                        monthlyIncome[index], commitments[index],
                        getStatusText(eligibility[index])) < 0) {
                success = 0;
            }
        } else if (fprintf(file, "\",%.2f,%.2f,%.2f,%s\n",
                           monthlyIncome[index], commitments[index],
                           dtiRatio[index],
                           getStatusText(eligibility[index])) < 0) {
            success = 0;
        }
    }

    if (fclose(file) != 0) {
        perror("Unable to close " FILE_NAME);
        success = 0;
    }
    if (!success) {
        perror("Unable to write " FILE_NAME);
    }
    return success;
}

/*
 * Coordinate input, assessment, summary, and file output.
 * Parameters: none.
 * Returns: zero after normal completion.
 */
int main(void)
{
    int customerID[MAX_CUSTOMERS];
    char customerName[MAX_CUSTOMERS][MAX_NAME_LEN];
    double monthlyIncome[MAX_CUSTOMERS];
    double commitments[MAX_CUSTOMERS];
    double dtiRatio[MAX_CUSTOMERS];
    int eligibility[MAX_CUSTOMERS];
    int count = 0;
    int inputEnded = 0;

    printf("==================================================\n");
    printf("      PERSONAL LOAN ELIGIBILITY SYSTEM\n");
    printf("==================================================\n");

    while (count < MAX_CUSTOMERS) {
        int id = getCustomerID();
        double income;
        double customerCommitments;
        int inputOk;

        if (id == 0) {
            break;
        }
        if (id < 0) {
            inputEnded = 1;
            break;
        }
        if (!getCustomerName(customerName[count], MAX_NAME_LEN)) {
            inputEnded = 1;
            break;
        }

        income = getValidatedAmount(
            "Enter Monthly Income (RM0 - RM100000): ",
            MIN_INCOME, MAX_INCOME, &inputOk);
        if (!inputOk) {
            inputEnded = 1;
            break;
        }
        customerCommitments = getValidatedAmount(
            "Enter Total Monthly Commitments (RM0 - RM50000): ",
            MIN_COMMITMENT, MAX_COMMITMENT, &inputOk);
        if (!inputOk) {
            inputEnded = 1;
            break;
        }

        customerID[count] = id;
        monthlyIncome[count] = income;
        commitments[count] = customerCommitments;
        dtiRatio[count] = calculateDTI(income, customerCommitments);
        eligibility[count] = determineEligibility(income, dtiRatio[count]);
        displayResult(customerID[count], customerName[count],
                      monthlyIncome[count], commitments[count],
                      dtiRatio[count], eligibility[count]);
        count++;
    }

    if (inputEnded) {
        printf("\nInput ended; any incomplete record was discarded.\n");
    }
    if (count == MAX_CUSTOMERS) {
        printf("\nMaximum customer capacity reached.\n");
    }

    if (count == 0) {
        printf("No customers processed.\n");
    } else {
        displaySummary(dtiRatio, eligibility, count);
        if (saveToFile(customerID, customerName, monthlyIncome,
                       commitments, dtiRatio, eligibility, count)) {
            printf("Customer records saved successfully to %s\n", FILE_NAME);
        } else {
            printf("Customer records could not be saved to %s\n", FILE_NAME);
        }
    }

    printf("Thank you for using the Personal Loan Eligibility System.\n");
    return 0;
}
