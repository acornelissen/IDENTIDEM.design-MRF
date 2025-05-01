#include <helpers.h>
#include <Arduino.h>
#include <Preferences.h>
#include <globals.h>     // Access to global variables like prefs, iso, lenses, etc.
#include <lenses.h>      // Definition of Lens struct
#include <mrfconstants.h> // For SMOOTHING_WINDOW_SIZE

// Helper functions
// ---------------------

/**
 * @brief Finds the index of the first non-zero aperture for the currently selected lens.
 * @return The index of the first valid aperture, or -1 if none are found (or lens has no apertures).
 */
int getFirstNonZeroAperture()
{
  // Ensure selected_lens is within bounds
  if (selected_lens < 0 || selected_lens >= (NUM_LENSES)) {
      // Handle error: maybe return -1 or default index
      return -1;
  }

  const size_t num_apertures = sizeof(lenses[selected_lens].apertures) / sizeof(lenses[selected_lens].apertures[0]);
  for (size_t i = 0; i < num_apertures; ++i)
  {
    if (lenses[selected_lens].apertures[i] != 0.0f) // Compare float with tolerance if needed, but 0.0f should be exact
    {
      return static_cast<int>(i); // Return the index
    }
  }
  return -1; // Return -1 if no non-zero aperture is found
}

void loadPrefs()
{
  prefs.begin("mrf", false); // Open preferences in read-write mode

  // Check if a key exists to determine if preferences have been saved before
  if (prefs.isKey("iso")) {
    iso = prefs.getInt("iso", 400);
    iso_index = prefs.getInt("iso_index", 5); // Default ISO index (assuming 400 is at index 5)

    // Load lens calibration data if it exists
    if (prefs.isKey("lenses")) {
        // Calculate total byte size: size of one Lens * number of lenses
        const size_t totalLensBytes = sizeof(Lens) * NUM_LENSES;
        byte tempLenses[totalLensBytes]; // Allocate buffer of correct byte size
        size_t len = prefs.getBytes("lenses", tempLenses, totalLensBytes);
        if (len == totalLensBytes) { // Verify the correct amount of data was read
            memcpy(lenses, tempLenses, NUM_LENSES);
        }
    }

    selected_format = prefs.getInt("selected_format", 3); // Default format index
    selected_lens = prefs.getInt("selected_lens", 1);   // Default lens index

    // Get default aperture based on loaded lens data
    int default_aperture_index = getFirstNonZeroAperture();
    float default_aperture = (default_aperture_index != -1) ? lenses[selected_lens].apertures[default_aperture_index] : 0.0f;

    aperture = prefs.getFloat("aperture", default_aperture);
    aperture_index = prefs.getInt("aperture_index", default_aperture_index);

    film_counter = prefs.getInt("film_counter", 0);
    encoder_value = prefs.getInt("encoder_value", 0);
  }

  prefs.end(); // Close preferences
}

void savePrefs()
{
  prefs.begin("mrf", false); // Open preferences in read-write mode
  prefs.putInt("iso", iso);
  prefs.putInt("iso_index", iso_index);
  prefs.putFloat("aperture", aperture);
  prefs.putInt("aperture_index", aperture_index);
  prefs.putInt("selected_format", selected_format);
  prefs.putInt("selected_lens", selected_lens);
  prefs.putInt("film_counter", film_counter);
  prefs.putInt("encoder_value", encoder_value);
  // Save the entire lenses array (including calibration data)
  // Calculate total byte size: size of one Lens * number of lenses
  const size_t totalLensBytes = sizeof(Lens) * NUM_LENSES;
  prefs.putBytes("lenses", (const byte *)lenses, totalLensBytes);
  prefs.end(); // Close preferences
}

String cmToReadable(int cm, int places)
{
  if (cm < 100)
  {
    // Use String constructor for integers directly
    return String(cm) + "cm";
  }
  else
  {
    // Use String constructor with float and decimal places
    return String(static_cast<float>(cm) / 100.0f, places) + "m";
  }
}

int calcMovingAvg(int index, int sensorVal)
{
  // Ensure index is valid
  if (index < 0 || index >= 2) return 0; 

  int readIndex = curReadIndex[index];

  // Subtract the oldest sample
  sampleTotal[index] -= samples[index][readIndex];

  // Add the new sample
  samples[index][readIndex] = sensorVal;
  sampleTotal[index] += samples[index][readIndex];

  // Advance the index, wrapping around if necessary
  curReadIndex[index]++;
  if (curReadIndex[index] >= SMOOTHING_WINDOW_SIZE)
  {
    curReadIndex[index] = 0;
  }

  // Calculate the average
  sampleAvg[index] = sampleTotal[index] / SMOOTHING_WINDOW_SIZE;
  return sampleAvg[index];
}

int_fast16_t getFocusRadius()
{
  const int minRadius = 3;
  const int maxRadius = 30; // Adjust based on screen size/preference

  // Calculate the absolute difference between LiDAR distance and Lens distance (both in cm)
  // Assuming 'distance' and 'lens_distance_raw' are in cm.
  int diff_cm = abs(distance - lens_distance_raw);

  // Clamp the difference to the desired radius range
  int radius = min(maxRadius, max(minRadius, diff_cm));

  // Using int_fast16_t for potential performance gain, though int might suffice
  return static_cast<int_fast16_t>(radius);
}

// ---------------------