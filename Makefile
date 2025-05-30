#
# If not stated otherwise in this file or this component's LICENSE file the
# following copyright and licenses apply:
#
# Copyright 2019 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


EXTRA_CXXFLAGS = -Wno-attributes -Wall -g -fpermissive -std=c++1y -fPIC
<<<<<<< HEAD
EXTRA_LDFLAGS = -lgstreamer-1.0 -lglib-2.0 -Wl,-rpath=../../,-rpath=./ -lrdkgstreamerutilsplatform
=======
EXTRA_LDFLAGS = -lgstreamer-1.0 -lglib-2.0 -Wl,-rpath=../../,-rpath=./
>>>>>>> 5ba3d89b9e1098ea625ad82a1e42fec0b18ee7c9

SOURCES = rdk_gstreamer_utils.cpp

PLATFORM_FLAGS += -DBUILD_TYPE=\"Release\" -DCOMCAST_BUILD=1 -DCOMCAST_BUILD_OPL=1 -DCOMCAST_ENABLE_RDKDS=1 -DDRM_BUILD_PROFILE=900 -DDRM_COMPILE_FOR_NORMAL_WORLD=0 -DDRM_COMPILE_FOR_SECURE_WORLD=1 -DDRM_SUPPORT_3RDPARTY_REVOCATION=0 -DDRM_SUPPORT_ASSEMBLY=0 -DDRM_SUPPORT_DATASTORE_PREALLOC=1 -DDRM_SUPPORT_ECCPROFILING=0 -DDRM_SUPPORT_FILE_LOCKING=1 -DDRM_SUPPORT_FORCE_ALIGN=1 -DDRM_SUPPORT_INLINEDWORDCPY=0 -DDRM_SUPPORT_MULTI_THREADING=0 -DDRM_SUPPORT_NATIVE_64BIT_TYPES=1 -DDRM_SUPPORT_PRECOMPUTE_GTABLE=1 -DDRM_SUPPORT_PROFILING=0 -DDRM_SUPPORT_SECUREOEMHAL=0 -DDRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY=0 -DDRM_SUPPORT_TEE=3 -DDRM_SUPPORT_TOOLS_NET_IO=0 -DDRM_SUPPORT_TRACING=1 -DOEM_TEE_VERSION_BUILD=0 -DOEM_TEE_VERSION_MAJOR=0 -DOEM_TEE_VERSION_MINOR=1 -DOEM_TEE_VERSION_RELEASE=0 -DTARGET_SUPPORTS_UNALIGNED_DWORD_POINTERS=1 -DUSE_CIPHER_CACHING=1 -DUSE_HDCP_2X_SUPPORT=1 -DUSE_OPENSSL_ECC_VERIFY=1 -DUSE_PR3FEATURES_CHECK=1 -DUSE_SVP=1 -DUSE_TK_PRELOAD -DUSE_TYPEJ_KEYS=1 -Drdkplayready_EXPORTS

OBJS=$(addsuffix .o, $(basename $(SOURCES)))
LIBRDKGSTREAMERUTILS_LIB=librdkgstreamerutils.so

.phony: lib clean


lib: $(LIBRDKGSTREAMERUTILS_LIB)

%.o: %.cpp
	 @echo Compiling $<...
	 $(CXX) -c $< $(EXTRA_CXXFLAGS) $(CXXFLAGS) $(PLATFORM_FLAGS) -o $@

$(LIBRDKGSTREAMERUTILS_LIB):  $(OBJS)
	@echo Dynamic library creating $(OBJS) ...
	$(CXX) $(OBJS) $(EXTRA_LDFLAGS) -shared -fPIC -o $@
clean:
	@rm -rf $(LIBRDKGSTREAMERUTILS_LIB)
cleanall:
	@rm -rf $(LIBRDKGSTREAMERUTILS_LIB) *.o

