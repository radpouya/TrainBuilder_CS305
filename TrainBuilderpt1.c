// Pouya Rad
// James Schmidt
// CS 305 A
// Programming Assignment #3
// 10/10/16
// Train Builder pt. 1
//
// This program constructs a Train of cars and makes sure that each
// car is aware of the one in front of it and behind it. 
// Note: I implemented a change that I think would be beneficial. The batch
// and manual state methods now return a trainCar pointer. I did this in case
// the user wanted to load a pre-existing train and then using the control
// loop in the manual mode, change the loaded train by editing it and such. 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure of a TrainCar Node
struct trainCar {
	int carID;
	char* owner;
	double weight; 
	int position;
	struct trainCar* nextCar;
	struct trainCar* prevCar;
};

// Method Declarations
struct trainCar* manualState(struct trainCar*);
struct trainCar* batchState(struct trainCar*, char* csvPath);
struct trainCar* newCar(int carID, char* owner, double weight);
struct trainCar* insertTrainCar(struct trainCar* engine, struct trainCar* newCar, int position);
struct trainCar* deleteTrainCar(struct trainCar* delete, int carID);
int trainToCSV(struct trainCar* engine, char* csvPath);
struct trainCar* batchCommand(char, struct trainCar*, char*);
struct trainCar* checkOrder(struct trainCar*);


// Main method. Runs in either a manual state (where user inputs all values for the train)
// or a batch state (where a provided file is read to create the train)
int main(int argc, char* args[])
{
	struct trainCar* mainCar = malloc(sizeof(struct trainCar));
	mainCar = NULL;
	if(argc != 2)
	{
		mainCar = manualState(mainCar);
	} else 
	{
		mainCar = batchState(mainCar, args[1]);
		printf("Would you like to edit the train you just uploaded? \n");
		printf("Press \"y\" for yes and \"n\" to exit the program. ");
		char choice;
		scanf(" %c", &choice);
		if(choice == 'y')
		{
			mainCar = manualState(mainCar);
		} else {
			printf("Goodbye.\n");
		}
	}	
}

// This is the method for the manual state. It accepts one parameter and that
// is the main train for that instance of the program running.
struct trainCar* manualState(struct trainCar* mainCar) 
{
	printf("Welcome to the Train Builder.\n");
	int loopControl = 0;
	while(loopControl == 0)
	{
		char c;
		printf("Would you like to print a set of menu options?\n");
		printf("Type \"y\" for yes and \"n\" for no and to exit the program.\n");
		scanf(" %c", &c);
		if(c == 'y') 
		{
			printf("Here are your menu options:\n");
			printf("	1. Insert a Car\n");
			printf("	2. Remove a Car\n");
			printf("	3. Print Cars to a CSV File\n");
			printf("Please type the option number you wish to do.\n");
			int choice;
			scanf(" %d", &choice);
			if(choice == 1)
			{
				struct trainCar* tempCar = malloc(sizeof(struct trainCar));
				printf("What is the Car ID? ");
				int carID;
				scanf(" %d", &carID);
				printf("Who is the owner? ");
				char owner[1000];
				scanf(" %[^\n]s", owner);
				printf("What is the car's weight? ");
				double weight;
				scanf(" %lf", &weight);
				tempCar = newCar(carID, owner, weight);
				printf("At what position should this car be? ");
				int pos;
				scanf(" %d", &pos);
				mainCar = insertTrainCar(mainCar, tempCar, pos);
			} else if(choice == 2)
			{
				printf("What is the Car ID for the car that you wish to remove? ");
				int carID;
				scanf(" %d", &carID);
				mainCar = deleteTrainCar(mainCar, carID);
			} else if(choice == 3) 
			{
				printf("What is the name of the text file that you wish to print the entire train to? ");
				char filePath[1000];
				scanf(" %[^\n]s", filePath);
				trainToCSV(mainCar, filePath);
			} else 
			{
				printf("Not a valid choice. Exiting program.\n");
				loopControl = 1;
			}

		} else 
		{
			loopControl = 1;
		}

	}	
	return mainCar;
}

// This method creates a new car with the Car ID, who the owner is, and the weight
// all of which were inputted by the user. 
struct trainCar* newCar(int carID, char* owner, double weight)
{
	struct trainCar* newCar = malloc(sizeof(struct trainCar));
	newCar->carID = carID;
	newCar->owner = malloc(strlen(owner));
	strcpy(newCar->owner, owner);
	newCar->weight = weight;
	newCar->position = -1;
	newCar->nextCar = NULL;
	newCar->prevCar = NULL;
	return newCar;
}

// This method is given the head of the train and a new car to insert in its respective and correct
// position in the train, which is also given.
struct trainCar* insertTrainCar(struct trainCar* engine, struct trainCar* newCar, int position)
{
	newCar->position = position;
	struct trainCar* curr = engine;
	struct trainCar* p = NULL;
	if(engine == NULL)
	{
		return newCar;
	} else {
		int currPos = 1;
		while(curr->nextCar != NULL)
		{
			if(currPos == position)
			{
				if(p != NULL)
				{
					p->nextCar = newCar;
					newCar->prevCar = p;
					newCar->nextCar = curr;
					curr->prevCar = newCar;
					break;
				} else 
				{
					curr->prevCar = newCar;
					newCar->nextCar = curr;
					newCar->prevCar = p;
					engine = newCar;
					break;
				}
			}
			p = curr;
			curr = curr->nextCar;
			currPos++;
		}
		if(curr->nextCar == NULL)
		{
			if(position > currPos)
			{
				curr->nextCar = newCar;
				newCar->prevCar = curr;
				newCar->nextCar = NULL;
			} else if(position <= currPos)
			{
				curr->prevCar = newCar;
				newCar->nextCar = curr;
				newCar->prevCar = p;
				p->nextCar = newCar;
			}
		} 
	}
	engine = checkOrder(engine);
	return engine;
}

