NAME=proj4

do: $(NAME).tex
	latex $(NAME).tex
	bibtex $(NAME)
	latex $(NAME).tex
	latex $(NAME).tex
	dvips $(NAME).dvi
	ps2pdf -sPAPERSIZE=a4 $(NAME).ps
	
clean:
	rm -f $(NAME).{dvi,aux,log,ps,out}
