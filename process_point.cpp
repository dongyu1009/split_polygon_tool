#include <iostream>
#include <stdio.h>
#include "ogrsf_frmts.h"

using namespace std;

int createPolygonDataset(char *name)
{
    const char* drivername = "ESRI Shapefile";
    GDALDriver *driver = GetGDALDriverManager()->GetDriverByName(drivername);
    if (driver == NULL)
    {
        cout << "driver is not avilable!" << endl;
        return 1;
    }
    GDALDataset *dataset = driver->Create(name, 0,0,0, GDT_Unknown, NULL);
    if (dataset == NULL)
    {
        cout << "creation fault!" << endl;
        return 1;
    }
    OGRLayer *poLayer;
    OGRSpatialReference * ref = new OGRSpatialReference();
    ref->importFromEPSG(4326);
    poLayer = dataset->CreateLayer( "out", ref, wkbPolygon , NULL ); // wkbPoint  = 1, wkbLineString  = 2, wkbPolygon  = 3
    if( poLayer == NULL )
    {
        printf( "Layer creation failed.\n" );
        return 1;
    }
    GDALClose(dataset);
    return 0;

}
int main(void)
{
    GDALAllRegister();
    CPLSetConfigOption("SHAPE_ENCODING",""); // Chinese Language Default : ISO-8859-1
    GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
    GDALDataset *dataset;
    dataset = (GDALDataset *) GDALOpenEx("../sites.shp", GDAL_OF_VECTOR, NULL, NULL, NULL);
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
        // GDALDataset *dsres = driver->CreateCopy(outputfilename, dataset, true, NULL, NULL, NULL);
        // GDALClose(dsres);
        createPolygonDataset(outputfilename);
        GDALDataset *dsres = (GDALDataset *) GDALOpenEx(outputfilename, GDAL_OF_UPDATE, NULL, NULL, NULL);
        OGRLayer *originlayer = dataset->GetLayer(0);
        OGRLayer *reslayer = dsres->GetLayer(0);
        for(int i = 0; i < layer->GetFeatureCount(); i++)
        {
            OGRFeature *feature = originlayer->GetFeature(i);
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
                geometry = polygon->Buffer(0.6);
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
                OGRFeature *poFeature = OGRFeature::CreateFeature( reslayer->GetLayerDefn() );
                poFeature->SetGeometry((OGRGeometry *)polygon);
                // reslayer->SetFeature(feature);
                if( reslayer->CreateFeature( poFeature ) != OGRERR_NONE )
                {
                    printf( "Failed to create feature in shapefile.\n" );
                    exit( 1 );
                }
                OGRFeature::DestroyFeature( poFeature );
            }
            else {
                // reslayer->DeleteFeature(i);
            }
            OGR_F_Destroy(feature);
        }
        GDALClose(dsres);
    }
    GDALClose(dataset);
    return 0;
}
