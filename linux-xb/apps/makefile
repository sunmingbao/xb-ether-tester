# 
# 本软件为免费、开源软件。
# 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
# 您可以自由使用、传播本软件。
# 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
# =====================
# 作者: 孙明保
# 邮箱: sunmingbao@126.com
# 

SRC_PATH=$(shell pwd)
OBJ_PATH=$(APP_CODE_OBJ_PATH)
DEP_PATH=$(APP_CODE_DEP_PATH)
SRC_FILES=$(wildcard $(SRC_PATH)/*.c)
SRC_FILES_BASE_NAMES=$(basename $(notdir $(SRC_FILES)))
OBJ_FILES=$(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(SRC_FILES_BASE_NAMES)))
DEP_FILES=$(addprefix $(DEP_PATH)/, $(addsuffix .d, $(SRC_FILES_BASE_NAMES)))

EXE_FILES=$(addprefix $(project_target_path)/, $(addsuffix .exe, $(SRC_FILES_BASE_NAMES)))

all:$(EXE_FILES)


.PHONY:all
.PRECIOUS: $(OBJ_FILES)  $(DEP_FILES)

$(project_target_path)/%.exe: $(OBJ_PATH)/%.o  $(LIB_FILE_STATIC)
	@echo -n Building $@ ...
	@$(CC)  $(LDFLAGS) -o $@  $^  $(C_LIBS)
	@echo done!

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c $(DEP_PATH)/%.d
	@echo -n Building $@ ...
	@$(CC) -c  $(CFLAGS) $(INC_DIRS) $< -o $@ 
	@echo done!

$(DEP_PATH)/%.d: $(SRC_PATH)/%.c
	@echo -n Building $@ ...
	@$(CC) -MM  $(CFLAGS) $(INC_DIRS) $< | \
        sed "s/$*\.o[ :]*/$(subst /,\/, $(DEP_PATH))\/$*.d : /g"> $@
	@echo done!

-include $(DEP_FILES)

