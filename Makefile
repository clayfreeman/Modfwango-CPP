CDEBUG		:= 1
CPPFLAGS	:= -std=c++11 -Wall -Wextra -pedantic -I. -DDEBUG=$(CDEBUG)
CXX		:= c++
CLEANO		:= $(shell find . -type f -name "*.o" | sed 's/^\.\///')
CLEANSO		:= $(shell find . -type f -name "*.so" | sed 's/^\.\///')
DEPEND		:= $(shell find src -maxdepth 1 -type f -name "*.cpp" \
                        | sed 's/cpp$$/o/')
MODULES		:= $(shell find src/modules -maxdepth 1 -type f -name "*.cpp" \
                        | sed 's/cpp$$/so/')

.PHONY:		all clean modules
all:		main modules
modules:	$(MODULES)
#	@mkdir -p modules
#	@$(foreach item,$(MODULES),mv $(item) $(shell echo $(item) | \
#	  sed 's/src\///');)

clean:
	@$(foreach item,$(CLEANO),echo $(item); rm -f $(item);)
	@$(foreach item,$(CLEANSO),echo $(item); rm -f $(item);)
	@rm -f main

main:		$(DEPEND) main.o
	@echo " [LNK] $@ ..."
	@$(CXX) $(CPPFLAGS) -o main $^ -ldl

test:		main
	@valgrind -v --tool=memcheck --leak-check=full --dsymutil=yes ./main

%.so:		%.cpp
	@echo " [CXX] $@ ..."
	@$(CXX) $(CPPFLAGS) -fPIC -shared -o $@ $^

%.o:		%.cpp
	@echo " [CXX] $@ ..."
	@$(CXX) $(CPPFLAGS) -c -o $@ $^
