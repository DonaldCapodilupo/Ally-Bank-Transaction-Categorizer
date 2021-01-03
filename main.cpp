#include <iostream>
#include <list>
#include <string>
#include <direct.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <istream>
#include <ostream>
#include <string>


void setUpFolders();
bool checkTransactionHistoryDownloaded();
std::vector<std::string> getBankTransactionInformation();
std::string getCategoryInformation(const std::string& description);
void addCategory(std::string oldDescription, std::string newCategory);
void backupCSV(const char *const);

std::vector<std::string> getVectorOfCategories();



int main() {
    //This will be filled with all of the "Description" values in transactions.csv
    std::vector<std::string> bankTransactionVector;

    //General directory setup
    setUpFolders();

    //Confirm that there is a transactions.csv in the same directory as the program
    bool userDownloaded;
    userDownloaded = checkTransactionHistoryDownloaded();
    if (userDownloaded == false){
        std::cout << "Terminating Program." << std::endl;
        return 0;
    }

    //This is the function that fills the vector with all of the "Description" values in transactions.csv
    bankTransactionVector = getBankTransactionInformation(); // vector of all descriptions in transactions.csv

    //Backup the transaction.csv
    std::string backupTitle;
    std::cout << "What is today's date? (Used to create the backup file)" << std::endl;
    std::cin >> backupTitle;

    backupCSV(backupTitle);


    //Iterate through the vector to see if each value is in categories.csv
    for(auto & bankDescription : bankTransactionVector) {
        if(bankDescription == " Description"){
            continue; //Ignore the header row.
        }
        std::cout << "Attempting to find a category for " << "\"" <<bankDescription << "\"" << std::endl;
        std::string category = getCategoryInformation(bankDescription);
    }



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

std::vector<std::string> getVectorOfCategories(){

    std::string line, date, time, amount, type, description;
    std::vector <std::string> categoryList;
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
        categoryList.push_back(getCategoryInformation(description));

    }

    for(auto & i : categoryList) {
        std::cout << i << std::endl;
    }
    return categoryList;
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

void backupCSV(const char *const fileName){
    std::string line, find, transaction;

    //Get Data from "Data.csv"
    std::vector <std::string> data;
    std::ifstream dataList("transactions.csv");
    std::string input;
    while (getline(dataList, input)){
        data.push_back(input);
    }
    dataList.close();

    _chdir("Statement History");

 


    std::ifstream myFile; //for reading records

    myFile.open(fileName);

    std::ofstream temp;
    temp.open("temp.csv");
    while (getline(myFile, line))
    {
        if (line != transaction)
            temp << line << std::endl;
    }
    myFile.close();
    temp.close();
    std::remove(fileName);
    std::rename("temp.csv", fileName);

}