###############################################################
# Mongoose <stu7440@westga.edu>
###############################################################
# + Cleaner clean
# + Add new include tree
# + Add new flags
# + Install/Uninstall
# + Debian and Redhat packaging
# + Lots of misc new features
###############################################################
BUILD_SELECT=debug

NAME=OpenRaider
NAME_TAR=openraider
MAJOR_VERSION=0
MINOR_VERSION=1
MICRO_VERSION=2
BUILD_ID=$(shell date "+%Y%m%d")
PRE=
VERSION=$(MAJOR_VERSION).$(MINOR_VERSION).$(MICRO_VERSION)$(PRE)
VERSION_DEB=$(MAJOR_VERSION).$(MINOR_VERSION).$(MICRO_VERSION).$(BUILD_ID)
BUILD_HOST=$(shell uname -s -n -r -m)
ARCH=$(shell uname -m -s | sed -e "s/ /-/g")
UNAME=$(shell uname -s)

###############################################################

# -DMULTITEXTURE			Add OpenGL multitexturing
# -DUNICODE_SUPPORT			Add unicode/internation keyboard support
# -DUSING_EMITTER_IN_GAME	Run particle test in game

BASE_DEFS=$(shell sdl-config --cflags) -Isrc -DSDL_INTERFACE \
	-DUSING_OPENGL -DZLIB_SUPPORT -DUSING_EMITTER \
	-DUSING_OPENAL -DUSING_MTK_TGA -DUSING_PTHREADS \
	-DUSING_HEL -DHAVE_SDL_TTF

BASE_LIBS=$(shell sdl-config --libs) -lz -lstdc++ \
	-lpthread -lSDL_ttf

# -DDEBUG_GL
DEBUG_DEFS=-DDEBUG -DEXPERIMENTAL
DEBUG_OBJ=

ifeq ($(UNAME),Darwin)
AUDIO_LIBS += -lalut
AUDIO_LIBS += -framework OpenAL
AUDIO_LIBS += -L/usr/local/lib
AUDIO_DEFS += -I/usr/local/include
BASE_LIBS += -L/opt/local/lib
BASE_DEFS += -I/opt/local/include
BASE_LIBS += -framework OpenGL
BASE_LIBS += -framework GLUT
else
AUDIO_LIBS += -lopenal
BASE_LIBS += -L/usr/local/lib
BASE_DEFS += -I/usr/local/include
endif

BASE_LIBS += $(AUDIO_LIBS)
BASE_DEFS += $(AUDIO_DEFS)

# libferit, File transfer via HTTP/FTP/etc support
LIBFERIT_LIB=/usr/local/lib/libferit.so
LIBFERIT=$(shell if test -e $(LIBFERIT_LIB) > /dev/null; then echo yes; fi)

ifeq ($(LIBFERIT), yes)
	BASE_DEFS += -DHAVE_LIBFERIT
	BASE_LIBS += -lferit
endif

###############################################################

TREE_DIR=OpenRaider
BUILD_DEBUG_DIR=bin/debug
BUILD_RELEASE_DIR=bin/release
BUILD_PROF_DIR=bin/prof
BUILD_TEST_DIR=bin/test
BUILD_MEM_DIR=bin/mem
BUILD_INSTALL_DIR=bin/$(BUILD_SELECT)
DEB_DIR=/tmp/$(NAME).deb

# Edited for Debian GNU/Linux.
DESTDIR =
INSTALL_BIN=$(DESTDIR)/usr/games
INSTALL_LIB=$(DESTDIR)/usr/lib
INSTALL_DOC=$(DESTDIR)/usr/share/doc/$(NAME)
INSTALL_SHARE=$(DESTDIR)/usr/share/$(NAME)
INSTALL_INCLUDE=$(DESTDIR)/usr/include

###############################################################
CC=gcc

BASE_CFLAGS=-Wall $(BASE_DEFS) \
	-DVERSION=\"\\\"$(NAME)-$(VERSION)-$(BUILD_ID)\\\"\" \
	-DBUILD_HOST=\"\\\"$(BUILD_HOST)\\\"\"

