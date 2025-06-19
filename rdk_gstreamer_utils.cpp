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


#include "rdk_gstreamer_utils.h"
#include "rdk_gstreamer_utils_soc.h"
#include <gst/gst.h>
#include <glib.h>
#include <string.h>


using namespace std;

namespace rdk_gstreamer_utils {

    GstElement* retrieveGstElementByName(GstElement *element, const char *targetName)
    {
        GstElement *re = NULL;
        if (GST_IS_BIN(element)) {
            GstIterator* it = gst_bin_iterate_elements(GST_BIN(element));
            GValue item = G_VALUE_INIT;
            bool done = false;
            while(!done) {
                switch (gst_iterator_next(it, &item)) {
                    case GST_ITERATOR_OK:
                    {
                        GstElement *next = GST_ELEMENT(g_value_get_object(&item));
                        done = (re = retrieveGstElementByName(next, targetName)) != NULL;
                        g_value_reset (&item);
                        break;
                    }
                    case GST_ITERATOR_RESYNC:
                        gst_iterator_resync (it);
                        break;
                    case GST_ITERATOR_ERROR:
                    case GST_ITERATOR_DONE:
                        done = true;
                        break;
                }
            }
            g_value_unset (&item);
            gst_iterator_free(it);
        } else {
            gchar* elemName = gst_element_get_name(element);
            if(elemName != NULL) {
                if (strstr(elemName, targetName)) {
                    re = element;
                }
                g_free(elemName);
            }

        }
        return re;
    }

    void initVirtualDisplayHeightandWidthFromPlatform(unsigned int* mVirtualDisplayHeight, unsigned int* mVirtualDisplayWidth)
    {
        initVirtualDisplayHeightandWidthFromPlatform_soc(mVirtualDisplayHeight, mVirtualDisplayWidth);
    }

    bool installUnderflowCallbackFromPlatform(GstElement *pipeline, GCallback underflowVideoCallback, GCallback underflowAudioCallback, gpointer data)
    {
        const char* audiodecodername = getAudioDecoderName_soc();
        GstElement* audiodecoder = retrieveGstElementByName(pipeline, audiodecodername);
        GstElement* videodecoder = retrieveGstElementByName(pipeline, "westerossink"); //default on RDK platforms
        const char* AudioUnderflowSignal = getAudioUnderflowSignalName_soc();
        const char* VideoUnderflowSignal = getVideoUnderflowSignalName_soc();

        gulong id_audio = g_signal_connect(audiodecoder, AudioUnderflowSignal, underflowAudioCallback, data);
        gulong id_video = g_signal_connect(videodecoder, VideoUnderflowSignal, underflowVideoCallback, data);

	return id_audio > 0 && id_video > 0;
    }

    bool IntialVolSettingNeeded()
    {
        return IntialVolSettingNeeded_soc();
    }

    bool isSocAudioFadeSupported()
    {
        return IsAudioFadeSupported_soc();
    }

    void doAudioEasingonSoc(double target, uint32_t duration, rgu_Ease ease)
    {
        EaseAudio_soc(target, duration, ease);
    }


    void setVideoProperty(GstElement *pipeline)
    {
        setVideoProperty_soc(pipeline);
    }

    void processAudioGap(GstElement *pipeline,gint64 gapstartpts,gint32 gapduration,gint64 gapdiscontinuity,bool audioaac)
    {
        processAudioGap_soc(pipeline,gapstartpts,gapduration,gapdiscontinuity,audioaac);
    }

    void enableAudioSwitch(GstElement *pipeline)
    {
        enableAudioSwitch_soc(pipeline);
    }

    GstElement * configureUIAudioSink(bool TTSenabled)
    {
        return configureUIAudioSink_soc(TTSenabled);
    }

    bool isUIAudioVGAudioMixSupported()
    {
        return isUIAudioVGAudioMixSupported_soc();
    }

    unsigned getNativeAudioFlag()
    {
        return getNativeAudioFlag_soc();
    }

    bool isPtsOffsetAdjustmentSupported()
    {
        return isPtsOffsetAdjustmentSupported_soc();
    }

    int getPtsOffsetAdjustment(const std::string& audioCodecString)
    {
        if (isPtsOffsetAdjustmentSupported_soc()) {
            return getPtsOffsetAdjustment_soc(audioCodecString);
        } else {
            return 0;
        }
    }

    void configAudioCap(AudioAttributes *pAttrib, bool *audioaac, bool svpenabled, GstCaps **appsrcCaps, bool passthroughProperty)
    {
        configAudioCap_soc(pAttrib,audioaac,svpenabled,appsrcCaps,passthroughProperty);
    }

