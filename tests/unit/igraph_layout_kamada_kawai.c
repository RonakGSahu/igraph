/*
   IGraph library.
   Copyright (C) 2022  The igraph development team <igraph@igraph.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <igraph.h>
#include "test_utilities.h"

void make_box(int vertices, float half_size, igraph_vector_t *bounds) {
    for (int i = 0; i < 4; i++) {
        igraph_vector_init(&bounds[i], vertices);
    }
    igraph_vector_fill(&bounds[0], -half_size);
    igraph_vector_fill(&bounds[1], half_size);
    igraph_vector_fill(&bounds[2], -half_size);
    igraph_vector_fill(&bounds[3], half_size);
}

void destroy_bounds(igraph_vector_t *bounds) {
    for (int i = 0; i < 4; i++) {
        igraph_vector_destroy(&bounds[i]);
    }
}

void check_and_destroy(igraph_matrix_t *result, igraph_real_t half_size) {
    igraph_real_t min, max;
    igraph_matrix_minmax(result, &min, &max);
    IGRAPH_ASSERT(min >= -half_size);
    IGRAPH_ASSERT(max <= half_size);
    igraph_matrix_destroy(result);
}

int main(void) {
    igraph_t g;
    igraph_matrix_t result;
    igraph_vector_t bounds[4];
    igraph_vector_t weights;
    igraph_real_t seed[20] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, -0.1, -0.2, -0.3, -0.4, -0.5, -0.6, -0.7, -0.8, -0.9, -1.0};

    igraph_rng_seed(igraph_rng_default(), 42);

    printf("Empty graph.\n");
    igraph_small(&g, 0, 0, -1);
    igraph_matrix_init(&result, 0, 0);
    igraph_layout_kamada_kawai(&g, &result, /*use_seed*/ 0, /*maxiter*/ 100,
            /*epsilon*/ 0.0001, /*kkconst */ 10,
            /*weight*/ NULL, /*minx*/ NULL, /*maxx*/ NULL, /*miny*/ NULL,
            /*maxy*/ NULL);
    print_matrix(&result);
    igraph_matrix_destroy(&result);
    igraph_destroy(&g);

    printf("Singleton graph in a box.\n");
    igraph_small(&g, 1, 0, -1);
    igraph_matrix_init(&result, 0, 0);
    make_box(1, 1.0, bounds);
    igraph_layout_kamada_kawai(&g, &result, /*use_seed*/ 0, /*maxiter*/ 100,
            /*epsilon*/ 0.0001, /*kkconst */ 10,
            /*weights*/ NULL, &bounds[0], &bounds[1], &bounds[2], &bounds[3]);
    check_and_destroy(&result, 1.0);
    igraph_destroy(&g);
    destroy_bounds(bounds);

    printf("Two connected vertices.\n");
    igraph_small(&g, 2, 0, 0,1, -1);
    igraph_matrix_init(&result, 0, 0);
    igraph_layout_kamada_kawai(&g, &result, /*use_seed*/ 0, /*maxiter*/ 1000,
            /*epsilon*/ 0, /*kkconst */ 2,
            /*weight*/ NULL, /*minx*/ NULL, /*maxx*/ NULL, /*miny*/ NULL,
            /*maxy*/ NULL);
    check_and_destroy(&result, 1.0);

    printf("Two connected vertices in a box.\n");
    igraph_matrix_init(&result, 0, 0);
    make_box(2, 1.0, bounds);
    igraph_layout_kamada_kawai(&g, &result, /*use_seed*/ 0, /*maxiter*/ 1000,
            /*epsilon*/ 0, /*kkconst */ 2,
            /*weights*/ NULL, &bounds[0], &bounds[1], &bounds[2], &bounds[3]);
    check_and_destroy(&result, 1.0);
    igraph_destroy(&g);
    destroy_bounds(bounds);

    printf("A few tests with a disconnected graph of 10 vertices with loops in a box from -1 to 1.\n");
    igraph_small(&g, 10, 0, 0,1, 1,2, 2,0, 5,6, 6,7, 7,6, 7,7, 8,8, -1);
    igraph_vector_init(&weights, 8);
    igraph_vector_fill(&weights, 100);
    make_box(10, 1.0, bounds);
    printf("Without weights or bounds.\n");
    igraph_matrix_init(&result, 0, 0);
    igraph_layout_kamada_kawai(&g, &result, /*use_seed*/ 0, /*maxiter*/ 100,
            /*epsilon*/ 0.0001, /*kkconst */ 10,
            /*weight*/ NULL, /*minx*/ NULL, /*maxx*/ NULL, /*miny*/ NULL,
            /*maxy*/ NULL);
    check_and_destroy(&result, 50.0);

    printf("With weights.\n");
    igraph_matrix_init(&result, 0, 0);
    igraph_layout_kamada_kawai(&g, &result, /*use_seed*/ 0, /*maxiter*/ 100,
            /*epsilon*/ 0.0001, /*kkconst */ 10,
            &weights, /*minx*/ NULL, /*maxx*/ NULL, /*miny*/ NULL,
            /*maxy*/ NULL);
    check_and_destroy(&result, 50.0);

    printf("With weights, bounds, and high kkconst.\n");
    igraph_matrix_init(&result, 0, 0);
    igraph_layout_kamada_kawai(&g, &result, /*use_seed*/ 0, /*maxiter*/ 100,
            /*epsilon*/ 0.0001, /*kkconst */ 1000,
            &weights, &bounds[0], &bounds[1], &bounds[2], &bounds[3]);
    check_and_destroy(&result, 1.0);

    printf("With weights, bounds, and low kkconst.\n");
    igraph_matrix_init(&result, 0, 0);
    igraph_layout_kamada_kawai(&g, &result, /*use_seed*/ 0, /*maxiter*/ 100,
            /*epsilon*/ 0.0001, /*kkconst */ 0.0001,
            &weights, &bounds[0], &bounds[1], &bounds[2], &bounds[3]);
    check_and_destroy(&result, 1.0);

    printf("With weights, bounds, and high kkconst and seed.\n");
    matrix_init_real_row_major(&result, 10, 2, seed);
    igraph_layout_kamada_kawai(&g, &result, /*use_seed*/ 1, /*maxiter*/ 100,
            /*epsilon*/ 0.0001, /*kkconst */ 1000,
            &weights, &bounds[0], &bounds[1], &bounds[2], &bounds[3]);
    check_and_destroy(&result, 1.0);
    igraph_destroy(&g);

    printf("Full graph of 5 vertices, seed and no iterations:\n");
    igraph_full(&g, 5, 0, 0);
    matrix_init_real_row_major(&result, 5, 2, seed);
    igraph_layout_kamada_kawai(&g, &result, /*use_seed*/ 1, /*maxiter*/ 0,
            /*epsilon*/ 0.0001, /*kkconst */ 10,
            /*weight*/ NULL, /*minx*/ NULL, /*maxx*/ NULL, /*miny*/ NULL,
            /*maxy*/ NULL);
    print_matrix(&result);
    igraph_matrix_destroy(&result);
    igraph_destroy(&g);
    destroy_bounds(bounds);
    igraph_vector_destroy(&weights);

    VERIFY_FINALLY_STACK();
    return 0;
}
