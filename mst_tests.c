#include "mst.h"

#include <stdio.h>
#include <stdlib.h>
#include <values.h>

#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { const char *message = test(); \
    if (message) printf ("Test %d failed: %s\n", tests_index, message); \
    else { tests_pass++; } tests_run++; } while (0)
int tests_pass, tests_run, tests_index;

static char *test_mst_dummy ()
{
  graph g;

  graph_create (&g);
  graph_vertex *vertex = graph_add_vertex (&g);
  int weight = mst_prim (&g, vertex);

  mu_assert ("error, vertex is null", vertex != NULL);
  mu_assert ("error, minimum weight is not zero", weight == 0);

  graph_destroy (&g);

  return NULL;
}

static char *test_graph_destroy_null ()
{
  graph *g = NULL;

  graph_destroy (g);

  mu_assert ("error deleting null graph", g == NULL);
  return NULL;
}

static char *test_graph_destroy_complete ()
{
  graph *g = calloc (1, sizeof (graph));

  graph_create (g);
  graph_vertex *v1 = graph_add_vertex (g);
  graph_vertex *v2 = graph_add_vertex (g);
  graph_vertex *v3 = graph_add_vertex (g);

  graph_add_edge (v1, v2, 0);
  graph_add_edge (v1, v3, 0);
  graph_add_edge (v2, v3, 0);

  graph_destroy (g);

  mu_assert ("error destroying complete graph", g->vertices == NULL);
  free (g);
  return NULL;
}

static char *test_graph_add_vertex_null ()
{
  graph *g = NULL;

  graph_add_vertex (g);

  mu_assert ("error adding a vertex to null graph", g == NULL);
  return NULL;
}

static char *test_graph_add_vertex_small ()
{
  graph *g = calloc (1, sizeof (graph));

  graph_create (g);
  for (size_t i = 0; i < 10; ++i)
    {
      graph_add_vertex (g);
    }

  mu_assert ("error adding 10 vertices", g->size == 10);

  graph_destroy (g);
  free (g);
  return NULL;
}

static char *test_graph_add_vertex_large ()
{
  graph *g = calloc (1, sizeof (graph));

  graph_create (g);
  for (size_t i = 0; i < 1000; ++i)
    {
      graph_add_vertex (g);
    }

  mu_assert ("error adding 10 vertices", g->size == 1000);

  graph_destroy (g);
  free (g);
  return NULL;
}

static char *test_graph_add_edge_null_vertex ()
{
  graph *g = calloc (1, sizeof (graph));

  graph_create (g);

  graph_vertex *v = graph_add_vertex (g);
  graph_add_edge (v, NULL, 0);

  mu_assert ("error adding an edge to null vertices", v->degree == 0);
  graph_destroy (g);
  free (g);
  return NULL;
}

static char *test_graph_add_edge_already_present ()
{
  graph *g = calloc (1, sizeof (graph));

  graph_create (g);

  graph_vertex *v1 = graph_add_vertex (g);
  graph_vertex *v2 = graph_add_vertex (g);

  graph_add_edge (v1, v2, 0);
  graph_add_edge (v1, v2, 0);

  mu_assert ("error adding duplicate of an edge (degree)", v1->degree == 2
             && v2->degree == 2);
  mu_assert ("error adding duplicate of an edge (neighbors)",
             (v1->neighbors[0] == v2 && v1->neighbors[1] == v2)
             && (v2->neighbors[0] == v1 && v2->neighbors[1] == v1));
  mu_assert ("error adding duplicate of an edge (weights)",
             (v1->weights[0] == 0 && v1->weights[1] == 0) &&
             (v2->weights[0] == 0 && v2->weights[1] == 0));
  graph_destroy (g);
  free (g);
  return NULL;
}

static char *test_graph_add_edge_large ()
{
  graph *g = calloc (1, sizeof (graph));

  graph_create (g);

  graph_vertex *v1 = graph_add_vertex (g);
  graph_vertex *v2 = graph_add_vertex (g);

  for (size_t i = 0; i < 100; ++i)
    {
      graph_add_edge (v1, v2, i);
    }

  mu_assert ("error adding large nuber of edges to two vertices",
             v1->degree == 100);
  graph_destroy (g);
  free (g);
  return NULL;
}

static char *test_queue_size_null ()
{
  queue *q = NULL;

  mu_assert ("error detecting size of null queue", queue_size (q) == 0);
  return NULL;
}

