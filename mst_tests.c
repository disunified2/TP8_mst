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

char *(*tests_functions[]) () = {
  test_mst_dummy,
  test_graph_destroy_null,
  test_graph_destroy_no_vertices,
  test_graph_destroy_no_edges,
  test_graph_destroy_complete,
  test_graph_add_vertex_null,
  test_graph_add_vertex_small,
  test_graph_add_vertex_large,
  test_graph_add_edge_null_vertex,
  test_graph_add_edge_already_present,
  test_graph_add_edge_large
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
