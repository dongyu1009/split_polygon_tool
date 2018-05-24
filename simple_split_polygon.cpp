#include <iostream>
#include <stdio.h>
#include "ogrsf_frmts.h"

using namespace std;
int main(void)
{
    GDALAllRegister();
    CPLSetConfigOption("SHAPE_ENCODING",""); // Chinese Language Default : ISO-8859-1
    GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
    GDALDataset *dataset;
    dataset = (GDALDataset *) GDALOpenEx("./Countries.shp", GDAL_OF_VECTOR, NULL, NULL, NULL);
    if (dataset == NULL)
        cout << "Open Failed!" << endl;
    OGRLayer *layer = dataset->GetLayer(0);
    cout << layer->GetName() << endl;
    cout << "Feature Count:" << layer->GetFeatureCount() << endl;
    layer->ResetReading();
    // while((pFeature = layer->GetNextFeature()) != NULL)
    for(int ii = 0; ii < layer->GetFeatureCount(); ii ++)
    {
        char outputfilename[30];
        sprintf(outputfilename, "./output/shape%d.shp", ii);
        cout << outputfilename << endl;
        GDALDataset *dsres = driver->CreateCopy(outputfilename, dataset, true, NULL, NULL, NULL);
        GDALClose(dsres);
        dsres = (GDALDataset *) GDALOpenEx(outputfilename, GDAL_OF_UPDATE, NULL, NULL, NULL);
        OGRLayer *reslayer = dsres->GetLayer(0);
        for(int i = 0; i < layer->GetFeatureCount(); i++)
        {
            // OGRFeature *feature = reslayer->GetFeature(i);
            if (i != ii) {
                reslayer->DeleteFeature(i);
            }
            // OGR_F_Destroy(feature);
        }
        GDALClose(dsres);
    }
    GDALClose(dataset);
    return 0;
}
