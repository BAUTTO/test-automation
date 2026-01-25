# Testsvit för biblioteket

Unit- och komponenttester för biblioteket `libatmega`, skrivna med Google Test.
Dessa tester måste köras i Linux.

## Förutsättningar

Installera Google Test såsom beskrivet i [här](../../README.md#installera-google-test).

## Kompilering samt exekvering av tester

Tack vara den bifogade [makefilen](./makefile) kan testerna kompileras samt köras via följande kommando (i denna katalog):

```make
make
```

Det går även att enbart kompilera testerna via följande kommando:

```make
make build
```

Det går även att enbart köra testerna utan att kompilera innan via följande kommando:

```make
make run
```

Ta bort kompilerade filer med följande kommando:

```
make clean
```

## Tillägg av nya filer

Lägg till nya testfiler i bygget genom att lägga till sökvägen för dessa till
`TEST_FILES` i [makefilen](./makefile). Som exempel, om en fil döpt `example/file.cpp` läggs
till i denna katalog, lägg till motsvarande rad såsom visas nedan:

```makefile
# Test files - update this list as new test files are added to the system.
TEST_FILES := driver/adc/atmega328p_test.cpp \
              driver/eeprom/atmega328p_test.cpp \
              driver/gpio/atmega328p_test.cpp \
              driver/serial/atmega328p_test.cpp \
              driver/tempsensor/smart_test.cpp \
              driver/tempsensor/tmp36_test.cpp \
              driver/timer/atmega328p_test.cpp \
              driver/watchdog/atmega328p_test.cpp \
              example/file.cpp \ # Lade till 'example/file.cpp' i bygget.
              logic/logic_test.cpp \
              ml/lin_reg/fixed_test.cpp \
              testsuite.cpp \
```

## Tillägg av nya källkodsfiler från biblioteket

Lägg till samtliga nya källkodsfiler från biblioteket i bygget genom att lägga till sökvägen för dessa till
`SOURCE_FILES` i [makefilen](./makefile). Som exempel, om en fil döpt `new/libfile.cpp` läggs
till i källkatalogen [source](../source/), lägg till motsvarande rad såsom visas nedan:

```makefile
# Source files - update this list as new source files are added to the system.
SOURCE_FILES := $(SOURCE_DIR)/arch/test/hw_platform.cpp \
                $(SOURCE_DIR)/driver/adc/atmega328p.cpp \
                $(SOURCE_DIR)/driver/eeprom/atmega328p.cpp \
                $(SOURCE_DIR)/driver/gpio/atmega328p.cpp \
                $(SOURCE_DIR)/driver/serial/atmega328p.cpp \
                $(SOURCE_DIR)/driver/tempsensor/smart.cpp \
                $(SOURCE_DIR)/driver/tempsensor/tmp36.cpp \
                $(SOURCE_DIR)/driver/timer/atmega328p.cpp \
                $(SOURCE_DIR)/driver/watchdog/atmega328p.cpp \
                $(SOURCE_DIR)/logic/logic.cpp \
                $(SOURCE_DIR)/ml/lin_reg/fixed.cpp \
                $(SOURCE_DIR)/new/libfile.cpp \ # Lade till 'new/libfile.cpp' i bygget.
                $(SOURCE_DIR)/utils/utils.cpp \
```