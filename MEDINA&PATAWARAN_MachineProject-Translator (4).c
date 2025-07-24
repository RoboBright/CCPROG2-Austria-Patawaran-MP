/*********************************************************************************************************  
This is to certify that this project is my own work, based on my personal efforts in studying and applying the concepts 
learned.  I have constructed the functions and their respective algorithms and corresponding code by myself.  The 
program was run, tested, and debugged by my own efforts.  I further certify that I have not copied in part or whole or 
otherwise plagiarized the work of other students and/or persons. 

                                    <Medina, Paolo>, DLSU ID# <12484172> & <Patawaran, Paolo Theodore>, DLSU ID# <12484008> 
*********************************************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ENTRIES 150
#define MAX_PAIRS 10
#define MAX_WORD_LENGTH 21  // 20 characters + null terminator
#define MAX_FILENAME 31     // 30 characters + null terminator
#define MAX_TEXT_LENGTH 151 // 150 characters + null terminator

// Structure to represent a language-translation pair
typedef struct {
    char language[MAX_WORD_LENGTH];
    char translation[MAX_WORD_LENGTH];
} LanguagePair;

// Structure to represent an entry with multiple language-translation pairs
typedef struct {
    LanguagePair pairs[MAX_PAIRS];
    int count;  // Number of language-translation pairs in this entry
} Entry;

// Function prototypes for Manage Data menu
void addEntry(Entry entries[], int *nEntries);
void addTranslations(Entry entries[], int nEntries);
void deleteEntry(Entry entries[], int *nEntries);
void deleteTranslation(Entry entries[], int *nEntries);
void displayAllEntries(Entry entries[], int nEntries);
void searchWord(Entry entries[], int nEntries);
void searchTranslation(Entry entries[], int nEntries);
void exportData(Entry entries[], int nEntries);
void importData(Entry entries[], int *nEntries);
void exportFlashcards(Entry entries[], int nEntries);
void viewFlashcards(Entry entries[], int nEntries);

// Function prototypes for Translate menu
void translateTextInput(Entry entries[], int nEntries);
void translateTextFile(Entry entries[], int nEntries);

// Helper function prototypes
void displayEntry(Entry entry, int entryNum);
void sortEntryByLanguage(Entry *entry);
void sortEntriesByEnglish(Entry entries[], int nEntries);
int findEntryWithLangTranslation(Entry entries[], int nEntries, const char *language, const char *translation);
char* translateWord(Entry entries[], int nEntries, const char *word, const char *srcLang, const char *targetLang);
void tokenizeAndTranslate(Entry entries[], int nEntries, char *text, const char *srcLang, const char *targetLang, char *result);
void clearEntries(Entry entries[], int *nEntries);
void displayEntryPaginated(Entry entries[], int nEntries, int *currentIndex, int *running);
void removePunctuation(char *word);
void convertToLowercase(char *text);
char toUpper(char c);

int main() {
    Entry entries[MAX_ENTRIES];
    int nEntries = 0;
    int mainChoice, subChoice;
    
    do {
        // Display main menu
        printf("\n===== SIMPLE TRANSLATOR =====\n");
        printf("1. Manage Data\n");
        printf("2. Translate Menu\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &mainChoice);
        getchar(); // Clear the newline
        
        switch (mainChoice) {
            case 1: // Manage Data Menu
                do {
                    printf("\n===== MANAGE DATA MENU =====\n");
                    printf("1. Add Entry\n");
                    printf("2. Add Translations\n");
                    printf("3. Delete Entry\n");
                    printf("4. Delete Translation\n");
                    printf("5. Display All Entries\n");
                    printf("6. Search Word\n");
                    printf("7. Search Translation\n");
                    printf("8. Export\n");
                    printf("9. Import\n");
                    printf("10. Export Flashcards\n");
                    printf("11. View Flashcards\n");
                    printf("12. Exit\n");
                    printf("Enter your choice: ");
                    scanf("%d", &subChoice);
                    getchar(); // Clear the newline
                    
                    switch (subChoice) {
                        case 1: addEntry(entries, &nEntries); break;
                        case 2: addTranslations(entries, nEntries); break;
                        case 3: deleteEntry(entries, &nEntries); break;
                        case 4: deleteTranslation(entries, &nEntries); break;
                        case 5: displayAllEntries(entries, nEntries); break;
                        case 6: searchWord(entries, nEntries); break;
                        case 7: searchTranslation(entries, nEntries); break;
                        case 8: exportData(entries, nEntries); break;
                        case 9: importData(entries, &nEntries); break;
                        case 10: exportFlashcards(entries, nEntries); break;
                        case 11: viewFlashcards(entries, nEntries); break;
                        case 12: clearEntries(entries, &nEntries); break;
                        default: printf("Invalid choice! Please try again.\n");
                    }
                } while (subChoice != 12);
                break;
                
            case 2: // Translate Menu
                if (nEntries > 0) {
                    clearEntries(entries, &nEntries); // Clear existing entries
                }
                
                // Import data for translation
                char filename[MAX_FILENAME];
                printf("Enter filename to import data from: ");
                fgets(filename, MAX_FILENAME, stdin);
                filename[strcspn(filename, "\n")] = '\0'; // Remove newline
                
                // Import data from file
                FILE *file = fopen(filename, "r");
                if (file == NULL) {
                    printf("Error opening file. Returning to main menu.\n");
                    break;
                }
                fclose(file);
                importData(entries, &nEntries);
                
                do {
                    printf("\n===== TRANSLATE MENU =====\n");
                    printf("1. Translate Text Input\n");
                    printf("2. Translate Text File\n");
                    printf("3. Exit\n");
                    printf("Enter your choice: ");
                    scanf("%d", &subChoice);
                    getchar(); // Clear the newline
                    
                    switch (subChoice) {
                        case 1: translateTextInput(entries, nEntries); break;
                        case 2: translateTextFile(entries, nEntries); break;
                        case 3: clearEntries(entries, &nEntries); break;
                        default: printf("Invalid choice! Please try again.\n");
                    }
                } while (subChoice != 3);
                break;
                
            case 3: // Exit
                printf("Thank you for using Simple Translator. Goodbye!\n");
                break;
                
            default:
                printf("Invalid choice! Please try again.\n");
        }
    } while (mainChoice != 3);
    
    return 0;
}

/* addEntry adds a new entry to the dictionary with one or more language-translation pairs
   @param entries - array of Entry structures where the new entry will be added
   @param nEntries - pointer to the number of entries, will be updated with new count
   Pre-condition: entries array has sufficient space for new entries
*/
void addEntry(Entry entries[], int *nEntries) {
    char language[MAX_WORD_LENGTH] = {0};
    char translation[MAX_WORD_LENGTH] = {0};
    char morePairs = 'N';
    int existingEntries[MAX_ENTRIES] = {0};
    int nExisting = 0;
    int i = 0;
    int j = 0;
    int maxReached = 0;
    int validInput = 0;
    char choice = '\0';
    int shouldProceed = 1;
    int isMaxEntriesReached = 0;
    int isInputValid = 1;
    int isNewEntry = 1;
    
    Entry newEntry;
    newEntry.count = 0;
    
    // Check if maximum entries reached
    isMaxEntriesReached = (*nEntries >= MAX_ENTRIES);
    if (isMaxEntriesReached) {
        printf("Maximum number of entries reached!\n");
        shouldProceed = 0;
    }
    
    if (shouldProceed) {
        printf("Enter language: ");
        fgets(language, MAX_WORD_LENGTH, stdin);
        language[strcspn(language, "\n")] = '\0'; // Remove newline
        convertToLowercase(language);
        
        printf("Enter translation: ");
        fgets(translation, MAX_WORD_LENGTH, stdin);
        translation[strcspn(translation, "\n")] = '\0'; // Remove newline
        convertToLowercase(translation);
        
        // Check if language and translation have at least 1 character
        isInputValid = (strlen(language) > 0 && strlen(translation) > 0);
        if (!isInputValid) {
            printf("Language and translation must have at least 1 character!\n");
            shouldProceed = 0;
        }
    }
    
    if (shouldProceed) {
        // Check if entry with given language-translation pair already exists
        while (i < *nEntries) {
            j = 0;
            while (j < entries[i].count) {
                if (strcmp(entries[i].pairs[j].language, language) == 0 &&
                    strcmp(entries[i].pairs[j].translation, translation) == 0) {
                    existingEntries[nExisting++] = i;
                    j = entries[i].count; // Exit inner loop
                }
                j++;
            }
            i++;
        }
        
        // If existing entries found, display them
        if (nExisting > 0) {
            printf("The following entries already exist with the given language-translation pair:\n");
            for (i = 0; i < nExisting; i++) {
                displayEntry(entries[existingEntries[i]], existingEntries[i] + 1);
            }
            
            printf("Is this a new entry? (Y/N): ");
            scanf("%c", &choice);
            getchar(); // Clear the newline
            
            isNewEntry = (toUpper(choice) == 'Y');
            if (!isNewEntry) {
                shouldProceed = 0;
            }
        }
    }
    
    if (shouldProceed) {
        // Add the first language-translation pair
        strcpy(newEntry.pairs[0].language, language);
        strcpy(newEntry.pairs[0].translation, translation);
        newEntry.count++;
        
        // Add more language-translation pairs
        do {
            printf("Add another language-translation pair? (Y/N): ");
            scanf("%c", &morePairs);
            getchar(); // Clear the newline
            
            if (toUpper(morePairs) == 'Y') {
                if (newEntry.count >= MAX_PAIRS) {
                    printf("Maximum number of language-translation pairs reached for this entry!\n");
                    break;
                }
                
                printf("Enter language: ");
                fgets(language, MAX_WORD_LENGTH, stdin);
                language[strcspn(language, "\n")] = '\0'; // Remove newline
                convertToLowercase(language);
                
                printf("Enter translation: ");
                fgets(translation, MAX_WORD_LENGTH, stdin);
                translation[strcspn(translation, "\n")] = '\0'; // Remove newline
                convertToLowercase(translation);
                
                // Check if language and translation have at least 1 character
                validInput = (strlen(language) > 0 && strlen(translation) > 0);
                if (validInput) {
                    strcpy(newEntry.pairs[newEntry.count].language, language);
                    strcpy(newEntry.pairs[newEntry.count].translation, translation);
                    newEntry.count++;
                } else {
                    printf("Language and translation must have at least 1 character! Try again.\n");
                }
            }
        } while (toUpper(morePairs) == 'Y' && newEntry.count < MAX_PAIRS);
        
        // Add the new entry to the list of entries
        entries[*nEntries] = newEntry;
        (*nEntries)++;
        
        printf("Entry added successfully!\n");
    }
}

