#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <cstdio>


struct queue {
	int* elements;
	int head = 0;
	int tail = 0;

	void push(int value) {
		elements[tail] = value;
		tail++;
	}
	int pop() {
		int value = elements[head];
		head++;
		return value;
	}
	bool isEmpty() {
		if (head == tail) {
			return true;
		}
		else {
			return false;
		}
	}
};

bool bfsEdgeColoring(int personCount, int* head, int* to, int* nextEdge, int* genderGroup) {

	queue q; //tworzenie lokalnej kolejki
	q.elements = new int[personCount + 1];
	for (int i = 1; i <= personCount; i++) {
		if (genderGroup[i] == -1 && head[i] != -1) {
			genderGroup[i] = 0;
			q.push(i);

			while (!q.isEmpty()) {
				int currentPerson = q.pop();
				int currentEdge = head[currentPerson];
				while (currentEdge != -1) {
					int possiblePartner = to[currentEdge];
					if (genderGroup[possiblePartner] == -1) {
						genderGroup[possiblePartner] = 1 - genderGroup[currentPerson];
						q.push(possiblePartner);
					}
					else if (genderGroup[possiblePartner] == genderGroup[currentPerson]) {
						delete[] q.elements;
						return false;
					}
					currentEdge = nextEdge[currentEdge];
				}
			}
		}
	}

	delete[] q.elements;
	return true;
}

long long hungarian(int womenCount, int menCount, int* head, int* to, long long* weight, int* nextEdge) {
	int totalMenWithDummies = womenCount + menCount;
	long long* womenLabel = new long long[womenCount + 1](); 
	long long* menLabel = new long long[totalMenWithDummies + 1]();
	int* match = new int[totalMenWithDummies + 1]();
	
	int* traceBack = new int[totalMenWithDummies + 1]();
	long long* slack = new long long[totalMenWithDummies + 1]();
	bool* menVisited = new bool[totalMenWithDummies + 1]();
	int* menUnvisited = new int[totalMenWithDummies + 1]();

	for (int i = 1; i <= womenCount; i++) {
		long long maxVal = 0; //znajdowanie najwyzszej wartosci relacji
		for (int edge = head[i]; edge != -1; edge = nextEdge[edge]) {
			if (weight[edge] > maxVal) {
				maxVal = weight[edge];
			}
		}
		womenLabel[i] = maxVal;
	}
	for (int i=1;i<=womenCount;i++){
		match[0] = i; //index zerowy nie jest uzywany wiec  sluzy jako bufor
		int currentMan = 0;
		int unvisitedCount = totalMenWithDummies;
		int nextIndex = -1;

		for (int j = 0; j <= totalMenWithDummies; j++) {
			slack[j] = 2000000000000000000LL;
			menVisited[j] = false;
			if (j > 0) {
				menUnvisited[j - 1] = j;
			}
		}
		do {
			menVisited[currentMan] = true;
			if (nextIndex != -1) {
				menUnvisited[nextIndex] = menUnvisited[unvisitedCount - 1]; //podmiana danego elementy na ostatni
				unvisitedCount--;
			}

			int currentWoman = match[currentMan]; //wyciagniecie obecnej kobiety
			long long delta = 2000000000000000000LL;
			int nextMan = 0;

			if (currentWoman != 0) {
				for (int edge = head[currentWoman]; edge != -1; edge = nextEdge[edge]) {
					int manId = to[edge];
					if (!menVisited[manId]) {
						long long gap = womenLabel[currentWoman] + menLabel[manId] - weight[edge]; //gap to roznica pomiedzy dwoma osobami, ile brakuje zeby relacja byla idealna i mozna bylo ich polaczyc
						if (gap < slack[manId]) {
							slack[manId] = gap;
							traceBack[manId] = currentMan;
						}
					}
				}
				int dummyMan = menCount + currentWoman; //"sztuczny" mezczyzna dla danej kobiety
				if (!menVisited[dummyMan]) {
					long long gap = womenLabel[currentWoman] + menLabel[dummyMan] - 0; //to zero jest dlatego ze waga sztucznej relacji jest rowna 0
					if (gap < slack[dummyMan]) {
						slack[dummyMan] = gap;
						traceBack[dummyMan] = currentMan;
					}
				}
			}

			//znajdowanie najnizszego slack
			for (int j = 0; j < unvisitedCount; j++) { 
				int manId = menUnvisited[j];

				if (slack[manId] < delta) {
					delta = slack[manId];
					nextMan = manId;
					nextIndex = j;
				}
			}

			//obnizenie wymagan nieodwiedzonych
			for (int j = 0; j <= totalMenWithDummies; j++) {
				if (menVisited[j]) {
					womenLabel[match[j]] -= delta; 
					menLabel[j] += delta;
				}
			}

			//aktualizowanie slack nieodwiedzonych
			for (int j = 0; j < unvisitedCount; j++) {
				slack[menUnvisited[j]] -= delta;
			}
			currentMan = nextMan;

		} 
		while (match[currentMan] != 0);
		//cofanie sie po sladach
		do {
			int prevMan = traceBack[currentMan];
			match[currentMan] = match[prevMan];
			currentMan = prevMan;
		} while (currentMan != 0);
	}
	long long totalWeight = 0;
	for (int j = 1; j <= menCount; j++) {
		if (match[j] != 0) {
			int matchedWoman = match[j];
			long long actualWeight = 0;

			for (int edge = head[matchedWoman]; edge != -1; edge = nextEdge[edge]) {
				if (to[edge] == j) {
					if (weight[edge] > actualWeight) {
						actualWeight = weight[edge];
					}
				}
			}
			totalWeight += actualWeight;
		}
	}
	delete[] womenLabel;
	delete[] menLabel;
	delete[] match;
	delete[] traceBack;
	delete[] slack;
	delete[] menVisited;
	delete[] menUnvisited;

	return totalWeight;
}

