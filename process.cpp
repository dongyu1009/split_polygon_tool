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
            OGRFeature *feature = reslayer->GetFeature(i);
            if (i == ii)
            {
                OGRGeometry *geometry = feature->GetGeometryRef();
                // Envelop
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
                // Buffer
                geometry = polygon->Buffer(1000);
                // Envelop
                geometry->getEnvelope(env);
                ring = new OGRLinearRing();
                ring->addPoint(env->MinX, env->MinY);
                ring->addPoint(env->MinX, env->MaxY);
                ring->addPoint(env->MaxX, env->MaxY);
                ring->addPoint(env->MaxX, env->MinY);
                ring->addPoint(env->MinX, env->MinY);
                polygon = new OGRPolygon();
                polygon->addRingDirectly(ring);
                feature->SetGeometry((OGRGeometry *)polygon);
                reslayer->SetFeature(feature);
            }
            else {
                reslayer->DeleteFeature(i);
            }
            OGR_F_Destroy(feature);
        }
        GDALClose(dsres);
    }
    GDALClose(dataset);
    return 0;
}