/* addTranslations adds additional language-translation pairs to an existing entry
   @param entries - array of Entry structures containing the dictionary data
   @param nEntries - number of entries in the dictionary
   Pre-condition: entries array is properly initialized and has at least one entry
*/
void addTranslations(Entry entries[], int nEntries) {
    char language[MAX_WORD_LENGTH] = {0};
    char translation[MAX_WORD_LENGTH] = {0};
    int existingEntries[MAX_ENTRIES] = {0};
    int nExisting = 0;
    int i = 0;
    int j = 0;
    int entryIndex = 0;
    char morePairs = 'N';
    int shouldProceed = 1;
    int hasEntries = (nEntries > 0);
    int foundExisting = 0;
    int validInput = 0;
    int maxReached = 0;
    
    if (!hasEntries) {
        printf("No entries exist. Please add an entry first.\n");
        shouldProceed = 0;
    }
    
    if (shouldProceed) {
        printf("Enter language: ");
        fgets(language, MAX_WORD_LENGTH, stdin);
        language[strcspn(language, "\n")] = '\0'; // Remove newline
        convertToLowercase(language);
        
        printf("Enter translation: ");
        fgets(translation, MAX_WORD_LENGTH, stdin);
        translation[strcspn(translation, "\n")] = '\0'; // Remove newline
        convertToLowercase(translation);
        
        // Check if entry with given language-translation pair exists
        while (i < nEntries) {
            j = 0;
            while (j < entries[i].count) {
                if (strcmp(entries[i].pairs[j].language, language) == 0 &&
                    strcmp(entries[i].pairs[j].translation, translation) == 0) {
                    existingEntries[nExisting++] = i;
                    j = entries[i].count; // Exit inner loop
                }
                j++;
            }
            i++;
        }
        
        foundExisting = (nExisting > 0);
        if (!foundExisting) {
            printf("No entries found with the given language-translation pair. Please use Add Entry first.\n");
            shouldProceed = 0;
        }
    }
    
    if (shouldProceed) {
        // Display existing entries
        printf("The following entries exist with the given language-translation pair:\n");
        for (i = 0; i < nExisting; i++) {
            displayEntry(entries[existingEntries[i]], existingEntries[i] + 1);
        }
        
        // Determine which entry to modify
        if (nExisting == 1) {
            entryIndex = existingEntries[0];
        } else {
            printf("Which entry do you want to add translations to? (1-%d): ", nExisting);
            scanf("%d", &entryIndex);
            getchar(); // Clear the newline
            
            if (entryIndex < 1 || entryIndex > nExisting) {
                printf("Invalid entry number!\n");
                shouldProceed = 0;
            } else {
                entryIndex = existingEntries[entryIndex - 1];
            }
        }
    }
    
    if (shouldProceed) {
        // Add more language-translation pairs
        morePairs = 'Y';
        while (morePairs == 'Y' && entries[entryIndex].count < MAX_PAIRS && !maxReached) {
            printf("Add another language-translation pair? (Y/N): ");
            scanf("%c", &morePairs);
            getchar(); // Clear the newline
            
            if (toUpper(morePairs) == 'Y') {
                printf("Enter new language: ");
                fgets(language, MAX_WORD_LENGTH, stdin);
                language[strcspn(language, "\n")] = '\0'; // Remove newline
                convertToLowercase(language);
                
                printf("Enter new translation: ");
                fgets(translation, MAX_WORD_LENGTH, stdin);
                translation[strcspn(translation, "\n")] = '\0'; // Remove newline
                convertToLowercase(translation);
                
                // Check if language and translation have at least 1 character
                validInput = (strlen(language) > 0 && strlen(translation) > 0);
                if (validInput) {
                    strcpy(entries[entryIndex].pairs[entries[entryIndex].count].language, language);
                    strcpy(entries[entryIndex].pairs[entries[entryIndex].count].translation, translation);
                    entries[entryIndex].count++;
                } else {
                    printf("Language and translation must have at least 1 character! Try again.\n");
                }
            }
        }
        
        if (entries[entryIndex].count >= MAX_PAIRS) {
            printf("Maximum number of language-translation pairs reached for this entry!\n");
        }
        
        printf("Translations added successfully!\n");
    }
}

