#!/bin/bash
set -o pipefail

COMMAND=$1
shift

case "$COMMAND" in
    name)
        echo "No build environment needed for Ubuntu, please install dependencies using apt." >&2

        if [ "$1" = "--ghactions" ]
        then
            echo "::set-output name=buildenv_name::"
        fi
        ;;

    setup)
        source /etc/lsb-release 2>/dev/null
        case "${DISTRIB_CODENAME}" in
            bionic)
                PACKAGES_EXTRA=libfaad-dev libmp4v2-dev
                ;;
            focal)
                PACKAGES_EXTRA=libavcodec-dev libavutil-dev
                ;;
            *)
                echo "Failed to detect a supported Ubuntu version, dependency installation will be skipped." >&2
                DISTRIB_CODENAME=
        esac

        if [ ! -z "${DISTRIB_CODENAME}" ]
        then
            sudo apt-get update
            sudo apt-get install -y --no-install-recommends \
                ccache \
                ninja-build \
                libavformat-dev \
                libchromaprint-dev \
                libebur128-dev \
                libfftw3-dev \
                libflac-dev \
                libid3tag0-dev \
                liblilv-dev \
                libmad0-dev \
                libmodplug-dev \
                libmp3lame-dev \
                libopus-dev \
                libopusfile-dev \
                libportmidi-dev \
                libprotobuf-dev \
                libqt5opengl5-dev \
                libqt5sql5-sqlite \
                libqt5svg5-dev \
                libqt5x11extras5-dev \
                librubberband-dev \
                libshout3-dev \
                libsndfile1-dev \
                libsoundtouch-dev \
                libsqlite3-dev \
                libtag1-dev \
                libupower-glib-dev \
                libusb-1.0-0-dev \
                libwavpack-dev \
                portaudio19-dev \
                protobuf-compiler \
                qt5-default \
                qt5keychain-dev \
                qtscript5-dev \
                $PACKAGES_EXTRA
        fi

        if [ "$1" = "--ghactions" ]
        then
            QT_QPA_PLATFORM_PLUGIN_PATH="$(qtpaths --plugin-directory)"

            echo "::set-output name=buildenv_path::"
            echo "::set-output name=macosx_deployment_target::"
            echo "::set-output name=cmake_prefix_path::"
            echo "::set-output name=path::${PATH}"
            echo "::set-output name=qt_path::"
            echo "::set-output name=qt_qpa_platform_plugin_path::${QT_QPA_PLATFORM_PLUGIN_PATH}"
        fi
        ;;
esac
