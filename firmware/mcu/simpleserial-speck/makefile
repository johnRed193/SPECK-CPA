# Hey Emacs, this is a -*- makefile -*-
#----------------------------------------------------------------------------
#
# Makefile for ChipWhisperer SimpleSerial-SPECK Program
#
#----------------------------------------------------------------------------

# 1. Имя выходного файла (имя прошивки .hex)
TARGET = simpleserial-speck

# 2. Твой C-файл с реализацией SPECK
SRC += simpleserial-speck.c

# -----------------------------------------------------------------------------
EXTRA_OPTS = NO_EXTRA_OPTS
CFLAGS += -D$(EXTRA_OPTS)

# 3. ВАЖНО: Ставим NONE, чтобы make не искал библиотеку в папке crypto.
# Так как код алгоритма у тебя внутри simpleserial-speck.c,
# внешняя библиотека не нужна.
CRYPTO_TARGET = NONE

${info Building for platform ${PLATFORM} with CRYPTO_TARGET=$(CRYPTO_TARGET)}

# 4. Подключаем правила сборки (пути как в GOST)
include ../simpleserial/Makefile.simpleserial

FIRMWAREPATH = ../.
include $(FIRMWAREPATH)/Makefile.inc