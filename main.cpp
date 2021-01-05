#include <iostream>
#include <list>
#include <string>
#include <direct.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <ostream>
#include <chrono>


void setUpFolders();
bool checkTransactionHistoryDownloaded();
std::vector<std::string> getBankTransactionInformation();
std::string getCategoryInformation(const std::string& description);
void addCategory(std::string oldDescription, std::string newCategory);
void mergeTransactionsWithCategories(std::vector<std::string> categoryVector );
void backupToCSV();


int main() {
    //This will be filled with all of the "Description" values in transactions.csv
    std::vector<std::string> bankTransactionVector;

    //General directory setup
    setUpFolders();

    //Confirm that there is a transactions.csv in the same directory as the program
    bool userDownloaded;
    userDownloaded = checkTransactionHistoryDownloaded();
    if (!userDownloaded){
        std::cout << "Terminating Program." << std::endl;
        return 0;
    }

    //This is the function that fills the vector with all of the "Description" values in transactions.csv
    bankTransactionVector = getBankTransactionInformation(); // vector of all descriptions in transactions.csv




    std::vector<std::string> bankTransactionsCategorized;
    //Iterate through the vector to see if each value is in categories.csv
    for(auto & bankDescription : bankTransactionVector) {
        if(bankDescription == " Description"){
            continue; //Ignore the header row.
        }
        std::cout << "Attempting to find a category for " << "\"" << bankDescription << "\"" << std::endl;
        std::string category = getCategoryInformation(bankDescription);
        bankTransactionsCategorized.push_back(category);
    }

    mergeTransactionsWithCategories(bankTransactionsCategorized);
    _chdir("Personal Finance Data");
    backupToCSV();

    return 0;
}


void setUpFolders(){
    _mkdir("Personal Finance Data");
    _chdir("Personal Finance Data");
    _mkdir("Statement History");
    _mkdir("Working Files");
    _chdir("..");
}

bool checkTransactionHistoryDownloaded(){
    _chdir("Personal Finance Data");
    std::ifstream file("transactions.csv");
    if (file)
    {
        std::cout << "Transaction history has been located." << std::endl;
        return true;
    }
    else{
        std::cout << "Please download the transaction history and put it into the \"Personal Finance Data\" folder."
        << std::endl;
        return false;
    }
}

std::vector<std::string> getBankTransactionInformation(){
    std::string line, date, time, amount, type, description;
    std::vector <std::string> data;
    _chdir("Personal Finance Data");
    std::ifstream dataList("transactions.csv");

    std::cout << "Iterating through the transaction history." << std::endl;


    while(getline(dataList,line)){
        std::stringstream ss(line);
        getline(ss, date, ',');
        getline(ss, time, ',');
        getline(ss, amount, ',');
        getline(ss, type, ',');
        getline(ss, description, ',');
        if(description == "Description"){
            continue;
        }
        data.push_back(description);

    }

    return data;

}

std::string getCategoryInformation(const std::string& bankDescription){

    std::string returnValue = "null"; //If this value is still null by the end of the function, prompts user to enter category.

    std::string line, description, category;
    _chdir("Personal Finance Data");
    std::ifstream dataList("categories.csv");

    //Iterate through each row in categories.csv
    while(getline(dataList,line)){
        std::stringstream ss(line);
        getline(ss, description, ',');
        //If the bank csv description matches the categories.csv current row's value under description
        if(bankDescription == description){
            //Go one column to the right in the categories.csv and get the vale under "categories"
            getline(ss, category, ',');
            std::cout << "\"" << bankDescription << "\"" << " will be given a category of:" << std::endl << "(" <<
            category << ")" << std::endl;
            returnValue = category;
            return returnValue;
        }

        if(bankDescription.substr(0,5) == "FEDEX"){
            //If the transaction has FEDEX in it, just consider it "Fedex Income"
            std::cout << "\"" << bankDescription << "\"" << " will be given a category of "
            << std::endl << "(Fedex Income)" << std::endl;
            returnValue = "Fedex Income";
            return returnValue;
        }
    }
    //If there is no row with a description of the bank.csv description, create a new category.csv description.
    if (returnValue == "null"){
        std::cout << bankDescription << " does not have a category on file." << std::endl;
        std::cout  << "Please provide a description for " << bankDescription << std::endl;
        std::cin >> returnValue;
        std::cout << "Adding " << returnValue << " to categories.csv." << std::endl;
        addCategory(bankDescription, returnValue);

    }
    return returnValue;

}

void addCategory(std::string oldDescription, std::string newCategory){
    _chdir("Personal Finance Data");


    std::ofstream file;
    file.open("Categories.csv", std::ios_base::app);
    file << oldDescription << ',' << newCategory << "\n";
    file.close();
    _chdir("..");

    std::cout << "The category has been added." << std::endl;

}

void backupToCSV(){

    //Get the date as a string, convert it to constant character so that the "rename" function accepts it.
    time_t t = time(0);
    struct tm * timeStruct = localtime(&t);
    std::string dateToReturn = std::to_string(timeStruct->tm_mon) + std::to_string(timeStruct->tm_mday)
            + std::to_string(timeStruct->tm_year) +".csv";
    const char * dateValue = dateToReturn.c_str();


    std::string line, find, transaction;

    //Get Data from "transactions.csv"
    std::vector <std::string> data;
    std::ifstream dataList("transactions.csv");
    std::string input;


    _chdir("Statement History");

 


    std::ifstream myFile; //for reading records


    std::ofstream temp;
    temp.open(dateValue);
    while (getline(dataList, line))
    {
        temp << line << std::endl;
    }
    dataList.close();
    temp.close();

    _chdir("..");
    std::remove("transactions.csv");

}

void mergeTransactionsWithCategories(std::vector<std::string> categoryVector ){
    std::string line, find, transaction;
    std::ifstream myFile;

    myFile.open("transactions.csv");

    std::ofstream temp;
    temp.open("temp.csv");

    int i = 0;
    while (getline(myFile, line)){
        if (i == 0){
            temp << line << ", Category" << std::endl;
            i+= 1;
        }
        else{
            temp << line << ", " << categoryVector[i-1] << std::endl;
            i += 1;
        }
    }
    myFile.close();
    temp.close();
    std::remove("transactions.csv");
    std::rename("temp.csv", "transactions.csv");


}