LD_FLAGS=-L/usr/X11/lib -lXmu -lXt -lSM -lICE -lXext -lX11 -lXi \
	 -lm $(BASE_LIBS)

ifneq ($(UNAME),Darwin)
LD_FLAGS+=-lGL -lGLU
endif

RELEASE_CFLAGS=$(BASE_CFLAGS) -ffast-math -funroll-loops \
	-fomit-frame-pointer -O2

DEBUG_CFLAGS=$(BASE_CFLAGS) -g -O0 $(DEBUG_DEFS)

################################################################

DO_CC=$(CC) $(CFLAGS) -o $@ -c $<
DO_SHLIB_CC=$(CC) $(CFLAGS) $(SHLIBCFLAGS) -o $@ -c $<

TARGETS=$(BUILDDIR)/$(NAME)

################################################################
auto: $(BUILD_SELECT)

run: $(BUILD_SELECT)
	bin/$(BUILD_SELECT)/OpenRaider

targets: $(TARGETS)

bundle: release
	mac_dist/bundle.sh
	mac_dist/plist.sh $(NAME) $(VERSION) $(BUILD_ID) > bin/OpenRaider.app/Contents/Info.plist
	mac_dist/frameworks.sh

bundle-image: bundle
	mac_dist/image.sh

bundle-archive: bundle
	mac_dist/archive.sh

all: debug release prof

debug:
	@-mkdir -p $(BUILD_DEBUG_DIR)
	$(MAKE) targets BUILDDIR=$(BUILD_DEBUG_DIR) \
	CFLAGS="$(DEBUG_CFLAGS)" \
	LD_FLAGS="$(LD_FLAGS)"

prof:
	@-mkdir -p $(BUILD_PROF_DIR)
	$(MAKE) targets BUILDDIR=$(BUILD_PROF_DIR) \
	CFLAGS="$(DEBUG_CFLAGS) -pg" \
	LD_FLAGS="$(LD_FLAGS) -pg"

release:
	@-mkdir -p $(BUILD_RELEASE_DIR)
	$(MAKE) targets BUILDDIR=$(BUILD_RELEASE_DIR) \
	CFLAGS="$(RELEASE_CFLAGS)" \
	LD_FLAGS="$(LD_FLAGS)"

#####################################

ded:
	@-mkdir -p $(BUILD_DEBUG_DIR)/ded
	$(MAKE) targets BUILDDIR=$(BUILD_DEBUG_DIR)/ded \
	CFLAGS="$(DEBUG_CFLAGS) -DDEDICATED_SERVER" \
	LD_FLAGS="$(LD_FLAGS)"

# -DDEBUG_MEMEORY_VERBOSE
# -DDEBUG_MEMEORY
memory:
	@-mkdir -p $(BUILD_MEM_DIR)
	$(MAKE) targets BUILDDIR=$(BUILD_MEM_DIR) \
	DEBUG_OBJ="$(BUILD_MEM_DIR)/memory_test.o" \
	CFLAGS="$(DEBUG_CFLAGS) -DDEBUG_MEMEORY" \
	LD_FLAGS="$(LD_FLAGS)"

depend:
	@-echo "Making deps..."
	@-echo "# Autogenerated dependency file" > depend
	@-find ./src -name "*.cpp" -exec ./deps.sh $(BASE_DEFS) {} \; >> depend
	@-echo "[DONE]"

################################################################

# Later hel will become a seperate library once it matures
HEL_OBJ = \
	$(BUILDDIR)/Mass.o \
	$(BUILDDIR)/Simulation.o \
	$(BUILDDIR)/Vector3d.o \
	$(BUILDDIR)/Matrix.o \
	$(BUILDDIR)/ViewVolume.o \
	$(BUILDDIR)/CollisionObject.o \
	$(BUILDDIR)/BoundingVolume.o \
	$(BUILDDIR)/Quaternion.o \
	$(BUILDDIR)/math.o \
	$(BUILDDIR)/Entity.o
	#$(BUILDDIR)/Spring.o

