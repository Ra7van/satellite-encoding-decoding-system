#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 16
#define MAX_SIZE 100
#define MAX_SEQUENCE 1000

typedef struct Node Node;
struct Node {
    char *name;
    int freq;
    Node *left, *right;
};

typedef struct {
    Node **array;
    int size;
    int capacity;
} Min_Heap;

Node *node_create(char *name, int freq, int name_size) { // creeaza un nod nou
    Node *new_node = malloc(sizeof(Node));
    new_node->name = malloc(name_size * sizeof(char)); // aloc spatiu pt nume
    strncpy(new_node->name, name, name_size - 1);
    new_node->name[name_size - 1] = '\0'; // asigur terminarea cu null
    new_node->freq = freq;
    new_node->left = new_node->right = NULL;
    return new_node;
}

Min_Heap *min_heap_create() { // creeaza un min-heap
    Min_Heap *heap = malloc(sizeof(Min_Heap));
    heap->capacity = MAX_SIZE;
    heap->array = malloc(heap->capacity * sizeof(Node*));
    heap->size = 0;
    return heap;
}

void swap(Node **a, Node **b) { // interschimb doua noduri
    Node *temp = *a;
    *a = *b;
    *b = temp;
}

int compare(Node *a, Node *b) { // compar doua noduri
    if (a->freq == b->freq)
        return strcmp(a->name, b->name);
    return a->freq - b->freq;
}

void heapify_down(Min_Heap *heap, int index) { // rearanjez heap-ul in jos
    int nr;
    int smallest = index;
    int left = 2 * index + 1; // calculez pozitia copilului stang
    int right = 2 * index + 2; // calculez pozitia copilului drept
    //verific daca copilul stang e mai mic
    if (left < heap->size  && heap->array[left]) {
        nr = compare(heap->array[left], heap->array[smallest]);
        if (nr < 0)
            smallest = left;
    }

    // verific daca copilul drept e mai mic
    if (right < heap->size && heap->array[right]) {
        nr = compare(heap->array[right], heap->array[smallest]);
        if (nr < 0)
            smallest = right;
    }
    
    // daca cel mai mic e diferit de indexul curent, interschimb
    if (smallest != index) {
        swap(&heap->array[index], &heap->array[smallest]);
        heapify_down(heap, smallest);
    }
}

void heapify_up(Min_Heap *heap, int index) { // rearanjez heap-ul in sus
    //verific daca fiul e mai mic decat parintele
    int nr = compare(heap->array[index], heap->array[(index - 1) / 2]);
    if (index && nr < 0) {
        swap(&heap->array[index], &heap->array[(index - 1) / 2]);
        heapify_up(heap, (index - 1) / 2);
    }
}

void min_heap_insert(Min_Heap *heap, Node *node) {
    // verific daca heap-ul e NULL
    if (!heap->array) {
        heap->capacity = MAX_SIZE;
        heap->array = malloc(heap->capacity * sizeof(Node*));
    }
    if (heap->size >= heap->capacity) {
        heap->capacity *= 2;
        Node **new_array = realloc(heap->array, heap->capacity *sizeof(Node*));
        if (!new_array) return;
        heap->array = new_array;
    }
    // introduc nodul in heap
    heap->array[heap->size] = node;
    // rearanjez heap-ul
    heapify_up(heap, heap->size);
    // cresc dimensiunea heap-ului
    heap->size++;
}

Node *min_heap_extract_min(Min_Heap *heap) { // extrage nodul minim din heap
    if (heap->size == 0)
        return NULL;

    Node *min_node = heap->array[0];
    heap->array[0] = heap->array[heap->size - 1];
    heap->array[heap->size - 1] = NULL; // elimin ultimul nod
    heap->size--; // elimin nodul minim
    if (heap->size > 1)
        heapify_down(heap, 0); // rearanjez heap-ul heap->array = NULL;

    return min_node;
}

