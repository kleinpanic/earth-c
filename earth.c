#include <stdio.h>
#include <ncurses.h>
#include <math.h>
#include <unistd.h>

#define WIDTH 80
#define HEIGHT 40
#define SPHERE_POINTS 1000 // Increased number of points for better realism
#define CONTINENT_POINTS 500 // More points for realistic continent outlines

// Structure for 3D points
typedef struct {
    float x, y, z;
} Point3D;

// Structure for 2D points (projected from 3D)
typedef struct {
    int x, y;
} Point2D;

// Arrays to hold points on the sphere and for continents
Point3D spherePoints[SPHERE_POINTS];
Point3D continentPoints[CONTINENT_POINTS];

// Function to project 3D points to 2D with perspective projection
Point2D project(Point3D point, float angleX, float angleY) {
    // Apply rotations around the X and Y axes
    float cosY = cos(angleY), sinY = sin(angleY);
    float cosX = cos(angleX), sinX = sin(angleX);
    float x = point.x * cosY - point.z * sinY;
    float z = point.x * sinY + point.z * cosY;
    float y = point.y * cosX - z * sinX;
    z = point.y * sinX + z * cosX;

    // Simple perspective projection
    float fov = 25.0;  // Field of view for 3D perspective
    Point2D projected = {
        (int)((x / (z + 3)) * fov) + WIDTH / 2, // Offset to the center of the screen
        (int)((y / (z + 3)) * fov) + HEIGHT / 2
    };
    return projected;
}

// Function to generate points on a 3D sphere using spherical coordinates
void generate_sphere() {
    int index = 0;
    for (int i = 0; i < SPHERE_POINTS; i++) {
        float theta = 2.0 * M_PI * (float)i / sqrt(SPHERE_POINTS);  // Horizontal angle
        float phi = acos(2.0 * (float)i / SPHERE_POINTS - 1);       // Vertical angle

        spherePoints[index].x = sin(phi) * cos(theta);
        spherePoints[index].y = cos(phi);
        spherePoints[index].z = sin(phi) * sin(theta);
        index++;
    }
}

// Function to convert latitude/longitude to 3D points for continents
void latlon_to_xyz(float lat, float lon, Point3D* point) {
    float radLat = lat * M_PI / 180.0;  // Convert to radians
    float radLon = lon * M_PI / 180.0;  // Convert to radians
    point->x = cos(radLat) * cos(radLon);
    point->y = sin(radLat);
    point->z = cos(radLat) * sin(radLon);
}

// Function to generate the wireframe outline of continents using more realistic data
void generate_continents() {
    // Example: a few detailed latitude/longitude points along major continents
    float latitudes[] = {51.1657, 40.7128, -33.9249, 35.6895, -34.6037};  // Major cities as an example
    float longitudes[] = {10.4515, -74.0060, 18.4241, 139.6917, -58.3816}; // Corresponding longitudes

    int index = 0;
    for (int i = 0; i < sizeof(latitudes) / sizeof(float); i++) {
        if (index < CONTINENT_POINTS) {
            latlon_to_xyz(latitudes[i], longitudes[i], &continentPoints[index++]);
        }
    }
}

// Draw the Earth with the rotating sphere and continents overlay
void draw_earth(float angleX, float angleY) {
    clear();

    // Draw the wireframe for the sphere
    for (int i = 0; i < SPHERE_POINTS; i++) {
        Point2D projected = project(spherePoints[i], angleX, angleY);
        if (projected.x >= 0 && projected.x < WIDTH && projected.y >= 0 && projected.y < HEIGHT) {
            mvprintw(projected.y, projected.x, "o"); // Draw points for the sphere
        }
    }

    // Draw the wireframe for the continents
    for (int i = 0; i < CONTINENT_POINTS; i++) {
        Point2D projected = project(continentPoints[i], angleX, angleY);
        if (projected.x >= 0 && projected.x < WIDTH && projected.y >= 0 && projected.y < HEIGHT) {
            mvprintw(projected.y, projected.x, "x"); // Draw points for the continents
        }
    }

    refresh();
}

int main() {
    initscr();            // Start ncurses
    curs_set(0);          // Hide cursor
    timeout(0);           // Non-blocking input
    noecho();             // Disable character echo

    // Generate sphere points and continent wireframe
    generate_sphere();
    generate_continents();

    float angleX = 0, angleY = 0;
    while (1) {
        draw_earth(angleX, angleY); // Draw the rotating Earth
        angleX += 0.01;            // Rotate around X axis
        angleY += 0.02;            // Rotate around Y axis
        usleep(50000);             // Delay for animation speed

        int ch = getch();          // Check for user input
        if (ch == 'q') break;      // Quit if 'q' is pressed
    }

    endwin();  // End ncurses
    return 0;
}

