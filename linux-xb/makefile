# 
# 本软件为免费、开源软件。
# 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
# 您可以自由使用、传播本软件。
# 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
# =====================
# 作者: 孙明保
# 邮箱: sunmingbao@126.com
# 

include common.mk

export project_root_path:=$(shell pwd)
export src_path:=$(project_root_path)/src
export INC_DIRS:=-I./ -I$(project_root_path)/inc

export project_target_path:=$(project_root_path)/target


export DEP_PATH:=$(project_target_path)/temps/dep
export OBJ_PATH:=$(project_target_path)/temps/obj

export APP_NAME:=xb_ether_tester

export target:=$(project_target_path)/$(APP_NAME).exe

.PHONY:default prepare clean

default:prepare
	make -C $(src_path)
	@echo build project done!

prepare:
	@$(MKDIR)  $(project_target_path)
	@$(MKDIR)  $(DEP_PATH)
	@$(MKDIR)  $(OBJ_PATH)

clean:
	$(RM)  $(project_target_path)
	@echo clean project done!

