#! @AWK@ -f

#######################################################################
#
#   Gtk wrapper creation scripts (C function declarations)
#
########################################################################


#######################################################################
#
# Copyright 2001, 2003, 2005 Free Software Foundation, Inc.
# Written by Paolo Bonzini and Dragomir Milivojevic
#
# This file is part of the GNU Smalltalk class library.
#
# The GNU Smalltalk class library is free software; you can redistribute it
# and/or modify it under the terms of the GNU Lesser General Public License
# as published by the Free Software Foundation; either version 2.1, or (at
# your option) any later version.
# 
# The GNU Smalltalk class library is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
# General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with the GNU Smalltalk class library; see the file COPYING.LESSER.
# If not, write to the Free Software Foundation, 59 Temple Place - Suite
# 330, Boston, MA 02111-1307, USA.  
#
########################################################################

BEGIN {
    print "\"Automatically generated, do not edit!\""
    FS = "[ (\t]+"

    type["void"] = "#void"
    type["int"] = "#int"
    type["char"] = "#char"
    type["double"] = "#double"
    type["gconstpointer"] = "#cObject"
    type["gpointer"] = "#cObject"

    type["va_list"] = "__skip_this__"
    type["GCallback"] = "__skip_this__"
    type["GClosure"] = "__skip_this__"
    type["GCClosure"] = "__skip_this__"
    type["GCompareDataFunc"] = "__skip_this__"
    type["GCompareFunc"] = "__skip_this__"
    type["GDestroyNotify"] = "__skip_this__"
    type["GFunc"] = "__skip_this__"
    type["GWeakNotify"] = "__skip_this__"

    # Skip GLib artifacts
    type["GFlags"] = "__skip_this__"
    type["GValue"] = "__skip_this__"
    type["GIo"] = "__skip_this__"
    type["GEnum"] = "__skip_this__"
    type["GPointer"] = "__skip_this__"
    type["GGstring"] = "__skip_this__"
    type["GStrdup"] = "__skip_this__"
    type["GTypeClass"] = "__skip_this__"
    type["GScanner"] = "__skip_this__"
    type["GBoxed"] = "__skip_this__"
    type["GSignal"] = "__skip_this__"

    type["PangoCoverage"] = "__skip_this__" #FIXME
    
    # FIXME: what about 64-bit architectures?
    type["gint8"] = "#int"
    type["guint8"] = "#int"
    type["gint16"] = "#int"
    type["guint16"] = "#int"
    type["gint32"] = "#int"
    type["guint32"] = "#int"
    type["GType"] = "#long"
    type["GtkType"] = "#long"

    type["gchar"] = "#char"
    type["guchar"] = "#int"
    type["gunichar"] = "#int"
    type["gshort"] = "#int"
    type["gushort"] = "#int"
    type["glong"] = "#long"
    type["gulong"] = "#long"
    type["gint"] = "#int"
    type["guint"] = "#int"
    type["gboolean"] = "#boolean"
    type["gfloat"] = "#float"
    type["gdouble"] = "#double"

    ptr_type["#int"] = "CInt type"
    ptr_type["#long"] = "CLong type"
    ptr_type["#boolean"] = "CBoolean type"
    ptr_type["#float"] = "CFloat type"
    ptr_type["#double"] = "CDouble type"
    ptr_type["#char"] = "#string"
    ptr_type["__byte__"] = "#byteArray"
    ptr_type["#cObject"] = "#cObject"

    # Fix asymmetry
    method_regexp = "^g_param_values?_|^g_param_type_|^gtk_file_chooser_"
    class["g_param_value_"] = "GParamSpec"
    class["g_param_values_"] = "GParamSpec"
    class["g_param_type_"] = "GParamSpec"

    # Not yet used
    method_skip_regexp = /^$/

    # Not really exact, this belongs in GtkFileChooserWidget too.
    # We need a way to do interfaces.
    class["gtk_file_chooser_"] = "GtkFileChooserDialog"
}

# Pick the correct case for the class (e.g. CList vs. Clist)
# and decide what will be the prefix for its methods