int tree_height(Node *node) { // calculeaza inaltimea arborelui
    if (node == NULL)
        return 0;
    int left_height = tree_height(node->left); // inaltimea subarborelui stang
    int right_height = tree_height(node->right);// inaltimea subarborelui drept
    return (left_height > right_height ? left_height : right_height) + 1;
}

// numar nodurile de la un nivel dat
int count_nodes_at_level(Node *node, int level) {
    if (node == NULL)
        return 0;
    if (level == 1)
        return 1;
    // numar nodurile din subarborele stang si drept
    int left_count = count_nodes_at_level(node->left, level - 1);
    int right_count = count_nodes_at_level(node->right, level - 1);
    return left_count + right_count;
}

// afiseaza un nivel al arborelui
void
print_level(Node *node, int level, int node_index, int total_nodes, FILE *out) {
    if (node == NULL)
        return;
    if (level == 1) {
        fprintf(out, "%d-%s", node->freq, node->name);
        if (node_index < total_nodes - 1) // verific daca e ultimul nod
            fprintf(out, " ");
    } else {
        // afisez nodurile din subarborele stang si drept, recursiv
        print_level(node->left, level - 1, node_index, total_nodes, out);
        int count = count_nodes_at_level(node->left, level - 1);
        print_level(node->right, level - 1, node_index + count, total_nodes, out);
    }
}

void print_tree(Node *root, FILE *out) { // afiseaza arborele
    int height = tree_height(root);
    for (int i = 1; i <= height; i++) {
        int total_nodes = count_nodes_at_level(root, i);
        print_level(root, i, 0, total_nodes, out);
        fprintf(out, "\n");
    }
}

Node* build_tree(int n, Node **nodes) { // construieste arborele din noduri
    Min_Heap *heap = min_heap_create();
    for (int i = 0; i < n; i++) 
        min_heap_insert(heap, nodes[i]); // introduc nodurile in heap

    while (heap->size > 1) {
        Node *left = min_heap_extract_min(heap); // extrag nodul minim
        Node *right = min_heap_extract_min(heap); // extrag al doilea nod minim

        if (compare(right, left) < 0) 
            swap(&left, &right);

        int name_size = strlen(left->name) + strlen(right->name) + 1;
        char *new_name = malloc(name_size * sizeof(char));
        // creez un nume nou pentru nodul parinte
        new_name[0] = '\0';
        strncat(new_name, left->name, name_size - 1);
        strncat(new_name, right->name, name_size - 1 - strlen(new_name));
        // calculez frecventa nodului parinte
        int new_freq = left->freq + right->freq;
        Node *parent = node_create(new_name, new_freq, name_size);
        parent->left = left;
        parent->right = right;
        free(new_name);
        // introduc nodul parinte in heap
        min_heap_insert(heap, parent);
    }
    // extrag radacina arborelui
    Node *root = min_heap_extract_min(heap);
    if (heap->array)
        free(heap->array);
    free(heap);
    return root;
}

void tree_free(Node *node) { // elibereaza arborele recursiv
    if (node == NULL)
        return;
    tree_free(node->left);
    tree_free(node->right);
    free(node->name);
    free(node);
}

// cauta un nod in arbore pe baza secventei binare
void path(Node *root, char *seq, FILE *out) {
    Node *current = root;
    for (int i = 0; i < (int)strlen(seq); i++) {
        // verific daca e frunza
        if (current->left == NULL && current->right == NULL) {
            fprintf(out, "%s ", current->name);
            current = root; // ma duc inapoi la radacina
        }
        if (seq[i] == '0' && current->left) // verific daca exista nodul stang
            current = current->left;
        else if (seq[i] == '1' && current->right) 
        //verific daca exista nodul drept
            current = current->right;
        else
            current = root; // ma intorc la radacina
    }
    if (current && !current->left && !current->right) //afisez ultimul nod
        fprintf(out, "%s", current->name);
}