/* deleteEntry removes an entry from the dictionary and shifts remaining entries
   @param entries - array of Entry structures containing the dictionary data
   @param nEntries - pointer to the number of entries, will be updated with new count
   Pre-condition: entries array is properly initialized and has at least one entry
*/
void deleteEntry(Entry entries[], int *nEntries) {
    int i = 0;
    int entryNum = 0;
    int shouldProceed = 1;
    int hasEntries = (*nEntries > 0);
    int validEntryNum = 0;
    
    if (!hasEntries) {
        printf("No entries exist!\n");
        shouldProceed = 0;
    }
    
    if (shouldProceed) {
        // Display all entries
        printf("===== All Entries =====\n");
        for (i = 0; i < *nEntries; i++) {
            displayEntry(entries[i], i + 1);
        }
        
        printf("Enter the entry number to delete (1-%d): ", *nEntries);
        scanf("%d", &entryNum);
        getchar(); // Clear the newline
        
        validEntryNum = (entryNum >= 1 && entryNum <= *nEntries);
        if (!validEntryNum) {
            printf("Invalid entry number!\n");
            shouldProceed = 0;
        }
    }
    
    if (shouldProceed) {
        // Shift entries to cover the deleted one
        for (i = entryNum - 1; i < *nEntries - 1; i++) {
            entries[i] = entries[i + 1];
        }
        
        (*nEntries)--;
        printf("Entry deleted successfully!\n");
    }
}

/* deleteTranslation removes one or more language-translation pairs from an entry
   @param entries - array of Entry structures containing the dictionary data
   @param nEntries - pointer to the number of entries, will be updated if last pair is deleted
   Pre-condition: entries array is properly initialized and has at least one entry
*/
void deleteTranslation(Entry entries[], int *nEntries) {
    int i = 0;
    int entryNum = 0;
    int pairNum = 0;
    int shouldProceed = 1;
    int hasEntries = (*nEntries > 0);
    int validEntryNum = 0;
    int validPairNum = 0;
    int isLastPair = 0;
    int shouldDeleteMore = 1;
    char moreDeletes = 'N';
    
    if (!hasEntries) {
        printf("No entries exist!\n");
        shouldProceed = 0;
    }
    
    if (shouldProceed) {
        // Display all entries
        printf("===== All Entries =====\n");
        for (i = 0; i < *nEntries; i++) {
            displayEntry(entries[i], i + 1);
        }
        
        printf("Enter the entry number to delete from (1-%d): ", *nEntries);
        scanf("%d", &entryNum);
        getchar(); // Clear the newline
        
        validEntryNum = (entryNum >= 1 && entryNum <= *nEntries);
        if (!validEntryNum) {
            printf("Invalid entry number!\n");
            shouldProceed = 0;
        }
    }
    
    if (shouldProceed) {
        entryNum--; // Convert to 0-based index
        
        while (shouldDeleteMore && entries[entryNum].count > 0) {
            // Display language-translation pairs for the selected entry
            printf("Language-Translation pairs for Entry %d:\n", entryNum + 1);
            for (i = 0; i < entries[entryNum].count; i++) {
                printf("%d. %s: %s\n", i + 1, entries[entryNum].pairs[i].language, entries[entryNum].pairs[i].translation);
            }
            
            printf("Enter the pair number to delete (1-%d): ", entries[entryNum].count);
            scanf("%d", &pairNum);
            getchar(); // Clear the newline
            
            validPairNum = (pairNum >= 1 && pairNum <= entries[entryNum].count);
            if (!validPairNum) {
                printf("Invalid pair number!\n");
                printf("Do you want to try deleting another pair? (Y/N): ");
                scanf("%c", &moreDeletes);
                getchar(); // Clear the newline
                shouldDeleteMore = (toUpper(moreDeletes) == 'Y');
            } else {
                pairNum--; // Convert to 0-based index
                
                // Check if this is the last pair
                isLastPair = (entries[entryNum].count == 1);
                
                if (isLastPair) {
                    // Shift entries to cover the deleted one
                    for (i = entryNum; i < *nEntries - 1; i++) {
                        entries[i] = entries[i + 1];
                    }
                    
                    (*nEntries)--;
                    printf("Last pair deleted. Entry %d has been removed.\n", entryNum + 1);
                    shouldDeleteMore = 0;
                } else {
                    // Shift pairs to cover the deleted one
                    for (i = pairNum; i < entries[entryNum].count - 1; i++) {
                        strcpy(entries[entryNum].pairs[i].language, entries[entryNum].pairs[i + 1].language);
                        strcpy(entries[entryNum].pairs[i].translation, entries[entryNum].pairs[i + 1].translation);
                    }
                    
                    entries[entryNum].count--;
                    printf("Pair deleted successfully!\n");
                    
                    printf("Do you want to delete another pair from this entry? (Y/N): ");
                    scanf("%c", &moreDeletes);
                    getchar(); // Clear the newline
                    shouldDeleteMore = (toUpper(moreDeletes) == 'Y' && entries[entryNum].count > 0);
                }
            }
        }
    }
}