$1 == "struct" && $2 ~ /^_/ {
  name = substr($2, 2)
  define_class(name)
}

$0 ~ /^typedef (struct|union)/ && $3 ~ /_(G|Pango|Atk)/ {
  gsub(/[_;]/, "", $0)
  define_class($4)
}

$1 == "typedef" && $2 ~ /^(G|Pango|Atk)/ {
  # Take into account typedefs from a struct named differently.
  gsub(/[_;]/, "", $0)
  define_class($3)
}

match ($0, /^G_CONST_RETURN[ \t]*/) {
  $0 = substr ($0, RLENGTH + 1)
}

match($0, /^[ \t]*([a-zA-Z][a-zA-Z0-9]*[ \t\*]+)((g[a-z]*|pango)_[a-zA-Z0-9_]*)[ \t]*(\(.*)/, first_line) { 
  
  first_line[1] = gensub(/[ \t]+/, "", "G", first_line[1])

  cFuncName = first_line[2]

  if (match(first_line[2], method_skip_regexp))
    next
  else if (match(first_line[2], method_regexp))
    className = class[substr(first_line[2], 1, RLENGTH)]
  else if (match (first_line[2], /^(g_)?[a-z]*_/))
    className = smalltalkize(toupper(substr(first_line[2], 1, RLENGTH - 1)))

  if (tolower(className) in correct_case)
    className = correct_case[tolower(className)]

  first_line[2] = substr(first_line[2], RLENGTH + 1)
  creation = first_line[2] ~ /^(newv?|alloc)($|_)/

  # special case for the one API exception: gtk_ui_manager_new_merge_id
  # doesn't make a new ui_manager
  if (first_line[2] == "new_merge_id")
    creation = 0

  # Lose some symmetry for the sake of intuitiveness
  self = first_line[2] ~ /(^|_)((un)?ref$|(dis)?connect)/
  if (match(first_line[2], /^(draw|paint)_/))
    className = "GtkStyle"

  smalltalkFuncName = smalltalkize(first_line[2])

  # create one long string and tokenize it
  decl = first_line[1] " " first_line[2] " " first_line[4]
  while( match( decl, ";" ) == 0 )
  {
    getline

    for (i = 1; i <= NF; i++)
      # Convert arrays to pointers
      while( $i ~ /\[[0-9]*\]/ )
      {
        $i = "*" $i
        sub (/\[[0-9]*\]/, "", $i)
      }

    decl = decl " " $0
  }

  # Check for presence of pointers to functions
  if (decl ~ /\([ \t]*\*/)
    next

  gsub (/const[ \t]+/, "", decl)
  split( decl, arr )

  # tokenize
  decl = save_decl = ""
  void = 0
  strip_variadic = 0
  
  for(i = 1; i in arr; i++) {
    last = strip(tst)
    tst = arr[i]

    if (substr(tst, 1, 3) == "...") {
      # remove last argument if it was actually the first variadic argument
      if (decl ~ /: first/ && save_decl !~ /: first/) {
        strip_variadic = 1
        decl = save_decl
      }

      decl = decl " varargs: varargs"
      break
    }

    if( tst !~ /[,\)]/ )
      continue
  
    tst = smalltalkize( strip( tst ) )

    if (tst == "void" || last == "void") {
      void = 1
      break
    }

    if (decl != "")
    {
      save_decl = decl
      decl = decl " " tst ": " tst
      continue
    }

    if (i < 5 && last == className)
      self = 1

    if (i >= 5 || !self)
      decl = smalltalkFuncName ": " tst
  }

  argdecl = save_decl = ""
  tst = ""

  for(i = 1; i in arr; i++)
  {
    last = tst
    tst = arr[i]

    if( tst !~ /[,\)]/ )
      continue
  
    if (i < 5 && self)
      argdecl = argdecl " #self"

    else if (substr(tst, 1, 3) == "...")
      {
        if (strip_variadic)
          argdecl = save_decl
        argdecl = argdecl " #variadic"
        break
      }

    else if ( index( tst, "," ) > 0 || index( tst, ")" ) > 0 )
      {
        save_decl = argdecl
        argdecl = argdecl " " ctype(last, tst)
      }
  }

  if (creation)
    retType = returned(className "*")
  else 
    retType = returned(first_line[1])

  if (void)
    argdecl = ""

  if (decl == "")
    decl = smalltalkFuncName

  # skip some functions that we don't have bindings for

  if (type[className] == "__skip_this__" \
      || className == "GType" \
      || className == "GtkType" \
      || argdecl ~ /__skip_this__/ \
      || retType == "__skip_this__")
    next

  # print the declaration

  print "!" className (self ? "" : " class") " methodsFor: 'C call-outs'!"
  print decl
  print "    <cCall: '" cFuncName "' returning: " retType
  print "\targs: #(" argdecl " )>! !\n"

  if (decl == "getType")
    print "GLib registerType: " className "!\n"
}