OBJS = \
	$(DEBUG_OBJ) \
	$(HEL_OBJ) \
	$(BUILDDIR)/mtk_tga.o \
	$(BUILDDIR)/System.o \
	$(BUILDDIR)/SDLSystem.o \
	$(BUILDDIR)/OpenRaider.o \
	$(BUILDDIR)/World.o \
	$(BUILDDIR)/TombRaider.o \
	$(BUILDDIR)/GLString.o \
	$(BUILDDIR)/Camera.o \
	$(BUILDDIR)/OpenGLMesh.o \
	$(BUILDDIR)/Render.o \
	$(BUILDDIR)/Sound.o \
	$(BUILDDIR)/Network.o \
	$(BUILDDIR)/Texture.o \
	$(BUILDDIR)/Emitter.o \
	$(BUILDDIR)/Particle.o \
	$(BUILDDIR)/SkeletalModel.o \
	$(BUILDDIR)/Light.o


$(BUILDDIR)/$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) $(LD_FLAGS) -o $@ $(OBJS)

#################################################################

clean: clean-small clean-dep

clean-small: clean-build clean-test clean-obj 
	@-rm -rf bin/OpenRaider.app
	@-rm -rf bin/OpenRaider.dmg
	@-rm -rf bin/OpenRaider.zip

clean-dep:
	@-echo "Cleaning dependencies"
	@-rm -f depend
	@-echo "[DONE]"

