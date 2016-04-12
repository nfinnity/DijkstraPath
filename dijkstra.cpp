#include "queue.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "graph.h"
#include <cstdlib>
#include <sstream>
#include <vector>
#include <climits>
#include <iomanip>
#include <stack>
#include <set>
#include <map>

using namespace std;

#include "SeparateChaining.h"


struct VertexType
{
	string name;
	bool marked;
	int distance;
	string previous;	
};

void insertionSort(string cities[], int numVertices);
void checkInFile(int argc);
void buildDGraph(Graph<string> &dGraph, string filename, string cities[], int &numVertices, int &numEdges, VertexType myVertices[]);
void printCities(Graph<string> dGraph, int numVertices, VertexType myVertices[], string cities[]);
bool cityExists(string city, VertexType myVertices[], int numVertices);
int indexOfVertex(int numVertices, VertexType myVertices[], string name);
string setStart(string start, Graph<string> &dGraph, int numVertices, VertexType myVertices[]);
void printRow(int location, VertexType myVertices[]);
int findMin(VertexType myVertices[], int numVertices);
map<int, string> findCycle(string pointer, string pointee, int numVertices, VertexType myVertices[], Graph<string> dGraph);
void dijkstra(Graph<string> &dGraph, int numVertices, VertexType myVertices[]);
bool allMarked(int numVertices, VertexType myVertices[]);
void assignWeights(Graph<string> &dGraph, int numVertices, VertexType myVertices[], int startingLocation, Queue<string>& getTo);
void printMultipleCycle(map<int, string> path);
void print2Cycle(int numVertices, VertexType cycleVertices[], Graph<string> dGraph);
map<int, string> pathArray[1000];
bool cycleGreeting = false;

int main(int argc, char* argv[])
{
	checkInFile(argc);	// Checks to see if Filename is given

	// Creation of Input File:
	string filename = argv[1];
	Graph<string> dGraph; 
	int numVertices = 0;
	int numEdges;
	VertexType myVertices [50];
	string cities [50];
	buildDGraph(dGraph, filename, cities, numVertices, numEdges, myVertices);	// Builds the Graph using the data from the external file.
	printCities(dGraph, numVertices, myVertices, cities);	// Prints Cities found in file
	dijkstra(dGraph, numVertices, myVertices);	// Computes Dijkstra's Algorithm:
}

// Checks to see if Filename is given:
void checkInFile(int argc)
{
	if (argc < 2)
	{
		cout << "Filename was not given." << endl;
		exit(0);
	}
}

// Computes Dijkstra's Algorithm:
void dijkstra(Graph<string> &dGraph, int numVertices, VertexType myVertices[])
{
	string emptyString = "";
	string currentName = setStart(emptyString, dGraph, numVertices, myVertices);
	int currentIndex;
	Queue<string> adjacencyQueue;
	map<int, string> path;
	int currIndex = indexOfVertex(numVertices, myVertices, currentName);

	//Declare current vertex and set it as marked
	myVertices[currIndex].marked = true;
	myVertices[currIndex].distance = 0;
	int cycles = 0;


	while (!(allMarked(numVertices, myVertices)))
	{
		//Create queue of all vertices adjacent to given vertex
		dGraph.GetToVertices(myVertices[currIndex].name, adjacencyQueue);

		while(!(adjacencyQueue.isEmpty()))
		{
			string adjacencyName = adjacencyQueue.dequeue();
			int adjIndex = indexOfVertex(numVertices, myVertices, adjacencyName);
			if(myVertices[adjIndex].marked == false)
			{
				int totalDistance = myVertices[currIndex].distance + dGraph.WeightIs(currentName, adjacencyName);
				if(totalDistance < myVertices[adjIndex].distance)
				{
					myVertices[adjIndex].distance = totalDistance;
					myVertices[adjIndex].previous = currentName;
				}
			}
			else
			{
				path = findCycle(currentName, adjacencyName, numVertices, myVertices, dGraph);
				printMultipleCycle(path);
				cycles++;
			}
		}
		currIndex = findMin(myVertices, numVertices);
		if (currIndex != -1)
			currentName = myVertices[currIndex].name;
		else 
			break;
		myVertices[currIndex].marked = true;
	}

	print2Cycle(numVertices, myVertices, dGraph);

	int numCitiesOrdered = 0;
	bool wasChosen[numVertices];

	for(int i = 0; i < numVertices; i++)
		wasChosen[i] = false;
	cout << endl;

	while(numCitiesOrdered < numVertices)
	{
		int indexOfMinimum = 0;
		int minimum = INT_MAX;
		for(int i = 0; i < numVertices; i++)
		{
			if ((myVertices[i].distance < minimum) && (wasChosen[i] == false))
			{
				indexOfMinimum = i;
				minimum = myVertices[i].distance;
			}
		}
		printRow(indexOfMinimum, myVertices);
		wasChosen[indexOfMinimum] = true;
		numCitiesOrdered++;
	}
}

