#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CITIES 10
#define MAX_BARANGAYS 20
#define CAESAR_SHIFT 3

// Struc for city with 2D arrays for barangay data
typedef struct {
    char name[50];
    int numBarangays;

    // arrays for barangay data
    int populationSizes[MAX_BARANGAYS];
    float areas[MAX_BARANGAYS];
    float avgHouseholdSizes[MAX_BARANGAYS];
    int formalHousingUnits[MAX_BARANGAYS];
    char barangayNames[MAX_BARANGAYS][50];

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
    char provinceName[50];
    char username[100];
    char password[100];
    City cities[MAX_CITIES];
    int numCities;
    float overallHPI;
} Province;

// Function prototypes
void caesar_encrypt(char *data, int len); // Carlos
void caesar_decrypt(char *data, int len); // Carlos
int saveProvinceEncrypted(const char *provinceName, Province *province); // Paolo
int loadProvinceEncrypted(const char *provinceName, Province *province); // Paolo
float calculateAvgHouseholdSize(City *c); // Paolo
void calculateCityMetrics(City *city); // Paolo
float calculateProvinceHPI(Province *p); // Paolo
void fillDummyData(Province *province); // Paolo
int promptLogin(Province *province); // Carlos
void editCityName(City *city); // Carlos
void removeBarangay(City *city); // Carlos
void addBarangay(City *city); // Carlos
void editBarangay(City *city); // Paolo
void sortBarangays(City *city); // Paolo
void sortCities(Province *province, int sortBy); // Paolo
void viewCities(Province *province); // Carlos
void addCity(Province *province); // Paolo
void deleteCity(Province *province); // Paolo
void editCity(Province *province); // Paolo
void searchCity(Province *province); // Carlos
void provinceMenu(Province *province, const char *filename); // Carlos

/**
 * Encrypts a buffer using Caesar cipher.
 * Shifts each byte by CAESAR_SHIFT.
 *
 * @param data Pointer to the buffer to encrypt.
 * @param len Length of the buffer.
 */
void caesar_encrypt(char *data, int len)
{
	char c;
	// Go through each character to chift it by 3.
    for (int i = 0; i < len; i++)
    {
        c = data[i] + CAESAR_SHIFT;
        data[i] = c;
    }
}

/**
 * Decrypts a buffer using Caesar cipher.
 * Reverses the shift applied during encryption.
 *
 * @param data Pointer to the buffer to decrypt.
 * @param len Length of the buffer.
 */
void caesar_decrypt(char *data, int len)
{
    char c;
    // Go through each character to chift it by 3.
    for (int i = 0; i < len; i++)
    {
        c = data[i] - CAESAR_SHIFT;
        data[i] = c;
    }
}

/**
 * Saves the Province struct to an encrypted file named <province>.txt.
 * Uses Caesar cipher for encryption.
 *
 * @param provinceName The province name to use as the filename.
 * @param province Pointer to the Province struct to save.
 * @return 1 on success, 0 on failure.
 */
int saveProvinceEncrypted(const char *provinceName, Province *province)
{
    char filename[128];
    FILE *fp;
    char *buffer;
    int len;
    char temp[sizeof(Province)];
    int result = 0;
    
    snprintf(filename, sizeof(filename), "%s.txt", provinceName);
    fp = fopen(filename, "wb");
    if (fp) {
        buffer = (char *)province;
        len = sizeof(Province);
        memcpy(temp, buffer, len);
        // Encrypts the struct before writing
        caesar_encrypt(temp, len);
        // Writes the encrypted data to file
        fwrite(temp, 1, len, fp);
        fclose(fp);
        result = 1;
    }
    return result;
}

/**
 * Loads the Province struct from an encrypted file named <province>.txt.
 * Decrypts the file using Caesar cipher.
 *
 * @param provinceName The province name to use as the filename.
 * @param province Pointer to the Province struct to load into.
 * @return 1 on success, 0 on failure.
 */
int loadProvinceEncrypted(const char *provinceName, Province *province)
{
    char filename[128];
    FILE *fp;
    int len;
    char temp[sizeof(Province)];
    int read;
    int result = 0;
    
    snprintf(filename, sizeof(filename), "%s.txt", provinceName);
    fp = fopen(filename, "rb");
    if (fp) {
        len = sizeof(Province);
        // Reads the encrypted data from file
        read = fread(temp, 1, len, fp);
        fclose(fp);
        if (read == len) {
            // Decrypts the struct after reading
            caesar_decrypt(temp, len);
            memcpy(province, temp, len);
            result = 1;
        }
    }
    return result;
}

/**
 * Calculates the weighted average household size for a city.
 * Uses population and average household size of each barangay.
 *
 * @param c Pointer to the City struct.
 * @return Weighted average household size.
 */
