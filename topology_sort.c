#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_VERTICES 100

typedef struct graphType {
    int n;
    int adj_mat[MAX_VERTICES][MAX_VERTICES];
} graphType;

typedef struct graphNode {
    int vertex;
    struct graphNode* next;
}graphNode;

typedef struct graphLinked {
    int n;
    graphNode* adj_list_H[MAX_VERTICES];
} graphLinked;

typedef struct queueType {
    graphNode* front;
    graphNode* rear;
    int cnt;
}queueType;

/*---------------------queue function---------------------*/
void init_queue(queueType* q) {
    q->front = NULL;
    q->rear = NULL;
    q->cnt = 0;
}

int is_empty(queueType* q) {
    return q->cnt == 0;
}

void enqueue(queueType* q, int i) {
    graphNode* cur = (graphNode*)malloc(sizeof(graphNode));
    cur->vertex = i;
    cur->next = NULL;
    if (is_empty(q))
        q->front = cur;
    else {
        q->rear->next = cur;
    }
    q->rear = cur;
    q->cnt++;
}

int dequeue(queueType* q) {
    int data;
    graphNode* p;
    if (is_empty(q)) {
        printf("Error: Queue is empty!\n");
        return 0;
    }
    p = q->front;
    data = p->vertex;
    q->front = p->next;
    free(p);
    q->cnt--;

    return data;
}

/*---------------------graph function---------------------*/
void create_graph(graphLinked* G, graphType* g) {
    int v;
    G->n = 0;
    for (v = 0; v < g->n; v++) {
        G->adj_list_H[v] = NULL;
    }
}

void insert_vertex(graphLinked* G, graphType* g, int v) {
    if ((G->n) + 1 > g->n)
        return;
    G->n++;
}

void insert_edge(graphLinked* G, int u, int v) {
    graphNode* node;
    if ((u >= G->n || v >= G->n))
        return;
    node = (graphNode*)malloc(sizeof(graphNode));
    node->vertex = v;
    node->next = G->adj_list_H[u];
    G->adj_list_H[u] = node;
}

/*
void print_adj_list(graphLinked* G) {
    for (int i = 0; i < G->n; i++) {
        graphNode* p = G->adj_list_H[i];
        printf("vertex %d's adjacency list", i);
        while (p != NULL) {
            printf("->%d", p->vertex);
            p = p->next;
        }
        printf("\n");
    }
}*/

void graph_init(graphType* g) {
    int r, c;
    g->n = 0;
    for (r = 0; r < MAX_VERTICES; r++)
        for (c = 0; c < MAX_VERTICES; c++)
            g->adj_mat[r][c] = 0;
}
/*----read "input file" as adj_mat and then convert to adj_list----*/
void graph_read(graphLinked* G, graphType* g, char* input) {
    FILE* fp;
    char line[255];
    int line_count = 0;
    int i, j;

    graph_init(g);

    fp = fopen(input, "r");

    if (fp == NULL) {
        fprintf(stderr, "can't read file.txt\n");
        exit(0);
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        char* pch = line;
        int tmp;
        int l = strtol(pch, &pch, 10);
        while ((tmp = strtol(pch, &pch, 10)) != 0) {
            g->adj_mat[l - 1][tmp - 1] = 1;
        }
        line_count++;
    }
    g->n = line_count;

    create_graph(G, g);

    for (i = 0; i < line_count; i++) {
        insert_vertex(G, g, i);
    }

    for (i = 0; i < G->n; i++)
        for (j = 0; j < G->n; j++)
            if (g->adj_mat[i][j] == 1)
                insert_edge(G, i, j);

    fclose(fp);
}

