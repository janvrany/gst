Digest_FILES = \

$(Digest_FILES):
$(srcdir)/packages/digest/stamp-classes: $(Digest_FILES)
	touch $(srcdir)/packages/digest/stamp-classes