float calculateAvgHouseholdSize(City *c)
{
    float totalPeople = 0;
    float totalHouseholds = 0;
    int i;
    float result = 0;
    
    // Sums up population and households for all barangays
    for (i = 0; i < c->numBarangays; i++)
    {
        totalPeople += c->populationSizes[i];
        totalHouseholds += c->populationSizes[i] / c->avgHouseholdSizes[i];
    }
    // Returns 0 if totalHouseholds is 0
    if (totalHouseholds != 0) {
        result = totalPeople / totalHouseholds;
    }
    return result;
}

/**
 * Calculates and updates all city-level aggregate metrics.
 * Updates population, area, housing units, household size, density, and HPI.
 *
 * @param city Pointer to the City struct to update.
 */
void calculateCityMetrics(City *city)
{
    int i;
    
    city->totalPopulation = 0;
    city->totalArea = 0;
    city->totalHousingUnits = 0;
    // Aggregates data from all barangays
    for (i = 0; i < city->numBarangays; i++)
    {
        city->totalPopulation += city->populationSizes[i];
        city->totalArea += city->areas[i];
        city->totalHousingUnits += city->formalHousingUnits[i];
    }
    // Calls calculateAvgHouseholdSize for weighted average
    city->avgHouseholdSize = calculateAvgHouseholdSize(city);
    city->populationDensity = (city->totalArea == 0) ? 0 : (float)city->totalPopulation / city->totalArea;
    city->HPI = (city->totalHousingUnits == 0) ? 0 :
             (city->populationDensity * city->avgHouseholdSize) / city->totalHousingUnits;
}

/**
 * Calculates the overall HPI for the province.
 * Aggregates city-level metrics.
 *
 * @param p Pointer to the Province struct.
 * @return The overall HPI value.
 */
float calculateProvinceHPI(Province *p)
{
    float totalPeople = 0;
    float totalUnits = 0;
    int i;
    City *c;
    float result = 0;
    
    // Iterates through all cities and updates their metrics
    for (i = 0; i < p->numCities; i++)
    {
        calculateCityMetrics(&p->cities[i]);
        c = &p->cities[i];
        totalPeople += c->populationDensity * c->avgHouseholdSize;
        totalUnits += c->totalHousingUnits;
    }
    // Returns 0 if totalUnits is 0
    if (totalUnits != 0) {
        result = totalPeople / totalUnits;
    }
    return result;
}

/**
 * Fills the Province struct with dummy data for testing.
 * Populates with two cities, each with two barangays.
 *
 * @param province Pointer to the Province struct to fill.
 */
void fillDummyData(Province *province)
{
    int i, j;
    City *city;
    
    strcpy(province->provinceName, "TestProvince");
    strcpy(province->username, "admin");
    strcpy(province->password, "admin");
    province->numCities = 2;
    // Fills cities and barangays with sample data
    for (i = 0; i < province->numCities; i++)
    {
        city = &province->cities[i];
        sprintf(city->name, "City%d", i + 1);
        city->numBarangays = 2;
        for (j = 0; j < city->numBarangays; j++)
        {
            sprintf(city->barangayNames[j], "Barangay%d", j + 1);
            city->populationSizes[j] = 1000 * (j + 1);
            city->areas[j] = 2.5f * (j + 1);
            city->avgHouseholdSizes[j] = 4.0f;
            city->formalHousingUnits[j] = 200 * (j + 1);
        }
        calculateCityMetrics(city);
    }
    // Calls calculateCityMetrics and calculateProvinceHPI
    province->overallHPI = calculateProvinceHPI(province);
}

/**
 * Prompts the user for username and password for login.
 * Compares input to stored values in the Province struct.
 *
 * @param province Pointer to the Province struct.
 * @return 1 if login successful, 0 otherwise.
 */
int promptLogin(Province *province)
{
    char username[100]; 
	char password[100];
    int result = 0;
    
    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0'; // remove new line
    
    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0'; // remove new line
    
    if (strcmp(username, province->username) == 0 && strcmp(password, province->password) == 0) // compare both username and password to the file
    {
        result = 1;
    }
    else
    {
        printf("Incorrect username or password.\n");
    }
    return result;
}

/**
 * Edits the name of a city.
 * Prompts the user for a new name and updates the city struct.
 *
 * @param city Pointer to the City struct.
 */
void editCityName(City *city)
{
    int exitFlag = 0;
    
    while (!exitFlag)
    {
        printf("Enter new city name: ");
        // Reads new name from user
        fgets(city->name, sizeof(city->name), stdin);
        // Removes newline character
        city->name[strcspn(city->name, "\n")] = '\0';
        printf("City name updated.\n");
        exitFlag = 1;
    }
}

