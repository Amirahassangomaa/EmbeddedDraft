#include "server.h"
#include "../Terminal/terminal.h"
#include "../Terminal/terminal.h"

#define MAX_TRANS 255


ST_accountsDB_t accountsDB[MAX_TRANS] =
{
	{2000.0, RUNNING, "8989374615436851"},
	{100000.0, BLOCKED, "5807007076043875"},
	{ 4000.0, RUNNING, "4903478079245246" },
	{ 4000.0, BLOCKED, "8751876247081216" },
	{ 6000.0, RUNNING, "4719802455979296" },
	{ 6000.0, BLOCKED, "6907940726884180" },
	{ 8000.0, RUNNING, "6263565997807508" },
	{ 8000.0, BLOCKED, "3358065963483406" },
	{ 10000.0, RUNNING, "4068975151313987" },
	{ 10000.0, BLOCKED, "2652623873444840" }
};






// Global array to store transactions
ST_transaction_t transactionDB[MAX_TRANS] = { 0 };

/////////////////////////////implement recieveTransactionData function ////////////////////////////////////////////// 

EN_transState_t recieveTransactionData (ST_transaction_t)* transData) {
    // 1. Validate card existence
    int accountIndex = isValidAccount(&transData->cardHolderData, accountsDB);
    if (accountIndex == -1) {
        return FRAUD_CARD;
    }

    // 2. Check if account is blocked
    if (accountsDB[accountIndex].state == BLOCKED) {
        return DECLINED_STOLEN_CARD;
    }

    // 3. Check available balance
    if (accountsDB[accountIndex].balance < transData->terminalData.amount) {
        return DECLINED_INSUFFECIENT_FUND;
    }

    // 4. Update balance
    accountsDB[accountIndex].balance -= transData->terminalData.amount;

    // 5. Save transaction
    EN_serverError_t saveResult = saveTransaction(transData);
    if (saveResult != SERVER_OK) {
        return INTERNAL_SERVER_ERROR;
    }

    // 6. Transaction successful
    return APPROVED;
}

// Test function for recieveTransactionData
void recieveTransactionDataTest(void) {
    printf("Tester Name: Amira Hassan\n");
    printf("Function Name: recieveTransactionData\n\n");

    ST_transaction_t transData;  // Variable to hold transaction data

    // Test Case 1: Happy scenario (valid transaction)
    printf("Test Case 1:\n");
    printf("Input Data: Valid transaction data (account exists, sufficient funds, not blocked)\n");
    printf("Expected Result: APPROVED\n");
    strcpy(transData.cardHolderData.primaryAccountNumber, "4068975151313987"); // Existing account number
    transData.terminalData.transactionAmount = 2000.0;  // Valid transaction amount
    EN_transState_t result = recieveTransactionData(&transData);  // Call the function
    printf("Actual Result: %d\n\n", result);  // Print actual result

  

    // Test Case 2: Account does not exist
    printf("Test Case 2:\n");
    printf("Input Data: Non-existent account data\n");
    printf("Expected Result: FRAUD_CARD\n");
    strcpy(transData.cardHolderData.primaryAccountNumber, "1234567890123456"); // Non-existent account number
    transData.terminalData.transactionAmount = 2000.0;  // Valid transaction amount
    result = recieveTransactionData(&transData);  // Call the function
    printf("Actual Result: %d\n\n", result);  // Print actual result

    
}


/////////////////////////////implement isValidAccount function //////////////////////////////////////////////


EN_serverError_t isValidAccount (ST_cardData_t* cardData, ST_accountsDB_t* accountRefrence) {
    // Iterate through accounts to find matching PAN
    for (int i = 0; i < MAX_TRANS; i++) {
        if (strcmp(accountsDB[i].primaryAccountNumber, cardData->primaryAccountNumber) == 0) {
            return SERVER_OK;  // Account found, return success
        }
    }
    return ACCOUNT_NOT_FOUND;  // Account not found
}

// Test function for isValidAccount
void isValidAccountTest(void) {
    printf("Tester Name: Amira Hassan\n");
    printf("Function Name: isValidAccount\n\n");

    // Test Case 1: Happy scenario (existing account)
    printf("Test Case 1:\n");
    printf("Input Data: Valid card data (account exists in DB)\n");
    printf("Expected Result: SERVER_OK\n");
    ST_cardData_t validCard = { "4903478079245246" }; // Account from test cases in main
    EN_serverError_t result = isValidAccount(&validCard, accountsDB);
    printf("Actual Result: %s\n\n", result == SERVER_OK ? "SERVER_OK" : "Error");

    // Test Case 2: Non-existent account
    printf("Test Case 2:\n");
    printf("Input Data: Card data with non-existent account number\n");
    printf("Expected Result: ACCOUNT_NOT_FOUND\n");
    ST_cardData_t nonExistentCard = { "1234567890123456" };
    result = isValidAccount(&nonExistentCard, accountsDB);
    printf("Actual Result: %s\n\n", result == ACCOUNT_NOT_FOUND ? "ACCOUNT_NOT_FOUND" : "Error");
}


/////////////////////////////implement isBlockedAccount function //////////////////////////////////////////////

EN_serverError_t isBlockedAccount (ST_accountsDB_t* accountRefrence) {
    if (accountRefrence->state == RUNNING) {
        return SERVER_OK;  // Account is running
    }
    else {
        return BLOCKED_ACCOUNT;  // Account is blocked
    }
}

void isBlockedAccountTest(void) {

    printf("Tester Name: Amira Hassan\n");
    printf("Function Name: isBlockedAccount\n\n");

    // Test Case 1: Running account
    printf("Test Case 1:\n");
    printf("Input Data: Account reference to a running account\n");
    printf("Expected Result: SERVER_OK\n");
    ST_accountsDB_t runningAccount = { 1000.0, RUNNING, "1234567890123456" };
    EN_serverError_t result = isBlockedAccount(&runningAccount);
    printf("Actual Result: %s\n\n", result == SERVER_OK ? "SERVER_OK" : "Error");

    // Test Case 2: Blocked account
    printf("Test Case 2:\n");
    printf("Input Data: Account reference to a blocked account\n");
    printf("Expected Result: BLOCKED_ACCOUNT\n");
    ST_accountsDB_t blockedAccount = { 5000.0, BLOCKED, "9876543210123456" };
    result = isBlockedAccount(&blockedAccount);
    printf("Actual Result: %s\n\n", result == BLOCKED_ACCOUNT ? "BLOCKED_ACCOUNT" : "Error");
}