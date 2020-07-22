/*
 @author      : Hussien Saleh 
 Here is a payment system application that asks the user for the card data 
 and interface it with the  terminal setting and server database.
 */

#include "../include/payment-app.h"

/**************************** GLOBAL VARIABLES ***************************************/  

// given primary accounts stored in the server data along with the balance
static ST_accountBalance_t serverData [] =  {{100.00,"123456789"}, {6000.00,"234567891"}, 
                                        {3250.25,"567891234"},{1500.12,"456789123"},
                                        {500.00,"258649173"}, {2100.00,"654823719"},
                                        {0.00,"971362485"}, {1.00,"793148625"},
                                        {10.12,"123123456"},{0.55,"456789321"}};


static ST_cardData_t cardData; // card data
static ST_terminalData_t terminalData; // terminal data
static ST_transaction_t transData; // transaction data
static ST_transaction_t saveTransactions [APPROVED_TRANSACTIONS]; // server data
static uint8 userIn[YES_OR_NO]; // user input yes or no to repeat the process
static uint8 expireYear[DATE_FORMAT_SIZE];  // expiry year 
static uint8 expireMonth[DATE_FORMAT_SIZE]; // expiry month
static uint8 transYear[DATE_FORMAT_SIZE];   // transaction year
static uint8 transMonth[DATE_FORMAT_SIZE];  // transaction month


static uint16 count =0; // counts the number of the approved transactions 

int main(){

    bool userFound = false; // check if the user is found
    bool transComplete = false; // check if the transaction is completed
    float currentBalance = 0.0;   // current balance in the user account
      

    while(true)
    {   
        // enter card holder data
        printf("\nPlease Enter Card Data:\n");
        printf("Please Enter the Card Holder Name: ");
        scanf("\n");
        scanf("%[^\n]%*c", cardData.cardHolderName);
        printf("\nPlease Enter the Primary Account Number: ");
        scanf("%s", cardData.primaryAccountNumber);
        printf("\nPlease Enter Card Expiry Date in MM/YY: ");
        scanf("%s", cardData.cardExpirationDate);
        // enter terminal data
        printf("\nPlease Enter Terminal Data:\n");
        printf("\nPlease Enter Transaction Date in DD/MM/YYYY: ");
        scanf("%s", terminalData.transactionDate);
        // separate the entire date into months and years
        setExpiryDate();
        setTransactionDate();

       
        bool temp_flag1 = checkExpTransYear(); // flag to check if expiry year > transaction year
        bool temp_flag2 = checkSameYears(); // flag to check if expiry year equals transaction year
        bool temp_flag3 = checkExpTransMonth(); // flag to check if expiry month > transaction month

        // enter the transaction amount
        printf("Please Enter the transaction Amount: ");
        scanf("%f", &terminalData.transAmount);
           
        // first check if the card is expired or not
        if ((temp_flag1) || ((temp_flag2) && (temp_flag3)))
        {  
            // check if the amount is accepted by the terminal
            if (isAmountAcceptedByTerminal())
            {   
                // traverse through the server accounts
                for(uint8 index=0; index < MAX_SERVER_ACCOUNTS; index++)
                {   
                    // check if the PAN number exists in the server
                    if(doesPANExist(index))
                    {   
                        userFound = true;
                        currentBalance = serverData[index].balance; // set the current balance to the current user
                    }
                }
                if(userFound)
                {   
                    // check if the transaction amount is accepted by the server
                    if(isAmountAcceptedByServer(currentBalance))
                    {
                        transData.transStat = APPROVED;
                        transComplete = true;
                        displayOut();
                    }
                    else
                    {
                        transData.transStat = DECLINED;
                        displayOut();    
                    }
                        
                }
                else
                {
                    transData.transStat = DECLINED;
                    displayOut(); 
                }
                   
            }
            else
            {
                transData.transStat = DECLINED;
                displayOut(); 
            }
                      
        }
        else 
        {
            transData.transStat = DECLINED;
            displayOut(); 
        }   
        // if transaction process is complete, save it into the server
        if(transComplete)
        {  
            saveIntoServer();
            count++;
            transComplete = false; 
        }
        
        if(userFound)
            userFound = false;

        // asking the user if he wants the process to continue
        printf("\nDo you want to continue (y/n)?: ");
        scanf("%s",userIn);

        if((strcmp(userIn,"n")== 0) ||
          (strcmp(userIn,"no")== 0))
            break;
    }
    // check if the number of approved transactions is less than maximum server capacity then sort them
    if(count < APPROVED_TRANSACTIONS)
        sortTransactions(saveTransactions, count);

    return 0;
}