void print2Cycle(int numVertices, VertexType myVertices[], Graph<string> dGraph)
{
	//This is the section for cycles that contain cycles between two nodes. 
	//It will be detected and the first one found will be sent as output.
	//If there are none in here, then the graph does not contain cycles
	//between two nodes and has at least three nodes in the cycle(s)

	//that exist(s).
	VertexType cycleVertices[numVertices];
	for (int i = 0; i < numVertices; i++)
	{
		VertexType newVertex;
		newVertex.name = myVertices[i].name;
		newVertex.marked = false;
		newVertex.distance = 0;
		newVertex.previous = "";
		cycleVertices[i] = newVertex;
	}
	int spaceIndex = 0;
	for (int i = 0; i < numVertices; i++)
	{
		for (int j = 0; j < numVertices; j++)
		{
			if((cycleVertices[i].name != "DONE") && (cycleVertices[j].name != "DONE"))
				if((dGraph.WeightIs(myVertices[i].name, myVertices[j].name) > 0) && (dGraph.WeightIs(myVertices[j].name, myVertices[i].name) > 0))
				{
					string vertex1 = cycleVertices[i].name;
					string vertex2 = cycleVertices[j].name;
					cout << vertex2 << "   ------>   " << vertex1 << endl;
					cout << vertex1 << "   ------>   " << vertex2 << endl;
					cout << endl;
					cycleVertices[i].name = "DONE";
					cycleVertices[j].name = "DONE";
					spaceIndex++;
				}
		}
		
	}
}

void printMultipleCycle(map<int, string> path)
{
	if (path.size() > 0)
	{
		for (int i = 0; i < path.size(); i++)
		{
			string edge = path[i];
			string vertex1 = edge.substr(0, edge.find(";"));
			string vertex2 = edge.substr(edge.find(";") + 1, edge.length());
			if((vertex1 != "") && (vertex2 != ""))
				cout << vertex2 << "   ------>   " << vertex1 << endl;
		}
	}
	cout << endl;
}


int indexOfVertex(int numVertices, VertexType myVertices[], string name)
{
	int i;
	for(i = 0; i < numVertices; i++)
		if (myVertices[i].name == name)
		{
			break;
		}
		return i;
}

