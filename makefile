CXXFLAGS = -g -std=c++17 -Wall -Wextra -Weffc++ -Wc++0x-compat -Wc++11-compat -Wc++14-compat -Waggressive-loop-optimizations -Walloc-zero -Walloca -Walloca-larger-than=8192 -Warray-bounds -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wdangling-else -Wduplicated-branches -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Wformat-overflow=2 -Wformat-truncation=2 -Winline -Wlarger-than=8192 -Wvla-larger-than=8192 -Wlogical-op -Wmissing-declarations -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wrestrict -Wshadow -Wsign-promo -Wstack-usage=8192 -Wstrict-null-sentinel -Wstrict-overflow=2 -Wstringop-overflow=4 -Wsuggest-attribute=noreturn -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wnarrowing -Wno-old-style-cast -Wvarargs -Waligned-new -Walloc-size-larger-than=1073741824 -Walloc-zero -Walloca -Walloca-larger-than=8192 -Wcast-align -Wdangling-else -Wduplicated-branches -Wformat-overflow=2 -Wformat-truncation=2 -Wmissing-attributes -Wmultistatement-macros -Wrestrict -Wshadow=global -Wsuggest-attribute=malloc -fcheck-new -fsized-deallocation -fstack-check -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer 

SrcDir = src
BinDir = bin

.PHONY: bin
bin: 
	mkdir bin

compiler.exe: $(BinDir)/compilation.o $(BinDir)/Text.o $(BinDir)/Utilities.o 
	g++ $(BinDir)/compilation.o $(BinDir)/Text.o $(BinDir)/Utilities.o -o compiler.exe

processor.exe: $(BinDir)/Processor.o $(BinDir)/Text.o $(BinDir)/Utilities.o $(BinDir)/Stack.o
	g++ $(BinDir)/Processor.o $(BinDir)/Text.o $(BinDir)/Utilities.o $(BinDir)/Stack.o -o processor.exe

$(BinDir)/compilation.o: $(SrcDir)/compilation.cpp $(SrcDir)/Compilation.h $(SrcDir)/Text.h $(SrcDir)/Utilities.h $(SrcDir)/commands.h $(SrcDir)/cmd_def.h
	g++ -c $(SrcDir)/compilation.cpp -o $(BinDir)/compilation.o $(CXXFLAGS)

$(BinDir)/Text.o: $(SrcDir)/Text.cpp $(SrcDir)/Text.h $(SrcDir)/Utilities.h
	g++ -c $(SrcDir)/Text.cpp -o $(BinDir)/Text.o $(CXXFLAGS)

$(BinDir)/Utilities.o: $(SrcDir)/Utilities.cpp $(SrcDir)/Utilities.h
	g++ -c $(SrcDir)/Utilities.cpp -o $(BinDir)/Utilities.o $(CXXFLAGS)

$(BinDir)/Stack.o: $(SrcDir)/stack.cpp $(SrcDir)/stack.h $(SrcDir)/commands.h
	g++ -c $(SrcDir)/stack.cpp -o $(BinDir)/Stack.o $(CXXFLAGS)

$(BinDir)/Processor.o: $(SrcDir)/Processor.cpp $(SrcDir)/Processor.h $(SrcDir)/commands.h $(SrcDir)/Text.h $(SrcDir)/Utilities.h $(SrcDir)/stack.h $(SrcDir)/cmd_def.h
	g++ -c $(SrcDir)/Processor.cpp -o $(BinDir)/Processor.o $(CXXFLAGS)

.PHONY: clean
clean:
	del /f /q $(BinDir)\*