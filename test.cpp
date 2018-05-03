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
    dataset = (GDALDataset *) GDALOpenEx("./input/input.shp", GDAL_OF_VECTOR, NULL, NULL, NULL);
    if (dataset == NULL)
        cout << "Open Failed!" << endl;
    OGRLayer *layer = dataset->GetLayer(0);
    cout << layer->GetName() << endl;
    cout << "Feature Count:" << layer->GetFeatureCount() << endl;
    OGRFeature *pFeature;
    layer->ResetReading();
    for(int i = 0; i < layer->GetFeatureCount(); i ++)
    {
        char outputfilename[30];
        sprintf(outputfilename, "./output/shape%d.shp", i);
        cout << outputfilename << endl;
        driver->CreateCopy(outputfilename, dataset, true, NULL, NULL, NULL);
    }
    while((pFeature = layer->GetNextFeature()) != NULL)
    {
        int fidnow = pFeature->GetFID();
        char outputfilename[30];
        sprintf(outputfilename, "./output/shape%d.shp", fidnow);
        GDALDataset *dsres = (GDALDataset *) GDALOpenEx(outputfilename, GDAL_OF_UPDATE, NULL, NULL, NULL);
        OGRLayer *reslayer = dsres->GetLayer(0);
        for(int i = 0; i < reslayer->GetFeatureCount(); i++)
        {
            OGRFeature *feature = reslayer->GetFeature(i);
            if (i == fidnow)
            {
                OGRGeometry *geometry = feature->GetGeometryRef();
                OGREnvelope *env = new OGREnvelope();
                geometry->getEnvelope(env);
                OGRLinearRing *ring = new OGRLinearRing();
                ring->addPoint(env->MinX, env->MinY);
                ring->addPoint(env->MinX, env->MaxY);
                ring->addPoint(env->MaxX, env->MaxY);
                ring->addPoint(env->MaxX, env->MinY);
                ring->addPoint(env->MinX, env->MinY);
                OGRPolygon *polygon = new OGRPolygon();
                polygon->addRingDirectly(ring);
                geometry = polygon->Buffer(0.1);
                feature->SetGeometry((OGRGeometry *)geometry);
                reslayer->SetFeature(feature);
            }
            else {
                reslayer->DeleteFeature(i);
            }
            OGR_F_Destroy(feature);
        }
        GDALClose(dsres);
        OGR_F_Destroy(pFeature);
    }
    GDALClose(dataset);
    return 0;
}
