function(targets MODULE_NAME MODULE_CPU MODULE_FCPU)

  if ( NOT (MODULE_NAME AND MODULE_CPU AND MODULE_FCPU))
    message(FATAL_ERROR "This file can be included only by main CMakeLists.txt")
  endif( NOT (MODULE_NAME AND MODULE_CPU AND MODULE_FCPU))

  set (AVRDUDE_FLAGS -c stk500v2 -p ${MODULE_CPU} -P avrdoper)

  if (EXISTS program_eeprom)
    set (INSTALL_FLAGS ${AVRDUDE_FLAGS} -U eeprom:w:${MODULE_NAME}.eep)
  else (EXISTS)
    set (INSTALL_FLAGS ${AVRDUDE_FLAGS})
  endif (EXISTS program_eeprom)

  add_custom_target(${MODULE_NAME}_backup
    sudo avrdude ${AVRDUDE_FLAGS} -D -U flash:r:${MODULE_NAME}.hex.bak:i -U eeprom:r:${MODULE_NAME}.eep.bak:i 
    )

  add_custom_target(${MODULE_NAME}_install
    sudo avrdude ${INSTALL_FLAGS} -U flash:w:${MODULE_NAME}.hex 
    )
    
    
  #FUSE BITY
  unset(L_FUSE)
  unset(H_FUSE)
  unset(E_FUSE)

  if (MODULE_CPU STREQUAL "atmega8")
    set(H_FUSE 0xD9)
    
    if (MODULE_CLKSRC STREQUAL "internal")
    
      if (MODULE_FCPU STREQUAL "1000000")
        set(L_FUSE 0xE1)
      elseif (MODULE_FCPU STREQUAL "2000000")
        set(L_FUSE 0xE2)
      elseif (MODULE_FCPU STREQUAL "4000000")
        set(L_FUSE 0xE3)
      elseif (MODULE_FCPU STREQUAL "8000000")
        set(L_FUSE 0xE4)
      else (MODULE_FCPU)
        message(FATAL_ERROR "Invalid CPU frequency")
      endif (MODULE_FCPU STREQUAL "1000000")
      
    elseif (MODULE_CLKSRC STREQUAL "oscilator")
      
      if (NOT MODULE_FCPU  LESS 8000000)
        set(L_FUSE 0xEF) 
      else (MODULE_FCPU)
        message(FATAL_ERROR "Invalid CPU frequency")
      endif (NOT MODULE_FCPU LESS 8000000)
      
    endif (MODULE_CLKSRC STREQUAL "internal")
    
  else(MODULE_CPU)
    message(WARNING "${MODULE_CPU} (module ${MODULE_NAME}) is not supported by cpus/CMakeLists.txt. ${MODULE_NAME}_setfuses will not be generated." )  
  endif (MODULE_CPU STREQUAL "atmega8")


  if (L_FUSE)
    set(FUSE_FLAGS -U lfuse:w:${L_FUSE}:m)
  endif(L_FUSE)

  if (H_FUSE)
    set(FUSE_FLAGS ${FUSE_FLAGS} -U hfuse:w:${H_FUSE}:m)
  endif(H_FUSE)

  if (E_FUSE)
    set(FUSE_FLAGS ${FUSE_FLAGS} -U efuse:w:${E_FUSE}:m)
  endif(E_FUSE)

  if (FUSE_FLAGS)
    #message("Generating ${MODULE_NAME}_setfuses with: ${FUSE_FLAGS}")
    add_custom_target(${MODULE_NAME}_setfuses
      sudo avrdude ${AVRDUDE_FLAGS} ${FUSE_FLAGS}
    )
  endif (FUSE_FLAGS)

endfunction(targets)