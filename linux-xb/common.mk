# 
# 本软件为免费、开源软件。
# 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
# 您可以自由使用、传播本软件。
# 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
# =====================
# 作者: 孙明保
# 邮箱: sunmingbao@126.com
# 

# CROSS_COMPILE=arm-linux-
export CC:=$(CROSS_COMPILE)gcc
export CFLAGS:=-c -O2 -Wall -fno-strict-overflow -fno-strict-aliasing
export LD:=$(CROSS_COMPILE)ld
export AR:=$(CROSS_COMPILE)ar

export RM:=rm -rf
export MKDIR:=mkdir -p

export LDFLAGS:=-rdynamic
export C_LIBS:=-ldl -lpthread 