static char *test_queue_size_small ()
{
  queue *q = calloc (1, sizeof (queue));
  graph *g = calloc (1, sizeof (graph));

  queue_create (q);
  graph_create (g);

  graph_vertex *v1 = graph_add_vertex (g);
  graph_vertex *v2 = graph_add_vertex (g);

  queue_insert (q, v1, 1);
  queue_insert (q, v2, 2);

  mu_assert ("error detecting size of small queue", queue_size (q) == 2);

  queue_destroy (q);
  graph_destroy (g);
  free (q);
  free (g);
  return NULL;
}

static char *test_queue_size_large ()
{
  queue *q = calloc (1, sizeof (queue));
  graph *g = calloc (1, sizeof (graph));

  queue_create (q);
  graph_create (g);

  for (int i = 0; i < 1000; ++i)
    {
      graph_vertex *v = graph_add_vertex (g);
      queue_insert (q, v, i);
    }

  mu_assert ("error detecting size of large queue", queue_size (q) == 1000);

  queue_destroy (q);
  graph_destroy (g);
  free (q);
  free (g);
  return NULL;
}

static char *test_queue_extract_min_null ()
{
  queue *q = NULL;

  mu_assert ("error finding minimum of null queue",
             queue_extract_min (q) == NULL);
  return NULL;
}

static char *test_queue_extract_min_single ()
{
  queue *q = calloc (1, sizeof (queue));
  graph *g = calloc (1, sizeof (graph));

  queue_create (q);
  graph_create (g);

  graph_vertex *v = graph_add_vertex (g);
  queue_insert (q, v, 1);

  mu_assert ("error extracting min of single element queue",
             queue_extract_min (q) == v);
  mu_assert ("error with size once element extracted of single element queue",
             queue_size (q) == 0);

  queue_destroy (q);
  graph_destroy (g);
  free (q);
  free (g);
  return NULL;
}

static char *test_queue_extract_min_many ()
{
  queue *q = calloc (1, sizeof (queue));
  graph *g = calloc (1, sizeof (graph));

  queue_create (q);
  graph_create (g);

  graph_vertex *min = graph_add_vertex (g);
  queue_insert (q, min, 1);

  for (int i = 2; i < 11; ++i)
    {
      graph_vertex *v = graph_add_vertex (g);
      queue_insert (q, v, i);
    }

  mu_assert ("error extracting minimum element",
             queue_extract_min (q) == min);
  mu_assert ("error reducing size with extract min", queue_size (q) == 9);

  queue_destroy (q);
  graph_destroy (g);
  free (q);
  free (g);
  return NULL;
}

static char *test_queue_extract_min_many_inverted ()
{
  queue *q = calloc (1, sizeof (queue));
  graph *g = calloc (1, sizeof (graph));

  queue_create (q);
  graph_create (g);

  graph_vertex *min = graph_add_vertex (g);
  queue_insert (q, min, 1);

  for (int i = 11; i > 2; --i)
    {
      graph_vertex *v = graph_add_vertex (g);
      queue_insert (q, v, i);
    }

  mu_assert ("error extracting minimum element",
             queue_extract_min (q) == min);
  mu_assert ("error reducing size with extract min", queue_size (q) == 9);

  queue_destroy (q);
  graph_destroy (g);
  free (q);
  free (g);
  return NULL;
}

static char *test_queue_extract_min_multiple ()
{
  queue *q = calloc (1, sizeof (queue));
  graph *g = calloc (1, sizeof (graph));

  queue_create (q);
  graph_create (g);

  graph_vertex *v1 = graph_add_vertex (g);
  graph_vertex *v2 = graph_add_vertex (g);
  graph_vertex *v3 = graph_add_vertex (g);
  graph_vertex *v4 = graph_add_vertex (g);
  graph_vertex *v5 = graph_add_vertex (g);
  graph_vertex *v6 = graph_add_vertex (g);

  queue_insert (q, v1, 5);
  queue_insert (q, v2, 12);
  queue_insert (q, v3, 4);
  queue_insert (q, v4, 2);
  queue_insert (q, v5, 35);
  queue_insert (q, v6, 55);

  mu_assert ("error extracting min",
             (queue_extract_min (q) == v4 && queue_extract_min (q) == v3)
             && (queue_extract_min (q) == v1 && queue_extract_min (q) == v2)
             && (queue_extract_min (q) == v5 && queue_extract_min (q) == v6));
  mu_assert ("error", queue_size (q) == 0);

  queue_destroy (q);
  graph_destroy (g);
  free (q);
  free (g);
  return NULL;
}

