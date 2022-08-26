#pragma once
#define debug

/*
 *      Import .rpc file from realflow hybrido simulation
 *      No framework needed.
 *      
 *      buggy, cant fix position data problem
 */

/*

 CHANNEL TYPES:

 1  float x 3
 2  double x 3
 3  float x 2
 4  uint64
 5  uint16
 6  uint8
 7  int32
 8  bool
 9  float
 10 double
 11 float x 4
 12 uint32
 
 */

#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdint.h>   /* For uint64_t */
#include <inttypes.h>

using namespace std;

class RfImporterRpc{
    
public:
    
    // grobal data
    uint32     gVerificationCode;   // 0x70FABADA
    uint32     gVersion;            // 3
    uint32     gNumParticles;
    uint32     gNumChannels;        // ?
    float   gMinBoundingBox[3];
    float   gMaxBoundingBox[3];
    
    // ch info
    vector<uint64> chDataFilePointers;
    
    // per particle data
    vector<float> pPosition;
    vector<float> pVelocity;
    vector<int> pId;
    vector<float> pAge;
    
    int chTypeDict[12][2] =
    {
        {sizeof(float),  3},
        {sizeof(double), 3},
        {sizeof(float),  2},
        {sizeof(uint64), 1},
        {sizeof(uint16), 1},
        {sizeof(uint8),  1},
        {sizeof(uint32), 1},
        {sizeof(bool),  1},
        {sizeof(float), 1},
        {sizeof(double), 1},
        {sizeof(float), 4},
        {sizeof(uint32), 1}
    };
    