/* displayAllEntries shows all entries in the dictionary sorted by English translation
   @param entries - array of Entry structures containing the dictionary data
   @param nEntries - number of entries in the dictionary
   Pre-condition: entries array is properly initialized
*/
void displayAllEntries(Entry entries[], int nEntries) 
{
    int i = 0;
    int currentIndex = 0;
    int running = 1;
    int hasEntries = (nEntries > 0);
    int shouldProceed = 1;
    Entry sortedEntries[MAX_ENTRIES];
    
    if (!hasEntries) {
        printf("No entries exist!\n");
        shouldProceed = 0;
    }
    
    if (shouldProceed) {
        // Sort entries by English language
        memcpy(sortedEntries, entries, sizeof(Entry) * nEntries);
        
        // Sort each entry internally by language
        for (i = 0; i < nEntries; i++) 
        {
            sortEntryByLanguage(&sortedEntries[i]);
        }
        
        // Sort entries based on English translation
        sortEntriesByEnglish(sortedEntries, nEntries);
        
        // Display entries with pagination
        displayEntryPaginated(sortedEntries, nEntries, &currentIndex, &running);
    }
}

/* searchWord searches for entries containing a specific word as translation
   @param entries - array of Entry structures containing the dictionary data
   @param nEntries - number of entries in the dictionary
   Pre-condition: entries array is properly initialized
*/
void searchWord(Entry entries[], int nEntries) 
{
    int i, j;
    int nMatching = 0;
    int foundMatch = 0;
    int currentIndex;
    int running;

    if (nEntries > 0) 
    {
        char word[MAX_WORD_LENGTH];
        printf("Enter the word to search: ");
        fgets(word, MAX_WORD_LENGTH, stdin);
        word[strcspn(word, "\n")] = '\0'; // newline removal
        convertToLowercase(word);
        
        // find entries with the given word as translation
        Entry matchingEntries[MAX_ENTRIES];
        
        for (i = 0; i < nEntries; i++) 
        {
            foundMatch = 0;
            for (j = 0; j < entries[i].count && !foundMatch; j++) 
            {
                if (strcmp(entries[i].pairs[j].translation, word) == 0) 
                {
                    matchingEntries[nMatching++] = entries[i];
                    foundMatch = 1;
                }
            }
        }
        
        if (nMatching > 0) 
        {
            // sort each matching entry internally by language
            for (i = 0; i < nMatching; i++) 
            {
                sortEntryByLanguage(&matchingEntries[i]);
            }
            
            // Display matching entries with pagination
            currentIndex = 0;
            running = 1;
            
            displayEntryPaginated(matchingEntries, nMatching, &currentIndex, &running);
        }

        else
        {
            printf("No entries found with the given word as translation!\n");
        }

    }
    else
    {
        printf("No entries exist!\n");
    }
}

/* searchTranslation searches for entries containing a specific language-translation pair
   @param entries - array of Entry structures containing the dictionary data
   @param nEntries - number of entries in the dictionary
   Pre-condition: entries array is properly initialized
*/
void searchTranslation(Entry entries[], int nEntries) 
{
    int i, j;
    
    int nMatching = 0;
    int foundMatch = 0;
    
    int currentIndex = 0;
    int running = 1;

    char language[MAX_WORD_LENGTH];
    char translation[MAX_WORD_LENGTH];
    Entry matchingEntries[MAX_ENTRIES];

    if (nEntries > 0) 
    {
        printf("Enter language: ");
        fgets(language, MAX_WORD_LENGTH, stdin);
        language[strcspn(language, "\n")] = '\0'; 
        convertToLowercase(language);
        
        printf("Enter translation: ");
        fgets(translation, MAX_WORD_LENGTH, stdin);
        translation[strcspn(translation, "\n")] = '\0'; 
        convertToLowercase(translation);
        
        // Find entries with the given language-translation pair
        for (i = 0; i < nEntries; i++) 
        {
            foundMatch = 0;
            for (j = 0; j < entries[i].count && !foundMatch; j++) 
            {
                if (strcmp(entries[i].pairs[j].language, language) == 0 && strcmp(entries[i].pairs[j].translation, translation) == 0) 
                {
                    matchingEntries[nMatching++] = entries[i];
                    foundMatch = 1;
                }
                
            }
        }
        
        if (nMatching > 0) 
        {
            // Sort each matching entry internally by language
            for (i = 0; i < nMatching; i++) 
            {
                sortEntryByLanguage(&matchingEntries[i]);
            }
            
            // Display matching entries with pagination
            displayEntryPaginated(matchingEntries, nMatching, &currentIndex, &running);
        }
        else
        {
            printf("No entries found with the given language-translation pair!\n");
        }
    }
    else
    {
        printf("No entries exist!\n");
    }
}

