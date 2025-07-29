#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CITIES 10
#define MAX_BARANGAYS 20
#define CAESAR_SHIFT 3

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
int saveProvinceEncrypted(const char *username, Province *province); // Paolo
int loadProvinceEncrypted(const char *username, Province *province); // Paolo
float calculateAvgHouseholdSize(City *c); // Paolo
void calculateCityMetrics(City *city); // Paolo
float calculateProvinceHPI(Province *p); // Paolo
void fillDummyData(Province *province); // Paolo
int promptLogin(Province *province); // Carlos
void editCityName(City *city); // Carlos
void removeBarangay(City *city); // Carlos
void addBarangay(City *city); // Carlos
void editBarangay(City *city); // Paolo
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
 *
 * relevant lines:
 * - for loop: Iterates through each byte and applies the shift.
 */
void caesar_encrypt(char *data, int len)
{
    for (int i = 0; i < len; i++)
    {
        data[i] = (char)(data[i] + CAESAR_SHIFT);
    }
}

/**
 * Decrypts a buffer using Caesar cipher.
 * Reverses the shift applied during encryption.
 *
 * @param data Pointer to the buffer to decrypt.
 * @param len Length of the buffer.
 *
 * relevant lines:
 * - for loop: Iterates through each byte and reverses the shift.
 */
void caesar_decrypt(char *data, int len)
{
    for (int i = 0; i < len; i++)
    {
        data[i] = (char)(data[i] - CAESAR_SHIFT);
    }
}

/**
 * Saves the Province struct to an encrypted file named <username>.txt.
 * Uses Caesar cipher for encryption.
 *
 * @param username The username to use as the filename.
 * @param province Pointer to the Province struct to save.
 * @return 1 on success, 0 on failure.
 *
 * relevant lines:
 * - caesar_encrypt: Encrypts the struct before writing.
 * - fwrite: Writes the encrypted data to file.
 */
int saveProvinceEncrypted(const char *username, Province *province)
{
    char filename[128];
    FILE *fp;
    char *buffer;
    int len;
    char temp[sizeof(Province)];
    int result = 0;
    
    snprintf(filename, sizeof(filename), "%s.txt", username);
    fp = fopen(filename, "wb");
    if (fp) {
        buffer = (char *)province;
        len = sizeof(Province);
        memcpy(temp, buffer, len);
        caesar_encrypt(temp, len);
        fwrite(temp, 1, len, fp);
        fclose(fp);
        result = 1;
    }
    return result;
}

/**
 * Loads the Province struct from an encrypted file named <username>.txt.
 * Decrypts the file using Caesar cipher.
 *
 * @param username The username to use as the filename.
 * @param province Pointer to the Province struct to load into.
 * @return 1 on success, 0 on failure.
 *
 * relevant lines:
 * - fread: Reads the encrypted data from file.
 * - caesar_decrypt: Decrypts the struct after reading.
 */