/**
 * Sorts the barangays in a city alphabetically by name.
 *
 * @param city Pointer to the City struct.
 */
void sortBarangays(City *city)
{
    int i, j;
    for (i = 0; i < city->numBarangays - 1; i++) {
        for (j = i + 1; j < city->numBarangays; j++) {
            if (strcasecmp(city->barangayNames[i], city->barangayNames[j]) > 0) {
                // Swap names
                char tempName[50];
                strcpy(tempName, city->barangayNames[i]);
                strcpy(city->barangayNames[i], city->barangayNames[j]);
                strcpy(city->barangayNames[j], tempName);
                // Swap populationSizes
                int tempInt = city->populationSizes[i];
                city->populationSizes[i] = city->populationSizes[j];
                city->populationSizes[j] = tempInt;
                // Swap areas
                float tempFloat = city->areas[i];
                city->areas[i] = city->areas[j];
                city->areas[j] = tempFloat;
                // Swap avgHouseholdSizes
                tempFloat = city->avgHouseholdSizes[i];
                city->avgHouseholdSizes[i] = city->avgHouseholdSizes[j];
                city->avgHouseholdSizes[j] = tempFloat;
                // Swap formalHousingUnits
                tempInt = city->formalHousingUnits[i];
                city->formalHousingUnits[i] = city->formalHousingUnits[j];
                city->formalHousingUnits[j] = tempInt;
            }
        }
    }
}

/**
 * Removes a barangay from a city.
 * Prompts the user to select a barangay to delete and removes it.
 *
 * @param city Pointer to the City struct.
 */
void removeBarangay(City *city)
{
    int i = 0;
	int delIdx = 0;
    int exitFlag = 0;
    
    while (!exitFlag)
    {
        if (city->numBarangays == 0) //Check if there are no barangays
        {
            printf("No barangays to remove.\n");
            exitFlag = 1;
        }
        else
        {
            printf("List of barangays\n");
            // Go through barangays to display options
            for (i = 0; i < city->numBarangays; i++)
            {
                printf("%d. %s\n", i + 1, city->barangayNames[i]);
            }
            
            printf("Enter the barangay to delete (1 to %d) 0 to cancel: ", city->numBarangays);
            // Reads user choice
            scanf("%d", &delIdx);
            // Clears buffer
            getchar();
            
            if (delIdx == 0)
            {
                exitFlag = 1;
            }
            else if (delIdx < 1 || delIdx > city->numBarangays)
            {
                printf("Invalid choice.\n");
                exitFlag = 1;
            }
            else
            {
                for (i = delIdx - 1; i < city->numBarangays - 1; i++)
                {
                    strcpy(city->barangayNames[i], city->barangayNames[i + 1]);
                    city->populationSizes[i] = city->populationSizes[i + 1];
                    city->areas[i] = city->areas[i + 1];
                    city->avgHouseholdSizes[i] = city->avgHouseholdSizes[i + 1];
                    city->formalHousingUnits[i] = city->formalHousingUnits[i + 1];
                }
                city->numBarangays--;
                sortBarangays(city);
                calculateCityMetrics(city);
                printf("Barangay removed.\n");
                exitFlag = 1;
            }
        }
    }
}

/**
 * Adds a new barangay to a city.
 * Prompts the user for barangay details and adds it to the city.
 *
 * @param city Pointer to the City struct.
 */
void addBarangay(City *city)
{
    int exitFlag = 0;
    int barangayIndex;
    
    // Continues until user decides not to add more
    while (!exitFlag)
    {
        // Checks if max barangays reached
        if (city->numBarangays >= MAX_BARANGAYS)
        {
            printf("Maximum number of barangays reached.\n");
            exitFlag = 1;
        }
        else
        {
            barangayIndex = city->numBarangays;
            //initialize values
            city->barangayNames[barangayIndex][0] = '\0';
            city->populationSizes[barangayIndex] = 0;
            city->areas[barangayIndex] = 0.0f;
            city->avgHouseholdSizes[barangayIndex] = 0.0f;
            city->formalHousingUnits[barangayIndex] = 0;

            printf("Enter barangay name: ");
            // Reads barangay name
            fgets(city->barangayNames[barangayIndex], sizeof(city->barangayNames[barangayIndex]), stdin);
            // Removes newline character
            city->barangayNames[barangayIndex][strcspn(city->barangayNames[barangayIndex], "\n")] = '\0';
            
            printf("Enter population size: ");
            //  population size, area, household size, housing units
            scanf("%d", &city->populationSizes[barangayIndex]);
            
            printf("Enter total area (km^2): ");
            scanf("%f", &city->areas[barangayIndex]);
            
            printf("Enter average household size: ");
            scanf("%f", &city->avgHouseholdSizes[barangayIndex]);
            
            printf("Enter number of formal housing units: ");
            scanf("%d", &city->formalHousingUnits[barangayIndex]);
            // Clears buffer
            getchar();
            
            city->numBarangays++;
            sortBarangays(city);
            calculateCityMetrics(city);
            printf("Barangay added.\n");
            exitFlag = 1;
        }
    }
}

