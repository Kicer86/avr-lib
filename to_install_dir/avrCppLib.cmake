
cmake_minimum_required(VERSION 2.8)

set(BASE_DIR "${CMAKE_INSTALL_PREFIX}/avr")                #katalog bazowy

include ("${CMAKE_CURRENT_LIST_DIR}/cpus/CMakeLists.txt")  #targety związane z instalacją, fuse bitami etc

macro (avr_module)
  set (VARIABLE_NAME "MODULE_NAME")
  
  set (MODULE_NAME "")                    #nazwa projektu
  set (MODULE_SOURCES "")                 #pliki źródłowe
  set (MODULE_HEADERS "")                 #pliki nagłówkowe, zmienna używana do instalowania bibliotek
  set (MODULE_MODE "program")             #typ buildu - program lub library
  set (MODULE_EXT_LIBRARIES "")           #lista dodatkowych bibliotek potrzebych do zlinkowania programu (np se1520 graphic)
  set (MODULE_BIN_PATH "bin")             #ścieżka (względem BASE_DIR) do instalowania binariów
  set (MODULE_INC_PATH "include")         #ścieżka (względem BASE_DIR) do instalowania plików nagłówkowych
  set (MODULE_LIB_PATH "lib")             #ścieżka (względem BASE_DIR) do instalowania bibliotek
  set (MODULE_CPU "")                     #procesor
  set (MODULE_FCPU "1000000")             #f procesora
  set (MODULE_CLKSRC "internal")          #typ zegara: internal, external, oscilator
  set (MODULE_C_COMPILER "avr-gcc")       #kompilator C
  set (MODULE_CPP_COMPILER "avr-g++")     #kompilator C++
  set (MODULE_FLAGS "")                   #dodatkowe flagi kompilacji
  set (MODULE_BUILD_TYPE "Size")          #typ wydania (Speed/Size)
  set (MODULE_DEBUG "")                   #debug?

  #zmienne lokalne
  set (MODULE_OBJECTS "")
  set (MODULE_TYPE "C")           #typ projektu c/c++

  foreach (ARG ${ARGV})
    if ("${VARIABLE_NAME}" STREQUAL "MODULE_NAME")
      set (MODULE_NAME ${ARG})
      set (VARIABLE_NAME "")
    elseif (ARG MATCHES "(MODULE_SOURCES|MODULE_HEADERS|MODULE_MODE|MODULE_EXT_LIBRARIES|MODULE_BIN_PATH|MODULE_INC_PATH|MODULE_LIB_PATH|MODULE_CPU|MODULE_FCPU|MODULE_CLKSRC|MODULE_C_COMPILER|MODULE_CPP_COMPILER|MODULE_FLAGS|MODULE_BUILD_TYPE|MODULE_DEBUG)")
      set (VARIABLE_NAME ${ARG})
    elseif (VARIABLE_NAME STREQUAL "")
      message (FATAL_ERROR "Invalid invocation of avr_module macro")
    else ("${VARIABLE_NAME}" STREQUAL "MODULE_NAME")
      if ("\\${VARIABLE_NAME}" MATCHES "(MODULE_SOURCES|MODULE_EXT_LIBRARIES)")
        set (${VARIABLE_NAME} "${${VARIABLE_NAME}};${ARG}")   #źródła i biblioteki dopisujemy do listy
        if (ARG MATCHES "(cpp|CPP)$")
          set(MODULE_TYPE "CPP")
        endif (ARG MATCHES "(cpp|CPP)$")
                                                              #dopisujemy, ale nie jako listę
      elseif ("\\${VARIABLE_NAME}" MATCHES "(MODULE_FLAGS|MODULE_HEADERS)")
        if ("${${VARIABLE_NAME}}" STREQUAL "")                #pusta lista?
          set (${VARIABLE_NAME} ${ARG})                       #zainicjuj
        else ("${${VARIABLE_NAME}}" STREQUAL "")
          set (${VARIABLE_NAME} ${${VARIABLE_NAME}} ${ARG})
        endif ("${${VARIABLE_NAME}}" STREQUAL "")

      else ("${VARIABLE_NAME}")                               #dla pozostałych podmiana
        set (${VARIABLE_NAME} ${ARG})

      endif ("\\${VARIABLE_NAME}" MATCHES "(MODULE_SOURCES|MODULE_EXT_LIBRARIES)")

    endif ("${VARIABLE_NAME}" STREQUAL "MODULE_NAME")
  endforeach (ARG)

  #przygotuj flagi
  if ("${MODULE_TYPE}" STREQUAL "CPP")
    set (COMPILER ${MODULE_CPP_COMPILER})
  else ("${MODULE_TYPE}" STREQUAL "CPP")
    set (COMPILER ${MODULE_C_COMPILER})
  endif ("${MODULE_TYPE}" STREQUAL "CPP")

	#http://www.stmental.net/~dfoster/avr-libc-user-manual-1.6.5/using_tools.html
  #http://www.nongnu.org/avr-libc/user-manual/using_tools.html
  set(MMCU "-mmcu=avr6")  #domyslna architektura dla bibliotek (maksymalna, aby dowolny kod się skompilował)
  if (NOT "${MODULE_CPU}" STREQUAL "")
    set(MMCU "-mmcu=${MODULE_CPU}")
  endif (NOT "${MODULE_CPU}" STREQUAL "")

  #podstawowe flagi
  set(COMMON_FLAGS ${MMCU} ${MODULE_FLAGS})
  set(LDFLAGS ${COMMON_FLAGS})
  set(LDFLAGS ${LDFLAGS} -Wl,--relax -Wl,-static -Wl,--gc-sections  -Wl,--warn-common)
  set(LDFLAGS ${LDFLAGS} -Wl,-O1 -fwhole-program)

  #dodatkowe biblioteki
  set(LIB_SOURCES "")
  foreach(LIB ${MODULE_EXT_LIBRARIES})
    include(${BASE_DIR}/lib/lib${LIB}.info)
    set(LIB_SOURCES ${LIB_SOURCES} ${lib${LIB}})
  endforeach(LIB ${MODULE_EXT_LIBRARIES})
  
  if (NOT EXISTS libs)
    message ("Creating directory libs")
    execute_process(COMMAND mkdir -p libs)
  endif (NOT EXISTS libs)

  #DEF_FLAGS są używane do generowania zależności, zawierają minimalny zestaw flag
  set (DEF_FLAGS "-DF_CPU=${MODULE_FCPU}UL" ${COMMON_FLAGS} -I${BASE_DIR}/include)

  #właściwe flagi używane podczas kompilacji
  set(CFLAGS ${DEF_FLAGS})           #flagi podstawowe

  #dołącz flagi optymalizacyjne
  if (MODULE_BUILD_TYPE MATCHES "Speed|speed|Size|size")
    if (MODULE_BUILD_TYPE MATCHES "Speed|speed")
      set(CFLAGS ${CFLAGS} -O3)
    else(MODULE_BUILD_TYPE)
      set(CFLAGS ${CFLAGS} -Os)   
    endif(MODULE_BUILD_TYPE MATCHES "Speed|speed")

    if (MODULE_DEBUG STREQUAL "true")
      set(CFLAGS ${CFLAGS} -g -gdwarf-2) 
    endif (MODULE_DEBUG STREQUAL "true")
    
    set(CFLAGS ${CFLAGS} -flto)
    set(CFLAGS ${CFLAGS} -frename-registers)          #niewiele zmienia ale kod chyba bardziej logicznie wygląda
    
    set(CFLAGS ${CFLAGS} -fuse-cxa-atexit)
    
    set(CFLAGS ${CFLAGS} -ffunction-sections -fdata-sections )   #this works only with -Wl,--gc-sections and (probably) -fuse-cxa-atexit
    set(CFLAGS ${CFLAGS} -fno-tree-scev-cprop)   #czasem pomaga
    set(CFLAGS ${CFLAGS} -fno-split-wide-types) #czasem pomaga, czasem przeszkadza
    set(CFLAGS ${CFLAGS} -fmerge-all-constants)
    set(CFLAGS ${CFLAGS} -finline-small-functions)

  else (MODULE_BUILD_TYPE)  #debug
    set(CFLAGS ${CFLAGS} -O0 -g -gdwarf-2)
  endif (MODULE_BUILD_TYPE MATCHES "Speed|speed|Size|size")

  #dołącz ostrzeżenia i flagi ogólne
  set(CFLAGS ${CFLAGS} -W -Wall -Wextra -Winit-self -Wformat=2 -Wshadow -Wlogical-op) #-Winline
  set(CFLAGS ${CFLAGS} -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums )
  set(CFLAGS ${CFLAGS} -ffreestanding -save-temps)

  #flagi dla c++
  set(CPPFLAGS ${CFLAGS} -pedantic -fno-exceptions -fno-rtti) # -Weffc++)  
  set(CPPFLAGS ${CPPFLAGS} -fno-threadsafe-statics)

  #standardy i przerzucenie CFLAGS bądź CPPFLAGS do FLAGS
  if ("${MODULE_TYPE}" STREQUAL "C")
    set(FLAGS ${CFLAGS} -std=gnu99)
  else ("${MODULE_TYPE}")
    set(FLAGS ${CPPFLAGS} -std=c++0x)
  endif ("${MODULE_TYPE}" STREQUAL "C")

  #dodaj reguły dla plików bibliotecznych
  foreach(LIB ${LIB_SOURCES})
    get_filename_component(FILENAME ${LIB} NAME)  #wyodrębnij nazwę pliku
    add_custom_command(
      OUTPUT libs/${FILENAME}.o
      COMMAND ${COMPILER}
      ARGS ${FLAGS} -c -o libs/${FILENAME}.o ${LIB}
    )
    set(MODULE_OBJECTS libs/${FILENAME}.o ${MODULE_OBJECTS})
  endforeach(LIB ${LIB_SOURCES})
  
  #argumenty przemielone, tworzymy targety dla kazdego pliku źródłowego
  foreach(SRC ${MODULE_SOURCES})
    set(OBJECT ${SRC}.o)
    set(MODULE_OBJECTS ${OBJECT} ${MODULE_OBJECTS})
    set(SOURCE_FILE "")
    
    if (IS_ABSOLUTE ${SRC})
      set(SOURCE_FILE ${SRC})
    else(IS_ABSOLUTE)
      set(SOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${SRC})
    endif (IS_ABSOLUTE ${SRC})

    #Cmake nie tworzy drzewa katalogow dla plików przetwarzanych manualnie. Tworzymy je ręcznie
    get_filename_component(DIRNAME ${CMAKE_CURRENT_BINARY_DIR}/${OBJECT} PATH)  #wyodrębnij nazwę pliku
    
    if (NOT EXISTS ${DIRNAME})
      message ("Creating directory ${DIRNAME} for file ${OBJECT}")
      execute_process(COMMAND mkdir -p ${DIRNAME})
    endif (NOT EXISTS ${DIRNAME})

    #tworzenie pliku zależności (lista includów)
    string(REGEX REPLACE "\\/" "_" DEPS ${OBJECT})  #usuń z nazwy pliku "/" i "."
    string(REGEX REPLACE "\\." "_" DEPS ${DEPS})

    #wygeneruj zależności
    execute_process(
      COMMAND ${COMPILER} ${DEF_FLAGS} -MM -MT ${DEPS} -MF ${CMAKE_CURRENT_BINARY_DIR}/${OBJECT}.d -c ${SOURCE_FILE}
      )

    #popraw plik .d, tak żeby się go dało doinkludować
    execute_process(
      COMMAND sed -e "s/\\(.*\\):/set(\\1/" -e "/[^\\]$/a)" -e "s/\\\\//" -i ${CMAKE_CURRENT_BINARY_DIR}/${OBJECT}.d
    )

    include(${CMAKE_CURRENT_BINARY_DIR}/${OBJECT}.d)

    #regula dla pojedynczego pliku
    add_custom_command(
      OUTPUT ${OBJECT}
      DEPENDS ${${DEPS}}  #dołącz zależności pliku
      COMMAND ${COMPILER}
      ARGS ${FLAGS} -c -o ${OBJECT} ${CMAKE_CURRENT_SOURCE_DIR}/${SRC}
    )
  endforeach(SRC) #koniec regul dla pojedynczych plików źródłowych

  #target ostateczny - różny dla programu i biblioteki
  set (TARGET_FILE "")
  if ("${MODULE_MODE}" STREQUAL "program")
  
    message("Generating targets for program ${MODULE_NAME}")
    set(TARGET_FILE ${MODULE_NAME}.elf)
    add_custom_target(${MODULE_NAME} ALL DEPENDS ${TARGET_FILE} ${MODULE_NAME}.size ${MODULE_NAME}.lst ${MODULE_NAME}.diff ${MODULE_NAME}.hex ${MODULE_NAME}.eep ${MODULE_NAME}.eep.bin)

    add_custom_command(
      OUTPUT ${MODULE_NAME}.elf
      DEPENDS ${MODULE_OBJECTS}
      COMMAND ${COMPILER}
      ARGS ${FLAGS} ${MODULE_OBJECTS} -o ${MODULE_NAME}.elf -Wl,-Map=${MODULE_NAME}.map ${LDFLAGS} #flagi linkera na koncu zeby opcje -l zadzialaly poprawnie
    )

  elseif ("${MODULE_MODE}" STREQUAL "library")
  
    message("Generating targets for library ${MODULE_NAME}")
    set(TARGET_FILE lib${MODULE_NAME}.info)

    if (NOT "${MODULE_SOURCES}" STREQUAL "")
      add_custom_target(${MODULE_NAME} ALL DEPENDS ${TARGET_FILE}) #${MODULE_NAME}.size ${MODULE_NAME}.lst ${MODULE_NAME}.diff)

      #zapisz pliki cpp biblioteki w pliku info (uwzględniając ich koncową lokalizację)
      set(NAKED_NAMES "")
      foreach(SRC ${MODULE_SOURCES}) 
        #wyodrębnij nazwę pliku źródłowego
        get_filename_component(FILENAME ${SRC} NAME)        
        set(NAKED_NAMES ${BASE_DIR}/${MODULE_INC_PATH}/${FILENAME} ${NAKED_NAMES})  #dopisz do listy
      endforeach(SRC ${MODULE_SOURCES})
      
      #stwórz plik info zawierający listę plików do skompilowania w momencie kompilacji projektu głównego
      add_custom_command(
        OUTPUT ${TARGET_FILE}
        DEPENDS ${MODULE_OBJECTS}
        COMMAND echo 'set(lib${MODULE_NAME} ${NAKED_NAMES})' > ${TARGET_FILE}
      )

      install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_FILE} 
               DESTINATION ${BASE_DIR}/${MODULE_LIB_PATH}
               PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
    endif (NOT "${MODULE_SOURCES}" STREQUAL "")

    install (FILES ${MODULE_HEADERS} 
             DESTINATION ${BASE_DIR}/${MODULE_INC_PATH}
             PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
    install (FILES ${MODULE_SOURCES} 
             DESTINATION ${BASE_DIR}/${MODULE_INC_PATH}
             PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
             
  else ("${MODULE_MODE}" STREQUAL "program")
 
    message(FATAL_ERROR "invalid module mode: ${MODULE_MODE}")
    
  endif("${MODULE_MODE}" STREQUAL "program")

  add_custom_command(
    OUTPUT ${MODULE_NAME}.lst
    DEPENDS ${TARGET_FILE}
    COMMAND avr-objdump
    ARGS -d ${TARGET_FILE} > ${MODULE_NAME}.lst
  )

  add_custom_command(
    OUTPUT ${MODULE_NAME}.size
    DEPENDS ${TARGET_FILE}
    COMMAND avr-size
    ARGS ${TARGET_FILE}
  )
  
  add_custom_command(
    OUTPUT ${MODULE_NAME}.hex
    DEPENDS ${TARGET_FILE}
    COMMAND avr-objcopy 
    ARGS -O ihex -R .eeprom -R .fuse -R .lock -R .signature ${TARGET_FILE} ${MODULE_NAME}.hex
  )
  
  add_custom_command(
    OUTPUT ${MODULE_NAME}.eep
    DEPENDS ${TARGET_FILE}
    COMMAND avr-objcopy
    ARGS -O ihex -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings ${TARGET_FILE} ${MODULE_NAME}.eep
  )
  
  add_custom_command(
    OUTPUT ${MODULE_NAME}.eep.bin
    DEPENDS ${TARGET_FILE}
    COMMAND avr-objcopy
    ARGS -O binary -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings ${TARGET_FILE} ${MODULE_NAME}.eep.bin
    
    COMMAND  [ -s ${MODULE_NAME}.eep.bin ] && touch program_eeprom || rm -f program_eeprom
  )

  #różnice między dwoma ostatnimi buildami
  add_custom_command(
    OUTPUT ${MODULE_NAME}.diff
    DEPENDS ${MODULE_NAME}.lst
    #usun z bieżącego i poprzedniego listingu adresy
    COMMAND sed -e "s/^[ 0-9a-f]*://" -e "s/\;.*//" ${MODULE_NAME}.lst > ${MODULE_NAME}.lst_cur
    COMMAND diff -Nurp ${MODULE_NAME}.lst_prev ${MODULE_NAME}.lst_cur > ${MODULE_NAME}.diff \; true #diff ma nieprzyjazne kody wyjściowe ;), trzeba je zagłuszyć
    COMMAND mv -f ${MODULE_NAME}.lst_cur ${MODULE_NAME}.lst_prev
  )
  
  if ("${MODULE_MODE}" STREQUAL "program")
    targets(${MODULE_NAME} ${MODULE_CPU} ${MODULE_FCPU})  #utwórz różne dodatkowe targety
  endif ("${MODULE_MODE}" STREQUAL "program")

endmacro (avr_module)
