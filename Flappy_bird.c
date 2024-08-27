#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

// Constants for game settings
#define xSize       32
#define ySize       16
#define pipeCount   3
#define qKey        0x51 // Virtual key code for 'Q'
#define GREEN       "\e[32m" // ANSI escape code for green text
#define YELLOW      "\e[33m" // ANSI escape code for yellow text
#define RED         "\e[31m" // ANSI escape code for red text
#define CYAN        "\e[36m" // ANSI escape code for cyan text
#define MAGENTA     "\e[35m" // ANSI escape code for magenta text
#define NC          "\e[0m"  // ANSI escape code for default text color

// Structure to represent a pixel's position
typedef struct
{
    int x;
    int y;
} PIX;

// Global variables
PIX bird;
PIX pipes[3];
int score = 0;
int highestScore = 0; // Variable to store the highest score

// Function prototypes
void Draw();
void Pipes();
int HitTest();
void GameOver();
void LoadHighestScore();
void UpdateHighestScore();
void ShowHighestScore();
void ShowMenu();
void ShowAboutGame();
void PauseGame();
void ResetHighestScore();
void ShowInstructions();

// Function to load the highest score from the file
void LoadHighestScore()
{
    FILE *file = fopen("scores.txt", "a+");

    // Check if the file exists; if not, create it with a default score
    if (file == NULL)
    {
        perror(RED"Error opening file"NC);
        file = fopen("scores.txt", "w");
        if (file == NULL)
        {
            perror(RED"Error creating file"NC);
            exit(1);
        }
        fprintf(file, "Player: %s\nScore: %d\n", "Default", highestScore);
        fclose(file);
        return;
    }

    char playerName[50];
    int playerScore;
    while (fscanf(file, "Player: %s\nScore: %d\n", playerName, &playerScore) != EOF)
    {
        if (playerScore > highestScore)
        {
            highestScore = playerScore;
        }
    }

    fclose(file);
}

// Function to update the highest score
void UpdateHighestScore()
{
    if (score > highestScore)
    {
        highestScore = score;
    }
}

// Function to display the highest score
void ShowHighestScore()
{
    FILE *file = fopen("scores.txt", "r+");
    if (file == NULL)
    {
        perror(RED"Error opening file"NC);
        return;
    }

    printf(CYAN "\n\n\n\n======== Highest Score ========\n\n" NC);

    char playerName[50];
    int playerScore;

    while (fscanf(file, "Player: %s\nScore: %d\n", playerName, &playerScore) != EOF)
    {
        if (playerScore == highestScore)
        {
            printf(RED "%s: %d\n" NC, playerName, playerScore);
        }
        else
        {
            printf("%s: %d\n", playerName, playerScore);
        }
    }
    printf(CYAN "\n===============================\n\n\n\n" NC);
    fclose(file);
}

// Function to display the main menu
void ShowMenu()
{
    printf("\n" YELLOW "====== Flappy Bird Menu ======\n" NC);
    printf("1. " CYAN "Start Game\n" NC);
    printf("2. " CYAN "View Highest Score\n" NC);
    printf("3. " MAGENTA "Reset Highest Score\n" NC);
    printf("4. " CYAN "Instructions\n" NC);
    printf("5. " CYAN "About Game\n" NC);
    printf("6. " RED "Quit\n" NC);
    printf(YELLOW "=============================\n" NC);
    printf("Enter your choice: ");
}

// Function to display information about the game
void ShowAboutGame()
{
    printf(CYAN "\n\n\n\n           ========= About Flappy Bird =========\n\n" NC);
    printf(CYAN "Flappy Bird is a simple and addictive game where you control a bird\n");
    printf(CYAN "and navigate it through a series of pipes. Your objective is to survive\n");
    printf(CYAN "as long as possible without hitting the pipes or the ground. The longer\n");
    printf(CYAN "you survive, the higher your score becomes. Challenge yourself to beat\n");
    printf(CYAN "your own highest score and enjoy the classic gameplay of Flappy Bird!\n");
    printf(CYAN "\n\n            =========== Developed By ===========\n");
    printf(CYAN "\n      |------------------------------------------|\n");
    printf(CYAN "                      Rakib Hossan           \n");
    printf(CYAN "      |------------------------------------------|\n");
    printf(CYAN "      |------------------------------------------|\n");
    printf(CYAN "                      Abdul Halim          \n");
    printf(CYAN "      |------------------------------------------|\n");
    printf(CYAN "      |------------------------------------------|\n");
    printf(CYAN "                      Kaniz Fatema          \n");
    printf(CYAN "      |------------------------------------------|\n");
    printf(CYAN "      |------------------------------------------|\n");
    printf(CYAN "                      Santo Mitro           \n");
    printf(CYAN "      |------------------------------------------|\n");
    printf(CYAN "      |------------------------------------------|\n");
    printf(CYAN "                     Monisa Biswas           \n");
    printf(CYAN "      |------------------------------------------|\n");

    printf(CYAN "      |------------------------------------------|\n");
    printf(CYAN "                  Fatema Akter Jhorna          \n");
    printf(CYAN "      |------------------------------------------|\n");
    printf(CYAN "\n         ======================================\n\n\n\n" NC);
}