/**
 * Edits a barangay in a city.
 * Prompts the user to select a barangay to edit and allows editing its details.
 *
 * @param city Pointer to the City struct.
 */
void editBarangay(City *city)
{
    int i = 0, bchoice = 0, bEditChoice = 0;
    int exitFlag = 0;
    int barangayIndex;
    int exitFlag2 = 0;
    
    while (!exitFlag)
    {
        if (city->numBarangays == 0)
        {
            printf("No barangays to edit.\n");
            exitFlag = 1;
        }
        else
        {
            printf("List of barangays\n");
            // Iterates through barangays to display options
            for (i = 0; i < city->numBarangays; i++)
            {
                printf("%d. %s\n", i + 1, city->barangayNames[i]);
            }
            printf("Enter the Barangay to edit (1 to %d) 0 to cancel: ", city->numBarangays);
            // Reads user choice
            scanf("%d", &bchoice);
            // Clears buffer
            getchar();
            if (bchoice == 0)
            {
                exitFlag = 1;
            }
            else if (bchoice < 1 || bchoice > city->numBarangays)
            {
                printf("Invalid choice.\n");
                exitFlag = 1;
            }
            else
            {
                barangayIndex = bchoice - 1;
                exitFlag2 = 0;
                // Continues until user decides to exit
                while (!exitFlag2)
                {
                    printf("\nBarangay: %s\n", city->barangayNames[barangayIndex]);
                    printf("Total population: %d\n", city->populationSizes[barangayIndex]);
                    printf("Total area: %.2f\n", city->areas[barangayIndex]);
                    printf("Total housing units: %d\n", city->formalHousingUnits[barangayIndex]);
                    printf("Average household size: %.2f\n", city->avgHouseholdSizes[barangayIndex]);
                    printf("Population density: N/A (handled at city level)\n");
                    printf("HPI: N/A (handled at city level)\n");
                    printf("---------------------------------------------\n");
                    printf("What would you like to edit?\n");
                    printf("1. Edit name\n");
                    printf("2. Edit population size\n");
                    printf("3. Edit area\n");
                    printf("4. Edit average household size\n");
                    printf("5. Edit formal housing units\n");
                    printf("6. Exit\n");
                    printf("Enter your choice: ");
                    bEditChoice = 0;
                    scanf("%d", &bEditChoice);
                    getchar();
                    // Handles different edit options
                    switch (bEditChoice)
                    {
                        case 1:
                            printf("Enter new barangay name: ");
                            fgets(city->barangayNames[barangayIndex], sizeof(city->barangayNames[barangayIndex]), stdin);
                            city->barangayNames[barangayIndex][strcspn(city->barangayNames[barangayIndex], "\n")] = '\0';
                            printf("Barangay name updated.\n");
                            break;
                        case 2:
                            printf("Enter new population size: ");
                            scanf("%d", &city->populationSizes[barangayIndex]);
                            getchar();
                            printf("Population size updated.\n");
                            break;
                        case 3:
                            printf("Enter new area (km^2): ");
                            scanf("%f", &city->areas[barangayIndex]);
                            getchar();
                            printf("Area updated.\n");
                            break;
                        case 4:
                            printf("Enter new average household size: ");
                            scanf("%f", &city->avgHouseholdSizes[barangayIndex]);
                            getchar();
                            printf("Average household size updated.\n");
                            break;
                        case 5:
                            printf("Enter new number of formal housing units: ");
                            scanf("%d", &city->formalHousingUnits[barangayIndex]);
                            getchar();
                            printf("Formal housing units updated.\n");
                            break;
                        case 6:
                            exitFlag2 = 1;
                            break;
                        default:
                            printf("Invalid choice.\n");
                            break;
                    }
                    sortBarangays(city);
                    calculateCityMetrics(city);
                }
                exitFlag = 1;
            }
        }
    }
}

/**
 * Sorts the cities in the province either alphabetically by name or by highest population density.
 *
 * @param province Pointer to the Province struct.
 * @param sortBy 0 for alphabetical, 1 for population density.
 *
 */
void sortCities(Province *province, int sortBy)
{
    int i, j;
    City temp;
    for (i = 0; i < province->numCities - 1; i++) {
        for (j = i + 1; j < province->numCities; j++) {
            int cmp = 0;
            if (sortBy == 0) {
                cmp = strcasecmp(province->cities[i].name, province->cities[j].name) > 0; // strcasecmp used for its case insensitivity
            } else if (sortBy == 1) {
                cmp = province->cities[i].populationDensity < province->cities[j].populationDensity;
            }
            if (cmp) {
                temp = province->cities[i];
                province->cities[i] = province->cities[j];
                province->cities[j] = temp;
            }
        }
    }
}