/*---------------------topology sort and make output---------------------*/
void topology_sort(graphLinked* G, char*output) {
    int i, j;
    int total_cnt_vertex = 0, total_cnt_edge = 0;
    int* in_degree = (int*)malloc(G->n * sizeof(int));    //array of indegree
    int* sort_result = (int*)malloc(G->n * sizeof(int));  //array of result after sorting
    int* cnt_vertex = (int*)malloc(G->n * sizeof(int));   //array of total #operation each vertex
    int cnt_edge[MAX_VERTICES][MAX_VERTICES];             //matrix of total #operation each edge from i to j

    queueType* q = (queueType*)malloc(sizeof(queueType));
    graphNode* node = NULL;

    for (i = 0; i < G->n; i++) {
        in_degree[i] = 0;
        sort_result[i] = 0;
        cnt_vertex[i] = 0;
    }

    for (i = 0; i < G->n; i++)
        for (j = 0; j < G->n; j++)
            cnt_edge[i][j] = 0;

    for (i = 0; i < G->n; i++) {                          // make in_degree array each vertex
        node = G->adj_list_H[i];
        while (node != NULL) {
            in_degree[node->vertex]++;
            node = node->next;
        }
    }

    init_queue(q);

    for (i = 0; i < G->n; i++) {                          // enqueue vertex which have 0 indegree
        if (in_degree[i] == 0) {
            enqueue(q, i);
            cnt_vertex[i]++;
        }
    }

    for (i = 0; i < G->n; i++) {                          // topology sort, then make result array and count 
        if (is_empty(q)) {
            printf("This graph has a cycle!");
            break;
        }
        else {
            int w;
            w = dequeue(q);
            sort_result[i] = w;
            cnt_vertex[w]++;
            node = G->adj_list_H[w];
            while (node != NULL) {
                int i = node->vertex;
                in_degree[i]--;
                cnt_edge[w][i]++;
                if (in_degree[i] == 0) {
                    enqueue(q, i);
                    cnt_vertex[i]++;
                }
                node = node->next;
            }
        }
    }

    for (int i = 0; i < G->n; i++) {
        total_cnt_vertex += cnt_vertex[i];
        for (int j = 0; j < G->n; j++)
            total_cnt_edge += cnt_edge[i][j];
    }

    /*---------------------print output---------------------*/
    FILE* fp = fopen(output, "w");

    if (fp == NULL) {
        printf("Error!\n");
        exit(1);
    }
    if (total_cnt_vertex != 2 * G->n) {
        fprintf(fp, "This graph has a cycle!");
        return;
    }
    else {
        fprintf(fp, "A\n");                                //output format A
        for (int i = 0; i < G->n; i++)
            fprintf(fp, "%d ", sort_result[i] + 1);

        fprintf(fp, "\n\n");
        
        fprintf(fp, "B\n");                                 //output format B
        for (int i = 0; i < G->n; i++)
            fprintf(fp, "Vertex %d : %d \n", i+1, cnt_vertex[i]);
        for (int i = 0; i < G->n; i++)
            for (int j = 0; j < G->n; j++)
                if (cnt_edge[i][j] == 1)
                    fprintf(fp, "Edge from %d to %d : %d \n", i+1, j+1, cnt_edge[i][j]);
        
        fprintf(fp, "C\n");
        fprintf(fp, "Total number of operations charged to all vertices is : %d \n", total_cnt_vertex);
        fprintf(fp, "Total number of operations charged to all edges is : %d \n", total_cnt_edge);
    }

    fclose(fp);
}


int main(int argc, char **argv) {
    struct graphLinked* G;
    struct graphType* g;

    char* input = NULL;
    char* output = NULL;

    int i=1;

    G = (graphLinked*)malloc(sizeof(graphLinked));
    g = (graphType*)malloc(sizeof(graphType));

    input = argv[i++];
    output = argv[i++];

    clock_t begin = clock();
    graph_read(G, g, input);
    free(g);
    topology_sort(G, output);

    free(G);
    clock_t end = clock();
    printf("Time taken = %lf\n", ((double)end - begin) / CLOCKS_PER_SEC);
    return 0;
}