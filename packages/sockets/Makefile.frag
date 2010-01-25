Sockets_FILES = \

$(Sockets_FILES):
$(srcdir)/packages/sockets/stamp-classes: $(Sockets_FILES)
	touch $(srcdir)/packages/sockets/stamp-classes
