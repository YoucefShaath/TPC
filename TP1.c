#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_CITY_NAME_LENGTH 255
#define MAX_CITIES 1000
#define INF INT_MAX 

typedef struct Connection {
    struct City* destination;
    int distance;
    struct Connection* next;
} Connection;

typedef struct City {
    char city_name[MAX_CITY_NAME_LENGTH];
    struct Connection* connections;
    struct City* next1; 
    int distance1;      
    int distance2;      
} City;

typedef City* ptrcity;

void allocate_city(ptrcity *p) {
    *p = malloc(sizeof(City));
}

void add_connection(ptrcity source, ptrcity destination, int distance) {
    Connection* newConn = malloc(sizeof(Connection));
    newConn->destination = destination;
    newConn->distance = distance;
    newConn->next = NULL;

    if (source->connections == NULL) {
        source->connections = newConn;
    } else {
        Connection* current = source->connections;
        while (current->next != NULL) current = current->next;
        current->next = newConn;
    }
}

void free_city(ptrcity p) {
    Connection* current = p->connections;
    while (current != NULL) {
        Connection* temp = current;
        current = current->next;
        free(temp);
    }
    free(p);
}

void ass_dist1(ptrcity p, int dist) {
    p->distance1 = dist;
}

void ass_dist2(ptrcity p, int dist) {
    p->distance2 = dist;
}

void ass_next1(ptrcity p, ptrcity q) {
    p->next1 = q;
}



void ass_city(ptrcity p, char name[]) {
    strncpy(p->city_name, name, MAX_CITY_NAME_LENGTH - 1);
    p->city_name[MAX_CITY_NAME_LENGTH - 1] = '\0';
}

int dist1(ptrcity p) {
    return p->distance1;
}

int dist2(ptrcity p) {
    return p->distance2;
}

char* cityname(ptrcity p) {
    return p->city_name;
}

ptrcity nex1(ptrcity p) {
    return p->next1;
}


ptrcity find_city(ptrcity head, char* name) {
    ptrcity current = head;
    while (current != NULL) {
        if (strcmp(current->city_name, name) == 0) {
            return current;
        }
        current = current->next1;
    }
    return NULL;
}

void create_list(ptrcity *head) {
    FILE* file = fopen("tp1.txt", "r");
    if (file == NULL) {
        printf("Problem while opening the file.\n");
        return;
    }

    char buffer[MAX_CITY_NAME_LENGTH * MAX_CITIES];
    char cities[MAX_CITIES][MAX_CITY_NAME_LENGTH];
    int numofcities = 0;

    if (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\n")] = '\0';

        char *token = strtok(buffer, ",");
        while (token != NULL && numofcities < MAX_CITIES) {
            if (strlen(token) > 0) {
                strncpy(cities[numofcities], token, MAX_CITY_NAME_LENGTH - 1);
                cities[numofcities][MAX_CITY_NAME_LENGTH - 1] = '\0';
                numofcities++;
            }
            token = strtok(NULL, ",");
        }
    }

    *head = NULL;
    ptrcity prev = NULL;
    ptrcity new_city;

    for (int i = 0; i < numofcities; i++) {
        if (strlen(cities[i]) == 0) continue;
        
        allocate_city(&new_city); 
        ass_city(new_city, cities[i]);
        
        if (*head == NULL) {
            *head = new_city;
        } 
        else {
            ass_next1(prev, new_city);
        }
        
        prev = new_city;
    }

    char city1[MAX_CITY_NAME_LENGTH], city2[MAX_CITY_NAME_LENGTH];
    int distance;

    while (fscanf(file, "%s %s %d", city1, city2, &distance) == 3) {
        ptrcity source = find_city(*head, city1);
        ptrcity destination = find_city(*head, city2);
        add_connection(source, destination, distance);
    }

    fclose(file);
}

void display_graph(ptrcity head) {
    ptrcity current = head;
    while (current != NULL) {
        printf("%s", cityname(current));
        
        Connection* conn = current->connections;
        while (conn != NULL) {
            printf(" ---> %s (%dKm)", cityname(conn->destination), conn->distance);
            conn = conn->next;
        }
        
        printf(" ---> NULL\n");
        current = nex1(current); 
    }
}

