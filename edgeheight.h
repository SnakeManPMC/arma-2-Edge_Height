#ifndef EDGEHEIGHT_H
#define EDGEHEIGHT_H

typedef struct
{
    short Elevation;
    short TexIndex;
} wrpformat[4096][4096];

class edgeHeight
{
public:
    edgeHeight(int argc, char *argv[]);
    void Open_Files(int argc, char *argv[]);
    void Read_Signatures();
    void Read_Elevations();
    void Read_Textures();
    void Write_Elevations();
    void Write_Textures();
    void Write_Objects();
    void Close_Files();

private:
    FILE *map;
    FILE *output;
    char sig[33];
    short TexIndex;
    int MapSize, CellSize, EdgeHeightNumber;
};

#endif // EDGEHEIGHT_H
