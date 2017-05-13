/****************************************************************************************
 Includes
*****************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stddef.h>

/****************************************************************************************
 Algorithm

 Definitions:
 Group =    A number of consecutive free stalls, in the beginning there is only one group.
 Layer =    A new layer is started when all groups of the previous layer are split up.
            Each layer holds therefore 2^(layer - 1) customers
            e.g.    1st layer: 1 customer
                    2nd layer: 2 customers
                    3rd layer: 4 customers
                    .
                    nth layer: 2^(n-1) customers

 With the above definitions it can be calculated how many layers are necessary.
     (Eq. 1) lastLayer = ceil(log(numberCustomers)/log(2))

 To calculate the size of the group, the last customer will be assigned to, the size of the
 groups in the last layer must be calculated. To do so, we need the following:
 - number of stalls in the last layer
     (Eq. 2) custPrevLayers = 2^(lastLayer - 1) - 1
     (Eq. 3) stallsLastLayer = totalStalls - custPrevLayers
 - the number of groups
     (Eq. 4) nbrGroupsLastLayer = 2^(lastLayer - 1)
 - and the number of customers in the last layer
     (Eq. 5) custLastLayer = totalCustomers - custPrevLayers

 Thus, the average group size is
     (Eq. 6) avgGroupSizeLastLayer = stallsLastLayer / nbrGroupsLastLayer

 Since avgGroupSizeLastLayer will in most cases not be an integer, we end up with two different
 group sizes:
     (Eq. 7) largeGroupSize = ceil(avgGroupSizeLastLayer)
     (Eq. 8) smallGroupSize = largeGroupSize - 1

 The last step is to figure out how many large groups are present in the last layer. This can
 be calculated by means of (Eq. 9) and (Eq. 10):
     (Eq. 9)  stallsLastLayer    = largeGroupSize * nbrLargeGroups + smallGroupSize * nbrSmallGroups
     (Eq. 10) nbrGroupsLastLayer = nbrLargeGroups + nbrSmallGroups

     Solving (Eq. 9) for nbrSmallGroups and inserting it into (Eq. 10) leaves us with
     (Eq. 11) nbrLargeGroups = (stallsLastLayer - (nbrGroupsLastLayer * smallGroupSize))/(largeGroupSize - smallGroupSize);

 If custLastLayer is smaller than the nbrLargeGroups min and max is to be calculated
 with the largeGroupSize, otherwise with the smallGroupSize.

*****************************************************************************************/


/****************************************************************************************
 Defines
*****************************************************************************************/
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#define MAX_LINE_LENGTH (80)

/****************************************************************************************
 Structs and typedefs
*****************************************************************************************/
typedef unsigned long long uint64;

typedef struct TestCase_s
{
    uint64 stalls;
    uint64 customers;
    uint64 max;
    uint64 min;
} TestCase_t;

/****************************************************************************************
 Macros
*****************************************************************************************/

/****************************************************************************************
 Forward declarations
*****************************************************************************************/
void   getTestCases(FILE *fp, TestCase_t testCases[], int nbrOfTestcases);
int    getNumberOfTestCases(FILE *fp);
uint64 getNumberOfStalls(char line[]);
uint64 getNumberOfCustomers(char line[]);
int    getLine(char line[], size_t buflen, FILE *stream);

uint64 getNumberOfLargeGroups(uint64 sizeLargeGroup, uint64 sizeSmallGroup, uint64 stallsFree, uint64 totalGroups);
uint64 getLayers(uint64 customers);
void   calcStallsLeftRight(uint64 stalls, uint64 *stallsLeft, uint64 *stallsRight);

void outputToFile(TestCase_t testCases[], int entries, char *outputFile);
void checkOutputFile(char *output, char *correctOutput);
void printOutput(TestCase_t testCases[], int entries);

/****************************************************************************************
 Local data
*****************************************************************************************/
int main (void)
{
    FILE *fp;
    int nbrTestCases = 0;

    //char input[] = "Debug/input.txt";

    //char input[] = "Debug/C-small-practice-1.in";
    //char correctOutput[] = "Debug/CorrectOutputSmallPractice1.txt";

    //char input[] = "Debug/C-small-practice-2.in";
    //char correctOutput[] = "Debug/CorrectOutputSmallPractice2.txt";

    char input[] = "Debug/C-large-practice.in";
    char correctOutput[] = "Debug/CorrectOutputLargePractice.txt";

    char output[] = "Debug/output.txt";

    fp = fopen(input, "r");

    if (fp == NULL)
    {
       perror("Error while opening the file.\n");
       exit(EXIT_FAILURE);
    }

    nbrTestCases = getNumberOfTestCases(fp);

    TestCase_t *testCases = calloc(nbrTestCases, sizeof(*testCases));

    getTestCases(fp, testCases, nbrTestCases);

    fclose(fp);

    int currentCase = 0;

    while (currentCase < nbrTestCases)
    {
        uint64 stallsToLeft;
        uint64 stallsToRight;
        TestCase_t *curTC = &testCases[currentCase];

        uint64 lastLayer = getLayers(curTC->customers);
        uint64 custPrevLayers = pow(2, lastLayer - 1) - 1;
        uint64 stallsLastLayer = curTC->stalls - custPrevLayers;
        uint64 nbrGroupsLastLayer = pow(2, lastLayer - 1);
        uint64 sizeLargeGroup = stallsLastLayer/nbrGroupsLastLayer + (stallsLastLayer % nbrGroupsLastLayer != 0); // ciel

        if (sizeLargeGroup == 1)
        {
            stallsToLeft = 0;
            stallsToRight = 0;
        }
        else
        {
            uint64 customersLastLayer = curTC->customers - custPrevLayers;
            uint64 sizeSmallGroup = sizeLargeGroup - 1; // size of small group is always one less than number of big group
            uint64 largeGroups = getNumberOfLargeGroups(sizeLargeGroup, sizeSmallGroup, stallsLastLayer, nbrGroupsLastLayer);

            if (largeGroups >= customersLastLayer)
            {
                calcStallsLeftRight(sizeLargeGroup, &stallsToLeft, &stallsToRight);
            }
            else
            {
                calcStallsLeftRight(sizeSmallGroup, &stallsToLeft, &stallsToRight);
            }
        }

        curTC->max = MAX(stallsToLeft, stallsToRight);
        curTC->min = MIN(stallsToLeft, stallsToRight);

        currentCase++;
    }

    printOutput(testCases, nbrTestCases);
    outputToFile(testCases, nbrTestCases, output);
    checkOutputFile(output, correctOutput);

    return 0;
}

/*
 * Calculates the number of layers needed for a number of customers.
 */
uint64 getLayers(uint64 customers)
{
    uint64 layers = 1;
    uint64 possibleCustomers = 1;

    while (customers > possibleCustomers)
    {
        layers++;
        possibleCustomers = possibleCustomers << 1;
        possibleCustomers = possibleCustomers | 1;
    }

    return layers;
}

/*
 * Searching the number of large groups
 */
uint64 getNumberOfLargeGroups(uint64 sizeLargeGroup, uint64 sizeSmallGroup, uint64 stallsFree, uint64 totalGroups)
{
    return (stallsFree - (totalGroups * sizeSmallGroup))/(sizeLargeGroup - sizeSmallGroup);
}

/**
 * Calculates the stalls the max number of stalls to the left and right of the chosen stall.
 * If stalls is an even number the stall to the left is chosen to base the calculation on.
 * @param stalls
 * @param stallsLeft
 * @param stallsRight
 */
void calcStallsLeftRight(uint64 stalls, uint64 *stallsLeft, uint64 *stallsRight)
{
    *stallsLeft = (stalls - 1)/2;
    *stallsRight = stalls/2;
}

/**
 * Reads test cases from file and fills them into the preallocated memory
 */
void getTestCases(FILE *fp, TestCase_t testCases[], int nbrOfTestcases)
{
    char *line = malloc(MAX_LINE_LENGTH);
    int i = 0;

    while((getLine(line, MAX_LINE_LENGTH, fp) != -1) && (i < nbrOfTestcases))
    {
        testCases[i].stalls = getNumberOfStalls(line);
        testCases[i].customers = getNumberOfCustomers(line);

        i++;
        line = malloc(MAX_LINE_LENGTH);
    }
    free(line);
}

/**
 * Number of stalls is the first number in a line
 */
uint64 getNumberOfStalls(char line[])
{
    return atoll(line);
}

/**
 * Number of customers is the second number in a line
 */
uint64 getNumberOfCustomers(char line[])
{
    int i = 0;
    while(line[i] != ' ')
    {
        i++;
    }
    i++;

    return atoll(&line[i]);
}

/*
 * Number of test cases is the first line in a file.
 */
int getNumberOfTestCases(FILE *fp)
{
    char *line = malloc(MAX_LINE_LENGTH);

    getLine(line, MAX_LINE_LENGTH, fp);

    return atoi(line);
}

int getLine(char line[], size_t buflen, FILE *stream)
{
    int i = 0;
    char c = getc(stream);

    if (c == EOF)
    {
        return EOF;
    }

    while(c != EOF && c != '\n' && i < buflen)
    {
       line[i++] = c;
       c = getc(stream);
    }

    line[i] = '\0';

    return 0;
}

/*
 * Writes the test results to an output file
 */
void outputToFile(TestCase_t testCases[], int entries, char *outputFile)
{
    FILE *fp;
    fp = fopen(outputFile,"wb"); // read mode

    if (fp == NULL)
    {
       perror("Error while opening the file.\n");
       exit(EXIT_FAILURE);
    }

    for (int i = 0; i < entries; i++)
    {
        fprintf(fp, "Case #%d: %I64d %I64d\n", i + 1, testCases[i].max, testCases[i].min);
    }

    fclose(fp);
}

void printOutput(TestCase_t testCases[], int entries)
{
    int i;

    for (i = 0; i < entries; i++)
    {
        printf("Case #%d: %I64d,\t%I64d,\t%I64d,\t%I64d\n", i + 1, testCases[i].stalls , testCases[i].customers, testCases[i].max, testCases[i].min);
    }
}

void checkOutputFile(char *output, char *correctOutput)
{
    char ch1, ch2;
    int cnt = 0;
    FILE *fp1, *fp2;
    fp1 = fopen(output, "r");
    fp2 = fopen(correctOutput, "r");

    if (fp1 == NULL)
    {
       printf("Cannot open %s for reading ", output);
       exit(1);
    }
    else if (fp2 == NULL)
    {
       printf("Cannot open %s for reading ", correctOutput);
       exit(1);
    }
    else
    {
       ch1 = getc(fp1);
       ch2 = getc(fp2);

       while ((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2))
       {
          ch1 = getc(fp1);
          ch2 = getc(fp2);
          cnt++;
       }

       if (ch1 == ch2)
       {
           printf("Files are identical \n");
       }

       else if (ch1 != ch2)
       {
           printf("Files are Not identical %c, %d\n", ch1, cnt);
       }

       fclose(fp1);
       fclose(fp2);
    }
}





