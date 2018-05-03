#include <iostream>
#include "ogrsf_frmts.h"
using namespace std;
int main(void)
{
    GDALAllRegister();
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
    return 0;
}
