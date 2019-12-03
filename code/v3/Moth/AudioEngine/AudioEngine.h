#ifndef __AUDIOENGINE_H__
#define __AUDIOENGINE_H__

#include "../Configuration.h"
#include "FeatureCollector.h"

class AutoGain {
  public:
    AutoGain(AudioAnalyzeRMS *r, AudioAnalyzePeak *p);
    void update();
  private:
    FeatureCollector fc;

};

#endif // __AUDIO_ENGINE_H__