    bool performAudioTrackCodecChannelSwitch(struct rdkGstreamerUtilsPlaybackGrp *pgstUtilsPlaybackGroup,const void *pSampleAttr, AudioAttributes *pAudioAttr, uint32_t *pStatus, unsigned int *pui32Delay,
                                                 llong *pAudioChangeTargetPts,const llong *pcurrentDispPts, unsigned int *audio_change_stage, GstCaps **appsrcCaps,
                                                 bool *audioaac, bool svpenabled, GstElement *aSrc, bool *ret, bool passthroughProperty)
    {
        return performAudioTrackCodecChannelSwitch_soc(pgstUtilsPlaybackGroup, pSampleAttr,pAudioAttr,pStatus, pui32Delay,
                                                pAudioChangeTargetPts,pcurrentDispPts,audio_change_stage,appsrcCaps,
                                                audioaac,svpenabled,aSrc,ret,passthroughProperty);
    }

    void setAppSrcParams(GstElement *aSrc,MediaType mediatype)
    {
        return setAppSrcParams_soc(aSrc,mediatype);
    }

    void setPixelAspectRatio(GstCaps ** ppCaps,GstCaps *appsrcCaps,uint32_t pixelAspectRatioX,uint32_t pixelAspectRatioY)
    {
        return setPixelAspectRatio_soc(ppCaps,appsrcCaps,pixelAspectRatioX,pixelAspectRatioY);
    }

    void deepElementAdded(struct rdkGstreamerUtilsPlaybackGrp *pgstUtilsPlaybackGroup, GstBin* pipeline, GstBin* bin, GstElement* element)
    {
	    deepElementAdded_soc(pgstUtilsPlaybackGroup,pipeline, bin,element);
    }

    void audioMixerGetDeviceInfo(uint32_t& preferredFrames, uint32_t& maximumFrames)
    {
        return  audioMixerGetDeviceInfo_soc(preferredFrames, maximumFrames);
    }

    size_t audioMixerGetBufferDelay(int64_t queuedBytes,int bufferDelayms)
    {
        return audioMixerGetBufferDelay_soc(queuedBytes,bufferDelayms);
    }

    uint64_t audioMixerGetFifoSize()
    {
        return audioMixerGetFifoSize_soc();
    }

    void setVideoSinkMode(GstElement * videoSink)
    {
        return setVideoSinkMode_soc(videoSink);
    }

    void configVideoCap(std::string vCodec,uint32_t imageWidth,uint32_t imageHeight,uint32_t frameRateValue,uint32_t frameRateScale,bool svpEnabled,gchar **capsString)
    {
        return configVideoCap_soc(vCodec,imageWidth,imageHeight,frameRateValue,frameRateScale,svpEnabled,capsString);
    }

    uint64_t audioMixerGetQueuedBytes(uint64_t bytesPushed,uint64_t bytesPlayed)
    {
        return audioMixerGetQueuedBytes_soc(bytesPushed,bytesPlayed);
    }

    void audioMixerConfigurePipeline(GstElement *gstPipeline,GstElement *aSink,GstElement *aSrc,bool attenuateOutput)
    {
        return audioMixerConfigurePipeline_soc(gstPipeline,aSink,aSrc,attenuateOutput);
    }

    void switchToAudioMasterMode()
    {
        return switchToAudioMasterMode_soc();
    }

    void setKeyFrameFlag(GstBuffer *gstBuffer,bool val)
    {
        setKeyFrameFlag_soc(gstBuffer,val);
    }

    bool getDelayTimerEnabled()
    {
        return getDelayTimerEnabled_soc();
    }

    void SetAudioServerParam(bool enabled)
    {
       SetAudioServerParam_soc(enabled);
    }

    void constructLLAudioPlayer(int numChannel ,GstElement *gstPipeline ,GstElement *aSrc,GstElement *aSink,GstElement *aFilter,GstElement *aDecoder)
    {
        constructLLAudioPlayer_soc(numChannel,gstPipeline ,aSrc,aSink,aFilter,aDecoder);
    }

// =========================================== DRM APIs ================================================
    int32_t Drmhal_Platform_Initialize(std::string DrmStorePath)
    {
        typedef int32_t (*Drmhal_Platform_Initialize_soc_Func)(std::string);
        static Drmhal_Platform_Initialize_soc_Func func = nullptr;
        if (!func) {
            func = (Drmhal_Platform_Initialize_soc_Func)dlsym(RTLD_DEFAULT, "Drmhal_Platform_Initialize_soc");
        }
        if (func) {
            return func(DrmStorePath);
        } else {
            // Handle error: symbol not found
            printf("Drmhal_Platform_Initialize_soc symbol not found via dlsym\n");
            return -1;
        }
    }

