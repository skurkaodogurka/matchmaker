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