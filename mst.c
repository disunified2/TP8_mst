#include "mst.h"

#include <stdlib.h>
#include <stdio.h>
#include <values.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

void graph_create (graph * self)
{
  if (self == NULL)
    {
      return;
    }

  self->size = 0;
  self->vertices = calloc (2, sizeof (graph_vertex));
}

void graph_destroy_vertices (graph_vertex * vertex)
{
  free (vertex->neighbors);
  free (vertex->weights);
  vertex->neighbors = NULL;
  vertex->weights = NULL;
}

void graph_destroy (graph * self)
{
  if (self == NULL)
    {
      return;
    }

  for (size_t i = 0; i < self->size; ++i)
    {
      graph_destroy_vertices (self->vertices[i]);
      free (self->vertices[i]);
    }
  free (self->vertices);
  self->vertices = NULL;
}

graph_vertex *graph_add_vertex (graph * self)
{
  if (self == NULL)
    {
      return NULL;
    }

  graph_vertex *vertex = calloc (1, sizeof (graph_vertex));
  vertex->degree = 0;
  vertex->neighbors = calloc (2, sizeof (graph_vertex));
  vertex->weights = calloc (2, sizeof (int));

  self->vertices =
    realloc (self->vertices, ++self->size * sizeof (graph_vertex *));
  self->vertices[self->size - 1] = vertex;

  return vertex;
}

void graph_add_edge (graph_vertex * source, graph_vertex * destination,
                     int weight)
{
  if (source == NULL || destination == NULL)
    {
      return;
    }

  if (source->degree != 0)
    {
      if (source->neighbors[source->degree - 1] != NULL)
        {
          graph_vertex **new =
            calloc (2 * source->degree, sizeof (graph_vertex *));
          memcpy (new, source->neighbors,
                  source->degree * sizeof (graph_vertex *));
          free (source->neighbors);
          source->neighbors = new;

          int *new_weights = calloc (2 * source->degree, sizeof (int));
          memcpy (new_weights, source->weights,
                  source->degree * sizeof (int));
          free (source->weights);
          source->weights = new_weights;
        }
    }
  source->neighbors[source->degree] = destination;
  source->weights[source->degree] = weight;
  source->degree++;

  if (destination->degree != 0)
    {
      if (destination->neighbors[destination->degree - 1] != NULL)
        {
          graph_vertex **new =
            calloc (2 * destination->degree, sizeof (graph_vertex *));
          memcpy (new, destination->neighbors,
                  destination->degree * sizeof (graph_vertex *));
          free (destination->neighbors);
          destination->neighbors = new;

          int *new_weights = calloc (2 * destination->degree, sizeof (int));
          memcpy (new_weights, destination->weights,
                  destination->degree * sizeof (int));
          free (destination->weights);
          destination->weights = new_weights;
        }
    }
  destination->neighbors[destination->degree] = source;
  destination->weights[destination->degree] = weight;
  destination->degree++;
}

void queue_create (queue * self)
{
  if (self == NULL)
    {
      return;
    }
  self->size = 0;
  self->priorities = calloc (2, sizeof (int));
  self->vertices = calloc (2, sizeof (graph_vertex));
}

void queue_destroy (queue * self)
{
  if (self == NULL)
    {
      return;
    }
  free (self->priorities);
  free (self->vertices);
}

size_t queue_size (queue * self)
{
  if (self == NULL)
    {
      return 0;
    }
  return self->size;
}

void min_heapify (queue * self, size_t i)
{
  if (i >= self->size)
    {
      return;
    }
  size_t left = 2 * i + 1;
  size_t right = 2 * i + 2;
  size_t min;

  if (left < self->size && self->priorities[left] < self->priorities[i])
    {
      min = left;
    }
  else
    {
      min = i;
    }
  if (right < self->size && self->priorities[right] < self->priorities[min])
    {
      min = right;
    }
  if (min != i)
    {
      graph_vertex *tmp = self->vertices[i];
      self->vertices[i] = self->vertices[min];
      self->vertices[min] = tmp;

      int temp = self->priorities[i];
      self->priorities[i] = self->priorities[min];
      self->priorities[min] = temp;

      min_heapify (self, min);
    }
}

graph_vertex *queue_minimum (queue * self)
{
  if (self->size < 1)
    {
      return NULL;
    }
  return self->vertices[0];
}

graph_vertex *queue_extract_min (queue * self)
{
  if (self == NULL)
    {
      return NULL;
    }

  graph_vertex *min = queue_minimum (self);
  self->priorities[0] = self->priorities[self->size - 1];
  self->vertices[0] = self->vertices[self->size - 1];
  --self->size;
  min_heapify (self, 0);
  return min;
}

void queue_decrease_key (queue * self, size_t i, int key)
{
  if (self == NULL)
    {
      return;
    }

  if (i > self->size - 1 || key > self->priorities[i])
    {
      return;
    }

  self->priorities[i] = key;
  self->vertices[i]->index = (int) i;
  while (i > 0 && self->priorities[(i - 1) / 2] > self->priorities[i])
    {
      graph_vertex *tmp = self->vertices[(i - 1) / 2];
      self->vertices[(i - 1) / 2] = self->vertices[i];
      self->vertices[i] = tmp;

      int temp = self->priorities[(i - 1) / 2];
      self->priorities[(i - 1) / 2] = self->priorities[i];
      self->priorities[i] = temp;

      self->vertices[(i - 1) / 2]->index = (int) (i - 1) / 2;
      self->vertices[i]->index = (int) i;

      i = (i - 1) / 2;
    }
}

void queue_insert (queue * self, graph_vertex * vertex, int key)
{
  if (self == NULL || vertex == NULL)
    {
      return;
    }

  if (self->size != 0 && self->vertices[self->size - 1] != NULL)
    {
      graph_vertex **new_vertices =
        calloc (2 * self->size, sizeof (graph_vertex *));
      memcpy (new_vertices, self->vertices,
              self->size * sizeof (graph_vertex *));
      free (self->vertices);
      self->vertices = new_vertices;

      int *new_priorities = calloc (2 * self->size, sizeof (int));
      memcpy (new_priorities, self->priorities, self->size * sizeof (int));
      free (self->priorities);
      self->priorities = new_priorities;
    }

  ++self->size;
  self->priorities[self->size - 1] = INT_MAX;
  self->vertices[self->size - 1] = vertex;
  queue_decrease_key (self, self->size - 1, key);
}

int mst_prim (const graph * self, graph_vertex * source)
{
  if (self == NULL || source == NULL)
    {
      return 0;
    }

  for (size_t i = 0; i < self->size; ++i)
    {
      self->vertices[i]->distance = INT_MAX;
      self->vertices[i]->parent = NULL;
    }
  source->distance = 0;

  queue *q = calloc (1, sizeof (queue));
  queue_create (q);

  for (size_t i = 0; i < self->size; ++i)
    {
      queue_insert (q, self->vertices[i], self->vertices[i]->distance);
    }

  while (queue_size (q) != 0)
    {
      graph_vertex *u = queue_extract_min (q);
      for (size_t i = 0; i < u->degree; ++i)
        {
          graph_vertex *v = u->neighbors[i];
          if (q->vertices[v->index] == v && u->weights[i] <= v->distance)
            {
              v->parent = u;
              v->distance = u->weights[i];
              queue_decrease_key (q, v->index, u->weights[i]);
            }
        }
    }

  int total = 0;
  for (size_t i = 0; i < self->size; ++i)
    {
      total += self->vertices[i]->distance;
    }

  queue_destroy (q);
  free (q);
  return total;
}