map<int, string> findCycle(string pointer, string pointee, int numVertices, VertexType myVertices[], Graph<string> dGraph)
{
	string key = pointee;
	bool foundCycle = false;
	bool weightFound = false;
	map<int, string> path;
	VertexType cycleVertices[numVertices];
	int pathEdge = 0;
	int currIndex = indexOfVertex(numVertices, myVertices, pointee);
	int prevIndex = 0;
	int i = 0;
	int runTime = 0;

	if (cycleGreeting = false)
	{
		cout << endl << endl << "THIS GRAPH CONTAINS A CYCLE! CONTAINS CYCLE(S): " << endl;
		cycleGreeting = true;
	}
	//Edit cycleVertices[]'s components and fills it up.
	for (int i = 0; i < numVertices; i++)
	{
		VertexType newVertex;
		newVertex.name = myVertices[i].name;
		newVertex.marked = false;
		newVertex.distance = 0;
		newVertex.previous = "";
		cycleVertices[i] = newVertex;
	}

	while ((foundCycle == false) && (runTime < 7))
	{
			Queue<string> adjacencyQueue;
			i = 0;
			dGraph.GetToVertices(myVertices[currIndex].name, adjacencyQueue);
			Queue<string> copy = adjacencyQueue;
			while (!(copy.isEmpty()))
			{
				string vertex = copy.dequeue();
				if(vertex == pointee)
				{	
					weightFound = true;
					i = indexOfVertex(numVertices, cycleVertices, vertex);
					break;
				}
			}
			while ((i < numVertices) && (weightFound == false))
			{
			
				if ((cycleVertices[i].name != "dead end"))
				{
					if(dGraph.WeightIs(cycleVertices[currIndex].name, cycleVertices[i].name) > 0)
						weightFound = true;
					else
						i++;
				}
				else
					i++;
			}
			if (weightFound == true)
			{
				prevIndex = currIndex;
				currIndex = i;
				if(pointee == cycleVertices[currIndex].name)
				{
					foundCycle = true;
					string vertex1 = cycleVertices[currIndex].name;
					string vertex2 = cycleVertices[prevIndex].name;
					path[pathEdge] = vertex1 + ";" + vertex2;
					pathEdge++;
				}
				else
				{
					cycleVertices[currIndex].previous = cycleVertices[prevIndex].name;
					string vertex1 = cycleVertices[currIndex].name;
					string vertex2 = cycleVertices[prevIndex].name;
					path[pathEdge] = vertex1 + ";" + vertex2;
					pathEdge++;
				}
			}
			else
			{
				path[pathEdge-1] = "";
				pathEdge--;
				cycleVertices[currIndex].name = "dead end";
				currIndex = indexOfVertex(numVertices, cycleVertices, cycleVertices[currIndex].previous);
				prevIndex = indexOfVertex(numVertices, cycleVertices, cycleVertices[currIndex].previous);
				
		}
				weightFound = false;
				runTime++;
	}	
	
			return path;
			cout << endl;
}

// Assign weights based on the starting location
void assignWeights(Graph<string> &dGraph, int numVertices, VertexType myVertices[], int startingLocation, Queue<string>& getTo)
{

}

// Checks to see if all Vertices have been marked:
bool allMarked(int numVertices, VertexType myVertices[])
{
	for(int i = 0; i < numVertices; i++)
	{
		if(myVertices[i].marked == false)
			return false;
	}
	return true;
}

// Builds the Graph using the data from the External File:
void buildDGraph(Graph<string> &dGraph, string filename, string cities[], int &numVertices, int &numEdges, VertexType myVertices[])
{
    	ifstream infile(filename.c_str());
    	
		//Increments the slots of the characters that are encountered in the file.
		if(infile.is_open())
		{
			string line;
			while (getline(infile, line))
			{
				string firstVertex = line.substr(0, line.find(';'));
				line = line.substr(line.find(';') + 1, line.length()-1);

				string secondVertex = line.substr(0, line.find(';'));			
				line = line.substr(line.find(';') + 1, line.length()-1);

				int distance = atoi(line.c_str());

				if(!(cityExists(firstVertex, myVertices, numVertices)))
				{					
					VertexType newCity;
					newCity.name = firstVertex;
					newCity.marked = false;
					newCity.distance = INT_MAX;
					newCity.previous = "N/A";
					dGraph.AddVertex(firstVertex);
					cities[numVertices] = firstVertex;
					myVertices[numVertices] = newCity;
					numVertices++;
				}

				if(!(cityExists(secondVertex, myVertices, numVertices)))
				{
					VertexType newCity;
					newCity.name = secondVertex;
					newCity.marked = false;
					newCity.distance = INT_MAX;
					newCity.previous = "N/A";
					dGraph.AddVertex(secondVertex);
					cities[numVertices] = secondVertex;
					myVertices[numVertices] = newCity;
					numVertices++;
				}

				dGraph.AddEdge(firstVertex, secondVertex, distance);
				numEdges++;
			}	
		}
}

