# Build the h265nal conformance summaries.
#
# Runs tools/h265nal-conformance.py over each suite of the H.265 conformance
# dataset, and writes one CSV per suite in this directory.
#
#   make                              # build every conformance.<suite>.csv
#   make conformance.hevc_v1.csv      # just one suite
#   make -B                           # rebuild everything
#   make DATASET=/path/to/201810      # point at a different copy
#   make clean
#
# The CSVs depend on the h265nal binary, so a parser fix followed by a
# rebuild makes every suite out of date, and "make" re-sweeps them all.

DATASET ?= $(HOME)/work/video/dataset/h265_conformance/201810
H265NAL ?= ../build/tools/h265nal
CONFORMANCE ?= ../tools/h265nal-conformance.py

# H.265 v1 plus the five extension suites
SUITES = hevc_v1 3d-hevc mv-hevc rext scc shvc
CSVS = $(patsubst %,conformance.%.csv,$(SUITES))

.PHONY: all clean

all: $(CSVS)

# the dataset directories hold bitstreams only ("shvc" also has a "replaced"
# subdirectory, which h265nal-conformance.py skips)
conformance.%.csv: $(CONFORMANCE) $(H265NAL)
	@test -d "$(DATASET)/$*" || \
	    { echo "error: no such suite directory: $(DATASET)/$*"; exit 1; }
	$(CONFORMANCE) --h265nal $(H265NAL) -o $@ $(DATASET)/$*/*

# there is nothing here that knows how to build the binary, so just explain
# how to get one. The recipe is a no-op when the binary is already there,
# which is what keeps "make -B" working.
$(H265NAL):
	@test -x "$@" || { \
	    echo "error: cannot find $@"; \
	    echo "build it first: (cd .. && mkdir -p build && cd build &&" \
	        "cmake .. && make)"; \
	    echo "or point at another one: make H265NAL=/path/to/h265nal"; \
	    exit 1; \
	}

clean:
	rm -f $(CSVS)
