/******************************** -*- C -*- ****************************
 *
 *	Byte code compiler inlines
 *
 *
 ***********************************************************************/

/***********************************************************************
 *
 * Copyright 2000, 2001, 2002 Free Software Foundation, Inc.
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



/* Returns the bytecodes of the CompiledMethod or CompiledBlock,
   METHODOOP */
#define GET_METHOD_BYTECODES(methodOOP) \
  ( ((gst_compiled_method)OOP_TO_OBJ(methodOOP))->bytecodes )

/* Returns the number of literals in the CompiledMethod or CompiledBlock,
   METHODOOP */
#define NUM_METHOD_LITERALS(methodOOP) \
  NUM_WORDS (OOP_TO_OBJ(((gst_compiled_method)OOP_TO_OBJ(methodOOP))->literals))

/* Returns the literals of the CompiledMethod or CompiledBlock,
   METHODOOP */
#define GET_METHOD_LITERALS(methodOOP) \
  ( OOP_TO_OBJ(((gst_compiled_method)OOP_TO_OBJ(methodOOP))->literals)->data )

/* Returns the header of the CompiledMethod, METHODOOP */
#define GET_METHOD_HEADER(methodOOP) \
  (((gst_compiled_method)OOP_TO_OBJ(methodOOP))->header)

/* Returns the class in which the CompiledMethod or CompiledBlock,
   METHODOOP, was defined */
#define GET_METHOD_CLASS(methodOOP) \
  (((gst_method_info)OOP_TO_OBJ(get_method_info(methodOOP)))->class)

/* Returns the selector under which the CompiledMethod or CompiledBlock,
   METHODOOP, was defined */
#define GET_METHOD_SELECTOR(methodOOP) \
  (((gst_method_info)OOP_TO_OBJ(get_method_info(methodOOP)))->selector)

/* Returns the header of the CompiledBlock, BLOCKOOP */
#define GET_BLOCK_HEADER(blockOOP) \
  (((gst_compiled_block)OOP_TO_OBJ(blockOOP))->header)

/* Returns the method for the CompiledBlock, BLOCKOOP */
#define GET_BLOCK_METHOD(blockOOP) \
  (((gst_compiled_block)OOP_TO_OBJ(blockOOP))->method)

/* Returns the number of arguments of the CompiledMethod or CompiledBlock
   pointed to by METHODOOP */
#define GET_METHOD_NUM_ARGS(methodOOP) \
  (GET_METHOD_HEADER (methodOOP).numArgs)

/* Returns the method descriptor of OOP (either the CompiledMethod's
   descriptor or the descriptor of the home method if a
   CompiledBlock).  */
static inline OOP get_method_info (OOP oop);

OOP
get_method_info (OOP oop)
{
  mst_Object obj;
  obj = OOP_TO_OBJ (oop);
  if UNCOMMON (obj->objClass == _gst_compiled_block_class)
    {
      oop = ((gst_compiled_block) obj)->method;
      obj = OOP_TO_OBJ (oop);
    }

  return ((gst_compiled_method) obj)->descriptor;
}