void bruteForce(int currentPerson, long long currentWeight, int personCount, int* head, int* to, long long* weight, int* nextEdge, bool* taken, long long& maxBruteWeight) {

	if (currentPerson > personCount) {
		if (currentWeight > maxBruteWeight) {
			maxBruteWeight = currentWeight;
		}
		return;
	}
	if (taken[currentPerson]) {
		bruteForce(currentPerson + 1, currentWeight, personCount, head, to, weight, nextEdge, taken, maxBruteWeight);
		return;
	}
	//glowna logika
	bruteForce(currentPerson + 1, currentWeight, personCount, head, to, weight, nextEdge, taken, maxBruteWeight); //przypadek w ktorym dana osoba zostaje singlem (nie jest polaczona w zadna pare), po prostyu wywolanie funkcji dla kolejnej osoby

	//proba sparowania osoby z kazdym mozliwym partnerem
	for (int edge = head[currentPerson]; edge != -1; edge = nextEdge[edge]) {
		int possiblePartner = to[edge];

		if (!taken[possiblePartner]) {
			taken[currentPerson] = true;
			taken[possiblePartner] = true;

			bruteForce(currentPerson + 1, currentWeight + weight[edge], personCount, head, to, weight, nextEdge, taken, maxBruteWeight); //przejscie do kolejnej osoby

			//backtracking w celu znalezienia nastepnego potencjalnego partnera
			taken[currentPerson] = false;
			taken[possiblePartner] = false;
		}
	}
}

void test() {
	int personCount, relationCount;
	scanf("%d %d", &personCount, &relationCount);

	//przypadek w ktorym nie ma zadnych polaczen (krawedzi)
	if (relationCount == 0) {
		printf("0 ");
		return;
	}

	int edgeCounter = 0;
	int* head = new int[personCount + 1];
	int* to = new int[(long long)2 * relationCount];
	int* nextEdge = new int[(long long)2 * relationCount];
	long long* weight = new long long[(long long)2 * relationCount];
	int* genderGroup = new int[personCount + 1];

	for (int i = 0; i <= personCount; i++) {
		head[i] = -1;
		genderGroup[i] = -1;
	}

	for (int i = 0; i < relationCount; i++) {
		int personA, personB;
		long long relationWeight;
		scanf("%d %d %lld", &personA, &personB, &relationWeight);

		//dodanie relacji u->v
		to[edgeCounter] = personB;
		weight[edgeCounter] = relationWeight;
		nextEdge[edgeCounter] = head[personA];
		head[personA] = edgeCounter;
		edgeCounter++;

		//dodanie relacji v->u
		to[edgeCounter] = personA;
		weight[edgeCounter] = relationWeight;
		nextEdge[edgeCounter] = head[personB];
		head[personB] = edgeCounter;
		edgeCounter++;

	}

	bool isBipartite = bfsEdgeColoring(personCount, head, to, nextEdge, genderGroup);

	if (isBipartite) {
		int womenCount = 0;
		int menCount = 0;
		int* mappedId = new int[personCount + 1]; //tablica do przekladania wierzcholkow na macierz

		for (int i = 1; i <= personCount; i++) {
			if (genderGroup[i] == 0) {
				womenCount++;
				mappedId[i] = womenCount;
			}
			else if (genderGroup[i] == 1) {
				menCount++;
				mappedId[i] = menCount;
			}
		}

		int* filteredHead = new int[womenCount + 1];
		for (int i = 0; i <= womenCount; i++) {
			filteredHead[i] = -1;
		}
		int* filteredTo = new int[relationCount + 1];
		long long* filteredWeight = new long long[relationCount + 1];
		int* filteredNextEdge = new int[relationCount + 1];
		int filteredEdgeCounter = 0;

		for (int i = 1; i <= personCount; i++) {
			if (genderGroup[i] == 0) { //jedynie krawedzie wychodzace od kobiet
				int womenId = mappedId[i];
				int currentEdge = head[i];

				while (currentEdge != -1) {
					int menIdOg = to[currentEdge];
					int menId = mappedId[menIdOg];
					filteredTo[filteredEdgeCounter] = menId;
					filteredWeight[filteredEdgeCounter] = weight[currentEdge];
					filteredNextEdge[filteredEdgeCounter] = filteredHead[womenId];
					filteredHead[womenId]=filteredEdgeCounter;
					filteredEdgeCounter++;

					currentEdge = nextEdge[currentEdge];
				}
			}
		}

		long long result = hungarian(womenCount,menCount, filteredHead, filteredTo, filteredWeight, filteredNextEdge);
		printf("%lld ", result);

		delete[] mappedId;
		delete[] filteredHead;
		delete[] filteredTo;
		delete[] filteredWeight;
		delete[] filteredNextEdge;
	}
	else {
		bool* taken = new bool[personCount + 1]();
		long long maxBruteWeight = 0;

		bruteForce(1, 0, personCount, head, to, weight, nextEdge, taken, maxBruteWeight);

		printf("%lld ", maxBruteWeight);
		delete[] taken;
	}

	delete[] genderGroup;
	delete[] head;
	delete[] weight;
	delete[] nextEdge;
	delete[] to;
}

int main() {
	int testCaseCount;
	scanf("%d", &testCaseCount);

	for (int i = 0; i < testCaseCount; i++) {
		test();
	}
	printf("\n");

	return 0;
}