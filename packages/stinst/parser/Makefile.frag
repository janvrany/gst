Parser_FILES = \

$(Parser_FILES):
$(srcdir)/packages/stinst/parser/stamp-classes: $(Parser_FILES)
	touch $(srcdir)/packages/stinst/parser/stamp-classes