// Function to draw the game screen
void Draw()
{
    char buff[5000];
    char scoreStr[50];
    sprintf(scoreStr, "Score: %d", score);
    sprintf(buff, "\e[17A");
    strcat(buff, "\e[s");
    strcat(buff, "\e[1;32;40m");
    strcat(buff, "\e[1;H");
    strcat(buff, scoreStr);
    strcat(buff, "\e[0m");
    strcat(buff, "\e[u");

    for (int y = 0; y <= ySize; y++)
    {
        for (int x = 0; x <= xSize; x++)
        {
            if (y == 0 || y == ySize || x == 0 || x == xSize)
            {
                strcat(buff,CYAN"[]"NC);
                continue;
            }

            for (int i = 0; i < pipeCount; i++)
            {
                if (pipes[i].x >= x - 1 && pipes[i].x <= x + 1 && (pipes[i].y == y + 3 || pipes[i].y == y - 3))
                {
                    strcat(buff, GREEN "[]");
                    goto bottom;
                }
                else if (pipes[i].x == x - 1 && pipes[i].y == y - 4)
                {
                    strcat(buff, GREEN "]/");
                    goto bottom;
                }
                else if (pipes[i].x == x && (pipes[i].y <= y - 4 || pipes[i].y >= y + 4))
                {
                    strcat(buff, MAGENTA "][");
                    goto bottom;
                }
                else if (pipes[i].x == x + 1 && pipes[i].y == y - 4)
                {
                    strcat(buff, GREEN "\\[");
                    goto bottom;
                }
                else if (pipes[i].x == x - 1 && pipes[i].y == y + 4)
                {
                    strcat(buff, GREEN "]\\");
                    goto bottom;
                }
                else if (pipes[i].x == x + 1 && pipes[i].y == y + 4)
                {
                    strcat(buff, GREEN "/[");
                    goto bottom;
                }
                else if (pipes[i].x == x + 1 && (pipes[i].y <= y - 5 || pipes[i].y >= y + 5))
                {
                    strcat(buff, GREEN " [");
                    goto bottom;
                }
                else if (pipes[i].x == x - 1 && (pipes[i].y <= y - 5 || pipes[i].y >= y + 5))
                {
                    strcat(buff, GREEN "] ");
                    goto bottom;
                }
            }

            if (bird.y == y && bird.x == x)
            {
                strcat(buff, YELLOW ")>");
            }
            else if (bird.y == y && bird.x == x + 1)
            {
                strcat(buff, YELLOW "_(");
            }
            else if (bird.y == y && bird.x == x + 2)
            {
                strcat(buff, YELLOW " _");
            }
            else if (bird.y == y - 1 && bird.x == x)
            {
                strcat(buff, YELLOW ") ");
            }
            else if (bird.y == y - 1 && bird.x == x + 1)
            {
                strcat(buff, YELLOW "__");
            }
            else if (bird.y == y - 1 && bird.x == x + 2)
            {
                strcat(buff, YELLOW " \\");
            }
            else
            {
                strcat(buff, NC "  ");
            }

bottom:
            ;
        }

        strcat(buff, "\n");
    }

    printf(buff);
}

// Function to handle pipe movement and generation
void Pipes()
{
    for (int i = 0; i < pipeCount; i++)
    {
        if (pipes[i].x == -1)
        {
            (i == 0) ? (pipes[i].x = pipes[2].x + 15) : (pipes[i].x = pipes[i - 1].x + 15);
            pipes[i].y = (rand() % 7) + 5;
        }
    }
}

// Function to check for collisions and determine the game state
int HitTest()
{
    if (bird.y == 15 || bird.y == 0)
    {
        return 1; // Bird hit the floor or ceiling
    }

    for (int i = 0; i < pipeCount; i++)
    {
        if ((bird.x - 2 < pipes[i].x + 2) && (bird.x > pipes[i].x - 2) && ((bird.y < pipes[i].y - 2) || (bird.y > pipes[i].y + 1)))
        {
            return 2; // Bird hit a pipe
        }
    }

    return 0; // Bird is still alive
}