/* exportData exports the dictionary data to a file in a sorted format
   @param entries - array of Entry structures containing the dictionary data
   @param nEntries - number of entries in the dictionary
   Pre-condition: entries array is properly initialized
*/
void exportData(Entry entries[], int nEntries) 
{
    int i, j;
    char filename[MAX_FILENAME];
    FILE *file = NULL;
    Entry sortedEntries[MAX_ENTRIES];
    int fileOpened = 0;

    if (nEntries > 0) 
    {
        printf("Enter filename to export to: ");
        fgets(filename, MAX_FILENAME, stdin);
        filename[strcspn(filename, "\n")] = '\0'; // Remove newline
        
        file = fopen(filename, "w");
        if (file != NULL) 
        {
            fileOpened = 1;
            
            // Sort entries by English for export
            memcpy(sortedEntries, entries, sizeof(Entry) *nEntries);
            
            // Sort each entry internally by language
            for (i = 0; i < nEntries; i++) 
            {
                sortEntryByLanguage(&sortedEntries[i]);
            }
            
            // Sort entries based on English translation
            sortEntriesByEnglish(sortedEntries, nEntries);
            
            // Write entries to file
            for (i = 0; i < nEntries; i++) 
            {
                for (j = 0; j < sortedEntries[i].count; j++) 
                {
                    fprintf(file, "%s: %s\n", sortedEntries[i].pairs[j].language, sortedEntries[i].pairs[j].translation);
                }
                fprintf(file, "\n"); // Empty line between entries
            }
            
            fclose(file);
            printf("Data exported successfully to %s!\n", filename);
        }
        else
        {
            printf("Error opening file for writing!\n");
        }
    }
    else
    {
        printf("No entries exist to export!\n");
    }
}

/* importData imports dictionary data from a file
   @param entries - array of Entry structures where imported data will be stored
   @param nEntries - pointer to the number of entries, will be updated with new count
   Pre-condition: entries array has sufficient space for new entries
*/
void importData(Entry entries[], int *nEntries) 
{
    int i = 0;
    char filename[MAX_FILENAME] = {0};
    char line[MAX_WORD_LENGTH * 2] = {0};
    char choice = '\0';
    char *colonPos = NULL;
    int langLen = 0;
    int maxReached = 0;
    int shouldProcessLine = 1;
    
    Entry tempEntry;
    tempEntry.count = 0;
    
    FILE *file = NULL;
    
    printf("Enter filename to import from: ");
    fgets(filename, MAX_FILENAME, stdin);
    filename[strcspn(filename, "\n")] = '\0'; // Remove newline
    
    file = fopen(filename, "r");
    if (file != NULL) 
    {
        while (fgets(line, sizeof(line), file) != NULL && !maxReached) 
        {
            shouldProcessLine = 1;
            // Remove newline
            line[strcspn(line, "\n")] = '\0';
            
            // Empty line indicates end of an entry
            if (strlen(line) == 0) 
            {
                if (tempEntry.count > 0) 
                {
                    // Display the entry and ask if it should be added
                    printf("Entry found in file:\n");
                    displayEntry(tempEntry, *nEntries + 1);
                    
                    printf("Add this entry? (Y/N): ");
                    scanf("%c", &choice);
                    getchar(); // Clear the newline
                    
                    if (toUpper(choice) == 'Y') 
                    {
                        if (*nEntries < MAX_ENTRIES) 
                        {
                            entries[*nEntries] = tempEntry;
                            (*nEntries)++;
                        } 
                        else 
                        {
                            printf("Maximum number of entries reached! Can't add more.\n");
                            maxReached = 1;
                        }
                    }
                    
                    // Reset temporary entry
                    tempEntry.count = 0;
                    shouldProcessLine = 0;
                }
            }
            
            if (shouldProcessLine)
            {
                // Find the position of the first colon
                colonPos = strchr(line, ':');
                if (colonPos != NULL) 
                {
                    // Calculate the length of the language part
                    langLen = colonPos - line;
                    
                    // Create a temporary buffer for the language
                    char language[MAX_WORD_LENGTH] = {0};
                    strncpy(language, line, langLen);
                    language[langLen] = '\0';
                    
                    // Get the translation (everything after the colon)
                    char *translation = colonPos + 1;
                    
                    // Remove leading and trailing spaces from both language and translation
                    while (*language == ' ') 
                    {
                        memmove(language, language + 1, strlen(language));
                    }
                    while (strlen(language) > 0 && language[strlen(language) - 1] == ' ') 
                    {
                        language[strlen(language) - 1] = '\0';
                    }
                    
                    while (*translation == ' ') translation++;
                    while (strlen(translation) > 0 && translation[strlen(translation) - 1] == ' ') 
                    {
                        translation[strlen(translation) - 1] = '\0';
                    }
                    
                    if (strlen(language) > 0 && strlen(translation) > 0 && tempEntry.count < MAX_PAIRS) 
                    {
                        // Convert language and translation to lowercase
                        convertToLowercase(language);
                        convertToLowercase(translation);
                        
                        strcpy(tempEntry.pairs[tempEntry.count].language, language);
                        strcpy(tempEntry.pairs[tempEntry.count].translation, translation);
                        tempEntry.count++;
                    }
                }
            }
        }
        
        // Process the last entry if it exists
        if (tempEntry.count > 0 && !maxReached) 
        {
            printf("Entry found in file:\n");
            displayEntry(tempEntry, *nEntries + 1);
            
            printf("Add this entry? (Y/N): ");
            scanf("%c", &choice);
            getchar(); // Clear the newline
            
            if (toUpper(choice) == 'Y') 
            {
                if (*nEntries < MAX_ENTRIES) 
                {
                    entries[*nEntries] = tempEntry;
                    (*nEntries)++;
                } 
                else 
                {
                    printf("Maximum number of entries reached! Can't add more.\n");
                }
            }
        }
        
        fclose(file);
        printf("Data imported successfully from %s!\n", filename);
    }
    else
    {
        printf("Error opening file for reading!\n");
    }
}

/* translateTextInput translates text input from one language to another
   @param entries - array of Entry structures containing the dictionary data
   @param nEntries - number of entries in the dictionary
   Pre-condition: entries array is properly initialized
*/
void translateTextInput(Entry entries[], int nEntries) 
{
    char srcLang[MAX_WORD_LENGTH];
    char targetLang[MAX_WORD_LENGTH];
    char text[MAX_TEXT_LENGTH];
    char result[MAX_TEXT_LENGTH * 2];
    char moreTranslations;
    int continueTranslation = 1;

    if (nEntries > 0) 
    {
        printf("Enter source language: ");
        fgets(srcLang, MAX_WORD_LENGTH, stdin);
        srcLang[strcspn(srcLang, "\n")] = '\0'; // Remove newline
        convertToLowercase(srcLang);
        
        printf("Enter target language: ");
        fgets(targetLang, MAX_WORD_LENGTH, stdin);
        targetLang[strcspn(targetLang, "\n")] = '\0'; // Remove newline
        convertToLowercase(targetLang);
        
        while (continueTranslation) 
        {
            printf("Enter text to translate (max 150 characters): ");
            fgets(text, MAX_TEXT_LENGTH, stdin);
            text[strcspn(text, "\n")] = '\0'; // Remove newline
            convertToLowercase(text);
            
            // Tokenize and translate the text
            tokenizeAndTranslate(entries, nEntries, text, srcLang, targetLang, result);
            
            printf("Translated text: %s\n", result);
            
            printf("Translate another text with the same languages? (Y/N): ");
            scanf("%c", &moreTranslations);
            getchar(); // Clear the newline
            
            if (toUpper(moreTranslations) != 'Y') 
            {
                continueTranslation = 0;
            }
        }
    }
    else
    {
        printf("No entries loaded for translation!\n");
    }
}