bool cityExists(string city, VertexType myVertices[], int numVertices)
{
	for (int i = 0; i < numVertices; i++)
	{
		if(myVertices[i].name == city)
			return true;
	}
	return false;
}


// Prints Cities found in External File:
void printCities(Graph<string> dGraph, int numVertices, VertexType myVertices[], string cities[])
{
	insertionSort(cities, numVertices);

	int numCities = numVertices;
	int i = 0;

	cout << endl << endl;
	cout << "^^^^^^^^^^^^^^^^^^^       DIJKSTRA'S ALGORITHM       ^^^^^^^^^^^^^^^^^^^" << endl << endl;
	cout << "The graph built for the file sent contains the following " << numVertices << " vertices: " << endl << endl;
	while(numCities > 0)
	{
		if (numCities > 2)
		{
			printf("%14s %20s %20s", cities[i].c_str(), cities[i+1].c_str(), cities[i+2].c_str());
			cout << endl;
			i = i+3;
			numCities = numCities-3;
		}
		else if (numCities == 2)
		{
			printf("%14s %20s", cities[i+0].c_str(), cities[i+1].c_str());
			cout << endl;
			i = i+2;
			numCities = numCities-2;
		}
		else
		{
			printf("%14s", cities[i].c_str());
			cout << endl;
			i++;
			numCities = numCities-1;	
		}
	}
	cout << endl;

}

// Sorts the Cities Alphabetically:
void insertionSort(string cities[], int numVertices)
{
	string temp = cities[0];
   	for(int i = 1; i < numVertices; i++)  // ar[i] is element to insert
    {
        temp = cities[i];
        int j = 0;
        for(j = i; j > 0; j--)
           if(temp < cities[j - 1])
               cities[j] = cities[j - 1];
           else break;
        cities[j] =  temp;
    }
}

//Sets the Starting Value in Dijsktra's Algorithm:
string setStart(string start, Graph<string> &dGraph, int numVertices, VertexType myVertices[])
{
	cout << " Please input your starting vertex: ";
	string response;
	getline(cin,response);
	bool isThere = false;
	for (int i = 0;	i < numVertices; i++)
	{
		if (myVertices[i].name.compare(response) == 0)
		{
			start = response;
			return start;
		}
	}
	while (!isThere)
	{
		cout << " Starting location does not exist..." << endl;
		cout << " Please input your new vertex: ";
        	string response;
        	cin >> response;
		for (int i = 0; i < numVertices; i++)
        	{
                	if (myVertices[i].name.compare(response) == 0)
			{
                        	start = response;
                        	return start;
                	}
        	}
	}
}

// Finds the minimum non-marked values in VertexType Array:
int findMin(VertexType myVertices[], int numVertices)
{
 		int minWeight = INT_MAX - 1;
        int minThing = - 1;
		for (int i = 0; i < numVertices; i++)
                {
                	
                        if (myVertices[i].distance < minWeight && !myVertices[i].marked)
                        {
                                minWeight = myVertices[i].distance;
                                minThing = i;
                        }
			else if (myVertices[i].distance == minWeight && !myVertices[i].marked)
        		{
                                minWeight = myVertices[i].distance;
                                minThing = i;
                        }

		}        
		return minThing;
}

// Prints a given row in a Summary Table:
void printRow(int location, VertexType myVertices[])
{
	if (myVertices[location].distance != -1)
	cout << setw(14) << myVertices[location].name << setw(18) << myVertices[location].distance << setw(16) << myVertices[location].previous << endl;
	else
	 cout << setw(14) << myVertices[location].name << setw(18) << "Not On Path" << setw(16) << myVertices[location].previous << endl;
}

