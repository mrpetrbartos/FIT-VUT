NAME=proj3

do: $(NAME).tex
	latex $(NAME).tex
	latex $(NAME).tex
	dvips $(NAME).dvi
	ps2pdf -sPAPERSIZE=a4 $(NAME).ps
	
clean:
	rm -f $(NAME).{dvi,aux,log,ps,out}
