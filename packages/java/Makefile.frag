Java_FILES = \

$(Java_FILES):
$(srcdir)/packages/java/stamp-classes: $(Java_FILES)
	touch $(srcdir)/packages/java/stamp-classes