/**
 * View cities and select one to edit (integrated menu logic).
 * Displays a list of cities and allows the user to view details of a selected city.
 *
 * @param province Pointer to the Province struct.
 */
void viewCities(Province *province)
{
    int cityChoice = 0;
    int i = 0;
    int exitFlag = 0;
    City *city = NULL;
    int sortChoice = 0;
    
    if (province->numCities == 0)
    {
        printf("No cities available.\n");
    }
    else
    {
        // Prompt user for sorting method
        printf("How would you like to view the cities?\n");
        printf("[1] Alphabetically\n");
        printf("[2] By highest population density\n");
        printf("Enter choice: ");
        scanf("%d", &sortChoice);
        getchar();
        if (sortChoice == 2) {
            // Ensure city metrics are up to date before sorting by density
            for (i = 0; i < province->numCities; i++) {
                calculateCityMetrics(&province->cities[i]);
            }
            sortCities(province, 1);
        } else {
            // Default: keep alphabetical order (already sorted after add/edit)
        }
        exitFlag = 0;
        while (!exitFlag)
        {
            printf("List of cities\n");
            // go through cities to display options
            for (i = 0; i < province->numCities; i++)
            {
                printf("%d. %s\n", i + 1, province->cities[i].name);
            }
            printf("---------------------------------------------\n");
            printf("Enter the City to view (1 to %d) 0 to cancel: ", province->numCities);
            cityChoice = 0;
            // Reads user choice
            scanf("%d", &cityChoice);
            // Clears buffer
            getchar();
            if (cityChoice == 0)
            {
                exitFlag = 1;
            }
            else if (cityChoice < 1 || cityChoice > province->numCities)
            {
                printf("Invalid choice.\n");
            }
            else
            {
                city = &province->cities[cityChoice - 1];
                // Display city details and barangays (view only)
                printf("\nCity: %s\n", city->name);
                printf("Total population: %d\n", city->totalPopulation);
                printf("Total area: %.2f\n", city->totalArea);
                printf("Total housing units: %d\n", city->totalHousingUnits);
                printf("Average household size: %.2f\n", city->avgHouseholdSize);
                printf("Population density: %.2f\n", city->populationDensity);
                printf("HPI: %.2f\n", city->HPI);
                printf("List of barangays:\n");
                sortBarangays(city);
                for (i = 0; i < city->numBarangays; i++)
                {
                    printf("%d. %s\n", i + 1, city->barangayNames[i]);
                    printf("   Population: %d\n", city->populationSizes[i]);
                    printf("   Area: %.2f\n", city->areas[i]);
                    printf("   Avg Household Size: %.2f\n", city->avgHouseholdSizes[i]);
                    printf("   Formal Housing Units: %d\n", city->formalHousingUnits[i]);
                }
                printf("---------------------------------------------\n");
            }
        }
    }
}

/**
 * Adds a new city to the province.
 * Prompts the user for city name and allows adding multiple barangays to it.
 *
 * @param province Pointer to the Province struct.
 */