void reached_cities(ptrcity head, char city[MAX_CITY_NAME_LENGTH]) {
    ptrcity source = find_city(head, city);
    if (source == NULL) {
        printf("City %s does not exist\n", city);
        getchar();
        return;
    }

    ptrcity queue[MAX_CITIES];
    int front = 0, rear = 0;

    bool visited[MAX_CITIES] = {false};

    ptrcity city_list[MAX_CITIES];
    int city_count = 0;
    ptrcity current = head;
    while (current != NULL && city_count < MAX_CITIES) {
        city_list[city_count] = current;
        city_count++;
        current = nex1(current);
    }

    int source_idx = -1;
    for (int i = 0; i < city_count; i++) {
        if (city_list[i] == source) {
            source_idx = i;
            break;
        }
    }

    visited[source_idx] = true;
    queue[rear++] = source;

    printf("Cities that can be reached from %s : ", city);

    while (front < rear) {
        ptrcity current_city = queue[front++];

        if (current_city != source) {
            printf("%s ", cityname(current_city));
        }

        Connection* conn = current_city->connections;
        while (conn != NULL) {
            ptrcity adjacent_city = conn->destination;

            int adjacent_idx = -1;
            for (int i = 0; i < city_count; i++) {
                if (city_list[i] == adjacent_city) {
                    adjacent_idx = i;
                    break;
                }
            }

            if (adjacent_idx != -1 && !visited[adjacent_idx]) {
                visited[adjacent_idx] = true;
                queue[rear++] = adjacent_city;
            }

            conn = conn->next;
        }
    }

    printf("\n");
    getchar();
}

void path_check(ptrcity head, char city1[MAX_CITY_NAME_LENGTH], char city2[MAX_CITY_NAME_LENGTH]) {
    ptrcity source = find_city(head, city1);
    ptrcity destination = find_city(head, city2);

    if (source == NULL) {
        printf("City %s does not exist\n", city1);
        getchar();
        return;
    } else if (destination == NULL) {
        printf("City %s does not exist\n", city2);
        getchar();
        return;
    }

    if (strcmp(city1, city2) == 0) {
        printf("There is a path between %s and %s (same city)\n", city1, city2);
        getchar();
        return;
    }

    ptrcity queue[MAX_CITIES];
    int front = 0, rear = 0;
    
    ptrcity city_list[MAX_CITIES];
    int city_indices[MAX_CITIES];
    int city_count = 0;
    
    ptrcity current = head;
    while (current != NULL && city_count < MAX_CITIES) {
        city_list[city_count] = current;
        city_indices[city_count] = (int)((long)current % MAX_CITIES);
        city_count++;
        current = nex1(current);
    }
    
    bool visited[MAX_CITIES] = {false};
    for (int i = 0; i < MAX_CITIES; i++) {
        visited[i] = false;
    }
    
    ptrcity parent[MAX_CITIES] = {NULL};
    for (int i = 0; i < MAX_CITIES; i++) {
        parent[i] = NULL;
    }
    
    int source_idx = -1, dest_idx = -1;
    for (int i = 0; i < city_count; i++) {
        if (city_list[i] == source) source_idx = i;
        if (city_list[i] == destination) dest_idx = i;
    }
    
    queue[rear++] = source;
    visited[source_idx] = true;
    
    bool path_found = false;
    
    while (front < rear && !path_found) {
        ptrcity current_city = queue[front++];
        
        int current_idx = -1;
        for (int i = 0; i < city_count; i++) {
            if (city_list[i] == current_city) {
                current_idx = i;
                break;
            }
        }
        
        Connection* conn = current_city->connections;
        while (conn != NULL) {
            ptrcity adjacent_city = conn->destination;
            
            int adjacent_idx = -1;
            for (int i = 0; i < city_count; i++) {
                if (city_list[i] == adjacent_city) {
                    adjacent_idx = i;
                    break;
                }
            }
            
            if (adjacent_idx != -1 && !visited[adjacent_idx]) {
                parent[adjacent_idx] = current_city;
                
                if (adjacent_city == destination) {
                    path_found = true;
                    break;
                }
                
                visited[adjacent_idx] = true;
                queue[rear++] = adjacent_city;
            }
            
            conn = conn->next;
        }
    }
    
    if (path_found) {
        printf("There is a path between %s and %s\n", city1, city2);
        
        printf("Path: ");
        
        ptrcity path[MAX_CITIES];
        int path_length = 0;
        
        ptrcity current_city = destination;
        path[path_length++] = current_city;
        
        while (current_city != source) {
            int current_idx = -1;
            for (int i = 0; i < city_count; i++) {
                if (city_list[i] == current_city) {
                    current_idx = i;
                    break;
                }
            }
            
            if (current_idx == -1 || parent[current_idx] == NULL) break;
            
            current_city = parent[current_idx];
            path[path_length++] = current_city;
        }
        
        for (int i = path_length - 1; i >= 0; i--) {
            printf("%s", cityname(path[i]));
            if (i > 0) printf(" -> ");
        }
        printf("\n");
    } else {
        printf("There is no path between %s and %s\n", city1, city2);
    }
    
    getchar();
}