clean-test:
	@-echo "Cleaning test builds"
	@-rm -f $(BUILD_TEST_DIR)/*.o
	@-rm -f $(BUILD_TEST_DIR)/*.test
	@-rm -rf $(BUILD_TEST_DIR)/*.build
	@-echo "[DONE]"

clean-obj:
	@-echo "Cleaning objects"
	@-rm -f $(BUILD_PROF_DIR)/*.o
	@-rm -f $(BUILD_DEBUG_DIR)/*.o
	@-rm -f $(BUILD_RELEASE_DIR)/*.o
	@-rm -f $(BUILD_TEST_DIR)/*.o
	@-rm -f $(BUILD_MEM_DIR)/*.o
	@-echo "[DONE]"

clean-build:
	@-echo "Cleaning builds"
	@-rm -f $(BUILD_PROF_DIR)/$(NAME)
	@-rm -f $(BUILD_DEBUG_DIR)/$(NAME)
	@-rm -f $(BUILD_RELEASE_DIR)/$(NAME)
	@-rm -f $(BUILD_MEM_DIR)/$(NAME)
	@-echo "[DONE]"


#################################################################

-include depend

#################################################################

ifneq ($(UNAME),Darwin)

install:
	mkdir -p $(INSTALL_SHARE)/data
	cp setup.sh $(INSTALL_SHARE)
	cp data/* $(INSTALL_SHARE)/data
	mkdir -p $(INSTALL_DOC)
	cp README.md README.old ChangeLog
	mkdir -p $(INSTALL_BIN)
	cp bin/$(BUILD_SELECT)/OpenRaider $(INSTALL_BIN)

bin-tarball: clean-build clean-test clean-obj $(BUILD_SELECT)
	@-cd .. && tar zcvf $(NAME_TAR)-$(VERSION_DEB)-$(ARCH).tar.gz \
		$(TREE_DIR)/Makefile $(TREE_DIR)/data \
		$(TREE_DIR)/bin/$(BUILD_SELECT)/OpenRaider \
		$(TREE_DIR)/README.md $(TREE_DIR)/ChangeLog

endif

#################################################################
# Unit Test builds
#################################################################
TEST_FLAGS=-Wall -g -O0 -DDEBUG -lstdc++

TEST_MAP_TR5=~/projects/Data/models/tombraider/tr5/demo.trc
TEST_MAP_TR4=~/projects/Data/models/tombraider/tr4/angkor1.tr4
TEST_MAP_TR3=~/projects/Data/models/tombraider/tr3/scotland.tr2
TEST_MAP_TR2=~/projects/Data/models/tombraider/tr2/unwater.tr2
TEST_MAP_TR1=~/projects/Data/models/tombraider/tr1/level1.phd

TombRaider.reg_test:
	$(MAKE) TombRaider.test
	$(BUILD_TEST_DIR)/TombRaider.test load $(TEST_MAP_TR1) > /tmp/log.tr1
	$(BUILD_TEST_DIR)/TombRaider.test load $(TEST_MAP_TR2) > /tmp/log.tr2
	$(BUILD_TEST_DIR)/TombRaider.test load $(TEST_MAP_TR3) > /tmp/log.tr3
	$(BUILD_TEST_DIR)/TombRaider.test load $(TEST_MAP_TR4) > /tmp/log.tr4
	$(BUILD_TEST_DIR)/TombRaider.test load $(TEST_MAP_TR5) > /tmp/log.tr5


TombRaider.test:
	@-mkdir -p $(BUILD_TEST_DIR)
	$(MAKE) targets NAME=TombRaider.test BUILDDIR=$(BUILD_TEST_DIR) \
	OBJS="$(BUILD_TEST_DIR)/TombRaider.o $(BUILD_TEST_DIR)/mtk_tga.o $(BUILD_TEST_DIR)/memeory_test.o" \
	CFLAGS="$(BASE_CFLAGS) -g -D__TOMBRAIDER_TEST__ -D__TEST_TR5_DUMP_TGA -D__TEST_32BIT_TEXTILES -DDEBUG_MEMEORY" \
	LD_FLAGS="-lz -lstdc++"

#################################################################

ifeq ($(UNAME),Darwin)
GLString.test:
	mkdir -p $(BUILD_TEST_DIR)
	$(CC) -Wall -Isrc -D__TEST__ -DHAVE_MTK -DHAVE_SDL -DUSING_MTK_TGA \
	$(shell sdl-config --cflags) $(shell sdl-config --libs) \
	-I/opt/local/include -I/usr/local/include \
	-framework OpenGL \
	-framework GLUT \
	-lm -lstdc++ \
	src/Texture.cpp src/mtk_tga.cpp \
	src/GLString.cpp -o $(BUILD_TEST_DIR)/GLString.test
else
GLString.test:
	mkdir -p $(BUILD_TEST_DIR)
	$(CC) -Wall -Isrc -D__TEST__ -DHAVE_MTK -DHAVE_SDL -DUSING_MTK_TGA \
	$(shell sdl-config --cflags) $(shell sdl-config --libs) \
	-lGL -lGLU -lm \
	src/Texture.cpp src/mtk_tga.cpp \
	src/GLString.cpp -o $(BUILD_TEST_DIR)/GLString.test
endif

#################################################################

# CollisionObject and HeightCollisionObject seem to be missing?
# -- xythobuz, 2014-01-04

SIMULATION_CFLAGS=-Wall -O0 -g -Isrc $(shell sdl-config --cflags) \
		-I/opt/local/include \
		-DUSING_OPENGL -DUSING_HEL -DUSING_MTK_TGA \
		-DUNIT_TEST_SIMULATION
SIMULATION_OBJS=$(BUILD_TEST_DIR)/mtk_tga.o $(BUILD_TEST_DIR)/Texture.o \
		$(BUILD_TEST_DIR)/Mass.o \
		$(BUILD_TEST_DIR)/CollisionObject.o \
		$(BUILD_TEST_DIR)/Simulation.o \
		$(BUILD_TEST_DIR)/Vector3d.o \
		$(BUILD_TEST_DIR)/Matrix.o \
		$(BUILD_TEST_DIR)/ViewVolume.o \
		$(BUILD_TEST_DIR)/BoundingVolume.o \
		$(BUILD_TEST_DIR)/Quaternion.o \
		$(BUILD_TEST_DIR)/math.o

Simulation.test:
	mkdir -p $(BUILD_TEST_DIR)
	$(MAKE) targets NAME=Simulation.test BUILDDIR=$(BUILD_TEST_DIR) \
		OBJS="$(SIMULATION_OBJS)" \
		CFLAGS="$(SIMULATION_CFLAGS)" \
		LD_FLAGS="-lm -lstdc++ -lSDL_ttf -lSDL -lGL -lGLU $(shell sdl-config --libs)"

#################################################################

# Spring files seem to be missing?
# -- xythobuz, 2014-01-04

HEL_PARTICLE_CFLAGS=-Wall -O0 -g -Isrc $(shell sdl-config --cflags) \
		-I/opt/local/include \
		-DUSING_OPENGL -DUSING_HEL -DUSING_MTK_TGA \
		-DUNIT_TEST_SPRING -DHAVE_SDL_TTF
HEL_PARTICLE_OBJS=$(BUILD_TEST_DIR)/mtk_tga.o $(BUILD_TEST_DIR)/Texture.o \
		$(BUILD_TEST_DIR)/Mass.o \
		$(BUILD_TEST_DIR)/Spring.o \
		$(BUILD_TEST_DIR)/CollisionObject.o \
		$(BUILD_TEST_DIR)/Simulation.o \
		$(BUILD_TEST_DIR)/Vector3d.o \
		$(BUILD_TEST_DIR)/Matrix.o \
		$(BUILD_TEST_DIR)/ViewVolume.o \
		$(BUILD_TEST_DIR)/BoundingVolume.o \
		$(BUILD_TEST_DIR)/Quaternion.o \
		$(BUILD_TEST_DIR)/math.o

Spring.test:
	mkdir -p $(BUILD_TEST_DIR)
	$(MAKE) targets NAME=Spring.test BUILDDIR=$(BUILD_TEST_DIR) \
		OBJS="$(HEL_PARTICLE_OBJS)" \
		CFLAGS="$(HEL_PARTICLE_CFLAGS)" \
		LD_FLAGS="-lm -lstdc++ -lSDL_ttf-lGL -lGLU $(shell sdl-config --libs)"

#################################################################

Hel.test: Quaternion.test Matrix.test Math.test

Matrix.test:
	@-echo "Building Matrix unit test"
	mkdir -p $(BUILD_TEST_DIR)
	$(CC) -Wall -g -DMATRIX_UNIT_TEST -lm -lstdc++ -Isrc \
	src/hel/Matrix.cpp src/hel/Quaternion.cpp src/hel/Vector3d.cpp \
	-o $(BUILD_TEST_DIR)/Matrix.test
	@-echo "================================================="
	@-echo "Running Matrix unit test"
	$(BUILD_TEST_DIR)/Matrix.test

Quaternion.test:
	@-echo "Building Quaternion unit test"
	mkdir -p $(BUILD_TEST_DIR)
	$(CC) -Wall -g -DUNIT_TEST_QUATERNION -lm -lstdc++ -Isrc \
	src/hel/Quaternion.cpp -o $(BUILD_TEST_DIR)/Quaternion.test
	@-echo "================================================="
	@-echo "Running Quaternion unit test"
	$(BUILD_TEST_DIR)/Quaternion.test

Math.test:
	@-echo "Building Math unit test"
	mkdir -p $(BUILD_TEST_DIR)
	$(CC) -Wall -g -DMATH_UNIT_TEST -lm -lstdc++ -Isrc \
	src/hel/math.cpp src/hel/Vector3d.cpp -o $(BUILD_TEST_DIR)/Math.test
	@-echo "================================================="
	@-echo "Running hel unit test"
	$(BUILD_TEST_DIR)/Math.test

#################################################################

Memory.test:
	mkdir -p $(BUILD_TEST_DIR)
	$(CC) -Wall -g -D__TEST__ -lstdc++ \
	-DDEBUG_MEMEORY -DDEBUG_MEMEORY_ERROR \
	src/memeory_test.cpp -o $(BUILD_TEST_DIR)/memory_test.test

#################################################################

Network.test:
	mkdir -p $(BUILD_TEST_DIR)
	$(CC) $(TEST_FLAGS) -DUNIT_TEST_NETWORK \
	src/Network.cpp -o $(BUILD_TEST_DIR)/Network.test

#################################################################

Sound.test:
	mkdir -p $(BUILD_TEST_DIR)
	$(CC) $(TEST_FLAGS) -DUNIT_TEST_SOUND \
		-DUSING_OPENAL $(AUDIO_LIBS) \
		src/Sound.cpp -o $(BUILD_TEST_DIR)/Sound.test
ifeq ($(UNAME),Darwin)
	install_name_tool -change libalut.0.1.0.dylib /opt/local/lib/libalut.0.1.0.dylib $(BUILD_TEST_DIR)/Sound.test
endif

#################################################################

