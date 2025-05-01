#include <lenses.h> // Include the header file with the struct definition and extern declarations

// Define the actual array of Lens objects
// Note: Using designated initializers could improve readability if compiler supports C++20: {.id = 5063, .name = "50/6.3", ...}
Lens lenses[] = {
    {5063, "50/6.3", {0, 0, 0, 0, 0, 0, 0}, {1.0f, 1.2f, 1.5f, 2.0f, 3.0f, 5.0f, 10.0f}, {0.0f, 6.3f, 8.0f, 11.0f, 16.0f, 22.0f, 32.0f, 0.0f, 0.0f}, {0, 17, 128, 109}, false},
    {6563, "65/6.3", {330, 312, 299, 287, 276, 268, 261}, {1.0f, 1.2f, 1.5f, 2.0f, 3.0f, 5.0f, 10.0f}, {0.0f, 6.3f, 8.0f, 11.0f, 16.0f, 22.0f, 32.0f, 0.0f, 0.0f}, {0, 17, 128, 109}, true}, // Example calibrated lens
    {7556, "75/5.6", {0, 0, 0, 0, 0, 0, 0}, {1.0f, 1.2f, 1.5f, 2.0f, 3.0f, 5.0f, 10.0f}, {0.0f, 5.6f, 8.0f, 11.0f, 16.0f, 22.0f, 32.0f, 45.0f, 0.0f}, {9, 24, 110, 95}, false},
    {9035, "90/3.5", {0, 0, 0, 0, 0, 0, 0}, {1.0f, 1.2f, 1.5f, 2.0f, 3.0f, 5.0f, 10.0f}, {3.5f, 4.0f, 5.6f, 8.0f, 11.0f, 16.0f, 22.0f, 32.0f, 0.0f}, {18, 32, 92, 79}, false},
    {10035, "100/3.5", {0, 0, 0, 0, 0, 0, 0}, {1.0f, 1.2f, 1.5f, 2.0f, 3.0f, 5.0f, 10.0f}, {3.5f, 4.0f, 5.6f, 8.0f, 11.0f, 16.0f, 22.0f, 32.0f, 0.0f}, {23, 36, 84, 71}, false},
    {10028, "100/2.8", {0, 0, 0, 0, 0, 0, 0}, {1.0f, 1.2f, 1.5f, 2.0f, 3.0f, 5.0f, 10.0f}, {2.8f, 4.0f, 5.6f, 8.0f, 11.0f, 16.0f, 22.0f, 32.0f, 0.0f}, {23, 36, 84, 71}, false},
    {12747, "127/4.7", {0, 0, 0, 0, 0, 0, 0}, {1.0f, 1.2f, 1.5f, 2.0f, 3.0f, 5.0f, 10.0f}, {4.7f, 5.6f, 8.0f, 11.0f, 16.0f, 22.0f, 32.0f, 45.0f, 64.0f}, {31, 43, 66, 57}, false},
    {15056, "150/5.6", {0, 0, 0, 0, 0, 0, 0}, {1.0f, 1.2f, 1.5f, 2.0f, 3.0f, 5.0f, 10.0f}, {5.6f, 8.0f, 11.0f, 16.0f, 22.0f, 32.0f, 45.0f, 0.0f, 0.0f}, {36, 48, 56, 47}, false},
    {25005, "250/5.0", {0, 0, 0, 0, 0, 0, 0}, {1.0f, 1.2f, 1.5f, 2.0f, 3.0f, 5.0f, 10.0f}, {5.0f, 8.0f, 11.0f, 16.0f, 22.0f, 32.0f, 45.0f, 0.0f, 0.0f}, {47, 57, 34, 29}, false}
};

// Define the constant for the number of lenses
// Correctly calculate the size based on the defined array
const size_t NUM_LENSES = sizeof(lenses) / sizeof(lenses[0]);