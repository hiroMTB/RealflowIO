#include "FileReader.h"
#include "RPCReader.h"

void cApp::update(){
    
    char m[255];
    sprintf(m, "_Domain_%05d.rpc", frame%1000);
    string fileName = std::string(m);
    
    RPCFile reader;

    string path = (simDir/fileName).string();
    if(reader.Open( path.c_str() )) {
    
        uint32_t numParticles = reader.GetNumParticles();
        uint32_t numChannels = reader.GetNumChannels();
        
        printf("%d particles, %d channels.\n", numParticles, numChannels);
        
        const float* bboxMin = reader.GetBBoxMin();
        const float* bboxMax = reader.GetBBoxMax();
        printf("bbox: { %.3f %.3f %.3f } -> { %.3f %.3f %.3f }\n\n", bboxMin[0], bboxMin[1], bboxMin[2], bboxMax[0], bboxMax[1], bboxMax[2]);
        
        float * posBuf = (float*)reader.ReadWholeChannel("position");
        float * velocityBuf = (float*)reader.ReadWholeChannel("velocity");
    
    
        float scale = 50.0f;
        
        for (int i=0; i<numParticles; i++) {

            float x = posBuf[i*3+0] * scale;
            float y = posBuf[i*3+1] * scale;
            float z = posBuf[i*3+2] * scale;

            // ...
        } 
    }
}