void addCity(Province *province)
{
    int exitFlag = 0;
    City *city;
    char addMore;
    int barangayIndex;
    
    // Continues until user decides not to add more barangays
    while (!exitFlag)
    {
        // Checks if max barangays reached for the city
        if (province->numCities >= MAX_CITIES)
        {
            printf("Maximum number of cities reached.\n");
            exitFlag = 1;
        }
        else
        {
            city = &province->cities[province->numCities];
            //initialize values
            city->name[0] = '\0';
            city->numBarangays = 0;
            city->totalPopulation = 0;
            city->totalArea = 0.0f;
            city->totalHousingUnits = 0;
            city->avgHouseholdSize = 0.0f;
            city->populationDensity = 0.0f;
            city->HPI = 0.0f;

            printf("Enter city name: ");
            // Reads city name
            fgets(city->name, sizeof(city->name), stdin);
            // Removes newline character
            city->name[strcspn(city->name, "\n")] = '\0';
            city->numBarangays = 0;
            // Prompt to add barangays
            addMore = 'y';
            // Continues until user decides not to add more barangays
            while (addMore == 'y' || addMore == 'Y')
            {
                if (city->numBarangays >= MAX_BARANGAYS)
                {
                    printf("Maximum number of barangays reached for this city.\n");
                    addMore = 'n';
                }
                else
                {
                    barangayIndex = city->numBarangays;
                    city->barangayNames[barangayIndex][0] = '\0';
                    city->populationSizes[barangayIndex] = 0;
                    city->areas[barangayIndex] = 0.0f;
                    city->avgHouseholdSizes[barangayIndex] = 0.0f;
                    city->formalHousingUnits[barangayIndex] = 0;
                    printf("Enter barangay name: ");
                    // Reads barangay name
                    fgets(city->barangayNames[barangayIndex], sizeof(city->barangayNames[barangayIndex]), stdin);
                    // Removes newline character
                    city->barangayNames[barangayIndex][strcspn(city->barangayNames[barangayIndex], "\n")] = '\0';
                    printf("Enter population size: ");
                    // Reads population size, area, household size, housing units
                    scanf("%d", &city->populationSizes[barangayIndex]);
                    printf("Enter total area (km^2): ");
                    scanf("%f", &city->areas[barangayIndex]);
                    printf("Enter average household size: ");
                    scanf("%f", &city->avgHouseholdSizes[barangayIndex]);
                    printf("Enter number of formal housing units: ");
                    scanf("%d", &city->formalHousingUnits[barangayIndex]);
                    // Clears buffer
                    getchar();
                    city->numBarangays++;
                    printf("Barangay added.\n");
                    if (city->numBarangays < MAX_BARANGAYS)
                    {
                        printf("Add another barangay to this city? (y/n): ");
                        scanf(" %c", &addMore);
                        getchar();
                    }
                    else
                    {
                        addMore = 'n';
                    }
                }
            }
            calculateCityMetrics(city);
            province->numCities++;
            sortCities(province, 0); // Always keep cities sorted alphabetically
            province->overallHPI = calculateProvinceHPI(province);
            printf("City added.\n");
            exitFlag = 1;
        }
    }
}

/**
 * Deletes a city from the province.
 * Prompts the user to select a city to delete and removes it.
 *
 * @param province Pointer to the Province struct.
 */
void deleteCity(Province *province)
{
    int i = 0, delIdx = 0;
    int exitFlag = 0;
    
    while (!exitFlag)
    {
        if (province->numCities == 0)
        {
            printf("No cities to delete.\n");
            exitFlag = 1;
        }
        else
        {
            printf("List of cities\n");
            // Iterates through cities to display options
            for (i = 0; i < province->numCities; i++)
            {
                printf("%d. %s\n", i + 1, province->cities[i].name);
            }
            printf("Enter the city to delete (1 to %d) 0 to cancel: ", province->numCities);
            delIdx = 0;
            // Reads user choice
            scanf("%d", &delIdx);
            // Clears buffer
            getchar();
            if (delIdx == 0)
            {
                exitFlag = 1;
            }
            else if (delIdx < 1 || delIdx > province->numCities)
            {
                printf("Invalid choice.\n");
                exitFlag = 1;
            }
            else
            {
                for (i = delIdx - 1; i < province->numCities - 1; i++)
                {
                    province->cities[i] = province->cities[i + 1];
                }
                province->numCities--;
                province->overallHPI = calculateProvinceHPI(province);
                printf("City deleted.\n");
                exitFlag = 1;
            }
        }
    }
}

/**
 * Edits a city in the province using the city/barangay management menu.
 * Displays a menu for managing a city's barangays and allows editing its name.
 *
 * @param province Pointer to the Province struct.
 */