    void load( string fileName ){
        
        FILE * pFile;
        pFile = fopen( fileName.c_str(), "rb");
        
        int bpos = 0;
        
        fread( &gVerificationCode,  sizeof(uint32),       1,   pFile );  bpos+=sizeof(uint32);
        fread( &gVersion,           sizeof(uint32),      1,   pFile );   bpos+=sizeof(uint32);
        fread( &gNumParticles,      sizeof(uint32),      1,   pFile );   bpos+=sizeof(uint32);
        fread( &gNumChannels,       sizeof(uint32),      1,   pFile );   bpos+=sizeof(uint32);
        fread( gMinBoundingBox,     sizeof(float),       3,   pFile );   bpos+=sizeof(float)*3;
        fread( gMaxBoundingBox,     sizeof(float),       3,   pFile );   bpos+=sizeof(float)*3;
        //total 40 byte

#ifdef debug
        printf("\nstart loading RPC file\n");
        printf("gVerificationCode : %x\n", gVerificationCode );
        printf("gVersion          : %i\n", gVersion );
        printf("gNumParticle      : %i\n", gNumParticles );
        printf("gNumChannels      : %i\n", gNumChannels );
        printf("\n");
#endif
        int posOffset;
        int velOffset;

        pPosition.clear();
        pVelocity.clear();
        pId.clear();
        pAge.clear();
        chDataFilePointers.clear();
        
        // Channel Data
        for( int i=0; i<gNumChannels; i++ ){

            uint32  chNameSize;
            fread( &chNameSize,  sizeof(uint32),             1,  pFile );   bpos+=sizeof(uint32);

            char    chName[chNameSize];
            fread(  chName,       sizeof(char),      chNameSize,  pFile );  bpos+=sizeof(char)*chNameSize;
            
            uint32  chType;
            fread( &chType,       sizeof(uint32),             1,  pFile );  bpos+=sizeof(uint32);
            
            uint64 chDataFilePointer;
            fread( &chDataFilePointer, sizeof(uint64),        1,  pFile);  bpos+=sizeof(uint64);
            chDataFilePointers.push_back(chDataFilePointer);
            uint64  chDataSize;
            fread( &chDataSize,   sizeof(uint64),             1,  pFile );  bpos+=sizeof(uint64);
#ifdef debug
            printf("ch settings %d\n", i);
            printf("chNameSize        : %i\n", chNameSize );
            printf("chName            : %s\n", chName );
            printf("chType            : %d\n", chType );
            printf("chDataFilePointer : %" PRIu64 "\n", chDataFilePointer );
            printf("chDataSize        : %" PRIu64"\n", chDataSize );
            
            if(string(chName)=="position") posOffset = chDataFilePointer;
            else if(string(chName)=="velocity") velOffset = chDataFilePointer;
            
#endif
            int     chTypeBaseSize = chTypeDict[chType-1][0];
            int     chTypeNumVal = chTypeDict[chType-1][1];
            int     chTypeSize = chTypeBaseSize * chTypeNumVal;
            
            switch( chType ){
                case 1:
                {
                    float minChVal[3];
                    float maxChVal[3];
                    float avgChVal[3];
                    float minModChVal;
                    float maxModChVal;
                    fread(   minChVal,     sizeof(float),      3,  pFile );
                    fread(   maxChVal,     sizeof(float),      3,  pFile );
                    fread(   avgChVal,     sizeof(float),      3,  pFile );
                    fread(  &minModChVal,  sizeof(float),      1,  pFile );
                    fread(  &maxModChVal,  sizeof(float),      1,  pFile );
                    
#ifdef debug
                    printf("minChVal          : %5.2f, %5.2f, %5.2f\n", minChVal[0], minChVal[1], minChVal[2] );
                    printf("maxChVal          : %5.2f, %5.2f, %5.2f\n", maxChVal[0], maxChVal[1], maxChVal[2] );
                    printf("avgChVal          : %5.2f, %5.2f, %5.2f\n", avgChVal[0], avgChVal[1], avgChVal[2] );
                    printf("minModChVal       : %5.2f\n", minModChVal );
                    printf("maxModChVal       : %5.2f\n", maxModChVal );
#endif
                    break;
                }
                default:
                {
                    char    minChVal[chTypeSize];
                    char    maxChVal[chTypeSize];
                    char    avgChVal[chTypeSize];
                    char    minModChVal[chTypeBaseSize];
                    char    maxModChVal[chTypeBaseSize];
                    fread(  minChVal,     sizeof(char),      chTypeSize,  pFile );  bpos+=sizeof(char)*chTypeSize;
                    fread(  maxChVal,     sizeof(char),      chTypeSize,  pFile );  bpos+=sizeof(char)*chTypeSize;
                    fread(  avgChVal,     sizeof(char),      chTypeSize,  pFile );  bpos+=sizeof(char)*chTypeSize;
                    fread(  minModChVal,  sizeof(char),      chTypeBaseSize,pFile );    bpos+=sizeof(char)*chTypeBaseSize;
                    fread(  maxModChVal,  sizeof(char),      chTypeBaseSize,pFile );    bpos+=sizeof(char)*chTypeBaseSize;
                    break;
                }
            }
            
            printf("\n");
        }
  
        
        //
        //  id : chType 4  uint64
        //  pos: chType 1, float*3
        //  vel: chType 1, float*3
        //
//        fseek( pFile, chDataFilePointers[0], SEEK_SET);
//        for( int j=0; j<gNumParticles; j++ ){
//
//            uint64 val;
//            fread( &val, sizeof(uint64), 1, pFile);
//            pId.push_back( val );
//            cout << "id " << val << endl;
//        }
//
//        fseek( pFile, posOffset, SEEK_SET);
//        for( int j=0; j<gNumParticles; j++ ){
//
//            float val[3];
//            fread( val, sizeof(float), 3, pFile);
//            pPosition.push_back( val[0] );
//            pPosition.push_back( val[1] );
//            pPosition.push_back( val[2] );
//            printf("pos : %f, %f,%f\n", val[0],val[1],val[2]);
//        }
//
//        fseek( pFile, velOffset, SEEK_SET);
//        for( int j=0; j<gNumParticles; j++ ){
//            float val[3];
//            fread( val, sizeof(float), 3, pFile);
//            pVelocity.push_back( val[0] );
//            pVelocity.push_back( val[1] );
//            pVelocity.push_back( val[2] );
//            //printf("vel : %+f,%+f,%+f\n", val[0],val[1],val[2]);
//        }

        
        for( int i=0; i<gNumChannels; i++ ){
            printf("ch data %d\n", i);


            printf("ftell: %ld\n", ftell(pFile));
            printf("this should be %" PRIu64 "\n", chDataFilePointers[i]);


            switch(i){
                case 0: // id
                {
                    for( int j=0; j<gNumParticles; j++ ){
                        uint64 val;
                        fread( &val, sizeof(uint64), 1, pFile);
                        pId.push_back( val );
                        //printf("id : %" PRIu64 "\n", val);
                        cout << "id: " << val << endl;
                    }
                    break;
                }

                case 1: // position
                {
                    for( int j=0; j<gNumParticles; j++ ){

                        float val[3];
                        fread( val, sizeof(float), 3, pFile);
                        pPosition.push_back( val[0] );
                        pPosition.push_back( val[1] );
                        pPosition.push_back( val[2] );
                        printf("position : %6.2f, %6.2f, %6.2f\n", val[0],val[1],val[2]);
                    }
                    break;
                }
                case 2: // velocity
                {
                    for( int j=0; j<gNumParticles; j++ ){
                        float val[3];
                        fread( val, sizeof(float), 3, pFile);
                        pVelocity.push_back( val[0] );
                        pVelocity.push_back( val[1] );
                        pVelocity.push_back( val[2] );
                        //printf("velocity : %+e,%+e,%+e\n", val[0],val[1],val[2]);
                    }
                    break;
                }

                case 3: // force
                {
                    for( int j=0; j<gNumParticles; j++ ){
                        float force[3];
                        fread( force, sizeof(float), 3, pFile);
                        //pForce.push_back( val[0] );
                        //pForce.push_back( val[1] );
                        //pForce.push_back( val[2] );
                        //printf("force : %+e,%+e,%+e\n", force[0], force[1], force[2]);
                    }
                    break;
                }

                case 4: // density
                {
                    for( int j=0; j<gNumParticles; j++ ){
                        float density;
                        fread( &density, sizeof(float), 1, pFile);
                        //pDensity.push_back( density );
                        //printf("density %5.3f", density);
                    }
                    break;
                }


                case 5: // age
                {
                    for( int j=0; j<gNumParticles; j++ ){
                        float age;
                        fread( &age, sizeof(float), 1, pFile);
                        pAge.push_back( age );
                        //printf("age %5.3f", age);
                    }
                    break;
                }

                case 6: // texture
                {
                    for( int j=0; j<gNumParticles; j++ ){
                        float tex[3];
                        fread( tex, sizeof(float), 3, pFile);
                        // push
                        //printf("texture : %+e,%+e,%+e\n", tex[0],tex[1],tex[2]);
                    }
                    break;
                }

                default:
                    printf("something went wrong!!!!!!!!!!!!!!");
                    break;

            }

            printf("\n");
        }

        
        fclose(pFile);
    }
    
};