// This method deletes a car in the train. 
// It is given the whole train and the car ID of the car that is to be 
// deleted. After going through the train and removing the car it then 
// returns the train. 
struct trainCar* deleteTrainCar(struct trainCar* delete, int carID)
{
	if(delete == NULL)
	{
		printf("No cars to delete.\n");
		return NULL;
	} else if(delete->nextCar == NULL) {
		if(delete->carID == carID)
		{
			free(delete);
			return NULL;
		} else {
			delete->position = 1;
			return delete;
		}
	} else {
		struct trainCar* curr = delete;
		struct trainCar* p = NULL;
		while(curr != NULL)
		{
			if(curr->carID == carID)
			{
				if(curr->prevCar == NULL) // first car in train
				{
					curr = curr->nextCar;
					curr->prevCar = NULL;
					delete = delete->nextCar;
					free(curr);
					break;
				} else if(curr->nextCar != NULL) // middle car
				{
					p->nextCar = curr->nextCar;
					curr->nextCar->prevCar = p;
					curr->nextCar = NULL;
					curr->prevCar = NULL;
					curr = NULL;
					free(curr);
					break;
				} else if(curr->nextCar == NULL) // last car
				{
					p->nextCar = NULL;
					curr->nextCar = NULL;
					curr->prevCar = NULL;
					free(curr);
					break;
				}
			} 
			p = curr;
			curr = curr->nextCar;
		}
	}
	delete = checkOrder(delete);	
	return delete;
}

// This method prints the entire train to a Comma Seperated Value file. 
// At accepts the beginning of the train and the name of the output file. 
// Then goes through the train and prints to the printstream. 
// It returns an arbitrary integer. 
int trainToCSV(struct trainCar* engine, char* csvPath) 
{
	char* constCSVPath = csvPath;
	FILE* trainOutput = fopen(constCSVPath, "w");
	fprintf(trainOutput, "CarID,Owner,Weight,Position\n");
	struct trainCar* curr = engine;
	while(curr->nextCar != NULL)
	{
		fprintf(trainOutput, "%d,%s,%lf,%d\n", curr->carID, curr->owner, curr->weight, curr->position);
		curr = curr->nextCar;
	}
	fprintf(trainOutput, "%d,%s,%lf,%d\n", curr->carID, curr->owner, curr->weight, curr->position);
	fclose(trainOutput);
	return 0;
}

// This method accepts a file name from the command line arguments with the 
// program is executed and uses that file as a source of input values to build
// a train, as opposed to the user manually entering all of the values. 
// It then returns the completed car. 
struct trainCar* batchState(struct trainCar* mainCar, char* csvPath)
{
	FILE* input;
	input = fopen(csvPath, "r");
	char buff[1000];
	char* line = fgets(buff, 999, input);
	char currCommand;
	while(line != NULL)
	{
		if(line[0] == '=') // assumes format of commands is maintained
		{
			if(line[1] == 'I')
			{		
				currCommand = 'I';
				char buff2[1000]; // takes in the second line and knows that its just the order for CSV values
				char* line2 = fgets(buff2, 999, input);
				char buff3[1000];
				char* line3 = fgets(buff3, 999, input);
				mainCar = batchCommand(currCommand, mainCar, line3);
			} else if(line[1] == 'R')
			{
				char buff2[1000]; // takes in the second line and knows that its just the order for CSV values
				char* line2 = fgets(buff2, 999, input);
				char buff3[1000];
				char* line3 = fgets(buff3, 999, input);
				currCommand = 'R';
				mainCar = batchCommand(currCommand, mainCar, line3);
			} else if(line[1] == 'P')
			{
				char buff2[1000];
				char* line2 = fgets(buff2, 999, input);
				currCommand = 'P';
				mainCar = batchCommand(currCommand, mainCar, line2);
			} 
		} else {
			if(line[1] != '\0')
			{
				mainCar = batchCommand(currCommand, mainCar, line);
			}
		} 
		line = fgets(buff, 999, input);
	} 
	fclose(input);
	return mainCar;
}

// This is a helper method for batch. It ensures that the current command is
// still run until another command line (starting with '=') is reached. 
struct trainCar* batchCommand(char currCommand, struct trainCar* mainCar, char* currLine)
{
	if(currCommand == 'I')
	{	
		struct trainCar* tempCar = malloc(sizeof(struct trainCar));
		const char delim[2] = ",";
		char* token;
		char* ptr;
		token = strtok(currLine, delim);
		tempCar->carID = atoi(token);

		token = strtok(NULL, delim);
		//tempCar->owner = token; 
		tempCar->owner = malloc(strlen(token));
		strcpy(tempCar->owner, token);

		token = strtok(NULL, delim);
		tempCar->weight = atof(token);

		token = strtok(NULL, delim);
		tempCar->position = atoi(token); 

		mainCar = insertTrainCar(mainCar, tempCar, tempCar->position); 
		return mainCar;
	} else if(currCommand == 'R')
	{
		int removeCarID = atoi(currLine);
		mainCar = deleteTrainCar(mainCar, removeCarID);
		return mainCar;
	} else if(currCommand == 'P')
	{
		trainToCSV(mainCar, currLine);
		return mainCar;
	} else 
	{
		return mainCar;
	}
}

// This method starts at the beginning of the train and goes through it to
// ensure that each position in the train is rightfully assigned before returning it. 
struct trainCar* checkOrder(struct trainCar* engine)
{
	int newPos = 1;
	struct trainCar* curr = engine;
	while(curr != NULL)
	{
		curr->position = newPos;
		curr = curr->nextCar;
		newPos++;
	}
	return engine;
}
