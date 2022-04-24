/**
 * @name Projekt 1 - Práce s textem
 * @author Petr Bartoš <xbarto0g@stud.fit.vutbr.cz>
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define PASS_MAX_LENGTH 102

// Each character in the ASCII table will have it's own element in distinctCharacters array
// after reading a character 1 is assigned to character's corresponding element
struct passStats
{
    int length;
    int distinctCharacters[128];
    int min;
    int passCount;
};

int getArgumentValue(const char str[])
{
    long argument;
    char *argNonNum;
    argument = strtol(str, &argNonNum, 10);

    // If there was a nonnumerical character, raise an error and return
    if (!(argNonNum && !argNonNum[0]))
    {
        fprintf(stderr, "Arguments weren't in correct format\n");
        return -1;
    }

    // If the argument is larger than INT limit, return INT_MAX since the program will
    // treat arguments larger than 100 the same way
    if (argument > INT_MAX)
        return INT_MAX;

    return argument;
}

int isAlpha(char character)
{
    return (character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z');
}

int compareStrings(const char *str1, char *str2)
{
    while (*str1 != '\0' && *str2 != '\0')
    {
        if (*str1 != *str2)
        {
            return 1;
        }
        str1++;
        str2++;
    }
    if (*str1 != '\0' || *str2 != '\0')
    {
        return 1;
    }
    return 0;
}

int checkCapitalAndLower(char pass[])
{
    int i = 0, capital = 0, lower = 0;
    while (pass[i] != '\0')
    {
        if (pass[i] >= 'A' && pass[i] <= 'Z')
        {
            capital = 1;
        }
        if (pass[i] >= 'a' && pass[i] <= 'z')
        {
            lower = 1;
        }
        i++;
    }
    return (capital && lower);
}

int checkCharacterGroup(char pass[], int param)
{
    int i = 0, num = 0, special = 0;
    while (pass[i] != '\0')
    {
        if (pass[i] >= '0' && pass[i] <= '9')
        {
            num = 1;
        }
        else if (pass[i] > 31 && pass[i] < 127 && !(isAlpha(pass[i])))
        {
            special = 1;
        }
        i++;
    }

    // Since both capital and lowercase characters will be checked in
    // checkCapitalAndLower function, we can assume that the password contains them
    param = (param > 4) ? 4 : param;
    return (2 + num + special >= param);
}

int checkRepeatingSequence(char pass[], int param)
{
    int i = 0;
    while (pass[i] != '\0')
    {
        int j = 0;
        while (pass[i + j] == pass[i])
        {
            j++;
            if (j >= param)
                return 0;
        }
        i++;
    }
    return 1;
}

int checkRepeatingSubstring(char pass[], int param)
{
    int i = 0;
    while (pass[i] != '\0')
    {
        int j = 1;
        while (pass[i + j] != '\0')
        {
            int common = 0;
            while (pass[i + common] == pass[i + j + common])
            {
                common++;
                if (common >= param)
                    return 0;
            }
            j++;
        }
        i++;
    }
    return 1;
}

int checkPasswordValidity(char pass[], int level, int param, struct passStats *statsStruct)
{
    int valid = 0, passLength = 0;
    int chars[128] = {0};


    for (int i = 0; i < PASS_MAX_LENGTH; i++)
    {
        if (pass[i] == '\n')
        {
            valid = 1;
            break;
        }
        chars[(int)pass[i]] = 1;
        passLength++;
    }

    // If the password length exceeds PASS_MAX_LENGTH, read characters until newline
    if (!valid)
    {
        int ch;
        while ((ch = fgetc(stdin)) != '\n')
        {
            passLength++;
        }
        fprintf(stderr, "The password is too long\n");
        return 1;
    }

    // Check if the password meets criteria of each level
    int passed = 1;
    switch (level)
    {
    case 4:
        passed *= checkRepeatingSubstring(pass, param);
        // fall through
    case 3:
        passed *= checkRepeatingSequence(pass, param);
        // fall through
    case 2:
        passed *= checkCharacterGroup(pass, param);
        // fall through
    case 1:
        passed *= checkCapitalAndLower(pass);
        break;
    }
    if (passed)
    {
        printf("%s", pass);
    }

    // Adjust stats variable in main() based on current password's stats
    if (statsStruct->min > passLength)
        statsStruct->min = passLength;
    statsStruct->length += passLength;
    statsStruct->passCount += 1;
    for (int i = 0; i < 128; i++)
    {
        statsStruct->distinctCharacters[i] = statsStruct->distinctCharacters[i] || chars[i];
    }

    return 0;
}

int main(int argc, char const *argv[])
{
    struct passStats stats = {.length = 0, .distinctCharacters = {0}, .min = 100, .passCount = 0};

    // Check if arguments meet the criteria
    int checkStats = 0;
    if (argc < 3)
    {
        fprintf(stderr, "Not enough input arguments\n");
        return 1;
    }

    if (argc > 3 && (checkStats = compareStrings(argv[3], "--stats")) == 1)
    {
        fprintf(stderr, "Arguments weren't in correct format\n");
        return 1;
    }

    if (argc > 4)
    {
        fprintf(stderr, "Too many arguments provided\n");
        return 1;
    }

    int i = 0;

    int level, param;
    if ((level = getArgumentValue(argv[1])) == -1 || (param = getArgumentValue(argv[2])) == -1)
    {
        return 1;
    }

    if (level < 1 || param < 1 || level > 4)
    {
        fprintf(stderr, "Arguments weren't in correct range\n");
        return 1;
    }

    // Read stdin password by password and pass them to funciton that validates them
    char pass[PASS_MAX_LENGTH];
    while (fgets(pass, sizeof(pass), stdin) != NULL)
    {
        if (checkPasswordValidity(pass, level, param, &stats) == 1)
            return 1;
    }

    int uniqueChars = 0;
    for (i = 0; i < 128; i++)
    {
        if (stats.distinctCharacters[i] == 1)
            uniqueChars++;
    }

    if (checkStats == 0 && argc > 3)
    {
        printf("Statistika:\nRuznych znaku: %d\nMinimalni delka: %d\nPrumerna delka: %.1f", uniqueChars, stats.min, ((float)stats.length / stats.passCount));
    }
    return 0;
}