void print_path(int parent[], int index, ptrcity city_list[]) {
    int path[MAX_CITIES];
    int path_length = 0;

    while (index != -1) {
        path[path_length++] = index;
        index = parent[index];
    }

    for (int i = path_length - 1; i >= 0; i--) {
        printf("%s", cityname(city_list[path[i]]));
        if (i > 0) {
            printf(" -> ");
        }
    }
}

int min_distance(int distance[], bool visited[], int city_count) {
    int min = INF, min_index = -1;

    for (int i = 0; i < city_count; i++) {
        if (!visited[i] && distance[i] < min) {
            min = distance[i];
            min_index = i;
        }
    }

    return min_index;
}

void shortest_path(ptrcity head, char city1[MAX_CITY_NAME_LENGTH], char city2[MAX_CITY_NAME_LENGTH]) {
    ptrcity source = find_city(head, city1);
    ptrcity destination = find_city(head, city2);

    if (source == NULL) {
        printf("City %s does not exist\n", city1);
        getchar();
        return;
    } else if (destination == NULL) {
        printf("City %s does not exist\n", city2);
        getchar();
        return;
    }

    if (strcmp(city1, city2) == 0) {
        printf("The shortest path between %s and %s is: %s\n", city1, city2, city1);
        getchar();
        return;
    }

    ptrcity city_list[MAX_CITIES];
    int city_count = 0;
    ptrcity current = head;
    while (current != NULL && city_count < MAX_CITIES) {
        city_list[city_count] = current;
        city_count++;
        current = nex1(current);
    }

    int distance[MAX_CITIES];
    bool visited[MAX_CITIES];
    int parent[MAX_CITIES];

    for (int i = 0; i < city_count; i++) {
        distance[i] = INF;
        visited[i] = false;
        parent[i] = -1;
    }

    int source_idx = -1;
    for (int i = 0; i < city_count; i++) {
        if (city_list[i] == source) {
            source_idx = i;
            break;
        }
    }

    distance[source_idx] = 0;

    for (int count = 0; count < city_count - 1; count++) {
        int u = min_distance(distance, visited, city_count);
        if (u == -1) break;

        visited[u] = true;

        Connection* conn = city_list[u]->connections;
        while (conn != NULL) {
            ptrcity adjacent_city = conn->destination;

            int v = -1;
            for (int i = 0; i < city_count; i++) {
                if (city_list[i] == adjacent_city) {
                    v = i;
                    break;
                }
            }

            if (v != -1 && !visited[v] && distance[u] != INF && distance[u] + conn->distance < distance[v]) {
                distance[v] = distance[u] + conn->distance;
                parent[v] = u;
            }
            conn = conn->next;
        }
    }

    int dest_idx = -1;
    for (int i = 0; i < city_count; i++) {
        if (city_list[i] == destination) {
            dest_idx = i;
            break;
        }
    }

    if (distance[dest_idx] == INF) {
        printf("There is no path between %s and %s\n", city1, city2);
    } else {
        printf("The shortest path between %s and %s has a distance of %d\n", city1, city2, distance[dest_idx]);
        printf("Path: ");
        print_path(parent, dest_idx, city_list);
        printf("\n");
    }

    getchar();
}

void add_city(ptrcity head, char city[MAX_CITY_NAME_LENGTH]) {
    ptrcity new_city;
    ptrcity prev;

    allocate_city(&new_city);
    while(head!= NULL){
        if(strcmp(cityname(head),city) == 0){
            printf("City %s already exists\n",city);
            return;
        }
        prev = head;
        head = nex1(head);
    }
    ass_city(new_city,city);
    ass_next1(prev,new_city);
}

void add_path(ptrcity head,char city1[MAX_CITY_NAME_LENGTH],char city2[MAX_CITY_NAME_LENGTH],int distance){
    ptrcity source = find_city(head,city1);
    ptrcity destination = find_city(head,city2);
    if(source == NULL){
        printf("City %s does not exist\n",city1);
    }
    else if(destination == NULL){
        printf("City %s does not exist\n",city2);
    }
    else{
        if((source->distance1 == 0)||(source->distance2 == 0)){
            add_connection(source,destination,distance);
        }
        else{
            printf("Impossible to add a path between these two cities");
        }
    }
    getchar();
}

