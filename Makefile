EXECUTABLES:=bin/norvig_py bin/norvig_cc_umap bin/norvig_cc_hat bin/norvig_hs bin/norvig_hs_trie bin/norvig_c_dp bin/norvig_rkt
CABAL_MODULES:=haskell haskell-trie
MAKE_MODULES:=cxx_umap cxx_hat c_dp
ALL_MODULES:=$(CABAL_MODULES) $(MAKE_MODULES)
BENCHMARKS=$(EXECUTABLES:bin/%=benchmarks/%.md)
DATAFILES:=data/train.txt data/test.txt data/output.txt

ALL: $(EXECUTABLES)

$(MAKE_MODULES):
	$(MAKE) -C $@

$(CABAL_MODULES):
	cd $@ &&\
	cabal configure --enable-optimization=2 &&\
	cabal build

$(OTHER_MODULES):

.PHONY: ALL $(ALL_MODULES)

bin/norvig_py: python2/norvig.py
	cp -a $< $@

bin/norvig_rkt: racket/norvig.rkt
	cp -a $< $@

bin/norvig_cc_umap: cxx_umap
	cp -a cxx_umap/norvig $@

bin/norvig_cc_hat: cxx_hat
	cp -a cxx_hat/norvig $@

bin/norvig_hs: haskell
	cp -a haskell/dist/build/norvig/norvig $@

bin/norvig_hs_trie: haskell-trie
	cp -a haskell-trie/dist/build/norvig/norvig $@

bin/norvig_c_dp: c_dp
	cp -a c_dp/norvig $@

benchmark: benchmarks/all.md
.PHONY: benchmark

benchmarks/all.md: $(BENCHMARKS)
	cat $^ > $@
benchmarks/%.md: bin/% util/mk_benchmark $(DATAFILES)
	util/mk_benchmark $< > $@

clean:
	for dir in $(CABAL_MODULES); do\
		cd $(CURDIR)/$$dir && cabal clean;\
	done
	for dir in $(MAKE_MODULES); do\
		$(MAKE) -C $$dir clean;\
	done
	rm -f $(EXECUTABLES) $(BENCHMARKS) benchmarks/all.md
.PHONY: clean