# strips garbage from string
function strip( var )
{
  gsub( /[()\\,;*]/, "", var )
  gsub( /\[[0-9]*\]/, "", var )
  return var
}

function ctype( the_type, name )
{
  if (match (the_type, /\*+$/))
    { name = substr (the_type, RSTART) name
      the_type = substr (the_type, 1, RSTART - 1)
    }

  # pointers to pointers are CObject, and pointers to functions have
  # a standard naming convention in Gtk+.
  if (name ~ /^\*\*/ || the_type ~ /Func$/)
    return "#cObject"

  pointer = ((name ~ /^\*/) || (name ~ /\[[0-9]*\]/))

  res = type[the_type]

  if (!pointer || res == "__skip_this__")
    {
      if (res == "" || res == "__byte__")
        res = "#int"
    }
  else {
    res = ptr_type[res]
    if (res !~ /^#/)
      res = "#cObject"
  }

  return res
}

function returned( var )
{
  pointer = var ~ /\*$/
  var = strip(var)
  res = type[var]

  if (!pointer || res == "__skip_this__")
    {
      if (res == "")
        res = "#int"
    }
  else {
    res = ptr_type[res]
    if (res == "")
      res = var " type"
  }

  return res
}

function smalltalkize( res )
{
  first = substr (res, 1, 1)
  res = tolower( substr (res, 2) )

  while (j = index (res, "_")) {
    first = first substr(res, 1, j - 1) toupper( substr (res, j + 1, 1))
    res = substr (res, j + 2)
  }

  return first res
}

function define_class(name) {
  if (name ~ /(Class|Iface)$/)
    return

  # Bug report from GTK+ 2.2.2.  This can be dropped if it is safe.
  if (name == "GParam")
    return

  correct_case[tolower(name)] = name
  prefix = method_prefix(name)
  if (prefix in class)
    return

  method_regexp = method_regexp "|^" prefix
  class[prefix] = name
  # if object methods turn up on the wrong class (i.e. GtkUIManager on Gtk,
  # check prefix here... its probably wrong.
}

function method_prefix(name, i, ch, prev_up)
{
  prefix = ""
  prev_up = 0
  # Initialize so that the heuristic for consecutive uppercase
  # characters fires for GObject, GParam, etc.
  prev_notup = -2
  for (i = 1; i < length (name); i++)
    {
      ch = substr (name, i, 1)
      if (ch >= "A" && ch <= "Z")
	{
          if (break_word_before_uppercase(name, i, prev_up, prev_notup))
	    prefix = prefix "_"

	  prev_up = i
	}
      else
        prev_notup = i

      prefix = prefix ch
    }

  # Add final character.
  prefix = prefix substr (name, length (name))
  return tolower (prefix) "_"
}


function break_word_before_uppercase(name, cur_index, prev_up, prev_notup)
{
  # Always break on uppercase character preceded by lowercase
  if (prev_up != cur_index - 1)
    return 1

  # Never break before the first character
  if (prev_up == 0)
    return 0

  # May break if there are at least three consecutive uppercase chars...
  if (cur_index < prev_notup + 3)
    return 0

  # ...if last char was capitalised, this is, but next isn't.
  # This is for things like ui_manager => UIManager amongst others.
  ch = substr(name, cur_index + 1, 1)
  return ch < "A" || ch > "Z"
}