void editCity(Province *province)
{
    int i = 0, editIdx = 0;
    int exitFlag = 0;
    City *city;
    int exitFlag2 = 0;
    int cityEditChoice = 0;
    
    while (!exitFlag)
    {
        if (province->numCities == 0)
        {
            printf("No cities to edit.\n");
            exitFlag = 1;
        }
        else
        {
            printf("List of cities\n");
            // Iterates through cities to display options
            for (i = 0; i < province->numCities; i++)
            {
                printf("%d. %s\n", i + 1, province->cities[i].name);
            }
            printf("Enter the city to edit (1 to %d) 0 to cancel: ", province->numCities);
            editIdx = 0;
            // Reads user choice
            scanf("%d", &editIdx);
            // Clears buffer
            getchar();
            if (editIdx == 0)
            {
                exitFlag = 1;
            }
            else if (editIdx < 1 || editIdx > province->numCities)
            {
                printf("Invalid choice.\n");
                exitFlag = 1;
            }
            else
            {
                city = &province->cities[editIdx - 1];
                exitFlag2 = 0;
                // Continues until user decides to exit
                while (!exitFlag2)
                {
                    calculateCityMetrics(city);
                    province->overallHPI = calculateProvinceHPI(province);
                    printf("\nCity: %s\n", city->name);
                    printf("Total population: %d\n", city->totalPopulation);
                    printf("Total area: %.2f\n", city->totalArea);
                    printf("Total housing units: %d\n", city->totalHousingUnits);
                    printf("Average household size: %.2f\n", city->avgHouseholdSize);
                    printf("Population density: %.2f\n", city->populationDensity);
                    printf("HPI: %.2f\n", city->HPI);
                    printf("List of barangays:\n");
                    for (i = 0; i < city->numBarangays; i++)
                    {
                        printf("%d. %s\n", i + 1, city->barangayNames[i]);
                        printf("   Population: %d\n", city->populationSizes[i]);
                        printf("   Area: %.2f\n", city->areas[i]);
                        printf("   Avg Household Size: %.2f\n", city->avgHouseholdSizes[i]);
                        printf("   Formal Housing Units: %d\n", city->formalHousingUnits[i]);
                    }
                    printf("---------------------------------------------\n");
                    printf("What would you like to edit?\n");
                    printf("1. Edit city name\n");
                    printf("2. Remove Barangay\n");
                    printf("3. Add Barangay\n");
                    printf("4. Edit Barangay\n");
                    printf("5. Exit\n");
                    printf("Enter your choice: ");
                    cityEditChoice = 0;
                    scanf("%d", &cityEditChoice);
                    getchar();
                    // Handles different edit options
                    switch (cityEditChoice)
                    {
                        case 1:
                            editCityName(city);
                            sortCities(province, 0); // Keep cities sorted after editing name
                            break;
                        case 2:
                            removeBarangay(city);
                            break;
                        case 3:
                            addBarangay(city);
                            break;
                        case 4:
                            editBarangay(city);
                            break;
                        case 5:
                            exitFlag2 = 1;
                            break;
                        default:
                            printf("Invalid choice.\n");
                            break;
                    }
                }
                exitFlag = 1;
            }
        }
    }
}

/**
 * Searches for a city by name and displays its details if found.
 * Prompts the user for a city name to search for and displays its information if found.
 *
 * @param province Pointer to the Province struct.
 */
void searchCity(Province *province)
{
    char searchName[50];
    int i = 0;
	int found = 0;
    int exitFlag = 0;
    City *city;
    
    while (!exitFlag)
    {
        printf("Enter city name to search: ");
        // Reads search name from user
        fgets(searchName, sizeof(searchName), stdin);
        // Removes newline character
        searchName[strcspn(searchName, "\n")] = '\0';
        
        found = 0;
        for (i = 0; i < province->numCities; i++)
        {
            if (strcasecmp(province->cities[i].name, searchName) == 0) // Check if the current city is the same as the one the user inputted
            {
                printf("City found: %s\n", province->cities[i].name);
                found = 1;
                // Optionally, show city details
                city = &province->cities[i];
                printf("  Total Population: %d\n", city->totalPopulation);
                printf("  Total Area: %.2f\n", city->totalArea);
                printf("  Population Density: %.2f\n", city->populationDensity);
                printf("  Avg Household Size: %.2f\n", city->avgHouseholdSize);
                printf("  Total Formal Housing Units: %d\n", city->totalHousingUnits);
                printf("  HPI: %.2f\n", city->HPI);
                exitFlag = 1;
            }
        }
        if (!found) // if the loop goes to the end and still hasnt found a matching city.
        {
            printf("City not found.\n");
            exitFlag = 1;
        }
    }
}

/**
 * Province-level menu for city and barangay management.
 * Displays a menu for managing cities, barangays, and overall province metrics.
 *
 * @param province Pointer to the Province struct.
 * @param filename Username used for saving the file.
 */