/* translateTextFile translates text from an input file to an output file
   @param entries - array of Entry structures containing the dictionary data
   @param nEntries - number of entries in the dictionary
   Pre-condition: entries array is properly initialized
*/
void translateTextFile(Entry entries[], int nEntries) 
{
    char srcLang[MAX_WORD_LENGTH] = {0};
    char targetLang[MAX_WORD_LENGTH] = {0};
    char inputFile[MAX_FILENAME] = {0};
    char outputFile[MAX_FILENAME] = {0};
    char line[MAX_TEXT_LENGTH] = {0};
    char result[MAX_TEXT_LENGTH * 2] = {0};
    FILE *inFile = NULL;
    FILE *outFile = NULL;
    int fileOpened = 0;
    int shouldProcessLine = 1;

    if (nEntries > 0) 
    {
        printf("Enter source language: ");
        fgets(srcLang, MAX_WORD_LENGTH, stdin);
        srcLang[strcspn(srcLang, "\n")] = '\0'; // Remove newline
        convertToLowercase(srcLang);
        
        printf("Enter input filename: ");
        fgets(inputFile, MAX_FILENAME, stdin);
        inputFile[strcspn(inputFile, "\n")] = '\0'; // Remove newline
        
        printf("Enter target language: ");
        fgets(targetLang, MAX_WORD_LENGTH, stdin);
        targetLang[strcspn(targetLang, "\n")] = '\0'; // Remove newline
        convertToLowercase(targetLang);
        
        printf("Enter output filename: ");
        fgets(outputFile, MAX_FILENAME, stdin);
        outputFile[strcspn(outputFile, "\n")] = '\0'; // Remove newline
        
        inFile = fopen(inputFile, "r");
        if (inFile != NULL) 
        {
            outFile = fopen(outputFile, "w");
            if (outFile != NULL) 
            {
                fileOpened = 1;
                
                while (fgets(line, MAX_TEXT_LENGTH, inFile) != NULL) 
                {
                    shouldProcessLine = 1;
                    // Remove newline
                    line[strcspn(line, "\n")] = '\0';
                    
                    // Skip empty lines
                    if (strlen(line) == 0) 
                    {
                        fprintf(outFile, "\n");
                        shouldProcessLine = 0;
                    }
                    
                    if (shouldProcessLine)
                    {
                        // Tokenize and translate the text
                        tokenizeAndTranslate(entries, nEntries, line, srcLang, targetLang, result);
                        
                        // Display translation
                        printf("Translated text: %s\n", result);
                        
                        // Write to output file
                        fprintf(outFile, "%s\n", result);
                    }
                }
                
                fclose(inFile);
                fclose(outFile);
                printf("Translation completed and saved to %s!\n", outputFile);
            }
            else
            {
                printf("Error opening output file!\n");
                fclose(inFile);
            }
        }
        else
        {
            printf("Error opening input file!\n");
        }
    }
    else
    {
        printf("No entries loaded for translation!\n");
    }
}

/* displayEntry shows a single entry's language-translation pairs
   @param entry - Entry structure to be displayed
   @param entryNum - number of the entry to be displayed
   Pre-condition: entry is properly initialized
*/
void displayEntry(Entry entry, int entryNum) {
    int i = 0;
    printf("\nEntry %d:\n", entryNum);
    printf("%-15s %s\n", "Language", "Translation");  // Using fixed width of 15 for language column
    for (i = 0; i < entry.count; i++) {
        printf("%-15s %s\n", entry.pairs[i].language, entry.pairs[i].translation);
    }
    printf("Count: %d\n", entry.count);
}

/* sortEntryByLanguage sorts language-translation pairs within an entry alphabetically
   @param entry - pointer to Entry structure to be sorted
   Pre-condition: entry is properly initialized
*/
void sortEntryByLanguage(Entry *entry) {
    int i = 0;
    int j = 0;
    LanguagePair temp = {0};

    for (i = 0; i < entry->count - 1; i++) 
    {
        for (j = 0; j < entry->count - i - 1; j++) {
            if (strcmp(entry->pairs[j].language, entry->pairs[j + 1].language) > 0) {
                // Swap language-translation pairs
                temp = entry->pairs[j];
                entry->pairs[j] = entry->pairs[j + 1];
                entry->pairs[j + 1] = temp;
            }
        }
    }
}

/* sortEntriesByEnglish sorts entries based on their English translations
   @param entries - array of Entry structures to be sorted
   @param nEntries - number of entries in the dictionary
   Pre-condition: entries array is properly initialized
*/
void sortEntriesByEnglish(Entry entries[], int nEntries) {
    int i = 0;
    int j = 0;
    int k = 0;
    char *eng1 = NULL;
    char *eng2 = NULL;
    Entry temp = {0};
    int foundEng1 = 0;
    int foundEng2 = 0;

    for (i = 0; i < nEntries - 1; i++) {
        for (j = 0; j < nEntries - i - 1; j++) {
            // Find English translation for both entries
            eng1 = NULL;
            eng2 = NULL;
            foundEng1 = 0;
            foundEng2 = 0;
            
            for (k = 0; k < entries[j].count && !foundEng1; k++) {
                if (strcmp(entries[j].pairs[k].language, "English") == 0) {
                    eng1 = entries[j].pairs[k].translation;
                    foundEng1 = 1;
                }
            }
            
            for (k = 0; k < entries[j + 1].count && !foundEng2; k++) {
                if (strcmp(entries[j + 1].pairs[k].language, "English") == 0) {
                    eng2 = entries[j + 1].pairs[k].translation;
                    foundEng2 = 1;
                }
            }
            
            // If both have English translations, sort by them
            if (foundEng1 && foundEng2) {
                if (strcmp(eng1, eng2) > 0) {
                    temp = entries[j];
                    entries[j] = entries[j + 1];
                    entries[j + 1] = temp;
                }
            }
            // If only first has English, it should come first
            else if (foundEng1) {
                // Keep current order
            }
            // If only second has English, swap
            else if (foundEng2) {
                temp = entries[j];
                entries[j] = entries[j + 1];
                entries[j + 1] = temp;
            }
        }
    }
}

