#if 0
clang -Wall -g -o nn neuralnetwork.c && ./nn;
exit;
#endif

// This stuff is pretty complicated!
// It can get hella meta too with how you do this.

// supervised learning (watching twitch)
// reinforcement learning (playing games itself)

const int NUM_INPUT_NODES = 2;
const int NUM_HIDDEN_NODES = 2;
const int NUM_OUTPUT_NODES = 1;
const int NUM_NODES = NUM_INPUT_NODES + NUM_HIDDEN_NODES + NUM_OUTPUT_NODES;
const int ARRAY_SIZE = NUM_NODES + 1; // 1-offset to match node1 node2
// we just skip 0 for some reason, not really necessesary

const int MAX_ITERATIONS = 131072;
const double E = 2.71828;
const double LEARNING_RATE = 0.2;

#define INDEX(i,j) (i * ARRAY_SIZE + j)

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void
initialize(double *weights, double *values,
           double *expected_values, double *thresholds)
{
    for (int x = 0; x <= NUM_NODES; x++) {
        values[x] = 0.0;
        expected_values[x] = 0.0;
        for (int y = 0; y <= NUM_NODES; y++) {
            weights[INDEX(x,y)] = 0.0;
        }
    }
}

void
connectNodes(double *weights, double *thresholds)
{
    for (int x = 1; x <= NUM_NODES; x++) {
        for (int y = 1; y <= NUM_NODES; y++) {
            weights[INDEX(x, y)] = (rand() % 200) / 100.0;
        }
    }

    thresholds[3] = rand() / (double)rand();
    thresholds[4] = rand() / (double)rand();
    thresholds[5] = rand() / (double)rand();

    printf("%f%f%f%f%f%f\n%f%f%f\n",
           weights[INDEX(1,3)], weights[INDEX(1,4)],
           weights[INDEX(2,3)], weights[INDEX(2,4)],
           weights[INDEX(3,5)], weights[INDEX(4,5)],
           thresholds[3], thresholds[4], thresholds[5]);
}

void
trainingExample(double *values, double *expected_values)
{
    static int counter = 0;

    switch(counter % 4) {
    case 0:
        values[1] = 1;
        values[2] = 1;
        expected_values[5] = 0;
        break;
    case 1:
        values[1] = 0;
        values[2] = 1;
        expected_values[5] = 1;
        break;
    case 2:
        values[1] = 1;
        values[2] = 0;
        expected_values[5] = 1;
        break;
    case 3:
        values[1] = 0;
        values[2] = 0;
        expected_values[5] = 0;
        break;
    }

    counter++;
}

void
activateNetwork(double *weights, double *values, double *thresholds)
{
    // for each hidden node
    for (int h = 1 + NUM_INPUT_NODES;
         h < 1 + NUM_INPUT_NODES + NUM_HIDDEN_NODES;
         h++) {
        // add up the weighted input
        double weighted_input = 0.0;

        for (int i = 1; i < 1+NUM_INPUT_NODES; i++) {
            weighted_input += weights[INDEX(i,h)] * values[i];
        }
        // handle the thresholds
        weighted_input += (-1 * thresholds[h]);
        // sigmoid
        values[h] = 1.0 / (1.0 + pow(E, -weighted_input));
    }

    // for each output node
    for (int o = 1 + NUM_INPUT_NODES + NUM_HIDDEN_NODES;
         o < 1 + NUM_NODES;
         o++) {
        // add up the weighted input
        double weighted_input = 0.0;
        
        for (int h = 1 + NUM_INPUT_NODES;
             h < 1 + NUM_INPUT_NODES + NUM_HIDDEN_NODES;
             h++) {
            weighted_input += weights[INDEX(h,o)] * values[h];
        }
        weighted_input += (-1 * thresholds[o]);
        values[o] = 1.0 / (1.0 + pow(E, -weighted_input));
    }
}

double
updateWeights(double *weights, double *values,
              double *expected_values, double *thresholds)
{
    double sum_of_squared_errors = 0.0;

    // for each output node
    for (int o = 1 + NUM_INPUT_NODES + NUM_HIDDEN_NODES;
         o < 1 + NUM_NODES;   
         o++) {
        double absolute_error = expected_values[o] - values[o];
        sum_of_squared_errors += pow(absolute_error, 2);
        double output_error_gradient = values[o] * \
            (1.0 - values[o]) * \
            absolute_error;

        // update each weighting from the hidden layer
        for (int h= 1 + NUM_INPUT_NODES;
             h < 1 + NUM_INPUT_NODES + NUM_HIDDEN_NODES;
             h++) {
            double delta = LEARNING_RATE * values[h] * output_error_gradient;
            weights[INDEX(h,o)] += delta;
            double hidden_error_gradient = values[h]* \
                (1 - values[h]) * output_error_gradient * weights[INDEX(h,o)];

            // update weighting from the input node.
            for (int i = 1; i < 1 + NUM_INPUT_NODES; i++) {
                double delta = LEARNING_RATE * values[i] * hidden_error_gradient;
                weights[INDEX(i,h)] += delta;
            }

            double threshold_delta = LEARNING_RATE * -1 * hidden_error_gradient;
            thresholds[h] += threshold_delta;
        }

        double delta = LEARNING_RATE * -1 * output_error_gradient;
        thresholds[o] += delta;
    }

    return sum_of_squared_errors;
}

void displayNetwork(double *values, double sum_of_squared_errors)
{
    static int counter = 0;
    if ((counter % 4) == 0) {
        printf("----------------------------------\n");
    }
    printf("%8.4f|", values[1]);
    printf("%8.4f|", values[2]);
    printf("%8.4f|", values[5]);
    printf(" err:%8.5f\n", sum_of_squared_errors);
    counter++;
}

int main()
{
    printf("Neural Network Program\n");
    
    static double weights[ARRAY_SIZE][ARRAY_SIZE];
    static double values[ARRAY_SIZE];
    static double expected_values[ARRAY_SIZE];
    static double thresholds[ARRAY_SIZE];

    initialize((double*)weights, values, expected_values, thresholds);
    connectNodes((double*)weights, thresholds);

    int counter = 0;
    while(counter < MAX_ITERATIONS) {
        trainingExample(values, expected_values);
        activateNetwork((double*)weights, values, thresholds);
        double sum_of_squared_errors = updateWeights((double*)weights,
                                                     values,
                                                     expected_values,
                                                     thresholds);
        if (MAX_ITERATIONS - counter <= 4) {
            displayNetwork(values, sum_of_squared_errors);
        }
        counter++;
    }

    return 0;
}
