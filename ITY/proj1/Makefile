NAME=proj1

do: $(NAME).tex
	latex $(NAME).tex
	latex $(NAME).tex
	dvips $(NAME).dvi
	ps2pdf $(NAME).ps
	
clean:
	rm -f $(NAME).{dvi,aux,log,ps,out}
