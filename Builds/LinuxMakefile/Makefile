# Automatically generated makefile, created by the Introjucer
# Don't edit this file! Your changes will be overwritten when you re-save the Introjucer project!

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifndef CONFIG
  CONFIG=Debug
endif

ifeq ($(CONFIG),Debug)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Debug
  OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=native
  endif

  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "DEBUG=1" -D "_DEBUG=1" -D "LUA_USE_LINUX" -D "JUCER_LINUX_MAKE_6D53C8B4=1" -D "JUCE_APP_VERSION=0.1.1" -D "JUCE_APP_VERSION_HEX=0x101" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode -I ../../../JUCE/modules
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0
  CXXFLAGS += $(CFLAGS) -std=c++11
  LDFLAGS += $(TARGET_ARCH) -L$(BINDIR) -L$(LIBDIR) -L/usr/X11R6/lib/ -lX11 -lXext -lXinerama -ldl -lfreetype -lpthread -lrt 

  TARGET := tremlauncher
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
  CLEANCMD = rm -rf $(OUTDIR)/$(TARGET) $(OBJDIR)
endif

ifeq ($(CONFIG),Release)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Release
  OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=native
  endif

  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "NDEBUG=1" -D "LUA_USE_LINUX" -D "JUCER_LINUX_MAKE_6D53C8B4=1" -D "JUCE_APP_VERSION=0.1.1" -D "JUCE_APP_VERSION_HEX=0x101" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode -I ../../../JUCE/modules
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -O3
  CXXFLAGS += $(CFLAGS) -std=c++11
  LDFLAGS += $(TARGET_ARCH) -L$(BINDIR) -L$(LIBDIR) -fvisibility=hidden -L/usr/X11R6/lib/ -lX11 -lXext -lXinerama -ldl -lfreetype -lpthread -lrt 

  TARGET := tremlauncher
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
  CLEANCMD = rm -rf $(OUTDIR)/$(TARGET) $(OBJDIR)
endif

OBJECTS := \
  $(OBJDIR)/sexp2bignum_223379b8.o \
  $(OBJDIR)/sexp_11b07bca.o \
  $(OBJDIR)/bignum_382783c6.o \
  $(OBJDIR)/gmp-glue_37a87d8.o \
  $(OBJDIR)/mini-gmp_9f11f8d4.o \
  $(OBJDIR)/pkcs1-rsa-sha256_69cbd2db.o \
  $(OBJDIR)/pkcs1_8ec2d430.o \
  $(OBJDIR)/rsa-sha256-verify_14df81dc.o \
  $(OBJDIR)/rsa-verify_2fc24066.o \
  $(OBJDIR)/rsa_7426e34a.o \
  $(OBJDIR)/sexp2rsa_91f60298.o \
  $(OBJDIR)/sha256-compress_8cc283d2.o \
  $(OBJDIR)/sha256_db8cc727.o \
  $(OBJDIR)/write-be32_eead0850.o \
  $(OBJDIR)/Component_efa9ebde.o \
  $(OBJDIR)/Crypto_b29bc7ca.o \
  $(OBJDIR)/IniFile_b5c6ffe1.o \
  $(OBJDIR)/Main_90ebc5c2.o \
  $(OBJDIR)/PublicKey_a4c2f997.o \
  $(OBJDIR)/Semver_b52214f1.o \
  $(OBJDIR)/SplashComponent_1ae79fb7.o \
  $(OBJDIR)/SplashWindow_b4770640.o \
  $(OBJDIR)/Updater_e325a4aa.o \
  $(OBJDIR)/BinaryData_ce4232d4.o \
  $(OBJDIR)/juce_core_9d6b33ad.o \
  $(OBJDIR)/juce_data_structures_dcd877a9.o \
  $(OBJDIR)/juce_events_e5b201a1.o \
  $(OBJDIR)/juce_graphics_a34b2885.o \
  $(OBJDIR)/juce_gui_basics_3e4cc581.o \

.PHONY: clean

$(OUTDIR)/$(TARGET): $(OBJECTS) $(RESOURCES)
	@echo Linking Tremulous Launcher
	-@mkdir -p $(BINDIR)
	-@mkdir -p $(LIBDIR)
	-@mkdir -p $(OUTDIR)
	@$(BLDCMD)

clean:
	@echo Cleaning Tremulous Launcher
	@$(CLEANCMD)

strip:
	@echo Stripping Tremulous Launcher
	-@strip --strip-unneeded $(OUTDIR)/$(TARGET)

$(OBJDIR)/sexp2bignum_223379b8.o: ../../Source/nettle/sexp2bignum.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling sexp2bignum.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/sexp_11b07bca.o: ../../Source/nettle/sexp.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling sexp.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/bignum_382783c6.o: ../../Source/nettle/bignum.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling bignum.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/gmp-glue_37a87d8.o: ../../Source/nettle/gmp-glue.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling gmp-glue.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/mini-gmp_9f11f8d4.o: ../../Source/nettle/mini-gmp.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling mini-gmp.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/pkcs1-rsa-sha256_69cbd2db.o: ../../Source/nettle/pkcs1-rsa-sha256.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling pkcs1-rsa-sha256.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/pkcs1_8ec2d430.o: ../../Source/nettle/pkcs1.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling pkcs1.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/rsa-sha256-verify_14df81dc.o: ../../Source/nettle/rsa-sha256-verify.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling rsa-sha256-verify.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/rsa-verify_2fc24066.o: ../../Source/nettle/rsa-verify.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling rsa-verify.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/rsa_7426e34a.o: ../../Source/nettle/rsa.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling rsa.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/sexp2rsa_91f60298.o: ../../Source/nettle/sexp2rsa.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling sexp2rsa.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/sha256-compress_8cc283d2.o: ../../Source/nettle/sha256-compress.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling sha256-compress.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/sha256_db8cc727.o: ../../Source/nettle/sha256.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling sha256.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/write-be32_eead0850.o: ../../Source/nettle/write-be32.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling write-be32.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Component_efa9ebde.o: ../../Source/Component.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Component.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Crypto_b29bc7ca.o: ../../Source/Crypto.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Crypto.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/IniFile_b5c6ffe1.o: ../../Source/IniFile.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling IniFile.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Main_90ebc5c2.o: ../../Source/Main.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Main.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/PublicKey_a4c2f997.o: ../../Source/PublicKey.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling PublicKey.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Semver_b52214f1.o: ../../Source/Semver.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Semver.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SplashComponent_1ae79fb7.o: ../../Source/SplashComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SplashComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SplashWindow_b4770640.o: ../../Source/SplashWindow.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SplashWindow.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Updater_e325a4aa.o: ../../Source/Updater.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Updater.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/BinaryData_ce4232d4.o: ../../JuceLibraryCode/BinaryData.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling BinaryData.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_core_9d6b33ad.o: ../../../JUCE/modules/juce_core/juce_core.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_core.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_data_structures_dcd877a9.o: ../../../JUCE/modules/juce_data_structures/juce_data_structures.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_data_structures.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_events_e5b201a1.o: ../../../JUCE/modules/juce_events/juce_events.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_events.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_graphics_a34b2885.o: ../../../JUCE/modules/juce_graphics/juce_graphics.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_graphics.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_gui_basics_3e4cc581.o: ../../../JUCE/modules/juce_gui_basics/juce_gui_basics.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_gui_basics.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

-include $(OBJECTS:%.o=%.d)
