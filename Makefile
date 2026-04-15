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


EXTRA_CXXFLAGS = -Wno-attributes -Wall -g -fpermissive -std=c++1y -fPIC -I.
EXTRA_LDFLAGS = -lgstreamer-1.0 -lglib-2.0 -Wl,-rpath=../../,-rpath=./ -lrdkgstreamerutilsplatform
RIALTO_CLIENT_EXTRA_LDFLAGS = -lgstreamer-1.0 -lglib-2.0 -Wl,-rpath=../../,-rpath=./ 

SOURCES = rdk_gstreamer_utils.cpp
RIALTO_CLIENT_SOURCES = rialto/rdk_gstreamer_utils_rltclient.cpp

OBJS = $(SOURCES:.cpp=.o)
RIALTO_CLIENT_OBJS = $(RIALTO_CLIENT_SOURCES:.cpp=.o)

LIBRDKGSTREAMERUTILS_LIB=librdkgstreamerutils.so
LIBRDKGSTREAMERUTILS_RLT_CLIENT_LIB=librdkgstreamerrltclientutils.so

.PHONY: lib clean cleanall

lib: $(LIBRDKGSTREAMERUTILS_LIB) $(LIBRDKGSTREAMERUTILS_RLT_CLIENT_LIB)

%.o: %.cpp
	@echo Compiling $<...
	$(CXX) -c $< $(EXTRA_CXXFLAGS) $(CXXFLAGS) $(PLATFORM_FLAGS) -o $@

$(LIBRDKGSTREAMERUTILS_LIB): $(OBJS)
	@echo Creating $@ ...
	$(CXX) $(OBJS) $(EXTRA_LDFLAGS) -shared -o $@

$(LIBRDKGSTREAMERUTILS_RLT_CLIENT_LIB): $(RIALTO_CLIENT_OBJS)
	@echo Creating $@ ...
	$(CXX) $(RIALTO_CLIENT_OBJS) $(RIALTO_CLIENT_EXTRA_LDFLAGS) -shared -o $@

clean:
	rm -f $(LIBRDKGSTREAMERUTILS_LIB) $(LIBRDKGSTREAMERUTILS_RLT_CLIENT_LIB)

cleanall:
	rm -f $(LIBRDKGSTREAMERUTILS_LIB) $(LIBRDKGSTREAMERUTILS_RLT_CLIENT_LIB) *.o rialto/*.o
