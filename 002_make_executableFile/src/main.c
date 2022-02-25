#include "get_image.h"
#include "myFuncts.h"

/*read_ini*/
char buffIni[40];
char iniFile[20];
int IMAGE_WIDTH;
int IMAGE_HEIGHT;
int IMAGE_SIZE;
/*read_ini*/

int main()
{
    int cam_fd;
    FILE *f;
    uint8_t *output_buffer;
    uint8_t file_header[16];
    char tmp[64] = {"---\n"};

    /*Read ini*/
    printf("Start calling <config.ini> to read raw data:\r\n");
    strcpy(iniFile,"config.ini");

    if(!initFuncts(iniFile))
    {
        puts("initFuncts error");
        return EXIT_FAILURE;
    }

    puts("initFuncts OK");
    /*Read ini*/

    /*Set buffer size*/
    output_buffer = (uint8_t *) malloc(IMAGE_WIDTH * IMAGE_HEIGHT * 2 * sizeof(uint8_t));

    /*init cam*/
    cam_fd = cam_init(IMAGE_WIDTH,IMAGE_HEIGHT);
    if (cam_fd == -1)
    {
        printf("fail to init\n");
        free(output_buffer);
        return -1;
    }
    printf("Init OK, sleep for 5 sec\n");
    sleep(5);
    /*init cam*/

    /*Get Data*/
    if (cam_get_image(output_buffer,IMAGE_SIZE,cam_fd) != 0 )
    {
        printf("error get image\n");
        goto end;
    }
    /*Get Data*/

    /*Show Data*/
    for (int i = 0; i < 16; i++)
        sprintf(&tmp[strlen(tmp)], "%02x ", output_buffer[i]);
    printf("%s\r\n", tmp);
    /*Show Data*/

    /*Write File*/
    f=fopen("./Pic/Test_Output.raw","wb");
    if (f== NULL)
    {
        printf("Fail to open file\n");
        goto end;
    }
    memset(file_header, 0, 16);
    file_header[8] = IMAGE_WIDTH % 256;
    file_header[9] = (int)IMAGE_WIDTH / 256;
    file_header[12] = IMAGE_HEIGHT % 256;
    file_header[13] = (int)IMAGE_HEIGHT / 256;
    fwrite(file_header, 1, 16, f);
    fseek(f,16,SEEK_SET);
    fwrite(output_buffer,1,IMAGE_SIZE,f);
    fclose(f);
    /*Write File*/

end:
    cam_close(cam_fd);
    free(output_buffer);

    return 0;
}
