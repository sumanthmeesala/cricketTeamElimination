/** A Mini project on Cricket Team Elimination
                      By
            1602-19-733-311 M Sumanth
**/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

/* Queue : START */
struct QNode {
        int value;
        struct QNode *next;
};

struct Queue {
        struct QNode *head;
        struct QNode *tail;
        int size;
};

struct Queue *createQueue(void) {
        struct Queue *queue = malloc(sizeof(struct Queue));
        queue -> head = queue -> tail = NULL;

        return queue;
}

void enQueue(struct Queue *queue, int value) {
    struct QNode *qNode = malloc(sizeof(struct QNode));
    qNode -> value = value;
        qNode -> next = NULL;

    if(queue -> head == NULL) queue -> head = qNode;
    else queue -> tail -> next = qNode;

        queue -> tail = qNode;

        queue -> size++;
}

int deQueue(struct Queue *queue) {
    if(queue -> head == NULL) return -1;

    int value = queue -> head -> value;

    struct QNode *qNode = queue -> head;
        queue -> head = qNode -> next;
    free(qNode);

        queue -> size--;

    return value;
}

bool isQueueEmpty(struct Queue *queue) {
        return queue -> size == 0;
}
/* Queue : END */

/* Ford-Fulkerson - Flow Network | Flow Edge : START */
struct FlowEdge {
        int from;
        int to;
        double capacity;
        double flow;
};

struct FlowEdge *createFlowEdge(int from, int to, double capacity) {
        struct FlowEdge * flowEdge = malloc(sizeof(struct FlowEdge));
        flowEdge -> from = from;
        flowEdge -> to = to;
        flowEdge -> capacity = capacity;

        return flowEdge;
}

int getOtherVertix(struct FlowEdge *flowEdge, int this) {
        return flowEdge -> from == this ? flowEdge -> to : flowEdge -> from;
}

double getResidualCapacityTo(struct FlowEdge *flowEdge, int vertex) {
        return flowEdge -> from == vertex ? flowEdge -> flow : flowEdge -> capacity - flowEdge -> flow;
}

void addResifualCapacityTo(struct FlowEdge * flowEdge, int vertex, double delta) {
        if(vertex == flowEdge -> from) flowEdge -> flow -= delta;
        else if(vertex == flowEdge -> to) flowEdge -> flow += delta;

        if(fabs(flowEdge -> flow) <= 1e-10)
                flowEdge -> flow = 0;
        if(fabs(flowEdge -> flow - flowEdge -> capacity) <= 1e-10)
                flowEdge -> flow = flowEdge -> capacity;
}
/* Ford-Fulkerson - Flow Network | Flow Edge : END */

/* Ford-Fulkerson - Flow Network | Flow Edge List : START */
struct FlowEdgeNode {
        struct FlowEdge *item;
        struct FlowEdgeNode *next;
};

struct FlowEdgeNode *createFlowEdgeNode() {
        struct FlowEdgeNode *flowEdgeNode = malloc(sizeof(struct FlowEdgeNode));
        flowEdgeNode -> item = NULL;
        flowEdgeNode -> next = NULL;

        return flowEdgeNode;
}

struct FlowEdgeList {
        struct FlowEdgeNode *first;
        int size;
};

struct FlowEdgeList *createFlowEdgeList() {
        struct FlowEdgeList *flowEdgeList = malloc(sizeof(struct FlowEdgeList));
        flowEdgeList -> first = NULL;
        flowEdgeList -> size = 0;

        return flowEdgeList;
}

void addFlowEdgeToList(struct FlowEdgeList *flowEdgeList, struct FlowEdge *flowEdge) {
        struct FlowEdgeNode *flowEdgeNode = malloc(sizeof(struct FlowEdgeNode));
        flowEdgeNode -> item = flowEdge;
        flowEdgeNode -> next = flowEdgeList -> first;

        flowEdgeList -> first = flowEdgeNode;
        flowEdgeList -> size++;
}
/* Ford-Fulkerson - Flow Network | Flow Edge List : END */


/* Ford-Fulkerson - Flow Network : START */
struct FlowNetwork {
        int totalNumberOfVertices;
        struct FlowEdgeList **adjList;
};

struct FlowNetwork *createFlowNetwork(int totalNumberOfVertices) {
        struct FlowNetwork *flowNetwork = malloc(sizeof(struct FlowNetwork));
        flowNetwork -> totalNumberOfVertices = totalNumberOfVertices;
        flowNetwork -> adjList = (struct FlowEdgeList**) malloc(totalNumberOfVertices * sizeof(struct FlowEdgeList*));
        for(int i = 0; i < totalNumberOfVertices; i++)
                flowNetwork -> adjList[i] = createFlowEdgeList();

        return flowNetwork;
}

void addFlowEdge(struct FlowNetwork *flowNetwork, struct FlowEdge *flowEdge) {
        addFlowEdgeToList(flowNetwork -> adjList[flowEdge -> from], flowEdge);
        addFlowEdgeToList(flowNetwork -> adjList[flowEdge -> to], flowEdge);

}
/* Ford-Fulkerson - Flow Network : END */

/* Ford-Fulkerson : START */
struct FordFulkerson {
        int numberOfVertices;
        double maxFlow;
        bool *marked;
        struct FlowEdge **edgeTo;
};

