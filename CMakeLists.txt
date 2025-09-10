cmake_minimum_required(VERSION 3.10)  # Updated from 3.5
project(FloatingNavbar)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable automatic MOC/UIC/RCC for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6 modules
find_package(Qt6 REQUIRED COMPONENTS
        Core
        Widgets
        Network
        MultimediaWidgets
        Multimedia
        WebSockets
        Concurrent
)

# Add QHotkey
add_subdirectory(external/QHotkey)

# Define libfvad include path
set(LIBFVAD_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/libfvad/include)

# Collect libfvad source files manually
set(FVAD_SOURCES
        external/libfvad/src/fvad.c
        external/libfvad/src/vad/vad_core.c
        external/libfvad/src/vad/vad_filterbank.c
        external/libfvad/src/vad/vad_gmm.c
        external/libfvad/src/vad/vad_sp.c
        external/libfvad/src/signal_processing/division_operations.c
        external/libfvad/src/signal_processing/energy.c
        external/libfvad/src/signal_processing/get_scaling_square.c
        external/libfvad/src/signal_processing/resample_48khz.c
        external/libfvad/src/signal_processing/resample_by_2_internal.c
        external/libfvad/src/signal_processing/resample_fractional.c
        external/libfvad/src/signal_processing/spl_inl.c
)

# Your source and header files
set(SOURCES
        main.cpp
        navbar.cpp
        audiowsclient.cpp
        MessagePopup.cpp
        TranscriptWindow.cpp
        PromptWindow.cpp
        moreoptionsdialog.cpp
        ScreenshotResponseWindow.cpp
        PromptLoader.cpp
        ScreenshotHandler.cpp
        GroqStreamer.cpp
        ConfigUtil.cpp
        SettingsDialog.cpp
        AudioCaptureManager.cpp
        QuestionExtractor.cpp
        VadProcessor.cpp
        ModelSelectionDialog.cpp
)

set(HEADERS
        navbar.h
        audiowsclient.h
        MessagePopup.h
        TranscriptWindow.h
        PromptWindow.h
        moreoptionsdialog.h
        PromptLoader.h
        ScreenshotHandler.h
        GroqStreamer.h
        ConfigUtil.h
        ScreenshotResponseWindow.h
        AudioDeviceSelector.h
        SettingsDialog.h
        VadProcessor.h
        QuestionExtractor.h
        AudioCaptureManager.h
        ModelSelectionDialog.h
        PipeServer.cpp
        PipeServer.h
        PipeWorker.cpp
        PipeWorker.h
)

# Add the executable with all sources
add_executable(FloatingNavbar ${SOURCES} ${HEADERS} ${FVAD_SOURCES})

# Include directories
target_include_directories(FloatingNavbar PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/QHotkey/QHotkey
        ${LIBFVAD_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(FloatingNavbar
        Qt6::Widgets
        Qt6::Network
        Qt6::Multimedia
        Qt6::MultimediaWidgets
        Qt6::WebSockets
        Qt6::Concurrent
        QHotkey::QHotkey
)