// Function to handle game over state and save the score
void GameOver()
{
    UpdateHighestScore();
    printf(RED "Game Over! Your Score: %d\n" NC, score);
    printf(YELLOW"Enter your name: "NC);
    char playerName[50];
    scanf("%s", playerName);
    printf("\n\n\n ");

    // Write the score to a file
    FILE *file = fopen("scores.txt", "a+");
    if (file == NULL)
    {
        perror(RED"Error opening file"NC);
        exit(1);
    }
    else
    {
        fprintf(file, "Player: %s\nScore: %d\n", playerName, score);
        fclose(file);
    }
}

// Function to pause the game
void PauseGame()
{
    printf(YELLOW"Game Paused. Press any key to resume...\n"NC);
    system("pause>nul");
}

// Function to reset the highest score
void ResetHighestScore()
{
    highestScore = 0;
    FILE *file = fopen("scores.txt", "w");
    if (file == NULL)
    {
        perror(RED"Error opening file"NC);
        exit(1);
    }
    else
    {
        fprintf(file, "Player: %s\nScore: %d\n", "Default", highestScore);
        fclose(file);
    }

    printf(RED"\n\n Score has been reset to 0.\n\n"NC);
}

// Function to display game instructions
void ShowInstructions()
{
    printf(GREEN "\n\n\n\n=========== Flappy Bird Instructions ===========\n\n" NC);
    printf(GREEN "1. Press " YELLOW "UP" GREEN " to make the bird jump.\n" NC);
    printf(GREEN "2. Avoid the pipes and survive as long as possible.\n" NC);
    printf(GREEN "3. Press " YELLOW "P" GREEN " to pause the game.\n" NC);
    printf(GREEN "4. Press " RED "Q" GREEN " to quit the game.\n" NC);
    printf(GREEN "5. Have fun and aim for the highest score!\n" NC);
    printf(GREEN "\n================================================\n\n\n\n" NC);
}

// Main function

int main()
{
    srand(time(NULL)); // Seed the random number generator
    system("title \"Flappy Bird\""); // Set the console title

    int choice;

    do
    {
        ShowMenu();

        // Validate user input to ensure it is an integer
        if (scanf("%d", &choice) != 1)
        {
            printf(RED "\nInvalid input. Please enter a number.\n" NC);
            // Clear the input buffer in case of invalid input
            while (getchar() != '\n');
            continue;
        }

        switch (choice)
        {
        case 1:
            system("cls");
            bird.x = 10;
            bird.y = 10;

            for (int i = 0; i < pipeCount; i++)
            {
                pipes[i].x = 25 + 15 * i;
                pipes[i].y = (rand() % 7) + 5;
            }

            LoadHighestScore(); // Load the highest score from the file

            int frame = 0;
            int gameState = 0;

            // Reset the score to 0 at the start of each game
            score = 0;

            printf("\nPress " YELLOW "UP" GREEN " to jump, " YELLOW "P" GREEN " to pause, and " RED "Q" GREEN " to quit.\n");

            for (int i = 0; i <= ySize; i++)
            {
                printf("\n");
            }

            Draw();

            while (1)
            {
                if (GetAsyncKeyState(VK_UP))
                {
                    bird.y -= 2;
                }

                else if (GetAsyncKeyState(0x50)) // 0x50 is the virtual key code for 'P'
                {
                    PauseGame();

                }

                if (GetAsyncKeyState(qKey))
                {
                    choice = 6; // Quit the game
                    break;
                }

                if (frame == 2)
                {
                    bird.y++;

                    for (int i = 0; i < 3; i++)
                    {
                        pipes[i].x--;
                    }

                    frame = 0;
                }

                gameState = HitTest();
                Draw();
                Pipes();

                if (gameState != 0)
                {
                    GameOver();
                    break;
                }

                frame++;
                Sleep(100);
                score++;
            }
            break;

        case 2:
            system("cls");
            LoadHighestScore();
            ShowHighestScore();
            break;

        case 3:
            system("cls");
            ResetHighestScore();
            break;

        case 4:
            system("cls");
            ShowInstructions();
            break;

        case 5:
            system("cls");
            ShowAboutGame();
            break;

        case 6:
            system("cls");
            printf(YELLOW "\n\n\n===============  Goodbye!  ================\n\n\n" NC);
            break;

        default:
            printf(RED "\nInvalid choice. Please try again.\n" NC);
            break;
        }

        // Clear the input buffer
        while (getchar() != '\n');

    }
    while (choice != 6);

    getch();
}
