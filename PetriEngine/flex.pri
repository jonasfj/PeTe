#GNU Bison qmake configuration by Thomas Capricelli, see:
#http://www.freehackers.org/thomas/2009/11/22/how-to-use-flex-and-bison-with-qmake-my-own-way/
flex.name = Flex ${QMAKE_FILE_IN}
flex.input = FLEXSOURCES
flex.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.lexer.cpp
flex.commands = flex -o ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.lexer.cpp ${QMAKE_FILE_IN}
flex.CONFIG += target_predeps
flex.variable_out = GENERATED_SOURCES
silent:flex.commands = @echo Lex ${QMAKE_FILE_IN} && $$flex.commands
QMAKE_EXTRA_COMPILERS += flex

OTHER_FILES += FLEXSOURCES
