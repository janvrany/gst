/******************************** -*- C -*- ****************************
 *
 *	GNU Smalltalk generic inclusions.
 *
 *
 ***********************************************************************/

/***********************************************************************
 *
 * Copyright 1988,89,90,91,92,94,95,99,2000,2001,2002
 * Free Software Foundation, Inc.
 * Written by Steve Byrne.
 *
 * This file is part of GNU Smalltalk.
 *
 * GNU Smalltalk is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2, or (at your option) any later 
 * version.
 * 
 * GNU Smalltalk is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * GNU Smalltalk; see the file COPYING.  If not, write to the Free Software
 * Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
 *
 ***********************************************************************/

#ifndef GST_GST_H
#define GST_GST_H

/* AIX is so broken that requires this to be the first thing in the file.  */
#if defined(_AIX)
#pragma alloca
#else
# if !defined(alloca)		/* predefined by HP cc +Olibcalls */
#  ifdef __GNUC__
#   define alloca(size) __builtin_alloca(size)
#  else
#   if HAVE_ALLOCA_H
#    include <alloca.h>
#   else
#    if defined(__hpux)
void *alloca ();
#    else
#     if !defined(__OS2__) && !defined(WIN32)
char *alloca ();
#     else
#      include <malloc.h>	/* OS/2 defines alloca in here */
#     endif
#    endif
#   endif
#  endif
# endif
#endif

/* Some compilers use different win32 definitions. Define WIN32 so we 
   have only to check for one symbol.  */
#if defined(_WIN32) || defined(__CYGWIN32__) || defined(__CYGWIN__) || defined(Win32) || defined(__WIN32)
#ifndef WIN32
#define WIN32 1
#endif
#endif

#ifdef _MSC_VER
/* Visual C++ does not define STDC */
#define __STDC__ 1
#endif

/* Defined as char * in traditional compilers, void * in
   standard-compliant compilers.  */
#ifndef PTR
#if !defined(__STDC__)
#define PTR char *
#else
#define PTR void *
#endif
#endif

/* A boolean type */
#ifdef __cplusplus
typedef bool mst_Boolean;
#else
typedef enum {
  false,
  true
} mst_Boolean;
#endif

/* An indirect pointer to object data.  */
typedef struct OOP *OOP;

/* A direct pointer to the object data.  */
typedef struct mst_Object *mst_Object;

/* The contents of an indirect pointer to object data.  */
struct OOP
{
  mst_Object object;
  unsigned long flags;		/* FIXME, use uintptr_t */
};

/* The header of all objects in the system.
   Note how structural inheritance is achieved without adding extra levels of 
   nested structures.  */
#define OBJ_HEADER \
  OOP		objSize; \
  OOP		objClass


/* Just for symbolic use in sizeof's */
typedef struct gst_object_header
{
  OBJ_HEADER;
}
gst_object_header;

#define OBJ_HEADER_SIZE_WORDS	(sizeof(gst_object_header) / sizeof(PTR))

/* A bare-knuckles accessor for real objects */
struct mst_Object
{
  OBJ_HEADER;
  OOP data[1];			/* variable length, may not be objects, 
				   but will always be at least this
				   big.  */
};

/* Convert an OOP (indirect pointer to an object) to the real object
   data.  */
#define OOP_TO_OBJ(oop) \
  ((oop)->object)

/* Retrieve the class for the object pointed to by OOP.  OOP must be
   a real pointer, not a SmallInteger.  */
#define OOP_CLASS(oop) \
  (OOP_TO_OBJ(oop)->objClass)


/* Answer whether OOP is a SmallInteger or a `real' object pointer.  */
#define IS_INT(oop) \
  ((intptr_t)(oop) & 1)

/* Answer whether both OOP1 and OOP2 are SmallIntegers, or rather at
   least one of them a `real' object pointer.  */
#define ARE_INTS(oop1, oop2) \
  ((intptr_t)(oop1) & (intptr_t)(oop2) & 1)

/* Answer whether OOP is a `real' object pointer or rather a
   SmallInteger.  */
#define IS_OOP(oop) \
  (! IS_INT(oop) )

#endif /* GST_GST_H */
