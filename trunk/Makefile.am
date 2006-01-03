SUBDIRS = src tests

dist-hook:
	cd $(top_srcdir) && svn2cl --strip-prefix=trunk --group-by-day
	cp $(top_srcdir)/ChangeLog $(distdir)
