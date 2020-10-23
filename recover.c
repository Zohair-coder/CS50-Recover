#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // checks whether argument for file name is given
    if (argc != 2)
    {
        printf("Usage: ./recover <file-name>");
        return 1;
    }

    // opens file in read mode
    FILE *file = fopen(argv[1], "r");

    //checks whether file name is valid
    if (file == NULL)
    {
        printf("Invalid file name\n");
        return 1;
    }

    //calculate file size
    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    printf("Size: %i\n", size);
    rewind(file);
    int yes = 0;
    int no = 0;
    char new_file_name[8];
    int file_number[3];
    char byte_block[512];
    unsigned char bytes[4];
    int new_signature_found;
    new_signature_found = 1;
    int is_first = 0;
    // printf("Position before start of loop: %li\n", ftell(file));
    while (ftell(file) <= size)
    {
        fread(bytes, 4, 1, file);
        // printf("Position right after fread(): %li\n", ftell(file));
        if ((bytes[0] == 0xff && bytes[1] == 0xd8 && bytes[2] == 0xff))
        {
            if ((bytes[3] & 0xf0) == 0xe0)
            {

                //count number of images
                yes ++;

                //go back 4 places to get the entire block
                fseek(file, -4, SEEK_CUR);

                //set file names
                file_number[0] = (yes - 1) % 10;
                file_number[1] = ((yes - 1) % 100) / 10;
                file_number[2] = (yes - 1) / 100;
                sprintf(new_file_name, "%i%i%i.jpg", file_number[2], file_number[1], file_number[0]);
                new_file_name[7] = '\0';

                //open new file in write mode
                FILE *file_new = fopen(new_file_name, "w");

                // boolean variable to exit loop once a new set of jpg is found
                new_signature_found = 1;

                //check if this is the first iteration and do not detect new jpgs if it is
                is_first = 0;

                while (new_signature_found != 0 && ftell(file) <= size - 512)
                {
                    //check if this is the first iteration and do not detect new jpgs if it is
                    is_first ++;

                    //read the first 4 bytes of the block
                    fread(bytes, 4, 1, file);

                    //if first 4 bytes are header of jpg then exit loop
                    if ((bytes[0] == 0xff && bytes[1] == 0xd8 && bytes[2] == 0xff && is_first != 1) && ((bytes[3] & 0xf0) == 0xe0))
                    {
                        new_signature_found = 0;
                        fseek(file, -4, SEEK_CUR);
                        fclose(file_new);
                        continue;
                    }
                    else
                    {
                        // go back 4 places because header is not being checked
                        fseek(file, -4, SEEK_CUR);
                        // read 512 bytes of file and store in byte_block
                        fread(byte_block, 512, 1, file);
                        // byte_block[512] = '\0';
                        // fprintf(file_new, "%s", byte_block);
                        // paste contents of byte_block in new file
                        fwrite(&byte_block, 1,  512, file_new);
                    }
                }
            }
        }
        else
        {
            no ++;
        }
        if (new_signature_found == 0)
        {
            continue;
        }
        fseek(file, -4, SEEK_CUR);
        fseek(file, 512, SEEK_CUR);
    }

//     // ======================
//     // CODE FOR CHECKING JPEG
//     // ======================
//     // Read first three bytes
//     unsigned char bytes[3];
//     fread(bytes, 3, 1, file);

//     // Check first three bytes
//     if (bytes[0] == 0xff && bytes[1] == 0xd8 && bytes[2] == 0xff)
//     {
//         printf("Maybe\n");
//     }
//     else
//     {
//         printf("No\n");
//     }

//     // Close file
//     fclose(file);
//     return 0;
}