void provinceMenu(Province *province, const char *filename)
{
    int choice = 0;
    
    // Continues until user chooses to save and exit
    do
    {
        printf("\nProvince: %s\n", province->provinceName);
        printf("[1] Add new city\n");
        printf("[2] Edit city\n");
        printf("[3] Delete city\n");
        printf("[4] Search city\n");
        printf("[5] View cities\n");
        printf("[6] View Province HPI\n");
        printf("[7] Save and Exit\n");
        printf("Enter choice: ");
        choice = 0;
        scanf("%d", &choice);
        getchar(); // clear buffer
        // Handles different menu options
        switch (choice)
        {
        case 1:
            addCity(province);
            break;
        case 2:
            editCity(province);
            break;
        case 3:
            deleteCity(province);
            break;
        case 4:
            searchCity(province);
            break;
        case 5:
            viewCities(province);
            break;
        case 6:

            printf("\n--- HPI Standards Reference ---\n");
            printf("| %-12s | %-20s |\n", "HPI Range", "Standard");
            printf("|-------------------|-------------------|\n");
            printf("| HPI >= 15         | Overcrowded       |\n");
            printf("| 6 <= HPI <= 14    | Urban standard    |\n");
            printf("| HPI <= 5          | Rural standard    |\n");
            printf("---------------------------------------------\n");

            printf("Overall Province HPI: %.2f\n", province->overallHPI);
            break;
        case 7:
            saveProvinceEncrypted(province->provinceName, province);
            printf("Data saved. Exiting...\n");
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (choice != 7);
}

/**
 * Main menu and program entry point. Handles file creation/loading, login, and launches the province menu.
 * Displays a main menu for file operations, test data, and exit.
 */
int main()
{
    Province province;
    char filename[100];
    int mainChoice;
    int loggedIn = 0;
    char username[100] = "";
    char password[100] = "";
    int passwordAttempts = 0;
    int resetChoice = 0;
    int mainExitFlag = 0;
    int returnValue = 0;
    int loginExitFlag = 0;

    while (!mainExitFlag)
    {
        printf("\n[1] Add new File\n");
        printf("[2] Load File\n");
        printf("[3] Test program (with dummy values for structures)\n");
        printf("[4] Exit\n");
        printf("Enter choice: ");
        scanf("%d", &mainChoice);
        getchar(); // clear buffer

        // Handles different main menu options
        if (mainChoice == 1)
        {
            // New file creation and login
            printf("Enter new province name: ");
            fgets(province.provinceName, sizeof(province.provinceName), stdin);
            province.provinceName[strcspn(province.provinceName, "\n")] = '\0';
            printf("Set username: ");
            fgets(province.username, sizeof(province.username), stdin);
            province.username[strcspn(province.username, "\n")] = '\0';
            printf("Set password: ");
            fgets(province.password, sizeof(province.password), stdin);
            province.password[strcspn(province.password, "\n")] = '\0';
            province.numCities = 0;
            province.overallHPI = 0;
            saveProvinceEncrypted(province.provinceName, &province);
            printf("File created and saved as %s.txt. Please login.\n", province.provinceName);
            // Login after creation
            loginExitFlag = 0;
            while (!loginExitFlag)
            {
                printf("Enter username: ");
                fgets(username, sizeof(username), stdin);
                username[strcspn(username, "\n")] = '\0';
                if (strcmp(username, province.username) != 0)
                {
                    printf("Incorrect username. Try again.\n");
                }
                else
                {
                    printf("Enter password: ");
                    fgets(password, sizeof(password), stdin);
                    password[strcspn(password, "\n")] = '\0';
                    if (strcmp(password, province.password) == 0)
                    {
                        loggedIn = 1;
                        loginExitFlag = 1;
                    }
                    else
                    {
                        printf("Incorrect password.\n");
                    }
                }
            }
            if (loggedIn)
                mainExitFlag = 1;
        }
        else if (mainChoice == 2)
        {
            // Load file and login/reset password
            printf("Enter province name to load: ");
            fgets(filename, sizeof(filename), stdin);
            filename[strcspn(filename, "\n")] = '\0';
            int fileLoaded = 0;
            if (!loadProvinceEncrypted(filename, &province))
            {
                printf("File not found for province '%s'.\n", filename);
                fileLoaded = 0;
            }
            else
            {
                fileLoaded = 1;
            }
            if (fileLoaded)
            {
                passwordAttempts = 0;
                int loginExitFlag = 0;
                while (!loginExitFlag)
                {
                    printf("Enter password: ");
                    fgets(password, sizeof(password), stdin);
                    password[strcspn(password, "\n")] = '\0';
                    if (strcmp(password, province.password) == 0)
                    {
                        loggedIn = 1;
                        loginExitFlag = 1;
                    }
                    else
                    {
                        printf("Incorrect password.\n");
                        passwordAttempts++;
                        if (passwordAttempts >= 1)
                        {
                            printf("Do you want to reset the password? (1 = Yes, 0 = No): ");
                            scanf("%d", &resetChoice);
                            getchar();
                            if (resetChoice == 1)
                            {
                                printf("Enter new password: ");
                                fgets(province.password, sizeof(province.password), stdin);
                                province.password[strcspn(province.password, "\n")] = '\0';
                                saveProvinceEncrypted(province.provinceName, &province);
                                printf("Password reset. Please login again.\n");
                                passwordAttempts = 0;
                            }
                            else
                            {
                                loginExitFlag = 1;
                            }
                        }
                    }
                }
                if (loggedIn)
                    mainExitFlag = 1;
            }
        }
        else if (mainChoice == 3)
        {
            // Fill with dummy data for testing
            fillDummyData(&province);
            strcpy(filename, "dummy.dat");
            loggedIn = 1;
            mainExitFlag = 1;
        }
        else if (mainChoice == 4)
        {
            printf("Exiting...\n");
            returnValue = 0;
            mainExitFlag = 1;
        }
        else
        {
            printf("Invalid choice.\n");
        }
    }

    if (loggedIn)
    {
        provinceMenu(&province, province.provinceName); // use province name for saving
    }

    return returnValue;
}
