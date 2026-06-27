#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define xSize       32
#define ySize       16
#define pipeCount   3
#define qKey        0x51
#define GREEN       "\e[32m"
#define YELLOW      "\e[33m"
#define NC          "\e[0m"

typedef struct
{
    int x;
    int y;
}PIX;

PIX bird;
PIX pipes[3];

void Draw();
void Pipes();
void HitTest();

void Draw()
{
    char buff[5000];
    strcpy(buff, "\e[17A");

    for (int y = 0; y <= ySize; y++)
    {
        for (int x = 0; x <= xSize; x++)
        {
            if
            (
                y == 0     ||
                y == ySize ||
                x == 0     ||
                x == xSize
            )
            {
                strcat(buff, NC "[]");              // Add '[]' to the buffer in white
                continue;                           // Move to the next column
            }

            for (int i = 0; i < pipeCount; i++)     // Loop over each pipe
            {
                if                                  // If its the top or bottom pipe face
                (
                    pipes[i].x >= x -1 &&
                    pipes[i].x <= x +1 &&
                    (
                        pipes[i].y == y + 3 ||
                        pipes[i].y == y - 3
                    )
                )
                {
                    strcat(buff, GREEN "[]");       // Add '[]' to the buffer in green
                    goto bottom;                    // Skip the rest of the tests
                }
                else if                             // If its the right angle of the bottom pipe
                (
                    pipes[i].x == x -1 &&
                    pipes[i].y == y - 4
                )
                {
                    strcat(buff, GREEN "]/");
                    goto bottom;
                }
                else if 
                (
                    pipes[i].x == x &&
                    (
                        pipes[i].y <= y -4 ||
                        pipes[i].y >= y +4
                    )
                )
                {
                    strcat(buff, GREEN "][");
                    goto bottom;
                }
                else if
                (
                    pipes[i].x == x +1 &&
                    pipes[i].y == y - 4
                )
                {
                    strcat(buff, GREEN "\\[");
                    goto bottom;
                }
                else if
                (
                    pipes[i].x == x -1 &&
                    pipes[i].y == y + 4
                )
                {
                    strcat(buff, GREEN "]\\");
                    goto bottom;
                }
                else if                             // If its the left angle of the top pipe
                (
                    pipes[i].x == x +1 &&
                    pipes[i].y == y + 4
                )
                {
                    strcat(buff, GREEN "/[");
                    goto bottom;
                }
                else if                             // If its the left side of the pipe
                (
                    pipes[i].x == x +1 &&
                    (
                        pipes[i].y <= y -5 ||
                        pipes[i].y >= y +5
                    )
                )
                {
                    strcat(buff, GREEN " [");
                    goto bottom;
                }
                else if                             // If its the right side of the pipe
                (
                    pipes[i].x == x -1 &&
                    (
                        pipes[i].y <= y -5 ||
                        pipes[i].y >= y +5
                    )
                )
                {
                    strcat(buff, GREEN "] ");
                    goto bottom;
                }
            }

            // The next bit will simply draw the bird in yellow based on x,y offsets
            if
            (
                bird.y == y &&
                bird.x == x
            )
            {
                strcat(buff, YELLOW ")>");
            }
            else if
            (
                bird.y == y &&
                bird.x == x +1
            )
            {
                strcat(buff, YELLOW "_(");
            }
            else if
            (
                bird.y == y &&
                bird.x == x +2)
            {
                strcat(buff, YELLOW " _");
            }
            else if
            (
                bird.y == y -1 &&
                bird.x == x
            )
            {
                strcat(buff, YELLOW ") ");
            }
            else if
            (
                bird.y == y -1 &&
                bird.x == x +1
            )
            {
                strcat(buff, YELLOW "__");
            }
            else if
            (
                bird.y == y -1 &&
                bird.x == x +2
            )
            {
                strcat(buff, YELLOW " \\");
            }
            else                                    // If its non of the other parts
            {
                strcat(buff, NC "  ");
            }

            bottom:;                                // The point moved to with "goto bottom;"
        }

        strcat(buff, "\n");                         // Append a new line to the buffer
    }

    printf(buff);                                   // Write the buffer
}

void Pipes()
{
    for (int i = 0; i < pipeCount; i++)
    {
        if (pipes[i].x == -1)                       // If the pipe is of screen
        {
            // If i = 0 then set pipes[i].x = pipes[2].x +15 else set pipes[i].x = pipes[i -1].x + 15
            // We do this so the distance between the pipes is alwas 15
            (i == 0) ? (pipes[i].x = pipes[2].x +15) : (pipes[i].x = pipes[i - 1].x +15);
            pipes[i].y = (rand()%7) +5;             // Set the pipes y to a random number between 5 and 11
        }
    }
}

void HitTest()
{
    if (bird.y == 15)                               // If the bird is on the floor
    {
        exit(0);
    }

    for (int i = 0; i < pipeCount; i++)
    {
        /*
            The next 2 ifs are collision detection.
            With the 2 simple diagrams bellow A is the bird and B is the pipe
            A1  A2
             +--+
             |  |
             +--+
            A3
            B1  B2
             +--+
             |  |
             +--+
            B3
        */

        if (
            (bird.x -2 < pipes[i].x +2) &&          // A1 < B2
            (bird.x > pipes[i].x -2) &&             // A2 > B1
            (
                (bird.y < pipes[i].y -2) ||         // A3 < B3
                (bird.y > pipes[i].y +1)            // A3 > B3
            )
           )
        {
            exit(0);
        }
    }
}

int main()
{
    srand(time(NULL));                              // Sets the seed for the random number generator to the current UNIX time stamp
    system("title \"Not Flappy Duck\"");

    bird.x = 10;                                    // Set the birds start position
    bird.y = 10;

    for (int i = 0; i < pipeCount; i++)
    {
        pipes[i].x = 25 + 15 * i;                   // Set the pipes x to be spaces 15 blocks appart
        pipes[i].y = (rand()%7) +5;                 // Set the pipes y to a random number between 5 and 11
    }

    int frame = 0;                                  // Variable to hold the passed frames

    printf("Press UP to jump and Q to quit.\n");    // List the controls

    for (int i = 0; i <= ySize; i++)                // Makes space for the game
    {
        printf("\n");
    }

    Draw();

    system("pause>nul");                            // Pause untill the user presses a key without showing a prompt

    while (1)
    {
        if (GetAsyncKeyState(VK_UP))                // If the user is pressing the up arrow
        {
            bird.y -= 2;                            // Move the bird up 2 pixels
        }

        if (GetAsyncKeyState(qKey))                 // If the user is pressing Q
        {
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

        HitTest();

        Draw();
        Pipes();

        frame++;
        Sleep(100);
    }

    return 0;
}
