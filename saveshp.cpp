#include <iostream>
#include "ogrsf_frmts.h"
using namespace std;
int main(void)
{
    const char* drivername = "ESRI Shapefile";
    GDALAllRegister();
    GDALDriver *driver = GetGDALDriverManager()->GetDriverByName(drivername);
    if (driver == NULL)
    {
        cout << "driver is not avilable!" << endl;
        return 1;
    }
    GDALDataset *dataset = driver->Create("./test.shp", 0,0,0, GDT_Unknown, NULL);
    if (dataset == NULL)
    {
        cout << "creation fault!" << endl;
        return 1;
    }
    OGRLayer *poLayer;
    poLayer = dataset->CreateLayer( "point_out", NULL, wkbPoint, NULL );
    if( poLayer == NULL )
    {
        printf( "Layer creation failed.\n" );
        exit( 1 );
    }
    cout << "Success!" <<endl;
    /*
    GDALDataset *dataset;
    dataset = (GDALDataset *) GDALOpenEx("./input/input.shp", GDAL_OF_VECTOR, NULL, NULL, NULL);
    if (dataset == NULL)
    {
        cout << "Open Failed!" << endl;
    }
    OGRLayer *layer = dataset->GetLayer(0);
    cout << layer->GetName() << endl;
    cout << "Feature Count:" << layer->GetFeatureCount() << endl;
    OGRFeature *pFeature;
    layer->ResetReading();
    while( (pFeature = layer->GetNextFeature()) != NULL)
    {
        OGR_F_Destroy(pFeature);
    }
    GDALClose(dataset);
    */
    return 0;
}