void displayOut(){

    printf("\nVerifying Data from the server....");
    if(transData.transStat == DECLINED)
    {
        printf("\nThe Transaction is DECLINED\n");
    }
    else if(transData.transStat == APPROVED)
    {
        printf("\nThe Transaction is APPROVED\n");
    }
}


void setExpiryDate(){
    
    // divide the expiry date into separate months and years
    for (uint8 index =0;index < 2; index++)
    {
        expireMonth[index] = cardData.cardExpirationDate[index];
        expireYear[index] = cardData.cardExpirationDate[index+3];
    }   
}
void setTransactionDate(){

    // divide the transaction date into separate months and years
    for (uint8 index =0 ;index < 2; index++){
        transMonth[index] = terminalData.transactionDate[index+3];
        transYear[index] = terminalData.transactionDate[index+8];
    }
}

bool checkExpTransYear()
{
    if (expireYear[0]=='0' && transYear[0] != '0')
        return false;
    if (expireYear[0]==transYear[0])
    {
        if(expireYear[1] > transYear[1])
            return true;
        else 
            return false;
    }
    if (expireYear[0] > transYear[0])
        return true;
    else
        return false;

}

bool checkSameYears(){

    // check if the expiry and transaction years are the same 
    if ((expireYear[0] == transYear[0]) && (expireYear[1] == transYear[1]))
        return true;
    else 
        return false;
}

bool checkExpTransMonth(){

     // check if the expiry month is at a later time of the transaction month
    if (expireMonth[0]=='0' && transMonth[0] != '0')
        return false;
    if (expireMonth[0]== transMonth[0])
    {
        if(expireMonth[1] > transMonth[1])
            return true;
        else 
            return false;
    }
    if (expireMonth[0] > transMonth[0])
        return true;
    else
        return false;

}

bool doesPANExist(uint8 index){

    if(strcmp(cardData.primaryAccountNumber,serverData[index].primaryAccountNumber)==0)
        return true;
    else
        return false;
}

bool isAmountAcceptedByTerminal(){

    terminalData.maxTransAmount = MAX_TRANS_AMOUNT; // maximum transaction amount

    if(terminalData.transAmount > terminalData.maxTransAmount)
        return false;
    else
        return true;

}

bool isAmountAcceptedByServer(float currentBalance){

    if(terminalData.transAmount < currentBalance)
        return true;
    else
        return false;

}


void sortTransactions(ST_transaction_t saveTransactions[], uint16 count){

    ST_transaction_t temp; // temp used for swapping the array values
    printf("\nTransactions after sorting:\n");

    for (uint32 i = 0; i < count; ++i) 
    {
        for (uint32 j = i + 1; j < count; ++j)
        {   
            if((strcmp(saveTransactions[i].cardHolderData.primaryAccountNumber,
                saveTransactions[j].cardHolderData.primaryAccountNumber)) > 0)
            {
                temp = saveTransactions[i];
                saveTransactions[i] = saveTransactions[j];
                saveTransactions[j] = temp;
            }
        }
    }
    // printing the transactions after sorting
    for (uint32 k=0 ; k < count; k++){
        printf("\nPAN: %s , Transaction value: %.2f\n", 
                saveTransactions[k].cardHolderData.primaryAccountNumber, 
                saveTransactions[k].transData.transAmount);
    }
        
}

void saveIntoServer()
{
    for (uint32 index =0; index < PAN_SIZE ; index++)
    {
        saveTransactions[count].cardHolderData.primaryAccountNumber[index] = cardData.primaryAccountNumber[index];
    }
    saveTransactions[count].transData.transAmount = terminalData.transAmount;
            
    printf("\nPAN: %s , Transaction value: %.2f\n", 
        saveTransactions[count].cardHolderData.primaryAccountNumber, 
        saveTransactions[count].transData.transAmount);
}