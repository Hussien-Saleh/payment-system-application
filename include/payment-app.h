#ifndef PAYMENT_APP_H
#define PAYMENT_APP_H

#include"typedefs.h"

/******************************* GLOBAL VARIABLES *******************************/

#define MAX_TRANS_AMOUNT  5000.00f // maxium amount allowed for transaction process
#define MAX_SERVER_ACCOUNTS 10 // maximum accounts in the server
#define PAN_SIZE 20 // pan number size
#define DATE_FORMAT_SIZE 2 // MM : YY format size
#define YES_OR_NO  4
#define APPROVED_TRANSACTIONS 30 // saved approved transactions array size

/********************************* STRUCTS ****************************************/

// card data
typedef struct ST_cardData_t{

    uint8 cardHolderName[25]; // name written on the card 24 chars
    uint8 primaryAccountNumber[20]; // PAN primary account number, array of 19 numbers
    uint8 cardExpirationDate[6];    // card expiry date in format of MM/YY

}ST_cardData_t;

// terminal date
typedef struct ST_terminalData_t{

    float transAmount;      // amount entered by the user 
    float maxTransAmount;   // maximum transaction amount.
    uint8 transactionDate[11]; // transaction date format DD/MM/YYYY

}ST_terminalData_t;


// transacation status
typedef enum EN_transStat_t{

    DECLINED, // process is declined
    APPROVED   // process is approved

}EN_transStat_t;

//transaction data 
typedef struct ST_transaction_t{

    ST_cardData_t cardHolderData; // card holder data
    ST_terminalData_t transData; // transaction data
    EN_transStat_t transStat; // transaction state; APPROVED or DECLINED

}ST_transaction_t;

//server data, only 10 accounts stored in global array
typedef struct ST_accountBalance_t
{
    float balance; // current balance in the user account    
    uint8 primaryAccountNumber[20]; // PAN number of the user

}ST_accountBalance_t;


/**************************** PROTOTYPES***************************************/  

/*
Function used to sort the approved transactions and print them in ascending order  
@param saveTransactions, pointer to the transaction data
@param count, counts the number of approved transactions
@return 
*/

void sortTransactions(ST_transaction_t* saveTransactions, uint16 count);

/*
Function used to display the process output whether is approved or declined
@return 
*/

void displayOut();

/*
Function used to separate the expiry date entired by the user into months and years formats 
@return 
*/

void setExpiryDate();

/*
Function used to separate the transaction date entired by the user into months and years formats 
@return 
*/

void setTransactionDate();

/*
Function used to compare between the expiry year and the transaction year
@return true if the expiry year is at a later time than the transaction year
*/

bool checkExpTransYear();

/*
Function used to compare between the expiry month and the transaction month
@return true if the expiry month is at a later time than the transaction month
*/

bool checkExpTransMonth();


/*
Function used to compare between the expiry year and the transaction year
@return true if the expiry year equals the transaction year
*/

bool checkSameYears();

/*
Function used to check if the amount with the terminal
@return true if the amount is accepted by the terminal
*/

bool isAmountAcceptedByTerminal();

/*
Function used to check if the amount with the server
@param currentBalance, the balance of the current user
@return true if the amount is accepted by the server
*/

bool isAmountAcceptedByServer(float currentBalance);

/*
Function used to save the approved transactions into server
@param count, the number of the approved transactions
*/

void saveIntoServer();

/*
Function used to check if the PAN exists in the server
@param index, the position of the current card holder in the server
@return true if the pan number is valid
*/

bool doesPANExist(uint8 index);

#endif