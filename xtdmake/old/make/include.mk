XTD_EXPORT_COMMANDS ?= 0
GITROOT ?= $(top_srcdir)
AR       = $(XTDMAKE_DIR)/arident --top-srcdir="$(XTD_SVCROOT_DIR)"


ifeq ($(XTD_EXPORT_COMMANDS),1)
	am__v_CXX_0 = @$(XTDMAKE_DIR)/trace_command.py "$(top_builddir)" "$(PWD)" "$<" "$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)"; echo "  CXX     " $@;
	am__v_CXX_1 = @$(XTDMAKE_DIR)/trace_command.py "$(top_builddir)" "$(PWD)" "$<" "$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)";
endif

ifndef XTD_NOIDENT
ifdef LIBTOOL
LDFLAGS += -XCClinker -D "__RCSID__=\"`$(XTDMAKE_DIR)/genident --top-srcdir=$(XTD_SVCROOT_DIR) --bin-name='$@' --deps='$(filter %.a, $^)'`\"" $(XTDMAKE_DIR)/ident.cc
else
LDFLAGS += -D "__RCSID__=\"`$(XTDMAKE_DIR)/genident --top-srcdir=$(XTD_SVCROOT_DIR) --bin-name='$@' --deps='$(filter %.a, $^)'`\"" $(XTDMAKE_DIR)/ident.cc
endif
endif

dummy:

%.a : dummy
	@echo "checking : $(notdir $@)"
	@$(MAKE) --no-print-directory -C $(dir $@) $(notdir $@)
