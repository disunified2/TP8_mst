#ifndef MST_H
#define MST_H

#include <stddef.h>

/* Graph data structure */

typedef struct sgraph_vertex
{
  size_t degree;
  struct sgraph_vertex **neighbors;
  int *weights;
  // Data for MST procedure
  int distance;
  int index;                    // index in the heap queue
  struct sgraph_vertex *parent;
} graph_vertex;

typedef struct sgraph
{
  size_t size;
  graph_vertex **vertices;
} graph;

/*
 * Create an empty graph
 */
void graph_create (graph * self);

/*
 * Destroy a graph
 */
void graph_destroy (graph * self);

/*
 * Add a vertex with given key
 */
graph_vertex *graph_add_vertex (graph * self);

/*
 * Add an edge between two vertices
 */
void graph_add_edge (graph_vertex * source, graph_vertex * destination,
                     int weight);


/* Queue data structure */

typedef struct
{
  size_t size;
  int *priorities;
  graph_vertex **vertices;
} queue;

/*
 * Create an empty priority queue
 */
void queue_create (queue * self);

/*
 * Destroy a queue
 */
void queue_destroy (queue * self);

/*
 * Size of the priority queue
 */
size_t queue_size (queue * self);

/*
 * Remove and return the vertex with minimum value in the priority queue
 */
graph_vertex *queue_extract_min (queue * self);

/*
 * Decrease the value of a vertex key at index i
 */
void queue_decrease_key (queue * self, size_t i, int key);

/*
 * Insert a vertex in the priority queue
 */
void queue_insert (queue * self, graph_vertex * vertex, int key);


/* Minimum Spanning Tree algorithm */

/*
 * Run Prim's algorithm to set parent and return the weight of the MST
 */
int mst_prim (const graph * self, graph_vertex * source);

#endif // MST_H
