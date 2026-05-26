#include <iostream>
#include <cstdlib>
#include <cstdio>

struct edge {
	int u;	//pierwszy wierzcholek
	int v;	//drugi wierzcholek
	long long w;	//waga polaczenia
};

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

bool bfsEdgeColoring(int n, int* head, int* to, int* nextEdge, int* colors) {

	queue q; //tworzenie lokalnej kolejki
	q.elements = new int[n + 1];
	for (int i = 1; i <= n; i++) {
		if (colors[i] == -1) {
			colors[i] = 0;
			q.push(i);

			while (!q.isEmpty()) {
				int u = q.pop();
				int currentEdge = head[u];
				while (currentEdge != -1) {
					int v = to[currentEdge];
					if (colors[v] == -1) {
						colors[v] = 1 - colors[u];
						q.push(v);
					}
					else if (colors[v] == colors[u]) {
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

bool findPath(int u, int matrixSize, long long** matrix, long long* leftLabel, long long* rightLabel, int* match, bool* leftMet, bool* rightMet, long long* slack) {
	leftMet[u] = true; //poczatek szukania dla osoby u

	for (int v = 0; v < matrixSize; v++) {
		if (!rightMet[v]) {
			long long gap = leftLabel[u] + rightLabel[v] - matrix[u][v]; //jesli gap wynosi 0 to para idealnie do siebie pasuje
			if (gap == 0) {
				rightMet[v] = true;
				if (match[v] == -1 || findPath(match[v], matrixSize, matrix, leftLabel, rightLabel, match, leftMet, rightMet, slack)) { //sprawdzanie czy prawy partner jest wolny lub czy moze on zmienic obecnego partnera
					match[v] = u; //utworzenie polaczenia
					return true;
				}
			}
			else {
				if (gap < slack[v]) {
					slack[v] = gap;
				}
			}
		}
	}
	return false; //jesli nie zostanie stworzone zadne polaczenie to zwracamy false
}

void test() {
	int n, m;
	scanf("%d %d", &n, &m);

	//przypadek w ktorym nie ma zadnych polaczen (krawedzi)
	if (m == 0) {
		printf("0 ");
		return;
	}


	int edgeCounter = 0;
	int* head = new int[n+1];
	int* to = new int[2 * m];
	int* nextEdge = new int[2 * m];
	long long* weight = new long long[2 * m];
	int* colors = new int[n + 1];

	for (int i = 0; i <= n; i++) {
		head[i] = -1;
		colors[i] = -1;
	}

	for (int i = 0; i < m; i++) {
		int u, v;
		long long w;
		scanf("%d %d %lld", &u, &v, &w);

		//dodanie relacji u->v
		to[edgeCounter] = v;
		weight[edgeCounter] = w;
		nextEdge[edgeCounter] = head[u];
		head[u] = edgeCounter;
		edgeCounter++;

		//dodanie relacji v->u
		to[edgeCounter] = u;
		weight[edgeCounter] = w;
		nextEdge[edgeCounter] = head[v];
		head[v] = edgeCounter;
		edgeCounter++;

	}

	bool isBipartite = bfsEdgeColoring(n, head, to, nextEdge, colors);

	if (isBipartite) {
		int leftCount = 0; 
		int rightCount = 0;
		int* mappedId = new int[n + 1]; //tablica do przekladania wierzcholkow na macierz

		for (int i = 1; i <= n; i++) {
			if (colors[i] == 0) {
				mappedId[i] = leftCount;
				leftCount++;
			}
			else if (colors[i] == 1) {
				mappedId[i] = rightCount;
				rightCount++;
			}
		}

		int matrixSize = 0;
		if (leftCount > rightCount) {
			matrixSize = leftCount;
		}
		else {
			matrixSize = rightCount;
		}

		long long** matrix = new long long* [matrixSize]; //macierz do ktorej beda zapisywane wagi relacji
		for (int i = 0; i < matrixSize; i++) {
			matrix[i] = new long long[matrixSize];
			for (int j = 0; j < matrixSize; j++) {
				matrix[i][j] = 0;
			}
		}

		for (int i = 1; i <= n; i++) {
			if (colors[i] == 0) {
				int row = mappedId[i];
				int currentEdge = head[i];
				
				while (currentEdge != -1) {
					int v = to[currentEdge];
					int column = mappedId[v];
					if (weight[currentEdge] > matrix[row][column]) {
						matrix[row][column] = weight[currentEdge];
					}
					currentEdge = nextEdge[currentEdge];
				}
			}
		}

		long long* leftLabel = new long long[matrixSize]; //etykiety (wymagania) lewej strony
		long long* rightLabel = new long long[matrixSize]; //etykiety (wymagania) prawej strony
		int* match = new int[matrixSize]; //tablica polaczen
		bool* leftMet = new bool[matrixSize]; //tablice odwiedzin 
		bool* rightMet = new bool[matrixSize]; 
		long long* slack = new long long[matrixSize]; //tablica negocjacji wymagan

		//zerowanie tablic
		for (int i = 0; i < matrixSize; i++) {
			rightLabel[i] = 0;
			match[i] = -1;
			leftLabel[i] = 0;
			for (int j = 0; j < matrixSize; j++) {
				if (matrix[i][j]>leftLabel[i]) {
					leftLabel[i] = matrix[i][j]; //znajdowanie najwyzszego labela dla lewej strony
				}
			}
		}

		for (int i = 0; i < matrixSize; i++) {
			while (true) {
				for (int j = 0; j < matrixSize; j++) {
					leftMet[j] = false;
					rightMet[j] = false;
					slack[j]= 2000000000000000000LL; //wielka wartosc ktora potem bedzie zmniejszana
				}
				if (findPath(i, matrixSize, matrix, leftLabel, rightLabel, match, leftMet, rightMet, slack)) {
					break;
				}
				long long delta = 2000000000000000000LL;
				for (int j = 0; j < matrixSize; j++) {
					if (!rightMet[j]) {
						if (slack[j] < delta) {
							delta = slack[j];
						}
					}
				}
				for (int j = 0; j < matrixSize; j++) {
					if (leftMet[j]) {
						leftLabel[j]-=delta;
					}
					if (rightMet[j]) {
						rightLabel[j] += delta;
					}
					else {
						slack[j] -= delta;
					}
				}
			}
		}

		long long totalWeight = 0;
		for (int i = 0; i < matrixSize; i++) {
			if (match[i] != -1 && matrix[match[i]][i] > 0) {
				totalWeight += matrix[match[i]][i];
			}
		}
		printf("%lld ", totalWeight);

		delete[] mappedId;
		delete[] rightLabel;
		delete[] leftLabel;
		delete[] slack;
		delete[] rightMet;
		delete[] leftMet;
		delete[] match;
		for (int i = 0; i < matrixSize; i++) {
			delete[] matrix[i];
		}
		delete[] matrix;
	}

	delete[] colors;
	delete[] head;
	delete[] weight;
	delete[] nextEdge;
	delete[] to;
}



int main() {
	int t;
	scanf("%d", &t);

	for (int i = 0; i < t; i++) {
		test();
	}
	printf("\n");

	return 0;
}