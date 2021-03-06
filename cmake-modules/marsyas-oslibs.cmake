## setup OS_LIBS
set (OS_LIBS)

if (MARSYAS_MACOSX)
	list (APPEND OS_LIBS ${COREFOUNDATION_LIBRARY})
	if (MARSYAS_AUDIOIO)
		list (APPEND OS_LIBS ${COREAUDIO_LIBRARY})
	endif (MARSYAS_AUDIOIO)
	if (MARSYAS_MIDIIO)
		list (APPEND OS_LIBS ${COREMIDI_LIBRARY})
	endif (MARSYAS_MIDIIO)
	if (CMAKE_BUILD_TYPE STREQUAL "Profile") 
		list (APPEND OS_LIBS Saturn)
	endif (CMAKE_BUILD_TYPE STREQUAL "Profile")
endif (MARSYAS_MACOSX)

if (MARSYAS_LINUX)
	if(MARSYAS_AUDIOIO OR MARSYAS_MIDIIO)
		if(WITH_JACK AND JACK_FOUND)
			list(APPEND OS_LIBS ${JACK_LIBRARIES})
		endif()
		if(WITH_ALSA AND ALSA_FOUND)
			list(APPEND OS_LIBS ${ALSA_LIBRARY})
		endif()
		# No libraries for OSS
	endif()

	list (APPEND OS_LIBS pthread)
endif (MARSYAS_LINUX)

if (MARSYAS_WIN32)
	if (MARSYAS_AUDIOIO)
		if (ASIO)
			# FIXME: insert ASIO library here!
		else (ASIO)
			list (APPEND OS_LIBS ${DSOUND_LIBRARY})
		endif (ASIO)
	endif (MARSYAS_AUDIOIO)
	if (MARSYAS_MIDIIO)
		list (APPEND OS_LIBS WinMM.lib)
	endif (MARSYAS_MIDIIO)
	set (CMAKE_MODULE_LINKER_FLAGS_PROFILE "/INCREMENTAL:NO")

endif (MARSYAS_WIN32)

if (MARSYAS_MINGW)
	if (MARSYAS_AUDIOIO)
		list (APPEND OS_LIBS ${DSOUND_LIBRARY})
		list (APPEND OS_LIBS -lwinmm)
	endif (MARSYAS_AUDIOIO)
	
	if (MARSYAS_MIDIIO)
	endif (MARSYAS_MIDIIO)
endif (MARSYAS_MINGW)

if (MARSYAS_CYGWIN)
	if (MARSYAS_AUDIOIO)
	endif (MARSYAS_AUDIOIO)
	if (MARSYAS_MIDIIO)
	endif (MARSYAS_MIDIIO)
endif (MARSYAS_CYGWIN)