/* findEntryWithLangTranslation locates an entry containing a specific language-translation pair
   @param entries - array of Entry structures to search through
   @param nEntries - number of entries in the dictionary
   @param language - language to search for
   @param translation - translation to search for
   @return index of the entry if found, -1 if not found
   Pre-condition: entries array is properly initialized
*/
int findEntryWithLangTranslation(Entry entries[], int nEntries, const char *language, const char *translation) {
    int i = 0;
    int j = 0;
    int foundIndex = -1;
    int found = 0;
    
    for (i = 0; i < nEntries && !found; i++) {
        for (j = 0; j < entries[i].count && !found; j++) {
            if (strcmp(entries[i].pairs[j].language, language) == 0 &&
                strcmp(entries[i].pairs[j].translation, translation) == 0) {
                foundIndex = i;
                found = 1;
            }
        }
    }
    
    return foundIndex;
}

/* translateWord translates a single word from source language to target language
   @param entries - array of Entry structures containing the dictionary data
   @param nEntries - number of entries in the dictionary
   @param word - word to be translated
   @param srcLang - source language
   @param targetLang - target language
   @return translated word if found, original word if not found
   Pre-condition: entries array is properly initialized
*/
char* translateWord(Entry entries[], int nEntries, const char *word, const char *srcLang, const char *targetLang) {
    int i = 0;
    int j = 0;
    int k = 0;
    char *translation = (char*)word;
    int found = 0;
    int foundTarget = 0;

    // First try to find the word in source language
    for (i = 0; i < nEntries && !found; i++) {
        for (j = 0; j < entries[i].count && !found; j++) {
            if (strcmp(entries[i].pairs[j].language, srcLang) == 0 &&
                strcmp(entries[i].pairs[j].translation, word) == 0) {
                found = 1;
                // Found the word, now look for target language translation
                for (k = 0; k < entries[i].count && !foundTarget; k++) {
                    if (strcmp(entries[i].pairs[k].language, targetLang) == 0) {
                        translation = entries[i].pairs[k].translation;
                        foundTarget = 1;
                    }
                }
            }
        }
    }
    
    return translation;
}

/* convertToLowercase converts all uppercase letters in a string to lowercase
   @param text - string to be converted
   Pre-condition: text is properly initialized
*/
void convertToLowercase(char *text) {
    int i = 0;
    for (i = 0; text[i] != '\0'; i++) {
        if (text[i] >= 'A' && text[i] <= 'Z') {
            text[i] = text[i] + 32;  // Convert to lowercase by adding 32 to ASCII value
        }
    }
}

/* toUpper converts a single lowercase letter to uppercase
   @param c - character to be converted
   @return uppercase version of the character if it's lowercase, otherwise returns the original character
   Pre-condition: c is a valid character
*/
char toUpper(char c) {
    char result = c;
    if (c >= 'a' && c <= 'z') {
        result = c - 32;  // Convert to uppercase by subtracting 32 from ASCII value
    }
    return result;
}

/* tokenizeAndTranslate translates a text string from source language to target language
   @param entries - array of Entry structures containing the dictionary data
   @param nEntries - number of entries in the dictionary
   @param text - text to be translated
   @param srcLang - source language
   @param targetLang - target language
   @param result - string where the translated text will be stored
   Pre-condition: entries array is properly initialized and result has sufficient space
*/
void tokenizeAndTranslate(Entry entries[], int nEntries, char *text, const char *srcLang, const char *targetLang, char *result) {
    char *token = NULL;
    result[0] = '\0'; // Initialize result string
    
    token = strtok(text, " ");
    while (token != NULL) {
        // Remove punctuation
        removePunctuation(token);
        
        // Convert word to lowercase before translation
        convertToLowercase(token);
        
        // Translate the word
        char *translation = translateWord(entries, nEntries, token, srcLang, targetLang);
        
        // Append to result with space
        if (strlen(result) > 0) {
            strcat(result, " ");
        }
        strcat(result, translation);
        
        token = strtok(NULL, " ");
    }
}

/* clearEntries resets the number of entries in the dictionary to zero
   @param entries - array of Entry structures to be cleared
   @param nEntries - pointer to the number of entries, will be set to 0
   Pre-condition: entries array is properly initialized
*/
void clearEntries(Entry entries[], int *nEntries) {
    *nEntries = 0;
}

