# Define the recursive wildcard function
rwildcard = $(foreach d,$(wildcard $(1)*),$(call rwildcard,$(d)/,$(2)) $(filter $(subst *,%,$(2)),$(d)))

# Usage: Find all .c files in the 'src' directory and its subdirectories
SRCFILES := $(call rwildcard, src/, *.cpp)
COMPILER := g++
CFLAGS := -Wall -Wextra -O3 -std=c++20 -I./inc
ifeq '$(findstring ;,$(PATH))' ';' # Detect Windows
    EXE := ./bin/brassc.exe
else # Or Unix
    EXE := ./bin/brassc
endif

# Get all goals (targets) passed on the command line
ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))

all: build run

# Create a rule to catch the arguments and do nothing
$(eval $(ARGS):;@:)

build:
	$(COMPILER) $(CFLAGS) $(SRCFILES) -o $(EXE)

run:
	$(EXE) $(ARGS)

git:
	git add .
	git commit -m '$(ARGS)'
	git push