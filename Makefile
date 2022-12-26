# Revision History
# Updated: 2022-10-19 Dr. J. Anvik <john.anvik@uleth.ca>
#  Changed the static command to make the pipeline fail on exit with errors.
# Updated: 2022-09-11 Nicole Wilson <n.wilson@uleth.ca>
#  Added reference to OS in setting of STYLE_CHECK.
#  This is a temporary measure until the pipelines are running on Ubuntu
#
# The targets in this file are used in .gitlab-ci.yml and 
# the files created are found in the .gitignore
# Changing any names below can change the target names which 
# will require that you update .gitlab_ci.yml and .gitignore
PROJECT = goFishREST
GTEST = test_${PROJECT}


# Compilation command and flags
CXX=g++
CXXVERSION= -std=c++17
CXXFLAGS= ${CXXVERSION} -g -fprofile-arcs -ftest-coverage

LINKFLAGS = -L libs -lrestbed -lpthread -lgofish
LINKFLAGS_TEST = -lgtest $(LINKFLAGS)

# Directories
SRC_DIR_SERVER = src/server
SRC_DIR_CLIENT = src/client
SRC_DIR_SERVICE = src/game
SRC_DIR_TEST = test

SERVICE_INCLUDE = -I include/game
RESTBED_INCLUDE = -I include/restbed
JSON_INCLUDE = -I include

# Tool variables
GCOV = gcov
LCOV = lcov
COVERAGE_RESULTS = results.coverage
COVERAGE_DIR = coverage

STATIC_ANALYSIS = cppcheck

# The variable STYLE_CHECK is dependant on whether make is called from Ubuntu,
# the lab machines and office machines (this includes student0, student1,
# student2, and faculty0), or from gitlab which uses Centos.
# OS is a variable set and exported when .bashrc is run (found in etc/bashrc}
ifeq (${OS},ubuntu)
	STYLE_CHECK = cpplint
else
	STYLE_CHECK = cpplint
endif

DOXY_DIR = docs/code

PROGRAM_SERVER = gameServer
PROGRAM_CLIENT = gameClient
PROGRAM_TEST = testServer

.PHONY: all
all: $(PROGRAM_SERVER) $(PROGRAM_CLIENT) coverage docs static style

# default rule for compiling .cc to .o
%.o: %.cpp
	${CXX} ${CXXFLAGS} -c $< -o $@

# clean up all files that should NOT be submitted
.PHONY: clean
clean:
	rm -rf *~ ${SRC}/*.o ${GTEST_DIR}/output/*.dat \
	*.gcov *.gcda *.gcno \
	$(PROGRAM_SERVER) $(PROGRAM_TEST) $(PROGRAM_CLIENT) \
	${COVERAGE_RESULTS} ${COVERAGE_DIR}\
	${MEMCHECK_RESULTS}	


.PHONY: server
server: $(PROGRAM_SERVER)

.PHONY: client
client: $(PROGRAM_CLIENT)

.PHONY: runServer
runServer: server
	./${PROGRAM_SERVER} &

.PHONY: stopServer
stopServer:
	pkill ${PROGRAM_SERVER} &

.PHONY: process
process:
	ps -ef | grep $(PROGRAM_SERVER)

$(PROGRAM_SERVER): $(SRC_DIR_SERVER)/*.cpp
	$(CXX) $(CXXFLAGS) -o $(PROGRAM_SERVER) $(SERVICE_INCLUDE) \
	$(SRC_DIR_SERVER)/*.cpp $(LINKFLAGS) $(RESTBED_INCLUDE) $(JSON_INCLUDE)

$(PROGRAM_CLIENT): $(SRC_DIR_CLIENT)/*.cpp
	$(CXX) $(CXXFLAGS) -o $(PROGRAM_CLIENT)	$(SRC_DIR_CLIENT)/*.cpp $(LINKFLAGS) \
	$(SERVICE_INCLUDE) $(RESTBED_INCLUDE) $(JSON_INCLUDE)

$(PROGRAM_TEST): $(PROGRAM_SERVER) $(SRC_DIR_TEST)/*.cpp
	$(CXX) $(CXXFLAGS) -o $(PROGRAM_TEST)	$(SRC_DIR_TEST)/*.cpp $(SERVICE_INCLUDE) $(LINKFLAGS_TEST) \
	$(RESTBED_INCLUDE) $(JSON_INCLUDE)

memcheck: ${GTEST}
	valgrind --tool=memcheck --leak-check=yes --error-exitcode=1 ./${GTEST}

.PHONY: coverage
coverage: $(PROGRAM_TEST)
		./$(PROGRAM_SERVER) &
		./$(PROGRAM_TEST)
		make stopServer

		# Determine code coverage
		$(LCOV) --capture --gcov-tool $(GCOV) --directory . --output-file $(COVERAGE_RESULTS) --rc lcov_branch_coverage=1
		# Only show code coverage for the source code files (not library files)
		$(LCOV) --extract $(COVERAGE_RESULTS) */$(SRC_DIR_SERVER)/* -o $(COVERAGE_RESULTS)
		#Generate the HTML reports
		genhtml $(COVERAGE_RESULTS) --output-directory $(COVERAGE_DIR)
		#Remove all of the generated files from gcov
		rm -f *.gcda *.gcno
		
static: $(SRC_DIR_SERVER) $(SRC_DIR_CLIENT) $(SRC_DIR_TEST)
	${STATIC_ANALYSIS} --verbose --enable=all $(SRC_DIR_SERVER) $(SRC_DIR_CLIENT) $(SRC_DIR_TEST) --suppress=missingInclude --error-exitcode=1

style: $(SRC_DIR_SERVER) $(SRC_DIR_CLIENT) $(SRC_DIR_TEST)
	${STYLE_CHECK} $(SRC_DIR_SERVER)/* $(SRC_DIR_CLIENT)/* $(SRC_DIR_TEST)/* 	

.PHONY: docs
docs: ${SRC_INCLUDE}
	doxygen ${DOXY_DIR}/doxyfile