bool hasAPath(struct FordFulkerson *fordFulkerson, struct FlowNetwork *flowNetwork, int source, int target) {
        fordFulkerson -> marked = (bool*) malloc(flowNetwork -> totalNumberOfVertices * sizeof(bool));
        fordFulkerson -> edgeTo = (struct FlowEdge**) malloc(flowNetwork -> totalNumberOfVertices * sizeof(struct FlowEdge*));

        struct Queue *queue = createQueue();
        enQueue(queue, source);
        fordFulkerson -> marked[source] = true;

        while(!isQueueEmpty(queue) && !fordFulkerson -> marked[target]) {
                int v = deQueue(queue);

                for(struct FlowEdgeNode *flowEdgeNode = flowNetwork -> adjList[v] -> first;
                                flowEdgeNode != NULL; flowEdgeNode = flowEdgeNode -> next) {
                        int w = getOtherVertix(flowEdgeNode -> item, v);

                        if(getResidualCapacityTo(flowEdgeNode -> item, w) > 0 && !fordFulkerson -> marked[w]) {
                                fordFulkerson -> edgeTo[w] = flowEdgeNode -> item;
                                fordFulkerson -> marked[w] = true;
                                enQueue(queue, w);
                        }
                }
        }

        return fordFulkerson -> marked[target];
}

double min(double x, double y) {
        return x < y ? x : y;
}

struct FordFulkerson *createFodFulkerson(struct FlowNetwork *flowNetwork, int source, int target) {
        struct FordFulkerson *fordFulkerson = malloc(sizeof(struct FordFulkerson));
        fordFulkerson -> numberOfVertices = flowNetwork -> totalNumberOfVertices;

        double excess = 0.0;
        for(struct FlowEdgeNode *flowEdgeNode = flowNetwork -> adjList[target] -> first;
                        flowEdgeNode != NULL; flowEdgeNode = flowEdgeNode -> next) {
                if (target == flowEdgeNode -> item -> from) excess -= flowEdgeNode -> item -> flow;
                else excess += flowEdgeNode -> item -> flow;
        }

        fordFulkerson -> maxFlow = excess;

        while(hasAPath(fordFulkerson, flowNetwork, source, target)) {
                double bottle = INFINITY;
                for(int v = target; v != source; v = getOtherVertix(fordFulkerson -> edgeTo[v], v))
                        bottle = min(bottle, getResidualCapacityTo(fordFulkerson -> edgeTo[v], v));

                for(int v = target; v != source; v = getOtherVertix(fordFulkerson -> edgeTo[v], v))
                        addResifualCapacityTo(fordFulkerson -> edgeTo[v], v, bottle);

                fordFulkerson -> maxFlow += bottle;
        }

        return fordFulkerson;
}
/* Ford-Fulkerson : END */

/* Cricket Elimination : START */
struct Team {
    char name[30];
    int wins;
    int loses;
    int remaining;
    int *against;
        bool isEliminated;
};

int numberOfTeams;
struct Team *teams;

void readAllTeamsInformation() {
    scanf("%d", &numberOfTeams);

    teams = (struct Team*)malloc(numberOfTeams * sizeof(struct Team));
    for(int i = 0; i < numberOfTeams; i++) {
        struct Team team;
        scanf("%s%d%d%d", &team.name, &team.wins, &team.loses, &team.remaining);
        team.against = (int*)malloc(numberOfTeams * sizeof(int));
        for(int j = 0; j < numberOfTeams; j++) scanf("%d", &team.against[j]);
        teams[i] = team;
    }
}

bool isEliminated(int teamIndex) {
        int numberOfGameVertices = numberOfTeams * (numberOfTeams - 1) / 2;
        int totalNumberOfVertices = numberOfGameVertices + numberOfTeams + 2;

        struct FlowNetwork *flowNetwork = createFlowNetwork(totalNumberOfVertices);

        int source = 0, target = totalNumberOfVertices - 1;

        int flow = 0;

        for(int i = 0, index = 1; i < numberOfTeams; i++) {
                if(i == teamIndex) continue;

                for(int j = i + 1; j < numberOfTeams; j++, index++) {
                        if(j == teamIndex) continue;

                        addFlowEdge(flowNetwork, createFlowEdge(source, index, teams[i].against[j]));

                        addFlowEdge(flowNetwork, createFlowEdge(index, i + numberOfGameVertices + 1, INFINITY));
                        addFlowEdge(flowNetwork, createFlowEdge(index, j + numberOfGameVertices + 1, INFINITY));

                        flow += teams[i].against[j];
                }

                int capacityLimit = teams[teamIndex].wins + teams[teamIndex].remaining - teams[i].wins;
                if(capacityLimit < 0) return true;
                addFlowEdge(flowNetwork, createFlowEdge(i + numberOfGameVertices + 1, target, capacityLimit));
        }

        return flow > createFodFulkerson(flowNetwork, source, target) -> maxFlow;
}

void determineEliminationForAllTeams() {
        for(int i = 0; i < numberOfTeams; i++)
                teams[i].isEliminated = isEliminated(i);
}

void printTeamInformation() {
        printf("%20s-------------------------------------------", "");
    printf("\n%44s : %d", "Number of Teams", numberOfTeams);
    printf("\n%20s-------------------------------------------", "");
    printf("\n%-8s | %-15s | %-6s | %-6s | %-9s | ", "Team No.",
                "Team Name", "Wins", "Loses", "Remaining");
    for(int i = 0; i <  numberOfTeams; i++) printf(" %-2d", i + 1);
    for(int i = 0; i < numberOfTeams; i++) {
        printf("\n  %-6d | %-15s | %-6d | %-6d | %-9d | ", i + 1,
                                teams[i].name,
                                teams[i].wins,
               teams[i].loses,
               teams[i].remaining);

        for(int j = 0; j < numberOfTeams; j++)
                        printf(" %-2d", teams[i].against[j]);

                printf("<- is %seliminated", teams[i].isEliminated ? "" : "Not ");
    }
}

/* Cricket Elimination : END */

int main() {
        readAllTeamsInformation();
        determineEliminationForAllTeams();
        printTeamInformation();
    return 0;
}