/* displayEntryPaginated displays entries with pagination controls
   @param entries - array of Entry structures to display
   @param nEntries - number of entries in the dictionary
   @param currentIndex - pointer to current entry index
   @param running - pointer to control variable for the display loop
   Pre-condition: entries array is properly initialized
*/
void displayEntryPaginated(Entry entries[], int nEntries, int *currentIndex, int *running) {
    char choice = '\0';
    int shouldContinue = 1;
    
    if (nEntries == 0) {
        printf("No entries to display!\n");
        *running = 0;
        shouldContinue = 0;
    }
    
    while (*running && shouldContinue) {
        // Display current entry
        displayEntry(entries[*currentIndex], *currentIndex + 1);
        
        // Ask for next action
        printf("\nOptions:\n");
        printf("N - Next entry\n");
        printf("P - Previous entry\n");
        printf("X - Exit display\n");
        printf("Enter choice: ");
        
        scanf("%c", &choice);
        getchar(); // Clear the newline
        
        switch (toUpper(choice)) {
            case 'N':
                if (*currentIndex < nEntries - 1) {
                    (*currentIndex)++;
                } else {
                    printf("Already at the last entry!\n");
                }
                break;
                
            case 'P':
                if (*currentIndex > 0) {
                    (*currentIndex)--;
                } else {
                    printf("Already at the first entry!\n");
                }
                break;
                
            case 'X':
                *running = 0;
                shouldContinue = 0;
                break;
                
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }
}


/* exportFlashcards exports dictionary entries in a visually pleasing flashcard format
   @param entries - array of Entry structures containing the dictionary data
   @param nEntries - number of entries in the dictionary
   Pre-condition: entries array is properly initialized
*/
void exportFlashcards(Entry entries[], int nEntries) {
    char filename[MAX_FILENAME];
    FILE *file = NULL;
    int i, j;
    Entry sortedEntries[MAX_ENTRIES];
    int fileOpened = 0;

    if (nEntries > 0) {
        printf("Enter filename to export flashcards to: ");
        fgets(filename, MAX_FILENAME, stdin);
        filename[strcspn(filename, "\n")] = '\0'; // Remove newline
        
        file = fopen(filename, "w");
        if (file != NULL) {
            fileOpened = 1;
            
            // Sort entries for export
            memcpy(sortedEntries, entries, sizeof(Entry) * nEntries);
            
            // Sort each entry internally by language
            for (i = 0; i < nEntries; i++) {
                sortEntryByLanguage(&sortedEntries[i]);
            }
            
            // Write header
            fprintf(file, "==========================================\n");
            fprintf(file, "           LANGUAGE FLASHCARDS\n");
            fprintf(file, "==========================================\n\n");
            
            // Write entries in flashcard format
            for (i = 0; i < nEntries; i++) {
                // Write front of flashcard (using first language as front)
                fprintf(file, "------------------------------------------\n");
                fprintf(file, "FRONT:\n");
                fprintf(file, "------------------------------------------\n");
                fprintf(file, "%s: %s\n", 
                    sortedEntries[i].pairs[0].language, 
                    sortedEntries[i].pairs[0].translation);
                fprintf(file, "------------------------------------------\n");
                
                // Write back of flashcard (all other translations)
                fprintf(file, "BACK:\n");
                fprintf(file, "------------------------------------------\n");
                for (j = 1; j < sortedEntries[i].count; j++) {
                    fprintf(file, "%s: %s\n", 
                        sortedEntries[i].pairs[j].language, 
                        sortedEntries[i].pairs[j].translation);
                }
                fprintf(file, "------------------------------------------\n\n");
            }
            
            // Write footer
            fprintf(file, "==========================================\n");
            fprintf(file, "           END OF FLASHCARDS\n");
            fprintf(file, "==========================================\n");
            
            fclose(file);
            printf("Flashcards exported successfully to %s!\n", filename);
        } else {
            printf("Error opening file for writing!\n");
        }
    } else {
        printf("No entries exist to export as flashcards!\n");
    }
}

/* viewFlashcards displays dictionary entries in an interactive flashcard format
   @param entries - array of Entry structures containing the dictionary data
   @param nEntries - number of entries in the dictionary
   Pre-condition: entries array is properly initialized
*/
void viewFlashcards(Entry entries[], int nEntries) {
    int currentIndex = 0;
    int showBack = 0;
    char choice;
    Entry sortedEntries[MAX_ENTRIES];
    int i;

    if (nEntries > 0) {
        // Sort entries for display
        memcpy(sortedEntries, entries, sizeof(Entry) * nEntries);
        
        // Sort each entry internally by language
        for (i = 0; i < nEntries; i++) {
            sortEntryByLanguage(&sortedEntries[i]);
        }
        
        printf("\n===== INTERACTIVE FLASHCARDS =====\n");
        printf("Commands (case-insensitive):\n");
        printf("N or n - Next card\n");
        printf("P or p - Previous card\n");
        printf("F or f - Flip the card\n");
        printf("X or x - Exit\n\n");
        
        do {
            // Display current card number
            printf("\nCard %d of %d\n", currentIndex + 1, nEntries);
            
            // Display front of card
            printf("\n------------------------------------------\n");
            printf("FRONT:\n");
            printf("------------------------------------------\n");
            printf("%s: %s\n", 
                sortedEntries[currentIndex].pairs[0].language, 
                sortedEntries[currentIndex].pairs[0].translation);
            printf("------------------------------------------\n");
            
            if (showBack) {
                // Display back of card
                printf("\nBACK:\n");
                printf("------------------------------------------\n");
                for (i = 1; i < sortedEntries[currentIndex].count; i++) {
                    printf("%s: %s\n", 
                        sortedEntries[currentIndex].pairs[i].language, 
                        sortedEntries[currentIndex].pairs[i].translation);
                }
                printf("------------------------------------------\n");
            }
            
            printf("\nEnter choice (N/P/F/X): ");
            scanf("%c", &choice);
            getchar(); // Clear the newline
            
            // Convert choice to uppercase for case-insensitive comparison
            choice = toUpper(choice);
            
            switch (choice) {
                case 'N':
                    if (currentIndex < nEntries - 1) {
                        currentIndex++;
                        showBack = 0; // Reset to front when moving to next card
                    } else {
                        printf("Already at the last card!\n");
                        getchar(); // Wait for user to press enter
                    }
                    break;
                    
                case 'P':
                    if (currentIndex > 0) {
                        currentIndex--;
                        showBack = 0; // Reset to front when moving to previous card
                    } else {
                        printf("Already at the first card!\n");
                        getchar(); // Wait for user to press enter
                    }
                    break;
                    
                case 'F':
                    showBack = !showBack; // Toggle between front and back
                    break;
                    
                case 'X':
                    printf("Exiting flashcard view...\n");
                    break;
                    
                default:
                    printf("Invalid choice! Please try again.\n");
                    getchar(); // Wait for user to press enter
            }
        } while (choice != 'X');
    } else {
        printf("No entries exist to display as flashcards!\n");
    }
}

/* removePunctuation removes punctuation from a word while preserving letters and numbers
   @param word - word to remove punctuation from
   Pre-condition: word is properly initialized
*/
void removePunctuation(char *word) {
    int i = 0;
    int j = 0;
    int len = strlen(word);

    for (i = 0, j = 0; i < len; i++) {
        // Check if character is NOT a punctuation mark
        if ((word[i] >= 'A' && word[i] <= 'Z') ||     // Uppercase letters
            (word[i] >= 'a' && word[i] <= 'z') ||     // Lowercase letters
            (word[i] >= '0' && word[i] <= '9') ||     // Numbers
            (word[i] == ' ')) {                       // Space
            word[j++] = word[i];
        }
    }
    word[j] = '\0';  // Null terminate the modified string
}