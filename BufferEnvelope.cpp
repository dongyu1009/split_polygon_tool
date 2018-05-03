#include <iostream>
#include "ogrsf_frmts.h"
using namespace std;
int main(void)
{
    GDALAllRegister();
    GDALDataset *dataset;
    // dataset = (GDALDataset *) GDALOpenEx("./input/input.shp", GDAL_OF_VECTOR, NULL, NULL, NULL);
    dataset = (GDALDataset *) GDALOpenEx("./input/input.shp", GDAL_OF_UPDATE, NULL, NULL, NULL);
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
        OGRGeometry *geometry = pFeature->GetGeometryRef();
        // Buffer
        // geometry = geometry->Buffer(0.1);
        // pFeature->SetGeometry(geometry);
        
        // Envelope
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
        pFeature->SetGeometry((OGRGeometry *)geometry);

        // Boundary
        // geometry = geometry->Boundary()->Polygonize();
        // pFeature->SetGeometry(geometry);
        // OGRPolygon *polygon = (OGRPolygon *)geometry;
        layer->SetFeature(pFeature);
        OGR_F_Destroy(pFeature);
    }
    GDALClose(dataset);
    return 0;
}