static char *test_queue_decrease_key_null ()
{
  queue *q = NULL;

  queue_decrease_key (q, 1, 10);

  mu_assert ("error decreasing key on null queue", q == NULL);
  return NULL;
}

static char *test_queue_decrease_key_invalid_ind ()
{
  queue *q = calloc (1, sizeof (queue));
  graph *g = calloc (1, sizeof (graph));

  queue_create (q);
  graph_create (g);

  queue_insert (q, graph_add_vertex (g), 3);

  queue_decrease_key (q, 2, 2);

  mu_assert ("error decreasing key with invalid indice",
             q->priorities[0] == 3);
  mu_assert ("error with size on invalid indice", queue_size (q) == 1);

  queue_destroy (q);
  graph_destroy (g);
  free (q);
  free (g);
  return NULL;
}

static char *test_queue_decrease_key_invalid_key ()
{
  queue *q = calloc (1, sizeof (queue));
  graph *g = calloc (1, sizeof (graph));

  queue_create (q);
  graph_create (g);

  queue_insert (q, graph_add_vertex (g), 3);

  queue_decrease_key (q, 0, 5);

  mu_assert ("error decreasing key with invalid indice",
             q->priorities[0] == 3);
  mu_assert ("error with size on invalid indice", queue_size (q) == 1);

  queue_destroy (q);
  graph_destroy (g);
  free (q);
  free (g);
  return NULL;
}

static char *test_queue_decrease_key_single ()
{
  queue *q = calloc (1, sizeof (queue));
  graph *g = calloc (1, sizeof (graph));

  queue_create (q);
  graph_create (g);

  queue_insert (q, graph_add_vertex (g), 3);

  queue_decrease_key (q, 0, 2);

  mu_assert ("error decreasing key with invalid indice",
             q->priorities[0] == 2);
  mu_assert ("error with size on invalid indice", queue_size (q) == 1);

  queue_destroy (q);
  graph_destroy (g);
  free (q);
  free (g);
  return NULL;
}

static char *test_queue_decrease_key_no_change ()
{
  queue *q = calloc (1, sizeof (queue));
  graph *g = calloc (1, sizeof (graph));

  queue_create (q);
  graph_create (g);

  queue_insert (q, graph_add_vertex (g), 3);
  queue_insert (q, graph_add_vertex (g), 4);
  queue_insert (q, graph_add_vertex (g), 5);

  queue_decrease_key (q, 0, 2);

  mu_assert ("error decreasing key with invalid indice",
             q->priorities[0] == 2);
  mu_assert ("error with size on invalid indice", queue_size (q) == 3);

  queue_destroy (q);
  graph_destroy (g);
  free (q);
  free (g);
  return NULL;
}

static char *test_queue_decrease_key_heapsort ()
{
  queue *q = calloc (1, sizeof (queue));
  graph *g = calloc (1, sizeof (graph));
  queue *expected = calloc (1, sizeof (queue));

  queue_create (q);
  graph_create (g);
  queue_create (expected);

  graph_vertex *v1 = graph_add_vertex (g);
  graph_vertex *v2 = graph_add_vertex (g);
  graph_vertex *v3 = graph_add_vertex (g);

  queue_insert (q, v1, 3);
  queue_insert (q, v2, 4);
  queue_insert (q, v3, 5);

  queue_insert (expected, v3, 2);
  queue_insert (expected, v2, 4);
  queue_insert (expected, v1, 3);

  queue_decrease_key (q, 2, 2);

  for (size_t i = 0; i < q->size; ++i)
    {
      mu_assert ("error decreasing positioning decreased key",
                 q->vertices[i] == expected->vertices[i]);
      mu_assert ("error on size of queue",
                 queue_size (q) == queue_size (expected));
    }

  queue_destroy (q);
  graph_destroy (g);
  queue_destroy (expected);
  free (q);
  free (g);
  free (expected);
  return NULL;
}

static char *test_queue_insert_null_queue ()
{
  queue *q = NULL;
  graph *g = calloc (1, sizeof (graph));

  graph_create (g);

  queue_insert (q, graph_add_vertex (g), 1);

  mu_assert ("error inserting in null queue", queue_size (q) == 0);
  graph_destroy (g);
  free (g);
  return NULL;
}

