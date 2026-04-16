/*
* If not stated otherwise in this file or this component's LICENSE file the
* following copyright and licenses apply:
*
* Copyright 2022 RDK Management
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
#include <gst/gst.h>
#include <glib.h>
#include <string.h>
#include <gst/app/gstappsrc.h>
#define GST_FIFO_SIZE_MS (48)
#define PLATFORM_VIRTUALDISPLAY_WIDTH 1280
#define PLATFORM_VIRTUALDISPLAY_HEIGHT 720
#define PLATFORM_AUDIODECODER "rialtomseaudiosink"
#define PLATFORM_VIDEODECODER "rialtomsevideosink"
#define PLATFORM_BUFFERUNDERFLOW_SIGNAL "buffer-underflow-callback"
#define PLATFORM_UIAUDIO_SINK "rialtowebaudiosink"

using namespace std;
namespace rdk_gstreamer_utils {
    enum audio_change_state
    {
        AUDCHG_INIT = 0,
        AUDCHG_CMD = 1,
        AUDCHG_SET = 2,
        AUDCHG_ALIGN = 3,
    };
    static bool mPassthroughEnabled = false;
    static GstElement* retrieveGstElementByName(GstElement *element, const char *targetName)
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
        *mVirtualDisplayHeight = PLATFORM_VIRTUALDISPLAY_HEIGHT;
        *mVirtualDisplayWidth = PLATFORM_VIRTUALDISPLAY_WIDTH;
    }
    bool installUnderflowCallbackFromPlatform(GstElement *pipeline, GCallback underflowVideoCallback, GCallback underflowAudioCallback, gpointer data)
    {
        const char* audiodecodername = PLATFORM_AUDIODECODER;
        const char* videoDecoderName = PLATFORM_VIDEODECODER;
        GstElement* audiodecoder = retrieveGstElementByName(pipeline, audiodecodername);
        GstElement* videodecoder = retrieveGstElementByName(pipeline, videoDecoderName);
        const char* AudioUnderflowSignal = PLATFORM_BUFFERUNDERFLOW_SIGNAL;
        const char* VideoUnderflowSignal = PLATFORM_BUFFERUNDERFLOW_SIGNAL;

        if (!audiodecoder)
        {
            g_warning("installUnderflowCallbackFromPlatform: Failed to find audio decoder element '%s' in pipeline", audiodecodername);
            return false;
        }

        if (!videodecoder)
        {
            g_warning("installUnderflowCallbackFromPlatform: Failed to find video decoder element '%s' in pipeline", videoDecoderName);
            return false;
        }

        gulong id_audio = g_signal_connect(audiodecoder, AudioUnderflowSignal, underflowAudioCallback, data);
        gulong id_video = g_signal_connect(videodecoder, VideoUnderflowSignal, underflowVideoCallback, data);
        return id_audio > 0 && id_video > 0;
    }
    bool IntialVolSettingNeeded()
    {
        return true;
    }
    bool isSocAudioFadeSupported()
    {
        return false;
    }
    void doAudioEasingonSoc(double target, uint32_t duration, rgu_Ease ease)
    {
        // no op
        return;
    }
    void setVideoProperty(GstElement *pipeline)
    {
        // no op. To be implemented if required later
        return;
    }
    void processAudioGap(GstElement *pipeline,gint64 gapstartpts,gint32 gapduration,gint64 gapdiscontinuity,bool audioaac)
    {
       if( (0 != gapduration) || (0 != gapdiscontinuity) )
       {
          const char* rialtoAudioSinkName = PLATFORM_AUDIODECODER;
          GstElement* rialtoAudioSink = retrieveGstElementByName(pipeline, rialtoAudioSinkName);
          if (!rialtoAudioSink)
          {
             LOG_RGU("processAudioGap: Failed to find audio sink '%s', ignoring gap parameters\n", rialtoAudioSinkName);
             return;
          }
          GstStructure *dataStruct = gst_structure_new("gap-params", "position", G_TYPE_INT64, gapstartpts, "duration",
                                                        G_TYPE_UINT, gapduration, "discontinuity-gap", G_TYPE_INT64,
                                                        gapdiscontinuity, "audio-aac", G_TYPE_BOOLEAN, audioaac, nullptr);
          LOG_RGU("processAudioGap StartPts[%lld] Duration [%u] gapdiscontinuity[%lld] aac[%d]\n", gapstartpts, gapduration, gapdiscontinuity, audioaac );
          g_object_set(rialtoAudioSink, "gap", dataStruct, nullptr);
          gst_structure_free(dataStruct);
       }
       return;
    }
    void enableAudioSwitch(GstElement *pipeline)
    {
        // no op. To be implemented if required later
        return;
    }
    GstElement * configureUIAudioSink(bool TTSenabled)
    {
        (void)TTSenabled;
        GstElement *audioSink = NULL;
        audioSink = gst_element_factory_make (PLATFORM_UIAUDIO_SINK,PLATFORM_UIAUDIO_SINK);
        return audioSink;
    }
    bool isUIAudioVGAudioMixSupported()
    {
        return true;
    }
    unsigned getNativeAudioFlag()
    {
        mPassthroughEnabled = false;
        return getGstPlayFlag("native-audio");
    }
    bool isPtsOffsetAdjustmentSupported()
    {
        return true;
    }
    int getPtsOffsetAdjustment(const std::string& audioCodecString)
    {
       return 0;
    }
    void configAudioCap(AudioAttributes *pAttrib, bool *audioaac, bool svpenabled, GstCaps **appsrcCaps, bool passthroughProperty)
    {
        gchar *caps_string;
        LOG_RGU("Config audio codec %s sampling rate %d channel %d alignment %d",
                pAttrib->mCodecParam.c_str(),
                pAttrib->mSamplesPerSecond,
                pAttrib->mNumberOfChannels,
                pAttrib->mBlockAlignment);
	    mPassthroughEnabled = passthroughProperty;
        if (pAttrib->mCodecParam.compare(0, 4, std::string("mp4a")) == 0)
        {
            LOG_RGU("####### Using AAC\n");
            caps_string = g_strdup_printf("audio/mpeg, mpegversion=4, enable-svp=(string)%s", svpenabled ? "true" : "false");
            *audioaac = true;
        }
        else if (pAttrib->mCodecParam.compare(0, 4, std::string("lpcm")) == 0)
        {
            LOG_RGU("####### Using LPCM via audio decoder\n");
            caps_string = g_strdup_printf(
                "audio/b-wav,format=S16LE,rate=(int)%u,channels=(int)%u,enable-svp=(string)%s,channel-mask=(bitmask)%u,layout=interleaved",
                pAttrib->mSamplesPerSecond,
                pAttrib->mNumberOfChannels,
                svpenabled ? "true" : "false",
                (pAttrib->mNumberOfChannels <= 2) ? 0x03 : 0x3f);
            *audioaac = false;
        }
        else
        {
            LOG_RGU("####### Using EAC3 \n");
            caps_string = g_strdup_printf("audio/x-eac3, framed=(boolean)true, rate=(int)%u, channels=(int)%u, alignment=(string)frame, enable-svp=(string)%s",
                                          pAttrib->mSamplesPerSecond,
                                          pAttrib->mNumberOfChannels,
                                          svpenabled ? "true" : "false");
            *audioaac = false;
        }
        *appsrcCaps = gst_caps_from_string(caps_string);
        g_free(caps_string);
    }
    
    bool performAudioTrackCodecChannelSwitch(struct rdkGstreamerUtilsPlaybackGrp *pgstUtilsPlaybackGroup,const void *pSampleAttr, AudioAttributes *pAudioAttr, uint32_t *pStatus, unsigned int *pui32Delay,
                                                 llong *pAudioChangeTargetPts,const llong *pcurrentDispPts, unsigned int *audio_change_stage, GstCaps **appsrcCaps,
                                                 bool *audioaac, bool svpenabled, GstElement *aSrc, bool *ret, bool passthroughProperty)
    {
       if (*pStatus != 0 || pSampleAttr == nullptr || pAudioAttr == nullptr ) {
                LOG_RGU( "performAudioTrackCodecChannelSwitch() No audio data ready yet");
                *pui32Delay = 100; //delay of 100ms
                return false;
        }
        LOG_RGU( "performAudioTrackCodecChannelSwitch(): Audio change current disp pts %lld\n",
              *pcurrentDispPts);
        gint64 currentPts = GST_CLOCK_TIME_NONE;
        // Rialto Server's pipeline should be in PAUSED state here. Query position before switching it to READY
        GstQuery* query = gst_query_new_position(GST_FORMAT_TIME);
        if (gst_element_query(pgstUtilsPlaybackGroup->gstPipeline, query))
           gst_query_parse_position(query, 0, &currentPts);
        gst_query_unref(query);
        GstEvent* flush_start = NULL;
        GstEvent* flush_stop  = NULL;
        *pAudioChangeTargetPts = *pcurrentDispPts;
        *audio_change_stage = AUDCHG_ALIGN;
        *pui32Delay = 0;
        LOG_RGU("performAudioTrackCodecChannelSwitch(): Sending flush-start event to rialto sink");
        flush_start = gst_event_new_flush_start();
        *ret = gst_element_send_event(aSrc, flush_start);
        if (!*ret)
             LOG_RGU("failed to send flush-start event");
        LOG_RGU("performAudioTrackCodecChannelSwitch(): Sending flush-stop event to rialto sink");
        flush_stop = gst_event_new_flush_stop(TRUE);
        *ret = gst_element_send_event(aSrc, flush_stop);
        if (!*ret)
          LOG_RGU("failed to send flush-stop event");
        GstElement *audioSinkGst = retrieveGstElementByName(pgstUtilsPlaybackGroup->gstPipeline, PLATFORM_AUDIODECODER);
        if( nullptr != audioSinkGst )
        {
            GstCaps *oldCaps = gst_caps_copy(*appsrcCaps);
            gchar *oldCapsString = gst_caps_to_string(oldCaps);
            LOG_RGU("performAudioTrackCodecChannelSwitch(): Old caps: %s" , oldCapsString);
            g_free(oldCapsString);
            gst_caps_unref(*appsrcCaps);
            *appsrcCaps = nullptr;
			rdk_gstreamer_utils::configAudioCap(pAudioAttr, audioaac, svpenabled, appsrcCaps);
            gchar *newCapsString = gst_caps_to_string(*appsrcCaps);
        	pgstUtilsPlaybackGroup->isAudioAAC = *audioaac;
            LOG_RGU("performAudioTrackCodecChannelSwitch(): New caps: %s" , newCapsString);
            g_free(newCapsString); 
            LOG_RGU("performAudioTrackCodecChannelSwitch(): Perform audio switch if caps changed");
            if (!gst_caps_is_equal(*appsrcCaps, oldCaps))
            {
               LOG_RGU("performAudioTrackCodecChannelSwitch(): Using new rialto audio switch method with property");
               GstStructure *eventStructure{gst_structure_new("switch-source", "caps", GST_TYPE_CAPS, *appsrcCaps, nullptr)};
               if (!gst_element_send_event(aSrc, gst_event_new_custom(GST_EVENT_CUSTOM_DOWNSTREAM, eventStructure)))
               {
                  LOG_RGU("performAudioTrackCodecChannelSwitch(): Send event failed");
               }
            }
            gst_caps_unref(oldCaps);
        }
       LOG_RGU("performAudioTrackCodecChannelSwitch():Exit" );
       return false;
    }
    void setAppSrcParams(GstElement *aSrc,MediaType mediatype)
    {
        if (mediatype == MEDIA_VIDEO)
            g_object_set(aSrc, "max-bytes", (guint64) 512 * 1024, NULL);
        else
            g_object_set(aSrc, "max-bytes", (guint64) 1 * 64 * 1024, NULL);
    }
    void setPixelAspectRatio(GstCaps ** ppCaps,GstCaps *appsrcCaps,uint32_t pixelAspectRatioX,uint32_t pixelAspectRatioY)
    {
        return;
    }
    void deepElementAdded(struct rdkGstreamerUtilsPlaybackGrp *pgstUtilsPlaybackGroup, GstBin* pipeline, GstBin* bin, GstElement* element)
    {
        gchar* elementName = gst_element_get_name(element);
        if (elementName) {
            LOG_RGU( "dElementAdded - Element Name = %s", elementName);
            if (g_strrstr(elementName, "rialtomseaudiosink")) {
                LOG_RGU( "limit-buffering-ms 1500\n" );
                g_object_set( G_OBJECT(element), "limit-buffering-ms", 1500, NULL); /* limit to 1500ms for faster audio switching */
                LOG_RGU( "setting use-buffering FALSE\n" );
                g_object_set( G_OBJECT(element), "use-buffering", FALSE, NULL);
            }
            if (g_strrstr(elementName, "rialtomsevideosink")) {
                    gboolean enableSyncmodeStreaming = (mPassthroughEnabled) ? FALSE : TRUE;
                    LOG_RGU( "setting syncmode-streaming %s\n", (mPassthroughEnabled) ? "FALSE" : "TRUE" );
                    g_object_set(G_OBJECT(element), "syncmode-streaming", enableSyncmodeStreaming, NULL);
            }
           g_free(elementName);
        }
        return;
    }
    void audioMixerGetDeviceInfo(uint32_t& preferredFrames, uint32_t& maximumFrames)
    {
        uint64_t maxBytes = GST_FIFO_SIZE_MS * 48 * 4;  // 48ms of PCM data at 48kHz = 2304 frames * 4 bytes
        maximumFrames = maxBytes / 4;
        preferredFrames = maximumFrames / 4;
    }
    size_t audioMixerGetBufferDelay(int64_t queuedBytes,int bufferDelayms)
    {
        return ((queuedBytes/256) * 64);
    }
    uint64_t audioMixerGetFifoSize()
    {
        return (GST_FIFO_SIZE_MS * 48 * 4);
    }
    void setVideoSinkMode(GstElement * videoSink)
    {
         return; //no op
    }
    static bool IsH265Stream(std::string codec)
    {
        bool retVal = false;
        if( codec.find(std::string("h265"))  != std::string::npos ||
            codec.find(std::string("hdr10")) != std::string::npos ||
            codec.find(std::string("dvhe"))  != std::string::npos ||
            codec.find(std::string("dvh1"))  != std::string::npos ||
            codec.find(std::string("hvc1"))  != std::string::npos ||
            codec.find(std::string("hev1"))  != std::string::npos ) {
            LOG_RGU("IsH265Stream found H265 stream, requested codec is %s", codec.c_str());
            retVal = true;
        }
        return retVal;
    }
    static bool IsAv1Stream(std::string codec)
    {
        bool retVal = false;
        if( codec.find(std::string("av1"))  != std::string::npos ) {
            LOG_RGU("IsAV1Stream found AV1 stream, requested codec is %s", codec.c_str());
            retVal = true;
        }
        return retVal;
    }
    
    void configVideoCap(std::string vCodec,uint32_t imageWidth,uint32_t imageHeight,uint32_t frameRateValue,uint32_t frameRateScale,bool svpEnabled,gchar **capsString)
    {
        if (IsH265Stream(vCodec))
        {
            LOG_RGU("####### Using HEVC codec\n");
            *capsString = g_strdup_printf("video/x-h265, alignment=(string)au, stream-format=(string)byte-stream, width=(int)%u, height=(int)%u, framerate=(fraction)%u/%u, enable-svp=(string)%s",
                                imageWidth,imageHeight,frameRateValue,frameRateScale,svpEnabled ? "true" : "false");
        } else if (IsAv1Stream(vCodec)) {
            LOG_RGU("####### Using AV1 codec\n");
            *capsString = g_strdup_printf("video/x-av1, alignment=(string)au, stream-format=(string)byte-stream, width=(int)%u, height=(int)%u, framerate=(fraction)%u/%u, enable-svp=(string)%s",
                                imageWidth,imageHeight,frameRateValue,frameRateScale,svpEnabled ? "true" : "false");
        }
        else
        {
            LOG_RGU("####### Using H264 codec\n");
            *capsString = g_strdup_printf("video/x-h264, alignment=(string)au, stream-format=(string)byte-stream, width=(int)%u, height=(int)%u, framerate=(fraction)%u/%u, enable-svp=(string)%s",
                                imageWidth,imageHeight,frameRateValue,frameRateScale,svpEnabled ? "true" : "false");
        }
    }
    uint64_t audioMixerGetQueuedBytes(uint64_t bytesPushed,uint64_t bytesPlayed)
    {
        return (bytesPushed-bytesPlayed);
    }
    void audioMixerConfigurePipeline(GstElement *gstPipeline,GstElement *aSink,GstElement *aSrc,bool attenuateOutput)
    {
        const float AUDIO_VOLUME_SCALE_FACTOR=1.0;
        g_object_set(G_OBJECT(aSink), "volume", 1.0 * AUDIO_VOLUME_SCALE_FACTOR, NULL);
        gst_bin_add_many (GST_BIN (gstPipeline), aSrc, aSink, NULL);
        gst_element_link_many (aSrc, aSink, NULL);
        if(attenuateOutput)
        {
           LOG_RGU("GstAudioMixerOutput: No Audio Equivalence, so attenuating mixer output");
           g_object_set(G_OBJECT(aSink), "volume", 0.5 * AUDIO_VOLUME_SCALE_FACTOR, NULL);
        }
        else
        {
           LOG_RGU("GstAudioMixerOutput: Audio Equivalence On, not attenuating mixer output");
        }
    }
    void switchToAudioMasterMode()
    {
        // no op. To be implemented if required later
        return;
    }
    
    void setKeyFrameFlag(GstBuffer *gstBuffer,bool val)
    {
        // no op. To be implemented if required later
        return;
    }
    bool getDelayTimerEnabled()
    {
        return true;
    }
    void SetAudioServerParam(bool enabled)
    {
        return;
    }
    void constructLLAudioPlayer(int numChannel ,GstElement *gstPipeline ,GstElement *aSrc,GstElement *aSink,GstElement *aFilter,GstElement *aDecoder)
    {
        LOG_RGU("constructLLAudioPlayer: NoChannel %d \n",numChannel);
        aSink = gst_element_factory_make("rialtomseaudiosink", NULL);
        /*  Rialto needs to know the number of streams on non playbin pipelines */
        GstContext *mGstContext = gst_context_new("streams-info", false);
        GstStructure *mGstContextStructure = gst_context_writable_structure(mGstContext);
        gst_structure_set(mGstContextStructure, "video-streams", G_TYPE_UINT, 0x0u, "audio-streams", G_TYPE_UINT, 0x1u,
                        "text-streams", G_TYPE_UINT, 0x0u, nullptr);
        gst_element_set_context(GST_ELEMENT(gstPipeline), mGstContext);
        gst_context_unref(mGstContext);
        g_object_set(G_OBJECT(aSink), "low-latency", TRUE, NULL);
        g_object_set(G_OBJECT(aSink), "sync", FALSE, NULL);
        if (numChannel == 6)
        {
            g_object_set(G_OBJECT(aSink), "sync-off", TRUE, NULL);       /* disable TSM, A/V sync */
            g_object_set(G_OBJECT(aSink), "stream_sync_mode", 0, NULL);  /* disable 2nd frame stream sync check, decode single frame if available */
        }
        gst_bin_add_many(GST_BIN(gstPipeline), aSrc, aSink, NULL);
        gst_element_link_many(aSrc, aSink, NULL);
   }
} // namespace rdk_gstreamer_utils