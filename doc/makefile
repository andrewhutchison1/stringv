TEX=pdflatex
TEXFLAGS=-shell-escape -interaction=nonstopmode -file-line-error

.PHONY: all
all: stringv.tex
	$(TEX) $(TEXFLAGS) $< && mv stringv.pdf release/

.PHONY: clean
clean:
	rm -rf *.log *.aux *.pdf release/stringv.pdf
