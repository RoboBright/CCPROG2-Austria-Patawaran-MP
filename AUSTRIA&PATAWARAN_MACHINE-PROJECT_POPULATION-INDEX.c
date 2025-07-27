#include <stdio.h>
#include <string.h>

#define MAX_CITIES 10
#define MAX_BARANGAYS 20

// Struc for barangay
typedef struct {
    char name[50];
    int populationSize;
    float area;
    float avgHouseholdSize;
    int formalHousingUnits;
} Barangay;

// Struc for city
typedef struct {
    char name[50];
    Barangay barangays[MAX_BARANGAYS];
    int numBarangays;

    // total data from the barangays
    int totalPopulation;
    float totalArea;
    int totalHousingUnits;
    float avgHouseholdSize;
    float populationDensity;
    float HPI;
} City;

// Struc for province
typedef struct {
    char name[50];
    City cities[MAX_CITIES];
    int numCities;
    float overallHPI;
} Province;

// Function to calculate weighted average household size
float calculateAvgHouseholdSize(City *c) {
    float totalPeople = 0;
    float totalHouseholds = 0;

    for (int i = 0; i < c->numBarangays; i++) {
        Barangay *b = &c->barangays[i];
        totalPeople += b->populationSize;
        totalHouseholds += b->populationSize / b->avgHouseholdSize;
    }

    if (totalHouseholds == 0) return 0;
    return totalPeople / totalHouseholds;
}

// Calculate city-level aggregates
void calculateCityMetrics(City *c) {
    c->totalPopulation = 0;
    c->totalArea = 0;
    c->totalHousingUnits = 0;

    for (int i = 0; i < c->numBarangays; i++) {
        Barangay *b = &c->barangays[i];
        c->totalPopulation += b->populationSize;
        c->totalArea += b->area;
        c->totalHousingUnits += b->formalHousingUnits;
    }

    c->avgHouseholdSize = calculateAvgHouseholdSize(c);
    c->populationDensity = (c->totalArea == 0) ? 0 : (float)c->totalPopulation / c->totalArea;
    c->HPI = (c->totalHousingUnits == 0) ? 0 :
             (c->populationDensity * c->avgHouseholdSize) / c->totalHousingUnits;
}

// Calculate province-level HPI
float calculateProvinceHPI(Province *p) {
    float totalPeople = 0;
    float totalUnits = 0;

    for (int i = 0; i < p->numCities; i++) {
        calculateCityMetrics(&p->cities[i]);
        City *c = &p->cities[i];

        totalPeople += c->populationDensity * c->avgHouseholdSize;
        totalUnits += c->totalHousingUnits;
    }

    return (totalUnits == 0) ? 0 : totalPeople / totalUnits;
}

int main() {
    Province province;
    char addAnotherCity = 'y';
    char addAnotherBarangay;

    printf("Enter province name: ");
    fgets(province.name, sizeof(province.name), stdin);
    province.name[strcspn(province.name, "\n")] = '\0';

    province.numCities = 0;

    while (addAnotherCity == 'y' || addAnotherCity == 'Y') {
        if (province.numCities >= MAX_CITIES) {
            printf("Maximum number of cities reached.\n");
            break;
        }

        City *city = &province.cities[province.numCities];
        city->numBarangays = 0;

        printf("\nEnter city name: ");
        getchar(); // Clear buffer
        fgets(city->name, sizeof(city->name), stdin);
        city->name[strcspn(city->name, "\n")] = '\0';

        addAnotherBarangay = 'y';

        while (addAnotherBarangay == 'y' || addAnotherBarangay == 'Y') {
            if (city->numBarangays >= MAX_BARANGAYS) {
                printf("Maximum number of barangays reached for this city.\n");
                break;
            }

            Barangay *b = &city->barangays[city->numBarangays];

            printf("\nEnter barangay name: ");
            fgets(b->name, sizeof(b->name), stdin);
            b->name[strcspn(b->name, "\n")] = '\0';

            printf("Enter population size: ");
            scanf("%d", &b->populationSize);

            printf("Enter total area (km²): ");
            scanf("%f", &b->area);

            printf("Enter average household size: ");
            scanf("%f", &b->avgHouseholdSize);

            printf("Enter number of formal housing units: ");
            scanf("%d", &b->formalHousingUnits);

            city->numBarangays++;
            getchar(); // Clear buffer

            printf("Add another barangay to this city? (y/n): ");
            scanf(" %c", &addAnotherBarangay);
            getchar(); // Clear buffer
        }

        province.numCities++;
        printf("\nAdd another city to the province? (y/n): ");
        scanf(" %c", &addAnotherCity);
        getchar(); // Clear buffer
    }

    province.overallHPI = calculateProvinceHPI(&province);

    // Output results
    printf("\n\n===== Province Summary =====\n");
    printf("Province Name: %s\n", province.name);
    printf("Number of Cities: %d\n", province.numCities);

    for (int i = 0; i < province.numCities; i++) {
        City *c = &province.cities[i];
        printf("\nCity: %s\n", c->name);
        printf("  Total Population: %d\n", c->totalPopulation);
        printf("  Total Area: %.2f km²\n", c->totalArea);
        printf("  Population Density: %.2f people/km²\n", c->populationDensity);
        printf("  Avg Household Size: %.2f\n", c->avgHouseholdSize);
        printf("  Total Formal Housing Units: %d\n", c->totalHousingUnits);
        printf("  HPI: %.2f\n", c->HPI);

        for (int j = 0; j < c->numBarangays; j++) {
            Barangay *b = &c->barangays[j];
            printf("    Barangay: %s\n", b->name);
            printf("      Population: %d\n", b->populationSize);
            printf("      Area: %.2f km²\n", b->area);
            printf("      Avg Household Size: %.2f\n", b->avgHouseholdSize);
            printf("      Formal Housing Units: %d\n", b->formalHousingUnits);
        }
    }

    printf("\nOverall Province HPI: %.2f\n", province.overallHPI);

    return 0;
}