// caut un nod in arbore pe baza secventei binare
void binary(Node *root, FILE *in, FILE *out) {
    int n;
    fscanf(in, "%d", &n);
    char seq[MAX_SEQUENCE];
    for (int i = 0; i < n; i++) {
        fscanf(in, "%s", seq); // citesc fiecare secventa
        path(root, seq, out);
        fprintf(out, "\n");
    }
}

// caut un nod in arbore pe baza numelui si afisez calea catre el
void name_path(Node *node, char *result, char *name, int len, FILE *out) {
    if (!node)
        return;
    if (!node->left && !node->right && strcmp(node->name, name) == 0) {
        result[len] = '\0'; // termin codul
        fprintf(out, "%s", result);
        return;
    }
    if (node->left) {
        result[len] = '0'; // adaug 0 la cod
        name_path(node->left, result, name, len + 1, out);
    }
    if (node->right) {
        result[len] = '1'; // adaug 1 la cod
        name_path(node->right, result, name, len + 1, out);
    }
}

// printez secventele de cod pentru fiecare satelit
void answear(Node *root, FILE *in, FILE *out) {
    int n;
    fscanf(in, "%d", &n);
    for (int i = 0; i < n; i++) {
        char name[MAX_NAME];
        char result[MAX_SIZE];
        fscanf(in, "%s", name); // citesc fiecare nume
        name_path(root, result, name, 0, out); // caut secventa de cod
    }
    fprintf(out, "\n");
}


Node* find_common_parent(Node *node, char **name, int n) {
    if (!node)
        return NULL;
    int cnt = 0;
    for (int i = 0; i < n; i++)
        // verific daca numele se regaseste in nod
        if (strstr(node->name, name[i]))
            cnt++;

    if (cnt == n) {
        // caut in subarborele stang
        Node *left = find_common_parent(node->left, name, n);
        // caut in subarborele drept
        Node *right = find_common_parent(node->right, name, n);
        if (!left && !right)
            return node; // daca nu am gasit nimic, returnez nodul curent
        return left ? left : right; // returnez nodul gasit
    }
    return NULL;
}

void repair(Node *root, FILE *in, FILE *out) {
    int n;
    fscanf(in, "%d", &n);
    char **name = malloc(n * sizeof(char*)); // aloc memorie pentru nume
    for (int i = 0; i < n; i++) {
        name[i] = malloc(MAX_NAME * sizeof(char));
        // aloc memorie pentru fiecare nume
        fscanf(in, "%s", name[i]); // citesc fiecare nume
    }

    Node *parent = find_common_parent(root, name, n); // caut parintele comun
    
    if (parent)
        fprintf(out, "%s\n", parent->name); // afisez numele parintelui

    for (int i = 0; i < n; i++)
        free(name[i]); // eliberez fiecare nume
    free(name); // eliberez vectorul de nume
}


int main(int argc, char *argv[]) {
    if (argc != 4) // verific numarul de argumente
        return 1;

    FILE *in = fopen(argv[2], "rt");
    FILE *out = fopen(argv[3], "wt");

    int n;
    fscanf(in, "%d", &n);

    Node **nodes = malloc(n * sizeof(Node*)); // aloc memorie pentru noduri
    for (int i = 0; i < n; i++) {
        int freq;
        char name[MAX_NAME];
        fscanf(in, "%d %s", &freq, name);
        nodes[i] = node_create(name, freq, MAX_NAME); // creez fiecare nod
    }

    Node *tree = build_tree(n, nodes); // construiesc arborele
    if (strcmp(argv[1], "-c1") == 0) {
        print_tree(tree, out); // afisez arborele
    } else if (strcmp(argv[1], "-c2") == 0) {
        binary(tree, in, out); // decodific fiecare secventa
    } else if (strcmp(argv[1], "-c3") == 0) {
        answear(tree, in, out); // caut fiecare satelit si il afisez
    } else if (strcmp(argv[1], "-c4") == 0) {
        repair(tree, in, out); 
        // gaseste parintele cel mai mic comun pt toti satelitii dati
    }

    tree_free(tree);
    free(nodes);
    fclose(in);
    fclose(out);
    return 0;
}
