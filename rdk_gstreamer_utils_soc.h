/*
* If not stated otherwise in this file or this component's LICENSE file the
* following copyright and licenses apply:
*
* Copyright 2019 RDK Management
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <string>

namespace rdk_gstreamer_utils
{
    void initVirtualDisplayHeightandWidthFromPlatform_soc(unsigned int* mVirtualDisplayHeight, unsigned int* mVirtualDisplayWidth);
    const char* getAudioDecoderName_soc();
    const char* getAudioUnderflowSignalName_soc();
    const char* getVideoUnderflowSignalName_soc();
    bool IntialVolSettingNeeded_soc();
    bool IsAudioFadeSupported_soc();
    void EaseAudio_soc(double target, uint32_t duration, rgu_Ease ease);
    bool isPtsOffsetAdjustmentSupported_soc();
    int getPtsOffsetAdjustment_soc(const std::string& audioCodecString);
    void setVideoProperty_soc(GstElement *pipeline);
    void processAudioGap_soc(GstElement *pipeline,gint64 gapstartpts,gint32 gapduration,gint64 gapdiscontinuity,bool audioaac);
    void enableAudioSwitch_soc(GstElement *pipeline);
    GstElement * configureUIAudioSink_soc(bool TTSenabled);
    bool isUIAudioVGAudioMixSupported_soc();
    unsigned int getNativeAudioFlag_soc();
    void configAudioCap_soc(AudioAttributes *pAttrib, bool *audioaac, bool svpenabled, GstCaps **appsrcCaps, bool passthroughProperty);
    bool performAudioTrackCodecChannelSwitch_soc(struct rdkGstreamerUtilsPlaybackGrp *pgstUtilsPlaybackGroup, const void *pSampleAttr, AudioAttributes *pAudioAttr, uint32_t *pStatus, unsigned int *pui32Delay,
                                                 llong *pAudioChangeTargetPts, const llong *pcurrentDispPts, unsigned int *audio_change_stage, GstCaps **appsrcCaps,
                                                 bool *audioaac, bool svpenabled, GstElement *aSrc, bool *ret, bool passthroughProperty);
    void setAppSrcParams_soc(GstElement *aSrc,MediaType mediatype);
    void setPixelAspectRatio_soc(GstCaps ** ppCaps,GstCaps *appsrcCaps,uint32_t pixelAspectRatioX,uint32_t pixelAspectRatioY);
    void deepElementAdded_soc (struct rdkGstreamerUtilsPlaybackGrp *pgstUtilsPlaybackGroup, GstBin* pipeline, GstBin* bin, GstElement* element);
    void audioMixerGetDeviceInfo_soc(uint32_t& preferredFrames, uint32_t& maximumFrames);
    size_t audioMixerGetBufferDelay_soc(int64_t queuedBytes,int bufferDelayms);
    uint64_t audioMixerGetFifoSize_soc();
    void audioMixerConfigurePipeline_soc(GstElement *gstPipeline,GstElement *aSink,GstElement *aSrc,bool attenuateOutput);
    uint64_t audioMixerGetQueuedBytes_soc(uint64_t bytesPushed,uint64_t bytesPlayed);
    void setVideoSinkMode_soc(GstElement * videoSink);
    void configVideoCap_soc(std::string vCodec,uint32_t imageWidth,uint32_t imageHeight,uint32_t frameRateValue,uint32_t frameRateScale,bool svpEnabled,gchar **capsString);
    void switchToAudioMasterMode_soc();
    void setKeyFrameFlag_soc(GstBuffer *gstBuffer,bool val);
    bool getDelayTimerEnabled_soc();
    void SetAudioServerParam_soc(bool enabled);
    void constructLLAudioPlayer_soc(int numChannel ,GstElement *gstPipeline ,GstElement *aSrc,GstElement *aSink,GstElement *aFilter,GstElement *aDecoder);

// =========================================== DRM APIs ================================================

    typedef void (*max_resolution_update_cb)(const unsigned char *databuffer, size_t len, const DRM_VOID *f_pv);

    DRM_RESULT Drmhal_Platform_Initialize_soc( std::string DrmStorePath );

    uint32_t Drmhal_DeleteDrmStore_soc(DRM_CONST_STRING mDrmStore, std::string DrmStorePath);

    bool Drmhal_QueryBatchIDFromLicenseRespone_soc(DRM_LICENSE_RESPONSE *pstdrmLicenseResponse, DRM_ID *pstDRMBatchID);

    bool Drmhal_bindCallbackPrecheck_soc( DRM_POLICY_CALLBACK_TYPE  f_dwCallbackType );

    DRM_RESULT Drmhal_FetchOuptutProtectionConfigData_soc(const DRM_VOID *f_pvCallbackData, DRM_POLICY_CALLBACK_TYPE  f_dwCallbackType,
                                                const DRM_KID *f_pKID, const DRM_LID *f_pLID, const DRM_VOID *f_pv, max_resolution_update_cb cb);

    DRM_RESULT Drmhal_PreDecrypt_soc(void * mDecryptContext, void * mSVPContext, bool mPreallocMemoryForDecrypt,
                                        DRM_DWORD f_cbEncryptedContent, void ** header, void * securehandle, int securehandleSz);

// =========================================== DRM APIs ================================================
} //namespace rdk_gstreamer_utils
