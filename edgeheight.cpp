#include <QtCore/QCoreApplication>
#include <cstdio>
#include <cstdlib>
#include "edgeheight.h"

static wrpformat wrp;

edgeHeight::edgeHeight(int argc, char *argv[])
{
    Open_Files(argc, argv);
    Read_Signatures();
    Read_Elevations();
    Read_Textures();
    Write_Elevations();
    Write_Textures();
    Write_Objects();
    Close_Files();
}


void edgeHeight::Open_Files(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Not enought parameters!\n\n\tEdgeHeight Source.WRP Destination.WRP <EdgeHeight>\n\n");
        exit (1);
    }

    printf ("Source WRP: %s\nDestination WRP: %s\n", argv[1], argv[2]);

    map = fopen (argv[1], "rb");
    if (!map)
    {
        printf ("error in %s\n", argv[1]);
        exit (1);
    }

    printf ("Opened %s\n", argv[1]);

    output = fopen (argv[2], "wb");
    if (!output)
    {
        printf ("error at %s file\n", argv[2]);
        exit (1);
    }

    printf ("Opened %s\n", argv[2]);

    // grab the edge height from cmd line
    EdgeHeightNumber = atoi(argv[3]);
    EdgeHeightNumber = (EdgeHeightNumber * 22);
    printf("EdgeHeight: %i\n", EdgeHeightNumber);
}


void edgeHeight::Read_Signatures()
{
    fread (sig,4,1,map);
    sig[4] = 0;
    fwrite (sig, 4, 1, output);

    fread(&MapSize,4,1,map);
    fwrite(&MapSize,4,1,output);

    fread(&MapSize,4,1,map);
    fwrite(&MapSize,4,1,output);

    if (MapSize > 4096)
    {
        printf("MapSize %i is too large! exiting!\n",MapSize);
        exit(1);
    };

    printf("Signature: %s\nMapSize: %i\nReading elevations...",sig,MapSize);
}


void edgeHeight::Read_Elevations()
{
    // read elevations
    int x = 0, z = 0;
    for (int zx = 0; zx < MapSize*MapSize; zx++)
    {
        fread(&wrp[x][z].Elevation,sizeof(wrp[x][z].Elevation),1,map);

        x++;
        if (x == MapSize)
        {
            z++; x = 0;
        }
        if (z == MapSize)
        {
            z = 0;
        }
    }
}


void edgeHeight::Read_Textures()
{
    printf(" Done\nReading Textures...");

    // read textures IDs
    int x = 0, z = 0;
    TexIndex = 0;

    for (int tx = 0; tx < MapSize*MapSize; tx++)
    {
        wrp[x][z].TexIndex = 0;
        fread(&wrp[x][z].TexIndex,sizeof(wrp[x][z].TexIndex),1,map);

        x++;
        if (x == MapSize)
        {
            z++;
            x = 0;
        }
        if (z == MapSize)
        {
            z = 0;
        }
    }
}


void edgeHeight::Write_Elevations()
{
    printf(" Done\nWriting elevations...");

    // write the elevations
    int x = 0, z = 0;
    for (int xx = 0; xx < MapSize*MapSize; xx++)
    {
        // here we check if we are processing edge data.
        if ( x == 0 || z == 0 || x == MapSize - 1 || z == MapSize - 1 )
        {
            wrp[x][z].Elevation = EdgeHeightNumber;
        }

        fwrite(&wrp[x][z].Elevation,sizeof(wrp[x][z].Elevation),1,output);
        x++;
        if (x == MapSize)
        {
            z++;
            x = 0;
        }
        if (z == MapSize)
        {
            z = 0;
        }
    }
}


void edgeHeight::Write_Textures()
{
    printf(" Done\nWriting texture index...");

    // write the texture indexes
    int x = 0, z = 0;
    for (int cr = 0; cr < MapSize*MapSize; cr++)
    {
        fwrite(&wrp[x][z].TexIndex,sizeof(wrp[x][z].TexIndex),1,output);
        x++;
        if (x == MapSize)
        {
            z++;
            x = 0;
        }
        if (z == MapSize)
        {
            z = 0;
        }
    }

    printf(" Done\nReading Texture names...");

    //textures 32 char length and total of 512
    for (int ix = 0; ix < 512; ix++)
    {
        sig[0] = 0;
        fread(sig,32,1,map);
        fwrite(sig,32,1,output);
    }
}


void edgeHeight::Write_Objects()
{
    printf(" Done\nReading writing 3dObjects...");

    char dObjName[76],EmptyName[76];
    float dDir,dDirX = 0,dDirZ = 0;
    long dObjIndex = 0;

    // clear the objname variable
    for (int i = 0; i < 76; i++) EmptyName[i] = 0;

    while (!feof(map))
    {
        dDir = 1;
        fread(&dDir,4,1,map);
        fwrite(&dDir,4,1,output);
        dDir = 0;
        fread(&dDir,4,1,map);
        fwrite(&dDir,4,1,output);
        fread(&dDir,4,1,map);
        fwrite(&dDir,4,1,output);
        fread(&dDir,4,1,map);
        fwrite(&dDir,4,1,output);
        dDir = 1;
        fread(&dDir,4,1,map);
        fwrite(&dDir,4,1,output);
        dDir = 0;
        fread(&dDir,4,1,map);
        fwrite(&dDir,4,1,output);
        fread(&dDir,4,1,map);
        fwrite(&dDir,4,1,output);
        fread(&dDir,4,1,map);
        fwrite(&dDir,4,1,output);
        dDir = 1;
        fread(&dDir,4,1,map);
        fwrite(&dDir,4,1,output);

        // X coord
        fread(&dDirX,4,1,map);
        fwrite(&dDirX,4,1,output);

        // here we do our magic for aling to ground
        fread(&dDir,4,1,map);

        // Z coord
        fread(&dDirZ,4,1,map);

        dObjIndex++;
        fread(&dObjIndex,4,1,map);
        strcpy(dObjName, EmptyName);
        fread(dObjName,76,1,map);
        //strlwr(dObjName);

        fwrite(&dDir,4,1,output);
        fwrite(&dDirZ,4,1,output);
        fwrite(&dObjIndex,4,1,output);

        fwrite(dObjName,sizeof(dObjName),1,output);
    };

    printf(" Done\n");
}


void edgeHeight::Close_Files()
{
    fclose(map);
    fclose(output);
}

/*

Edges in ArmA world. In this situation 12.8km x 12.8km terrain.
0, 0 - lower left.
0, 12800 - upper left.
12800, 12800 - upper right.
12800, 0 - lower right.

*/