void remove_city(ptrcity* head, char* city_name) {
    ptrcity city_to_remove = find_city(*head, city_name);
    if (city_to_remove == NULL) {
        printf("City '%s' not found.\n", city_name);
        getchar();
        return;
    }
    
    ptrcity current = *head;
    while (current != NULL) {
        if (current == city_to_remove) {
            current = nex1(current);
            continue;
        }
        
        Connection* prev = NULL;
        Connection* conn = current->connections;
        
        while (conn != NULL) {
            if (conn->destination == city_to_remove) {
                if (prev == NULL) {
                    current->connections = conn->next;
                    free(conn);
                    conn = current->connections;
                } else {
                    prev->next = conn->next;
                    free(conn);
                    conn = prev->next;
                }
            } else {
                prev = conn;
                conn = conn->next;
            }
        }
        
        current = nex1(current);
    }
    
    if (*head == city_to_remove) {
        *head = nex1(city_to_remove);
    } else {
        current = *head;
        while (current != NULL && nex1(current) != city_to_remove) {
            current = nex1(current);
        }
        
        if (current != NULL) {
            ass_next1(current, nex1(city_to_remove));
        }
    }
    
    free_city(city_to_remove);
    printf("City '%s' and all connections to it removed successfully.\n", city_name);
    getchar();
}

void opening() {
    printf("**************************************************************************\n");
    printf("*                         WELCOME TO THE TP PROGRAM                      *\n");
    printf("*------------------------------------------------------------------------*\n");
    printf("*               Created by : YOUCEF SHAATH & RAYANE NAIT SAIDI           *\n");
    printf("*                             TP 1: LINKED LISTS                         *\n");
    printf("**************************************************************************\n\n");

    printf("                                  Loading");
    for (int i = 0; i < 3; i++) {
        printf(".");
        sleep(1);
    }
    printf("\n\n");
    printf("                Setup Complete! Press any Key to Start...\n\n");
    sleep(1);
}

void menu()    
{
    printf("**************************************************************************\n");
    printf("*                       WELCOME TO THE MAIN MENU                         *\n");
    printf("*------------------------------------------------------------------------*\n");
    printf("*   1.Display the graph of cities.                                       *\n");
    printf("*   2.Show the cities that can be reached from a given city.             *\n");
    printf("*   3.Check if there is a path between two cities.                       *\n");
    printf("*   4.From a given city, determine the best path to reach another city.  *\n");
    printf("*   5.Add a city.                                                        *\n");
    printf("*   6.Add A path from a city to another.                                 *\n");
    printf("*   7.remove a city.                                                     *\n");
    printf("*   8.QUIT.                                                              *\n");
    printf("**************************************************************************\n\n");
    printf("\n") ; 
}

void clrscr() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

int main() {
    opening();
    ptrcity head = NULL;
    create_list(&head);
    
    int choice;
    char city1[MAX_CITY_NAME_LENGTH], city2[MAX_CITY_NAME_LENGTH];
    int distance;
    char city[MAX_CITY_NAME_LENGTH];
    
    do {
        clrscr();
        menu();
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); 
        
        switch(choice) {
            case 1:
                printf("\n--- City Graph ---\n");
                display_graph(head);
                break;
                
            case 2:
                printf("Enter the city name: ");
                scanf("%s", city);
                reached_cities(head,city);
                break;
                
            case 3:
                printf("Enter the first city name: ");
                scanf("%s", city1);
                printf("Enter the second city name: ");
                scanf("%s", city2);
                path_check(head,city1,city2);
                break;
                
            case 4:
                printf("Enter the first city name: ");
                scanf("%s", city1);
                printf("Enter the second city name: ");
                scanf("%s", city2);
                shortest_path(head, city1, city2);
                printf("\n");
                break;
                
            case 5:
                printf("Enter the city name: ");
                scanf("%s", city);
                add_city(head, city);
                break;
                
            case 6:
                printf("Enter the first city name: ");
                scanf("%s", city1);
                printf("Enter the second city name: ");
                scanf("%s", city2);
                printf("Enter the distance: ");
                scanf("%d", &distance);
                add_path(head, city1, city2, distance);
                break;
                
            case 7:
                printf("Enter the city name: ");
                scanf("%s", city);
                remove_city(&head, city);
                break;
                
            case 8:
                printf("Exiting program...\n");
                break;
                
            default:
                printf("Invalid choice. Please enter a number between 1 and 8.\n");
        }
        
        if (choice != 8) {
            printf("\nPress Enter to continue...");
            getchar();
        }
    } while (choice != 8);
    
    return 0;
}