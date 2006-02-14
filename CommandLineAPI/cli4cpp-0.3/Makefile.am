AUTOMAKE_OPTIONS = 1.4
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = include src test examples doc

#man_MANS = cli.3


EXTRA_DIST = 
  autogen.sh
  $(m4sources)

m4sources = \
  m4/bb_enable_doxygen.m4

# Not sure what is creating the timestamp file.
# The so_locations file only happens on IRIX.
DISTCLEANFILES = config/stamp-h1 so_locations

.PHONY: release snapshot rpm docs doc-dist

release:
  rm -rf .deps */.deps
  $(MAKE) distcheck

snapshot:
  $(MAKE) dist distdir=$(PACKAGE)-`date +%Y-%m-%d`

rpm: dist
  rpm -ta $(PACKAGE)-$(VERSION).tar.gz
  mv -f /usr/src/redhat/SRPMS/$(PACKAGE)-$(VERSION)-*.rpm .
  mv -f /usr/src/redhat/RPMS/*/$(PACKAGE)-$(VERSION)-*.rpm .
  mv -f /usr/src/redhat/RPMS/*/$(PACKAGE)-doc-$(VERSION)-*.rpm .

debian:
  chmod a+x debian/rules
  dpkg-buildpackage -rfakeroot -sa -us -uc -tc 

doc-dist:
  $(MAKE) -C doc doc-dist
  mv -f doc/$(PACKAGE)-docs-$(VERSION).tar.gz .


