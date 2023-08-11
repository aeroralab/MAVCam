ifeq ($(APP_PARAM), )
    APP_PARAM:=../Makefile.param
endif

-include $(APP_PARAM)

export LC_ALL=C
SHELL:=/bin/bash

CURRENT_DIR := $(dir $(abspath $(firstword $(MAKEFILE_LIST))))

PKG_BUILD := build_dir
PKG_BIN := stage_dir
PKG_BIN_INSTALL := $(PKG_BIN)/oem

PKG_TARGET += build-MAVCam

all: $(PKG_TARGET)
	@echo "build MAVCam done"

build-MAVCam:
	@mkdir -p $(CURRENT_DIR)/$(PKG_BUILD) && (\
	pushd $(CURRENT_DIR)/$(PKG_BUILD)/; \
		cmake -DCMAKE_INSTALL_PREFIX=/MAVCam -DCMAKE_TOOLCHAIN_FILE=$(CURRENT_DIR)/cmake/gcc-linaro-aarch64-linux.toolchain.cmake -DCMAKE_BUILD_TYPE=Release -DSTAGING_DIR=$(CURRENT_DIR)/$(PKG_BIN) ../ ; \
		make -j$(RK_APP_JOBS); \
		make DESTDIR=$(CURRENT_DIR)/$(PKG_BIN) install; \
		rm -rf $(CURRENT_DIR)$(PKG_BIN_INSTALL)/usr/share; \
	popd; )
	$(call MAROC_COPY_PKG_TO_APP_OUTPUT, $(RK_APP_OUTPUT), $(PKG_BIN_INSTALL))

clean: distclean

distclean:
	@rm -rf $(PKG_BUILD)
	@rm -rf $(PKG_BIN)

info:
	@echo -e "$(C_YELLOW)-------------------------------------------------------------------------$(C_NORMAL)"
	@echo -e "$(C_YELLOW)-------------------------------------------------------------------------$(C_NORMAL)"
