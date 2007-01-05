EXTRA_DIST = $(doxygen_input) $(static_pages)

doxygen_input = header.html footer.html 
static_pages = 

htmldir = $(pkgdatadir)/html
html_DATA = $(static_pages) html/index.html

install-data-hook:
  cp -pR html/* $(DESTDIR)$(htmldir)

# Automake's "distcheck" is sensitive to having files left over
# after "make uninstall", so we have to clean up the install hook.
uninstall-local:
  rm -rf $(DESTDIR)$(htmldir)

dox: html/index.html

all-local: dox


html/index.html: Doxyfile $(doxygen_input)
  "@DOXYGEN@"


# Make tarfile to distribute the HTML documentation.
doc-dist: dox
  cp $(static_pages) html
  tar -czf $(PACKAGE)-docs-$(VERSION).tar.gz -C html .

pdf: @PACKAGE@.pdf
@PACKAGE@.pdf:
  $(MAKE) -C ./latex pdf
  ln -s ./latex/refman.ps   @PACKAGE@.ps
  ln -s ./latex/refman.pdf @PACKAGE@.pdf

clean-local:
  $(RM) -r latex
  $(RM) -r html man @PACKAGE@.ps @PACKAGE@.pdf