int loadProvinceEncrypted(const char *username, Province *province)
{
    char filename[128];
    FILE *fp;
    int len;
    char temp[sizeof(Province)];
    int read;
    int result = 0;
    
    snprintf(filename, sizeof(filename), "%s.txt", username);
    fp = fopen(filename, "rb");
    if (fp) {
        len = sizeof(Province);
        read = fread(temp, 1, len, fp);
        fclose(fp);
        if (read == len) {
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
 *
 * relevant lines:
 * - for loop: Sums up population and households for all barangays.
 * - Returns 0 if totalHouseholds is 0.
 */
float calculateAvgHouseholdSize(City *c)
{
    float totalPeople = 0;
    float totalHouseholds = 0;
    int i;
    Barangay *b;
    float result = 0;
    
    for (i = 0; i < c->numBarangays; i++)
    {
        b = &c->barangays[i];
        totalPeople += b->populationSize;
        totalHouseholds += b->populationSize / b->avgHouseholdSize;
    }
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
 *
 * relevant lines:
 * - for loop: Aggregates data from all barangays.
 * - Calls calculateAvgHouseholdSize for weighted average.
 */
void calculateCityMetrics(City *city)
{
    int i;
    Barangay *b;
    
    city->totalPopulation = 0;
    city->totalArea = 0;
    city->totalHousingUnits = 0;
    for (i = 0; i < city->numBarangays; i++)
    {
        b = &city->barangays[i];
        city->totalPopulation += b->populationSize;
        city->totalArea += b->area;
        city->totalHousingUnits += b->formalHousingUnits;
    }
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
 *
 * relevant lines:
 * - for loop: Iterates through all cities and updates their metrics.
 * - Returns 0 if totalUnits is 0.
 */
float calculateProvinceHPI(Province *p)
{
    float totalPeople = 0;
    float totalUnits = 0;
    int i;
    City *c;
    float result = 0;
    
    for (i = 0; i < p->numCities; i++)
    {
        calculateCityMetrics(&p->cities[i]);
        c = &p->cities[i];
        totalPeople += c->populationDensity * c->avgHouseholdSize;
        totalUnits += c->totalHousingUnits;
    }
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
 *
 * relevant lines:
 * - for loop: Fills cities and barangays with sample data.
 * - Calls calculateCityMetrics and calculateProvinceHPI.
 */
void fillDummyData(Province *province)
{
    int i, j;
    City *city;
    Barangay *b;
    
    strcpy(province->provinceName, "TestProvince");
    strcpy(province->username, "admin");
    strcpy(province->password, "admin");
    province->numCities = 2;
    for (i = 0; i < province->numCities; i++)
    {
        city = &province->cities[i];
        sprintf(city->name, "City%d", i + 1);
        city->numBarangays = 2;
        for (j = 0; j < city->numBarangays; j++)
        {
            b = &city->barangays[j];
            sprintf(b->name, "Barangay%d", j + 1);
            b->populationSize = 1000 * (j + 1);
            b->area = 2.5f * (j + 1);
            b->avgHouseholdSize = 4.0f;
            b->formalHousingUnits = 200 * (j + 1);
        }
        calculateCityMetrics(city);
    }
    province->overallHPI = calculateProvinceHPI(province);
}

/**
 * Prompts the user for username and password for login.
 * Compares input to stored values in the Province struct.
 *
 * @param province Pointer to the Province struct.
 * @return 1 if login successful, 0 otherwise.
 *
 * relevant lines:
 * - fgets: Reads username and password from user.
 * - strcmp: Compares input to stored credentials.
 */
int promptLogin(Province *province)
{
    char username[100], password[100];
    int result = 0;
    
    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';
    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0';
    if (strcmp(username, province->username) == 0 && strcmp(password, province->password) == 0)
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
 *
 * relevant lines:
 * - fgets: Reads new name from user.
 * - strcspn: Removes newline character.
 */
void editCityName(City *city)
{
    int exitFlag = 0;
    
    while (!exitFlag)
    {
        printf("Enter new city name: ");
        fgets(city->name, sizeof(city->name), stdin);
        city->name[strcspn(city->name, "\n")] = '\0';
        printf("City name updated.\n");
        exitFlag = 1;
    }
}

/**
 * Removes a barangay from a city.
 * Prompts the user to select a barangay to delete and removes it.
 *
 * @param city Pointer to the City struct.
 *
 * relevant lines:
 * - for loop: Iterates through barangays to display options.
 * - scanf: Reads user choice.
 * - getchar: Clears buffer.
 */
void removeBarangay(City *city)
{
    int i = 0, delIdx = 0;
    int exitFlag = 0;
    
    while (!exitFlag)
    {
        if (city->numBarangays == 0)
        {
            printf("No barangays to remove.\n");
            exitFlag = 1;
        }
        else
        {
            printf("List of barangays\n");
            for (i = 0; i < city->numBarangays; i++)
            {
                printf("%d. %s\n", i + 1, city->barangays[i].name);
            }
            printf("Enter the barangay to delete (1 to %d) 0 to cancel: ", city->numBarangays);
            scanf("%d", &delIdx);
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
                    city->barangays[i] = city->barangays[i + 1];
                }
                city->numBarangays--;
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
 *
 * relevant lines:
 * - while loop: Continues until user decides not to add more.
 * - if statement: Checks if max barangays reached.
 * - fgets: Reads barangay name.
 * - strcspn: Removes newline character.
 * - scanf: Reads population size, area, household size, housing units.
 * - getchar: Clears buffer.
 */
void addBarangay(City *city)
{
    int exitFlag = 0;
    Barangay *b;
    
    while (!exitFlag)
    {
        if (city->numBarangays >= MAX_BARANGAYS)
        {
            printf("Maximum number of barangays reached.\n");
            exitFlag = 1;
        }
        else
        {
            b = &city->barangays[city->numBarangays];
            //initialize values
            b->name[0] = '\0';
            b->populationSize = 0;
            b->area = 0.0f;
            b->avgHouseholdSize = 0.0f;
            b->formalHousingUnits = 0;

            printf("Enter barangay name: ");
            fgets(b->name, sizeof(b->name), stdin);
            b->name[strcspn(b->name, "\n")] = '\0';
            printf("Enter population size: ");
            scanf("%d", &b->populationSize);
            printf("Enter total area (km^2): ");
            scanf("%f", &b->area);
            printf("Enter average household size: ");
            scanf("%f", &b->avgHouseholdSize);
            printf("Enter number of formal housing units: ");
            scanf("%d", &b->formalHousingUnits);
            getchar();
            city->numBarangays++;
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
 *
 * relevant lines:
 * - for loop: Iterates through barangays to display options.
 * - scanf: Reads user choice.
 * - getchar: Clears buffer.
 * - while loop: Continues until user decides to exit.
 * - switch statement: Handles different edit options.
 */
void editBarangay(City *city)
{
    int i = 0, bchoice = 0, bEditChoice = 0;
    int exitFlag = 0;
    Barangay *b;
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
            for (i = 0; i < city->numBarangays; i++)
            {
                printf("%d. %s\n", i + 1, city->barangays[i].name);
            }
            printf("Enter the Barangay to edit (1 to %d) 0 to cancel: ", city->numBarangays);
            scanf("%d", &bchoice);
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
                b = &city->barangays[bchoice - 1];
                exitFlag2 = 0;
                while (!exitFlag2)
                {
                    printf("\nBarangay: %s\n", b->name);
                    printf("Total population: %d\n", b->populationSize);
                    printf("Total area: %.2f\n", b->area);
                    printf("Total housing units: %d\n", b->formalHousingUnits);
                    printf("Average household size: %.2f\n", b->avgHouseholdSize);
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
                    switch (bEditChoice)
                    {
                        case 1:
                            printf("Enter new barangay name: ");
                            fgets(b->name, sizeof(b->name), stdin);
                            b->name[strcspn(b->name, "\n")] = '\0';
                            printf("Barangay name updated.\n");
                            break;
                        case 2:
                            printf("Enter new population size: ");
                            scanf("%d", &b->populationSize);
                            getchar();
                            printf("Population size updated.\n");
                            break;
                        case 3:
                            printf("Enter new area (km^2): ");
                            scanf("%f", &b->area);
                            getchar();
                            printf("Area updated.\n");
                            break;
                        case 4:
                            printf("Enter new average household size: ");
                            scanf("%f", &b->avgHouseholdSize);
                            getchar();
                            printf("Average household size updated.\n");
                            break;
                        case 5:
                            printf("Enter new number of formal housing units: ");
                            scanf("%d", &b->formalHousingUnits);
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
                    calculateCityMetrics(city);
                }
                exitFlag = 1;
            }
        }
    }
}

/**
 * View cities and select one to edit (integrated menu logic).
 * Displays a list of cities and allows the user to view details of a selected city.
 *
 * @param province Pointer to the Province struct.
 *
 * relevant lines:
 * - for loop: Iterates through cities to display options.
 * - scanf: Reads user choice.
 * - getchar: Clears buffer.
 */
void viewCities(Province *province)
{
    int cityChoice = 0;
    int i = 0;
    int exitFlag = 0;
    City *city = NULL;
    Barangay *b = NULL;
    
    if (province->numCities == 0)
    {
        printf("No cities available.\n");
    }
    else
    {
        exitFlag = 0;
        while (!exitFlag)
        {
            printf("List of cities\n");
            for (i = 0; i < province->numCities; i++)
            {
                printf("%d. %s\n", i + 1, province->cities[i].name);
            }
            printf("---------------------------------------------\n");
            printf("Enter the City to view (1 to %d) 0 to cancel: ", province->numCities);
            cityChoice = 0;
            scanf("%d", &cityChoice);
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
                for (i = 0; i < city->numBarangays; i++)
                {
                    b = &city->barangays[i];
                    printf("%d. %s\n", i + 1, b->name);
                    printf("   Population: %d\n", b->populationSize);
                    printf("   Area: %.2f\n", b->area);
                    printf("   Avg Household Size: %.2f\n", b->avgHouseholdSize);
                    printf("   Formal Housing Units: %d\n", b->formalHousingUnits);
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
 *
 * relevant lines:
 * - while loop: Continues until user decides not to add more barangays.
 * - if statement: Checks if max barangays reached for the city.
 * - fgets: Reads city name.
 * - strcspn: Removes newline character.
 * - while loop: Continues until user decides not to add more barangays.
 * - fgets: Reads barangay name.
 * - strcspn: Removes newline character.
 * - scanf: Reads population size, area, household size, housing units.
 * - getchar: Clears buffer.
 */
void addCity(Province *province)
{
    int exitFlag = 0;
    City *city;
    char addMore;
    Barangay *b;
    
    while (!exitFlag)
    {
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
            fgets(city->name, sizeof(city->name), stdin);
            city->name[strcspn(city->name, "\n")] = '\0';
            city->numBarangays = 0;
            // Prompt to add barangays
            addMore = 'y';
            while (addMore == 'y' || addMore == 'Y')
            {
                if (city->numBarangays >= MAX_BARANGAYS)
                {
                    printf("Maximum number of barangays reached for this city.\n");
                    break;
                }
                b = &city->barangays[city->numBarangays];
                b->name[0] = '\0';
                b->populationSize = 0;
                b->area = 0.0f;
                b->avgHouseholdSize = 0.0f;
                b->formalHousingUnits = 0;
                printf("Enter barangay name: ");
                fgets(b->name, sizeof(b->name), stdin);
                b->name[strcspn(b->name, "\n")] = '\0';
                printf("Enter population size: ");
                scanf("%d", &b->populationSize);
                printf("Enter total area (km^2): ");
                scanf("%f", &b->area);
                printf("Enter average household size: ");
                scanf("%f", &b->avgHouseholdSize);
                printf("Enter number of formal housing units: ");
                scanf("%d", &b->formalHousingUnits);
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
            calculateCityMetrics(city);
            province->numCities++;
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
 *
 * relevant lines:
 * - for loop: Iterates through cities to display options.
 * - scanf: Reads user choice.
 * - getchar: Clears buffer.
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
            for (i = 0; i < province->numCities; i++)
            {
                printf("%d. %s\n", i + 1, province->cities[i].name);
            }
            printf("Enter the city to delete (1 to %d) 0 to cancel: ", province->numCities);
            delIdx = 0;
            scanf("%d", &delIdx);
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
 *
 * relevant lines:
 * - for loop: Iterates through cities to display options.
 * - scanf: Reads user choice.
 * - getchar: Clears buffer.
 * - while loop: Continues until user decides to exit.
 * - switch statement: Handles different edit options.
 */
void editCity(Province *province)
{
    int i = 0, editIdx = 0;
    int exitFlag = 0;
    City *city;
    int exitFlag2 = 0;
    int cityEditChoice = 0;
    Barangay *b;
    
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
            for (i = 0; i < province->numCities; i++)
            {
                printf("%d. %s\n", i + 1, province->cities[i].name);
            }
            printf("Enter the city to edit (1 to %d) 0 to cancel: ", province->numCities);
            editIdx = 0;
            scanf("%d", &editIdx);
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
                        b = &city->barangays[i];
                        printf("%d. %s\n", i + 1, b->name);
                        printf("   Population: %d\n", b->populationSize);
                        printf("   Area: %.2f\n", b->area);
                        printf("   Avg Household Size: %.2f\n", b->avgHouseholdSize);
                        printf("   Formal Housing Units: %d\n", b->formalHousingUnits);
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
                    switch (cityEditChoice)
                    {
                        case 1:
                            editCityName(city);
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
 *
 * relevant lines:
 * - fgets: Reads search name from user.
 * - strcspn: Removes newline character.
 */
void searchCity(Province *province)
{
    char searchName[50];
    int i = 0, found = 0;
    int exitFlag = 0;
    City *city;
    
    while (!exitFlag)
    {
        printf("Enter city name to search: ");
        fgets(searchName, sizeof(searchName), stdin);
        searchName[strcspn(searchName, "\n")] = '\0';
        found = 0;
        for (i = 0; i < province->numCities; i++)
        {
            if (strcasecmp(province->cities[i].name, searchName) == 0)
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
                break;
            }
        }
        if (!found)
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
 *
 * relevant lines:
 * - do-while loop: Continues until user chooses to save and exit.
 * - switch statement: Handles different menu options.
 */
void provinceMenu(Province *province, const char *filename)
{
    int choice = 0;
    
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
            printf("Overall Province HPI: %.2f\n", province->overallHPI);
            break;
        case 7:
            saveProvinceEncrypted(filename, province);
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
 *
 * relevant lines:
 * - while loop: Continues until user chooses to exit.
 * - switch statement: Handles different main menu options.
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

    while (1)
    {
        printf("\n[1] Add new File\n");
        printf("[2] Load File\n");
        printf("[3] Test program (with dummy values for structures)\n");
        printf("[4] Exit\n");
        printf("Enter choice: ");
        scanf("%d", &mainChoice);
        getchar(); // clear buffer

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
            saveProvinceEncrypted(province.username, &province);
            printf("File created and saved as %s.txt. Please login.\n", province.username);
            // Login after creation
            while (1)
            {
                printf("Enter username: ");
                fgets(username, sizeof(username), stdin);
                username[strcspn(username, "\n")] = '\0';
                if (strcmp(username, province.username) != 0)
                {
                    printf("Incorrect username. Try again.\n");
                    continue;
                }
                printf("Enter password: ");
                fgets(password, sizeof(password), stdin);
                password[strcspn(password, "\n")] = '\0';
                if (strcmp(password, province.password) == 0)
                {
                    loggedIn = 1;
                    break;
                }
                else
                {
                    printf("Incorrect password.\n");
                }
            }
            if (loggedIn)
                break;
        }
        else if (mainChoice == 2)
        {
            // Load file and login/reset password
            printf("Enter username to load: ");
            fgets(username, sizeof(username), stdin);
            username[strcspn(username, "\n")] = '\0';
            if (!loadProvinceEncrypted(username, &province))
            {
                printf("File not found for username '%s'.\n", username);
                continue;
            }
            passwordAttempts = 0;
            while (1)
            {
                printf("Enter password: ");
                fgets(password, sizeof(password), stdin);
                password[strcspn(password, "\n")] = '\0';
                if (strcmp(password, province.password) == 0)
                {
                    loggedIn = 1;
                    break;
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
                            saveProvinceEncrypted(username, &province);
                            printf("Password reset. Please login again.\n");
                            passwordAttempts = 0;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }
            if (loggedIn)
                break;
        }
        else if (mainChoice == 3)
        {
            // Fill with dummy data for testing
            fillDummyData(&province);
            strcpy(filename, "dummy.dat");
            loggedIn = 1;
            break;
        }
        else if (mainChoice == 4)
        {
            printf("Exiting...\n");
            return 0;
        }
        else
        {
            printf("Invalid choice.\n");
        }
    }

    if (loggedIn)
    {
        provinceMenu(&province, province.username); // use username for saving
    }

    return 0;
}