    uint32_t Drmhal_DeleteDrmStore(void* mDrmStore, std::string DrmStorePath)
    {
        typedef uint32_t (*Drmhal_DeleteDrmStore_soc_Func)(void*, std::string);
        static Drmhal_DeleteDrmStore_soc_Func func = nullptr;
        if (!func) {
            func = (Drmhal_DeleteDrmStore_soc_Func)dlsym(RTLD_DEFAULT, "Drmhal_DeleteDrmStore_soc");
        }
        if (func) {
            return func(mDrmStore, DrmStorePath);
        } else {
            printf("Drmhal_DeleteDrmStore_soc symbol not found via dlsym\n");
            return 0;
        }
    }

    bool Drmhal_QueryBatchIDFromLicenseRespone(void *pstdrmLicenseResponse, void *pstDRMBatchID)
    {
        typedef bool (*Drmhal_QueryBatchIDFromLicenseRespone_soc_Func)(void*, void*);
        static Drmhal_QueryBatchIDFromLicenseRespone_soc_Func func = nullptr;
        if (!func) {
            func = (Drmhal_QueryBatchIDFromLicenseRespone_soc_Func)dlsym(RTLD_DEFAULT, "Drmhal_QueryBatchIDFromLicenseRespone_soc");
        }
        if (func) {
            return func(pstdrmLicenseResponse, pstDRMBatchID);
        } else {
            printf("Drmhal_QueryBatchIDFromLicenseRespone_soc symbol not found via dlsym\n");
            return false;
        }
    }

    bool Drmhal_bindCallbackPrecheck(int f_dwCallbackType)
    {
        typedef bool (*Drmhal_bindCallbackPrecheck_soc_Func)(int);
        static Drmhal_bindCallbackPrecheck_soc_Func func = nullptr;
        if (!func) {
            func = (Drmhal_bindCallbackPrecheck_soc_Func)dlsym(RTLD_DEFAULT, "Drmhal_bindCallbackPrecheck_soc");
        }
        if (func) {
            return func(f_dwCallbackType);
        } else {
            printf("Drmhal_bindCallbackPrecheck_soc symbol not found via dlsym\n");
            return false;
        }
    }

    int32_t Drmhal_FetchOuptutProtectionConfigData(const void *f_pvCallbackData, int  f_dwCallbackType,
                                    const void *f_pKID, const void *f_pLID, const void *f_pv, max_resolution_update_cb cb)
    {
        typedef int32_t (*Drmhal_FetchOuptutProtectionConfigData_soc_Func)(const void*, int, const void*, const void*, const void*, max_resolution_update_cb);
        static Drmhal_FetchOuptutProtectionConfigData_soc_Func func = nullptr;
        if (!func) {
            func = (Drmhal_FetchOuptutProtectionConfigData_soc_Func)dlsym(RTLD_DEFAULT, "Drmhal_FetchOuptutProtectionConfigData_soc");
        }
        if (func) {
            return func(f_pvCallbackData, f_dwCallbackType, f_pKID, f_pLID, f_pv, cb);
        } else {
            printf("Drmhal_FetchOuptutProtectionConfigData_soc symbol not found via dlsym\n");
            return -1;
        }
    }

    int32_t Drmhal_PreDecrypt(void * mDecryptContext, void * mSVPContext, bool mPreallocMemoryForDecrypt,
                                        int f_cbEncryptedContent, void ** header, void * securehandle, int securehandleSz)
    {
        typedef int32_t (*Drmhal_PreDecrypt_soc_Func)(void*, void*, bool, int, void**, void*, int);
        static Drmhal_PreDecrypt_soc_Func func = nullptr;
        if (!func) {
            func = (Drmhal_PreDecrypt_soc_Func)dlsym(RTLD_DEFAULT, "Drmhal_PreDecrypt_soc");
        }
        if (func) {
            return func(mDecryptContext, mSVPContext, mPreallocMemoryForDecrypt, f_cbEncryptedContent, header, securehandle, securehandleSz);
        } else {
            printf("Drmhal_PreDecrypt_soc symbol not found via dlsym\n");            
            return -1;
        }
    }// =========================================== DRM abstract APIs ================================================
    
} // namespace rdk_gstreamer_utils