static char *test_queue_insert_null_vertex ()
{
  queue *q = calloc (1, sizeof (queue));

  queue_create (q);

  queue_insert (q, NULL, 1);

  mu_assert ("error adding null vertex to queue", queue_size (q) == 0);
  queue_destroy (q);
  free (q);
  return NULL;
}

static char *test_queue_insert_single ()
{
  queue *q = calloc (1, sizeof (queue));
  graph *g = calloc (1, sizeof (graph));

  queue_create (q);
  graph_create (g);

  graph_vertex *v1 = graph_add_vertex (g);

  queue_insert (q, v1, 1);

  mu_assert ("error with size on insertion", queue_size (q) == 1);
  mu_assert ("error with vertex on insertion", q->vertices[0] == v1);
  mu_assert ("error with priorities on insertion", q->priorities[0] == 1);

  queue_destroy (q);
  graph_destroy (g);
  free (q);
  free (g);
  return NULL;
}

static char *test_queue_insert_ordered ()
{
  queue *q = calloc (1, sizeof (queue));
  graph *g = calloc (1, sizeof (graph));

  queue_create (q);
  graph_create (g);

  graph_vertex *v1 = graph_add_vertex (g);
  graph_vertex *v2 = graph_add_vertex (g);
  graph_vertex *v3 = graph_add_vertex (g);

  queue_insert (q, v1, 2);
  queue_insert (q, v2, 3);
  queue_insert (q, v3, 4);

  mu_assert ("error in order when inserting",
             (q->vertices[0] == v1 && q->vertices[1] == v2)
             && q->vertices[2] == v3);
  mu_assert ("error with size", queue_size (q) == 3);

  queue_destroy (q);
  graph_destroy (g);
  free (q);
  free (g);
  return NULL;
}

static char *test_queue_insert_heapsort_needed ()
{
  queue *q = calloc (1, sizeof (queue));
  graph *g = calloc (1, sizeof (graph));

  queue_create (q);
  graph_create (g);

  graph_vertex *v1 = graph_add_vertex (g);
  graph_vertex *v2 = graph_add_vertex (g);
  graph_vertex *v3 = graph_add_vertex (g);

  queue_insert (q, v1, 5);
  queue_insert (q, v2, 3);
  queue_insert (q, v3, 4);

  mu_assert ("error in order when inserting",
             (q->vertices[0] == v2 && q->vertices[1] == v1)
             && q->vertices[2] == v3);
  mu_assert ("error with size", queue_size (q) == 3);

  queue_destroy (q);
  graph_destroy (g);
  free (q);
  free (g);
  return NULL;
}

char *(*tests_functions[]) () = {
  test_mst_dummy,
  test_graph_destroy_null,
  test_graph_destroy_complete,
  test_graph_add_vertex_null,
  test_graph_add_vertex_small,
  test_graph_add_vertex_large,
  test_graph_add_edge_null_vertex,
  test_graph_add_edge_already_present,
  test_graph_add_edge_large,
  test_queue_size_null,
  test_queue_size_small,
  test_queue_size_large,
  test_queue_extract_min_null,
  test_queue_extract_min_single,
  test_queue_extract_min_many,
  test_queue_extract_min_many_inverted,
  test_queue_extract_min_multiple,
  test_queue_decrease_key_null,
  test_queue_decrease_key_invalid_ind,
  test_queue_decrease_key_invalid_key,
  test_queue_decrease_key_single,
  test_queue_decrease_key_no_change,
  test_queue_decrease_key_heapsort,
  test_queue_insert_null_queue,
  test_queue_insert_null_vertex,
  test_queue_insert_single,
  test_queue_insert_ordered,
  test_queue_insert_heapsort_needed
};

int main (int argc, const char *argv[])
{
  size_t n = sizeof (tests_functions) / sizeof (tests_functions[0]);
  if (argc == 1)
    {
      for (tests_index = 0; (size_t) tests_index < n; tests_index++)
        mu_run_test (tests_functions[tests_index]);
      if (tests_run == tests_pass)
        printf ("All %d tests passed\n", tests_run);
      else
        printf ("Tests passed/run: %d/%d\n", tests_pass, tests_run);
    }
  else
    {
      tests_index = atoi (argv[1]);
      if (tests_index < 0)
        printf ("%zu\n", n);
      else if ((size_t) tests_index < n)
        {
          mu_run_test (tests_functions[tests_index]);
          if (tests_run == tests_pass)
            printf ("Test %d passed\n", tests_index);
        }
    }
}
