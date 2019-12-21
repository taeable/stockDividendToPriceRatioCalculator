#include <iostream> // used for printing outputs
#include <fstream> // used for opening a file
#include <string> // used for string types
#include <stdlib.h> // used for system calls

#define STOCKLISTFILE "symList.txt"
int main(){
	//string to hold stock sym read from stockList line by line
	std::string stockSym;
	//system call
	std::string command;
	std::string command1("curl https://api.nasdaq.com/api/quote/");
	std::string command2;
	std::string command3("/info?assetclass=stocks -s | tee ./stockJsonTemp/");
	std::string command4;
	std::string command5(".json");
	std::string command6("/dividends?assetclass=stocks -s | tee ./stockJsonTemp/");
	std::string command7("dividends");
	//dividends and price
	double dividends, price, dividendsToPriceRatio;
	std::string dividendsString, priceString;
	int firstSubstring, secondSubstring;
	//open file
	std::ifstream stockList(STOCKLISTFILE);
	std::ofstream recordData("stockRecord.csv");
	//read line by line
	while (std::getline(stockList, stockSym)){
		command2 = stockSym;
		command4 = stockSym;

		std::cout << "recording pricing results for " << stockSym << std::endl;
		command = command1 + command2 + command3 + command4 + command5;
		system(command.c_str());
		std::cout << std::endl;

		std::cout << "recording dividends results for " << stockSym << std::endl;
		command = command1 + command2 + command6 + command4 + command7 + command5;
		system(command.c_str());
		std::cout << std::endl;

		//parse the json files created #NOTE better to get the json variables straight instead of making files...
		//get price
		std::ifstream parsingFile("./stockJsonTemp/"+ command4 + command5);
		std::getline(parsingFile, priceString);
		firstSubstring = priceString.find("lastSalePrice");
		secondSubstring = priceString.find("\",\"netChange\":\"");
		priceString = priceString.substr(firstSubstring+17,secondSubstring-firstSubstring-17);
		price = stod(priceString);
		parsingFile.close();
		//get dividends
		std::ifstream parsingFile2("./stockJsonTemp/" + command4 + "dividends" + command5);
		std::getline(parsingFile2, dividendsString);
		firstSubstring = dividendsString.find("amount\":\"$");
		secondSubstring = dividendsString.find("declarationDate", firstSubstring);
		//if dividends data exists
		if(firstSubstring != -1 && secondSubstring != -1){
			dividendsString = dividendsString.substr(firstSubstring+10, secondSubstring-firstSubstring-10-3);
			dividends = stod(dividendsString);
			dividendsToPriceRatio = dividends/price;
			if(recordData.is_open())
				recordData << stockSym << ',' << priceString << ',' << dividendsString << ',' << dividendsToPriceRatio << '\n';
		}
		//std::cin.ignore();
		parsingFile2.close();
		
	}
	
	//close file
	recordData.close();
	stockList.close();

	return 0;
}
