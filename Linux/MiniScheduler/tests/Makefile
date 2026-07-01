# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra --coverage -g -I../include -I.
LDFLAGS = -lgtest -lgmock -lpthread -lrt --coverage

# Source
SRC_DIR = ../src
SRC_TEST = ./main.cpp

# Dependency
MINISCHED_SRC = \
	$(SRC_DIR)/config/config_loader.cpp \
	$(SRC_DIR)/job/job_queue.cpp \
	$(SRC_TEST)

# Target Build
TAR_MINISCHED = UT_MINISCHED

# Build Target
$(TAR_MINISCHED): $(MINISCHED_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Build and run
all: $(TAR_MINISCHED)
	@echo "Running..."
	./$(TAR_MINISCHED)

# Coverage
coverage: clean $(TAR_MINISCHED)
	@echo "Running test for coverage..."
	-./$(TAR_MINISCHED)
	@echo "Generating coverage report..."
	lcov --capture --directory . --output-file minischeduler.info --ignore-errors mismatch --ignore-errors unused
	lcov --remove minischeduler.info '/usr/*' '*gtest*' '*gmock*' --output-file minischeduler_clean.info --ignore-errors unused
	genhtml minischeduler_clean.info --output-directory coverage_report
	@echo "Coverage report generated in coverage_report/index.html"

# Clean
clean:
	find . -name '*.o' -delete
	find . -name '*.gcda' -delete
	find . -name '*.gcno' -delete
	find . -name '*.info' -delete
	rm -rf $(TAR_MINISCHED)
	rm -rf coverage_report
	rm -f minischeduler.info minischeduler_clean.info

.PHONY: all coverage clean
