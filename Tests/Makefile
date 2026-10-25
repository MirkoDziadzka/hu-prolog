PYTHON = python3
BINARY = ../Sources/huprolog

.PHONY: test test-one smoke clean help

test:
	$(PYTHON) run_tests.py --binary $(BINARY)

test-one:
	$(PYTHON) run_tests.py --binary $(BINARY) $(TEST)

smoke:
	$(PYTHON) run_tests.py --binary $(BINARY) --include-smoke 00_smoke.pl

clean:
	rm -f /tmp/hu_*.pl

help:
	@echo "make test                  run all tests"
	@echo "make test-one TEST=<file>  run one test file, e.g. TEST=05_arithmetic.pl"
	@echo "make smoke                 run 00_smoke.pl"
	@echo "make clean                 remove the scratch files the tests write to /tmp"
	@echo "BINARY=<path> selects the interpreter (default $(BINARY))"
