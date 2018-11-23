# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

include_directories(/Users/wandun/Documents/freetype/include)

add_library(freetype
            SHARED
            IMPORTED)

set_target_properties(
            freetype
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libfreetype.so
)

set(opengl-bil GLESv2)

include_directories(/Users/wandun/Qt5.9.5/5.9.5/android_armv7/include)
include_directories(/usr/local/Cellar/boost/1.67.0_1/include/boost/)

add_library(Qt5Core
            SHARED
            IMPORTED)

set_target_properties(
            Qt5Core
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libQt5Core.so
)

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/native-lib.cpp
             src/main/jni/libdxfrw/libdxfrw.cpp
             src/main/jni/libdxfrw/libdwgr.cpp
             src/main/jni/libdxfrw/drw_header.cpp
             src/main/jni/libdxfrw/drw_classes.cpp
             src/main/jni/libdxfrw/drw_entities.cpp
            src/main/jni/libdxfrw/drw_objects.cpp
            src/main/jni/libdxfrw/intern/drw_textcodec.cpp
            src/main/jni/libdxfrw/intern/dxfreader.cpp
            src/main/jni/libdxfrw/intern/dxfwriter.cpp
            src/main/jni/libdxfrw/intern/dwgreader.cpp
            src/main/jni/libdxfrw/intern/dwgbuffer.cpp
            src/main/jni/libdxfrw/intern/drw_dbg.cpp
            src/main/jni/libdxfrw/intern/dwgreader21.cpp
            src/main/jni/libdxfrw/intern/dwgreader18.cpp
            src/main/jni/libdxfrw/intern/dwgreader15.cpp
            src/main/jni/libdxfrw/intern/dwgutil.cpp
            src/main/jni/libdxfrw/intern/rscodec.cpp
            src/main/jni/libdxfrw/intern/dwgreader27.cpp
            src/main/jni/libdxfrw/intern/dwgreader24.cpp
            src/main/jni/libdxfrw/libdxfrw.h
            src/main/jni/libdxfrw/libdwgr.h
            src/main/jni/libdxfrw/drw_interface.h
            src/main/jni/libdxfrw/drw_base.h
            src/main/jni/libdxfrw/drw_header.h
            src/main/jni/libdxfrw/drw_classes.h
            src/main/jni/libdxfrw/drw_entities.h
            src/main/jni/libdxfrw/drw_objects.h
            src/main/jni/libdxfrw/intern/drw_textcodec.h
            src/main/jni/libdxfrw/intern/dxfreader.h
            src/main/jni/libdxfrw/intern/dxfwriter.h
            src/main/jni/libdxfrw/intern/dwgreader.h
            src/main/jni/libdxfrw/intern/dwgbuffer.h
            src/main/jni/libdxfrw/intern/drw_cptables.h
            src/main/jni/libdxfrw/intern/drw_cptable950.h
            src/main/jni/libdxfrw/intern/drw_cptable949.h
            src/main/jni/libdxfrw/intern/drw_cptable936.h
            src/main/jni/libdxfrw/intern/drw_cptable932.h
            src/main/jni/libdxfrw/intern/drw_dbg.h
            src/main/jni/libdxfrw/intern/dwgreader21.h
            src/main/jni/libdxfrw/intern/dwgreader18.h
            src/main/jni/libdxfrw/intern/dwgreader15.h
            src/main/jni/libdxfrw/intern/dwgutil.h
            src/main/jni/libdxfrw/intern/rscodec.h
            src/main/jni/libdxfrw/intern/dwgreader27.h
            src/main/jni/libdxfrw/intern/dwgreader24.h
            src/main/jni/filters/rs_filterdxfrw.cpp
            src/main/jni/filters/rs_filterdxfrw.h
            src/main/jni/filters/rs_filterinterface.h
             src/main/jni/engine/rs_font.h
                        src/main/jni/engine/rs_font.cpp
            src/main/jni/debug/rs_debug.cpp
            src/main/jni/debug/rs_debug.h
            src/main/jni/engine/rs_vector.cpp
            src/main/jni/engine/rs_vector.h
            src/main/jni/engine/rs_settings.cpp
            src/main/jni/engine/rs_settings.h
            src/main/jni/engine/rs_units.cpp
            src/main/jni/engine/rs_units.h
            src/main/jni/engine/lc_rect.cpp
            src/main/jni/engine/lc_rect.h
            src/main/jni/engine/rs_polyline.cpp
            src/main/jni/engine/rs_polyline.h
            src/main/jni/engine/rs_line.cpp
            src/main/jni/engine/rs_line.h
            src/main/jni/engine/rs_flags.cpp
            src/main/jni/engine/rs_flags.h
            src/main/jni/engine/rs_entity.cpp
            src/main/jni/engine/rs_entity.h
            src/main/jni/engine/rs_arc.cpp
            src/main/jni/engine/rs_arc.h
            src/main/jni/engine/rs_layer.cpp
            src/main/jni/engine/rs_layer.h
            src/main/jni/engine/rs_entitycontainer.cpp
            src/main/jni/engine/rs_entitycontainer.h
            src/main/jni/engine/rs_document.cpp
            src/main/jni/engine/rs_document.h
            src/main/jni/engine/rs_atomicentity.cpp
            src/main/jni/engine/rs_atomicentity.h
            src/main/jni/engine/rs_block.cpp
            src/main/jni/engine/rs_block.h
            src/main/jni/engine/rs_layerlist.cpp
            src/main/jni/engine/rs_layerlist.h
            src/main/jni/engine/rs_blocklistlistener.h
            src/main/jni/engine/rs_blocklist.cpp
            src/main/jni/engine/rs_blocklist.h
            src/main/jni/engine/rs_graphic.cpp
            src/main/jni/engine/rs_graphic.h
            src/main/jni/engine/rs_variable.h
            src/main/jni/engine/rs_variabledict.h
            src/main/jni/engine/rs_variabledict.cpp
            src/main/jni/math/rs_math.cpp
            src/main/jni/math/rs_math.h
           src/main/jni/muparser/muParserTokenReader.h
           src/main/jni/muparser/muParserToken.h
           src/main/jni/muparser/muParserStack.h
           src/main/jni/muparser/muParserFixes.h
           src/main/jni/muparser/muParserError.h
           src/main/jni/muparser/muParserDef.h
           src/main/jni/muparser/muParserCallback.h
           src/main/jni/muparser/muParserBytecode.h
           src/main/jni/muparser/muParserBase.h
           src/main/jni/muparser/muParser.h
           src/main/jni/muparser/muParserTemplateMagic.h
           src/main/jni/muparser/muParserTokenReader.cpp
           src/main/jni/muparser/muParserError.cpp
           src/main/jni/muparser/muParserCallback.cpp
           src/main/jni/muparser/muParserBytecode.cpp
           src/main/jni/muparser/muParserBase.cpp
           src/main/jni/muparser/muParser.cpp
             )





# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )



# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib Qt5Core

                       GLESv2

                       freetype

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )