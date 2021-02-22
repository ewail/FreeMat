/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkWrapOctave.c,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen, Dragan Tubic
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.
  
  =========================================================================*/

/* Derived from octaviz file vtkWrapOctave.c from the octaviz project */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "vtkParse.h"

int numberOfWrappedFunctions = 0;
FunctionInfo *wrappedFunctions[1000];
extern FunctionInfo *currentFunction;

const char * typeName(int p)
{
  switch (p)
    {
    case 0x1: return "float";
    case 0x2: return "void";
    case 0x3: return "char";
    case 0x4: return "int";
    case 0x5: return "short";
    case 0x6: return "long";
    case 0x7: return "double";
    case 0x13: return "unsigned char";
    case 0x14: return "unsigned int";
    case 0x15: return "unsigned short";
    case 0x16: return "unsigned long";
    default:
      return "unknown";
    }
}

static void emitTempScalarFromArgs(FILE *fp, int i, int j, const char *typecode)
{
  fprintf(fp,"  if ( !arg[%i].isScalar() )\n",j);
  fprintf(fp,"    throw Exception(\"Expecting a scalar argument\");\n");
  fprintf(fp,"  temp%i = (%s) arg[%i].asDouble();\n",i, typecode, j);
}

static void emitTempArrayFromArgs(FILE *fp, int i, int k, int j, const char *typecode)
{
  fprintf(fp,"  if ( !arg[%i].isScalar() )\n",j);
  fprintf(fp,"    throw Exception(\"Expecting a scalar argument\");");
  fprintf(fp,"  temp%i[%i] = (%s) arg[%i].asDouble();\n",i, k, typecode, j);
}

int isVoidType(int aType)
{
  return (((aType % 0x10) == 2)&&(!((aType%0x1000)/0x100)));
}

void outputFunctionHelp( FILE *fp, FileInfo *data )
{
  int i, j;
  fprintf(fp,"void populate_help_map%s( std::map<std::string,std::string> &help_map ) \n{\n",data->ClassName);
  fprintf(fp,"  help_map[\"List\"] = \"");
  for ( i = 0; i < numberOfWrappedFunctions; i++ )
    {
      fprintf(fp,"%s\\n",wrappedFunctions[i]->Name);
    }
  fprintf(fp,"\";\n");
	
  for ( i = 0; i < numberOfWrappedFunctions; i++ )
    {
      if ( wrappedFunctions[i]->Comment == NULL ) continue;
		
      fprintf(fp,"  help_map[\"%s\"] = \"", wrappedFunctions[i]->Name);
      j = 0;
      while ( wrappedFunctions[i]->Comment[j] != 0 )
	{
	  if ( wrappedFunctions[i]->Comment[j] == '"' ) fprintf(fp,"\\");
			
	  if ( wrappedFunctions[i]->Comment[j] == '\n' )
	    {
	      /* Don't want the last newline */
	      if ( j + 2 < strlen(wrappedFunctions[i]->Comment) )
		{
		  fprintf(fp,"\\n\\\n  ");
		}
	      j++;
	    } else
	    {
	      // double escape everything that is escaped;
	      // gcc doesn't like things like \$, \frac, \sum, ...
	      // in strings; luckily, this happens only in
	      // the 'Descriptions:' field
	      if (wrappedFunctions[i]->Comment[j] == '\\')
		{
		  fprintf(fp, "\\\\");
		  j++;
		}

	      fprintf(fp,"%c",wrappedFunctions[i]->Comment[j++]);
	    }
	}
      fprintf(fp,"\";\n" );
    }
  fprintf(fp,"}\n");
}

void output_temp(FILE *fp, int i, int aType, char *Id, int count)
{
  /* handle VAR FUNCTIONS */
  if (aType == 0x5000)
    {
      fprintf(fp,"    vtkTclVoidFuncArg *temp%i = new vtkTclVoidFuncArg;\n",i);
      return;
    }
  
  /* ignore void */
  if (isVoidType(aType))
    {
      return;
    }

  /* for const * return types prototype with const */
  if ((i == MAX_ARGS) && (aType%0x2000 >= 0x1000))
    {
      fprintf(fp,"  const ");
    }
  else
    {
      fprintf(fp,"  ");
    }

  if ((aType%0x100)/0x10 == 0x1)
    {
      fprintf(fp,"unsigned ");
    }

  switch (aType%0x10)
    {
      /* FIXME 7 should be double but then vtkActor2D wont compile! */
      /* 7 should be double! */
    case 0x1:   fprintf(fp,"float  "); break;
    case 0x7:   fprintf(fp,"double "); break;
    case 0x4:   fprintf(fp,"int    "); break;
    case 0x5:   fprintf(fp,"short  "); break;
    case 0x6:   fprintf(fp,"long   "); break;
    case 0x2:     fprintf(fp,"void   "); break;
    case 0x3:     fprintf(fp,"char   "); break;
    case 0xA:   fprintf(fp,"vtkIdType "); break;
    case 0xB:   fprintf(fp,"long long "); break;
    case 0xC:   fprintf(fp,"__int64 "); break;
    case 0xD:     fprintf(fp,"signed char "); break;
    case 0xE:     fprintf(fp,"bool "); break;
    case 0x9:     fprintf(fp,"%s ",Id); break;
    case 0x8: return;
    }

  /* handle array arguements */
  if (count > 1)
    {
      fprintf(fp,"temp%i[%i];\n",i,count);
      return;
    }
  
  switch ((aType%0x1000)/0x100)
    {
    case 0x1: fprintf(fp, " *"); break; /* act " &" */
    case 0x2: fprintf(fp, "&&"); break;
    case 0x3: fprintf(fp, " *"); break;
    case 0x4: fprintf(fp, "&*"); break;
    case 0x5: fprintf(fp, "*&"); break;
    case 0x7: fprintf(fp, "**"); break;
    default: fprintf(fp,"  "); break;
    }
  
  fprintf(fp,"temp%i",i);
  fprintf(fp,";\n");
}

/* when the cpp file doesn't have enough info use the hint file */
void use_hints(FILE *fp)
{
  int  i;
  /* use the hint */
  switch (currentFunction->ReturnType%0x1000)
    {
    case 0x301: case 0x307:  
    case 0x304: case 0x305: case 0x306: 
    case 0x30A: case 0x30B: case 0x30C: case 0x30D: case 0x30E:
      /* float array */
      fprintf(fp,"  BasicArray<double> tempResult(NTuple(%i,1));\n",currentFunction->HintSize);
      for (i = 0; i < currentFunction->HintSize; i++)
        {
	  fprintf(fp,"  tempResult[%i] = (double)temp%i[%i];\n",i+1,MAX_ARGS,i);
        }
      fprintf(fp,"  retval = Array(tempResult);\n");
      break;
    }
}

void return_result(FILE *fp)
{
  switch (currentFunction->ReturnType%0x1000)
    {
    case 0x2:
      /* void, do nothing */
      break;
    case 0x1: case 0x7: 
    case 0x4: case 0x5: case 0x6: case 0xA: case 0xB: case 0xC: case 0xD:
    case 0x13: case 0x14: case 0x15: case 0x16: case 0x1A: case 0x1B: case 0x1C:
    case 0x3:
      fprintf(fp,"  retval = Array(double(temp%i));\n",MAX_ARGS); 
      break;
    case 0x303:
      /* string. i.e. char* */ 
      fprintf(fp,"  retval = Array(QString(temp%i));\n",MAX_ARGS);
      break;
    case 0x109:
    case 0x309:  
      fprintf(fp,"  retval = MakeVTKPointer((vtkObjectBase*)(temp%i),\"%s\",eval,true);\n",MAX_ARGS,
	      currentFunction->ReturnClass);
      break;
      /* handle functions returning vectors */
      /* this is done by looking them up in a hint file */
    case 0x301: case 0x307: case 0x313:
    case 0x304: case 0x305: case 0x306: case 0x30A: case 0x30B: case 0x30C:
    case 0x30D: case 0x30E: case 0x31A: case 0x31B: case 0x31C:
      use_hints(fp);
      break;
    default:
      fprintf(fp,"  retval = Array(QString(\"unable to return result.\"));\n");
      break;
    }
}

void handle_return_prototype(FILE *fp)
{
  switch (currentFunction->ReturnType%0x1000)
    {
    case 0x109:
    case 0x309:  
      /* FIXME
	 fprintf(fp,"    int %sCommand(ClientData, Tcl_Interp *, int, char *[]);\n",currentFunction->ReturnClass);
      */
      break;
    }
}

void get_args(FILE *fp, int i)
{
  int j;
  int start_arg = 1;
  
  /* what arg do we start with */
  for (j = 0; j < i; j++)
    {
      start_arg = start_arg + 
	(currentFunction->ArgCounts[j] ? currentFunction->ArgCounts[j] : 1);
    }
  
  /* handle VAR FUNCTIONS */
  if (currentFunction->ArgTypes[i] == 0x5000)
    {
      fprintf(fp,"    temp%i->interp = interp;\n",i);
      fprintf(fp,"    temp%i->command = strcpy(new char [strlen(argv[2])+1],argv[2]);\n",i);
      return;
    }

  /* ignore void */
  if (((currentFunction->ArgTypes[i] % 0x10) == 2)&&
      (!((currentFunction->ArgTypes[i]%0x1000)/0x100)))
    {
      return;
    }
  
  switch (currentFunction->ArgTypes[i]%0x1000)
    {
    case 0x1: case 0x7:  
      /* floating point, i.e. double */
      emitTempScalarFromArgs(fp,i,start_arg,"double");
      break;
    case 0x4: case 0x5: case 0x6: case 0xA: case 0xB: case 0xC: case 0xD:
      /* int */ 
      emitTempScalarFromArgs(fp,i,start_arg,"int");
      break;
    case 0x3:
      /* char */
      emitTempScalarFromArgs(fp,i,start_arg,"char");
      break;
    case 0x13:
      /* unsigned char */
      emitTempScalarFromArgs(fp,i,start_arg,"unsigned char");
      break;
    case 0x14:
      /* unsigned int */
      emitTempScalarFromArgs(fp,i,start_arg,"unsigned int");
      break;
    case 0x15:
      /* unsigned short */
      emitTempScalarFromArgs(fp,i,start_arg,"unsigned short");
      break;
    case 0x16:
      /* unsigned long */
      emitTempScalarFromArgs(fp,i,start_arg,"unsigned long");
      break;
    case 0x303:
      /* char* */
      fprintf(fp,"  char tmp_string%i[1024];\n",i);
      fprintf(fp,"  strcpy(tmp_string%i,qPrintable(arg[%i].asString())); \n",i,start_arg);
      fprintf(fp,"  temp%i = tmp_string%i;\n",i,i);
      break;
      /* FLOAT, int, short, long, double FIX */
    case 0x301: case 0x304: case 0x305: case 0x306: case 0x307: 
    case 0x314: case 0x313: case 0x315: case 0x316:
      /* Check if the args(start_arg) is a vector */
      if ( currentFunction->ArgCounts[i] <= 1 )
	{
	  fprintf(fp,"    if ( !arg[%i].isVector() )\n",start_arg,start_arg);
	  fprintf(fp,"       throw Exception(\"expecting a vector\");\n");
	  /* now allocate the array and copy vector */
	  fprintf(fp,"    else \n      {\n");
	  fprintf(fp,"      Array vect = arg[%i].asDenseArray().toClass(Double);\n",start_arg);
	  fprintf(fp,"      BasicArray<double> data = vect.real<double>();\n");
	  fprintf(fp,"      int length = data.length();\n");
	  fprintf(fp,"      temp%i = new %s[length];\n", i, typeName(currentFunction->ArgTypes[i]%0x100));
	  fprintf(fp,"      for ( int k = 0; k < length; k++ ) temp%i[k] = (%s)data[k+1];\n      }\n",
		  i,typeName(currentFunction->ArgTypes[i]%0x100));
	} else
	{
	  fprintf(fp,"  if ( !(arg[%i].isVector() && arg[%i].length() == %i) )\n",
		  start_arg,start_arg,currentFunction->ArgCounts[i]);
	  fprintf(fp,"    throw Exception(\"Mismatch in vector lengths\");\n");
	  /* now allocate the array and copy octave vector */
	  fprintf(fp,"  {\n");
	  fprintf(fp,"    Array vect = arg[%i].asDenseArray().toClass(Double);\n",start_arg);
	  fprintf(fp,"    BasicArray<double> data = vect.real<double>();\n");
	  fprintf(fp,"    int length = data.length();\n");
	  fprintf(fp,"    for ( int k = 0; k < length; k++ ) temp%i[k] = (%s)data[k+1];\n",
		  i,typeName(currentFunction->ArgTypes[i]%0x100));
	  fprintf(fp,"  }\n");
	}
      break;
    case 0x109:
    case 0x309:
      fprintf(fp,"  temp%i = GetVTKPointer<%s>(arg[%i]);\n",
	      i,currentFunction->ArgClasses[i],start_arg);
      break;
    case 0x2:    
    case 0x9:
      break;
    default:
      if (currentFunction->ArgCounts[i] > 1)
        {
	  for (j = 0; j < currentFunction->ArgCounts[i]; j++)
	    {
	      switch (currentFunction->ArgTypes[i]%0x100)
		{
		case 0x1: case 0x7:  
		  /* floating point, i.e. double */
		  emitTempArrayFromArgs(fp, i, j, start_arg, "double");
		  break;
		case 0x4: case 0x5: case 0x6: 
		  /* int */
		  emitTempArrayFromArgs(fp, i, j, start_arg, "int");
		  break;
		case 0x3:
		  /* char */
		  emitTempArrayFromArgs(fp, i, j, start_arg, "char");
		  break;
		case 0x13:
		  /* unsigned char */
		  emitTempArrayFromArgs(fp, i, j, start_arg, "unsigned char");
		  break;
		case 0x14:
		  /* unsigned int */
		  emitTempArrayFromArgs(fp, i, j, start_arg, "unsigned int");
		  break;
		case 0x15:
		  /* unsigned short */
		  emitTempArrayFromArgs(fp, i, j, start_arg, "unsigned short");
		  break;
		case 0x16:
		  /* unsigned long */
		  emitTempArrayFromArgs(fp, i, j, start_arg, "unsigned long");
		  break;
		}
	      start_arg++;
	    }
        }
      
    }
}

#if 0
void outputSubsasgnFunction(FILE *fp, FileInfo *data)
{
  int i;
  fprintf(fp,"//@@Signature\n");
  fprintf(fp,"//sfunction @%s:subsasgn %SubsasgnFunction\n",data->ClassName,
	  data->ClassName);
  fprintf(fp,"//input varargin\n");
  fprintf(fp,"//output varargin\n");
  fprintf(fp,"ArrayVector %sSubsasgnFunction(int nargout, const ArrayVector& arg) {\n",
	  data->ClassName);
  fprintf(fp,"  if (arg.size() != 3) return ArrayVector();\n");
  fprintf(fp,"  %s* vtk_pointer = GetVTKPointer<%s>(arg[0]);\n",data->ClassName,data->ClassName);
  fprintf(fp,"  const StructArray& sa = arg[1].constStructPtr();\n");
  fprintf(fp,"  const BasicArray<Array>& typea = sa[\"type\"];\n");
  fprintf(fp,"  const BasicArray<Array>& subsa = sa[\"subs\"];\n");
  fprintf(fp,"  if ((typea.length() == 1) && (typea[1].asString() == \".\"))\n");
  fprintf(fp,"  {\n");
  for (i=0;i<data->NumberOfFunctions;i++)
    if (data->Functions[i].Name &&
	strncmp(data->Functions[i].Name,"Set",3) == 0)
      {
	fprintf(fp,"    if (subsa[1].asString() == \"%s\")\n",
		data->Functions[i].Name+3);
	fprintf(fp,"    {\n");
	fprintf(fp,"      ArrayVector tmp(arg[0]);\n");
	fprintf(fp,"      tmp.push_back(arg[2]);\n");
	fprintf(fp,"      return %s%sFunction(nargout,tmp,eval);\n",
		data->ClassName,data->Functions[i].Name);
	fprintf(fp,"    }\n");
      }
  fprintf(fp,"  }\n");
  fprintf(fp,"  return ArrayVector();\n");
  fprintf(fp,"}\n");
}
#endif

void outputSubsrefFunction(FILE *fp, FileInfo *data)
{						
  int i;
  fprintf(fp,"//@@Signature\n");
  fprintf(fp,"//function @%s:subsref %sSubsrefFunction\n",data->ClassName,
	  data->ClassName);
  fprintf(fp,"//input varargin\n");
  fprintf(fp,"//output varargout\n");
  fprintf(fp,"ArrayVector %sSubsrefFunction(int nargout, const ArrayVector& arg) {\n",
	  data->ClassName);
  fprintf(fp,"  if (arg.size() != 2) return ArrayVector();\n");
  fprintf(fp,"  %s* vtk_pointer = GetVTKPointer<%s>(arg[0]);\n",data->ClassName,data->ClassName);
  fprintf(fp,"  const StructArray& sa = arg[1].constStructPtr();\n");
  fprintf(fp,"  const BasicArray<Array>& typea = sa[\"type\"];\n");
  fprintf(fp,"  const BasicArray<Array>& subsa = sa[\"subs\"];\n");
  fprintf(fp,"  if ((typea.length() == 1) && (typea[1].asString() == \".\"))\n");
  fprintf(fp,"  {\n");
  for (i=0;i<data->NumberOfFunctions;i++)
    if (data->Functions[i].Name &&
	(strncmp(data->Functions[i].Name,"Get",3) == 0) &&
	(data->Functions[i].NumberOfArguments == 0) &&
	data->Functions[i].IsValid)
      {
	fprintf(fp,"    if (subsa[1].asString() == \"%s\")\n",
		data->Functions[i].Name+3);
	fprintf(fp,"      return %s%sFunction(nargout,arg,eval);\n",
		data->ClassName,data->Functions[i].Name);
      }
  fprintf(fp,"  }\n");
  fprintf(fp,"  if ((typea.length() == 2) && (typea[1].asString() == \".\")\n");
  fprintf(fp,"      && (typea[2].asString() == \"()\"))\n");
  fprintf(fp,"  {\n");
  for (i=0;i<data->NumberOfFunctions;i++)
    if (data->Functions[i].Name &&
	(strncmp(data->Functions[i].Name,"Get",3) == 0) &&
	(data->Functions[i].NumberOfArguments > 0) &&
	data->Functions[i].IsValid)
      {
	fprintf(fp,"    if (subsa[1].asString() == \"%s\")\n",
		data->Functions[i].Name+3);
	fprintf(fp,"    {\n");
	fprintf(fp,"      ArrayVector tmp(arg[0]);\n");
	fprintf(fp,"      tmp += ArrayVectorFromCellArray(subsa[2]);\n");
	fprintf(fp,"      return %s%sFunction(nargout,tmp,eval);\n",
		data->ClassName,data->Functions[i].Name);
	fprintf(fp,"    }\n");
      }
  fprintf(fp,"  }\n");
  fprintf(fp,"}\n\n");
}

void outputDisplayFunction(FILE *fp, FileInfo *data)
{
  int i;
  fprintf(fp,"//@@Signature\n");
  fprintf(fp,"//sfunction @%s:display %sDisplayFunction\n",
	  data->ClassName,data->ClassName);
  fprintf(fp,"//input varargin\n");
  fprintf(fp,"//output varargout\n");
  fprintf(fp,"ArrayVector %sDisplayFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {\n",
	  data->ClassName);
  fprintf(fp,"  if (arg.size() == 0) return ArrayVector();\n");
  fprintf(fp,"  if (arg[0].length() > 1) {\n");
  fprintf(fp,"     PrintArrayClassic(arg[0],100,eval);\n");
  fprintf(fp,"     return ArrayVector();\n");
  fprintf(fp,"  }\n");
  fprintf(fp,"  %s* vtk_pointer = GetVTKPointer<%s>(arg[0]);\n",data->ClassName,data->ClassName);
  fprintf(fp,"  std::ostringstream ss;\n");
  fprintf(fp,"  vtkIndent ind;\n");
  fprintf(fp,"  vtk_pointer->PrintSelf(ss,ind);\n");
  fprintf(fp,"  eval->outputMessage(QString::fromStdString(ss.str()));\n");
  fprintf(fp,"  return ArrayVector();\n");
  fprintf(fp,"}\n");
}

void outputDeleteFunction(FILE *fp, FileInfo *data)
{
  fprintf(fp,"//@@Signature\n");
  fprintf(fp,"//function @%s:delete %sDeleteFunction\n",
	  data->ClassName,data->ClassName);
  fprintf(fp,"//input varargin\n");
  fprintf(fp,"//output none\n");
  fprintf(fp,"ArrayVector %sDeleteFunction(int nargout, const ArrayVector& arg) {\n",data->ClassName);
  fprintf(fp,"  if (arg.size() == 0) return ArrayVector();\n");
  fprintf(fp,"  DeleteVTKObject(arg[0]);\n");
  fprintf(fp,"  return ArrayVector();\n");
  fprintf(fp,"}\n");
}


void computeAbbreviatedSignature(FunctionInfo *func, char *buf)
{
  int i;
  int argtype;
  for (i=0;i<func->NumberOfArguments;i++)
    {
      argtype = func->ArgTypes[i] % 0x1000;
      switch (argtype)
	{
	case 0x301: case 0x304:	case 0x307:
	case 0x30A: case 0x30B:	case 0x30C:
	  *buf = 'V'; buf++;
	  break;
	case 0x1: case 0x7: case 0x14: case 0x4:
	case 0x15: case 0x5: case 0x16:	case 0x6:
	case 0x1A: case 0xA: case 0x1B: case 0x1C:
	case 0xB: case 0xC: case 0xD: case 0x3:
	case 0x13: case 0xE:
	  *buf = 's'; buf++;
	  break;
	case 0x302:
	  *buf = 'S'; buf++;
	  break;
	case 0x109:
	case 0x309:
	  *buf = 'O'; buf++;
	  strcpy(buf,func->ArgClasses[i]); buf += strlen(func->ArgClasses[i]);
	  break;
	default:
	  *buf = '?'; buf++;
	}
    }
  *buf = 0;
}

void outputOverloadedFunction(FILE *fp, FileInfo *data)
{
  int i, j, argtype;
  fprintf(fp,"//@@Signature\n");
  fprintf(fp,"//sfunction @%s:%s %s%sFunction\n",
	  data->ClassName,currentFunction->Name,
	  data->ClassName,currentFunction->Name);
  fprintf(fp,"//input varargin\n");
  fprintf(fp,"//output varargout\n");
  fprintf(fp,"ArrayVector %s%sFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {\n",
	  data->ClassName,currentFunction->Name);
  for (i=0;i<data->NumberOfFunctions;i++)
    if (data->Functions[i].Name &&
	(strcmp(data->Functions[i].Name,currentFunction->Name) == 0)
	&& data->Functions[i].IsValid)
      {
	fprintf(fp,"  if ((arg.size() == %d)",data->Functions[i].NumberOfArguments+1);
	// Loop over the arguments
	for (j=0;j<data->Functions[i].NumberOfArguments;j++)
	  {
	    argtype = data->Functions[i].ArgTypes[j] % 0x1000;
	    switch (argtype)
	      {
	      case 0x301: case 0x304:	case 0x307:
	      case 0x30A: case 0x30B:	case 0x30C:
		fprintf(fp," && VTKIsVectorArg(arg[%d])",j+1);
		break;
	      case 0x1: case 0x7: case 0x14: case 0x4:
	      case 0x15: case 0x5: case 0x16:	case 0x6:
	      case 0x1A: case 0xA: case 0x1B: case 0x1C:
	      case 0xB: case 0xC: case 0xD: case 0x3:
	      case 0x13: case 0xE:
		fprintf(fp," && VTKIsScalarArg(arg[%d])",j+1);
		break;
	      case 0x302:
		fprintf(fp," && VTKIsStringArg(arg[%d])",j+1);
		break;
	      case 0x109:
	      case 0x309:
		fprintf(fp," && VTKIsClass(arg[%d],\"%s\")",
			j+1,data->Functions[i].ArgClasses[j]);
		break;
	      default:
		break;
	      }
	  }
	fprintf(fp,")\n    return %s%s__%dFunction(nargout,arg,eval);\n",
		data->ClassName,
		data->Functions[i].Name,
		data->Functions[i].OverloadCount);
      }
  fprintf(fp,"  throw Exception(\"unable to resolve to an overloaded instance of %s%s\");\n",
	  data->ClassName,currentFunction->Name);
  fprintf(fp,"}\n\n");
}

void outputFunction(FILE *fp, FileInfo *data)
{
  int i;
  int args_ok = 1;
 
  //  printf("Processing function %s\n",currentFunction->Name);

  /* some functions will not get wrapped no matter what else */
  if (currentFunction->IsOperator || 
      currentFunction->ArrayFailure ||
      !currentFunction->IsPublic ||
      !currentFunction->Name) 
    {
      return;
    }

  /* check to see if we can handle the args */
  for (i = 0; i < currentFunction->NumberOfArguments; i++)
    {
      /* FIXME */
      if (currentFunction->ArgTypes[i] == 0x5000) return;
      if ((currentFunction->ArgTypes[i]%0x10) == 0x8) args_ok = 0;
      /* if its a pointer arg make sure we have the ArgCount */
      if ((currentFunction->ArgTypes[i]%0x1000 >= 0x100) &&
	  (currentFunction->ArgTypes[i]%0x1000 != 0x303)&&
	  (currentFunction->ArgTypes[i]%0x1000 != 0x301)&& /* FLOAT* FIX */
	  (currentFunction->ArgTypes[i]%0x1000 != 0x303)&& /* char* FIX */
	  (currentFunction->ArgTypes[i]%0x1000 != 0x304)&& /* int* FIX */
	  (currentFunction->ArgTypes[i]%0x1000 != 0x305)&& /* short* FIX */
	  (currentFunction->ArgTypes[i]%0x1000 != 0x306)&& /* long* FIX */
	  (currentFunction->ArgTypes[i]%0x1000 != 0x307)&& /* double* FIX */
	  (currentFunction->ArgTypes[i]%0x1000 != 0x313)&& /* unsigned char* FIX */
	  (currentFunction->ArgTypes[i]%0x1000 != 0x314)&& /* unsigned int* FIX */
	  (currentFunction->ArgTypes[i]%0x1000 != 0x315)&& /* unsigned short* FIX */
	  (currentFunction->ArgTypes[i]%0x1000 != 0x316)&& /* unsigned long* FIX */
	  (currentFunction->ArgTypes[i]%0x1000 != 0x309)&&
	  (currentFunction->ArgTypes[i]%0x1000 != 0x109)) 
	{
	  if (currentFunction->NumberOfArguments > 1 ||
	      !currentFunction->ArgCounts[i])
	    {
	      args_ok = 0;
	    }
	}
      /*			
				if ((currentFunction->ArgTypes[i]%100 >= 10)&&
				(currentFunction->ArgTypes[i] != 13)&&
				(currentFunction->ArgTypes[i] != 14)&&
				(currentFunction->ArgTypes[i] != 15)&&
				(currentFunction->ArgTypes[i] != 16)) args_ok = 0;
      */				
    }
  if ((currentFunction->ReturnType%0x10) == 0x8) args_ok = 0;
  if (((currentFunction->ReturnType%0x1000)/0x100 != 0x3)&&
      ((currentFunction->ReturnType%0x1000)/0x100 != 0x1)&&
      ((currentFunction->ReturnType%0x1000)/0x100)) args_ok = 0;
  if (currentFunction->NumberOfArguments && 
      (currentFunction->ArgTypes[0] == 0x5000)
      &&(currentFunction->NumberOfArguments != 0x1)) args_ok = 0;

  /* we can't handle void * return types */
  if ((currentFunction->ReturnType%0x1000) == 0x302) 
    {
      args_ok = 0;
      printf("void* return type!\n");
    }
  
  /* watch out for functions that dont have enough info */
  switch (currentFunction->ReturnType%0x1000)
    {
    case 0x301: case 0x307:
    case 0x304: case 0x305: case 0x306:
    case 0x313: case 0x314: case 0x315: case 0x316:
      args_ok = currentFunction->HaveHint;
      break;
    }

  //  printf("Is Valid %d\n",args_ok);
  /* if (!args_ok) */
  /*   { */
  /*     printf("Signature %s\n",currentFunction->Signature); */
  /*     printf("Return type %x\n",currentFunction->ReturnType); */
  /*   } */

  currentFunction->IsValid = args_ok;
  /* if the args are OK and it is not a constructor or destructor */
  if (args_ok && 
      strcmp(data->ClassName,currentFunction->Name) &&
      strcmp(data->ClassName,currentFunction->Name + 1))
    {
      char funcname[1024];

      if (currentFunction->OverloadCount == 0)
	strcpy(funcname,currentFunction->Name);
      else
	sprintf(funcname,"%s__%d",currentFunction->Name,currentFunction->OverloadCount);

      fprintf(fp,"//@@Signature\n");
      fprintf(fp,"//sfunction @%s:%s %s%sFunction\n",
	      data->ClassName,funcname,
	      data->ClassName,funcname);
      fprintf(fp,"//input ");
      for (i = 0;i < currentFunction->NumberOfArguments+1; i++)
	fprintf(fp,"a%i ",i);
      fprintf(fp,"\n");
      fprintf(fp,"//output ");
      if (isVoidType(currentFunction->ReturnType))
	fprintf(fp,"none\n");
      else
	fprintf(fp,"y\n");
      fprintf(fp,"ArrayVector %s%sFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {\n",
	      data->ClassName,funcname);
      /* process the args */
      fprintf(fp,"  /* Signature %s */\n",currentFunction->Signature); 
      fprintf(fp,"  if (arg.size() < %d) \n",currentFunction->NumberOfArguments+1);
      fprintf(fp,"    throw Exception(\"Function %s for class %s requires %d argument(s)\");\n",
	      funcname,data->ClassName,currentFunction->NumberOfArguments+1);
      fprintf(fp,"  %s* vtk_pointer = GetVTKPointer<%s>(arg[0]);\n",
	      data->ClassName,data->ClassName);
      fprintf(fp,"  Array retval;\n");
      for (i = 0; i < currentFunction->NumberOfArguments; i++)
	{
	  output_temp(fp, i, currentFunction->ArgTypes[i],
		      currentFunction->ArgClasses[i], 
		      currentFunction->ArgCounts[i]);
	}
      output_temp(fp, MAX_ARGS,currentFunction->ReturnType,
		  currentFunction->ReturnClass, 0);
      handle_return_prototype(fp);
    
      /* now get the required args from the stack */
      for (i = 0; i < currentFunction->NumberOfArguments; i++)
	{
	  get_args(fp,i);
	}
    
      switch (currentFunction->ReturnType%0x1000)
	{
	case 0x2:
	  fprintf(fp,"  vtk_pointer->%s(",currentFunction->Name);
	  break;
	case 0x109:
	  fprintf(fp,"  temp%i = &(vtk_pointer)->%s(",MAX_ARGS,currentFunction->Name);
	  break;
	default:
	  fprintf(fp,"  temp%i = (vtk_pointer)->%s(",MAX_ARGS,currentFunction->Name);
	}
      for (i = 0; i < currentFunction->NumberOfArguments; i++)
	{
	  if (i)
	    {
	      fprintf(fp,",");
	    }
	  if (currentFunction->ArgTypes[i] == 0x109)
	    {
	      fprintf(fp,"*(temp%i)",i);
	    }
	  else if (currentFunction->ArgTypes[i] == 0x5000)
	    {
	      /* FIXME				
		 fprintf(fp,"vtkTclVoidFunc,(void *)temp%i",i);
	      */				
	    }
	  else
	    {
	      fprintf(fp,"temp%i",i);
	    }
	}
      fprintf(fp,");\n");
      if (currentFunction->NumberOfArguments && 
	  (currentFunction->ArgTypes[0] == 0x5000))
	{
	  /* FIXME
	     fprintf(fp,"      vtk_pointer->%sArgDelete(vtkTclVoidFuncArgDelete);\n",
	     currentFunction->Name);
	  */
	}
      return_result(fp);
      fprintf(fp,"  return retval;\n");
      fprintf(fp,"}\n\n");
    
      wrappedFunctions[numberOfWrappedFunctions] = currentFunction;
      numberOfWrappedFunctions++;
    }
}

#define max(a,b) (((a) > (b)) ? (a) : (b))

int isOverloadedFunction(int j, FileInfo *data)
{
  int i;
  for (i=0;i<data->NumberOfFunctions;i++)
    if ((i!=j) && 
	(data->Functions[i].Name) &&
	(data->Functions[j].Name) &&
	(strcmp(data->Functions[i].Name,
		data->Functions[j].Name)==0) &&
	(strcmp(data->Functions[i].Name,data->ClassName)))
      return 1;
  return 0;
}

/*
 * Set firstonly to 0 to replace all
 * Set firstonly to 1 to replace once only
 * Set firstonly to 2 to replace even instances only
 */
void StringReplace(char *buffer, const char *src, const char *rep, int firstonly)
{
  char *lbuff = (char*) malloc(strlen(buffer)*2);
  char *dp = buffer;
  char *lp = lbuff;
  int replaced = 0;
  int matches = 0;
  int count = 0;
  while (*dp)
    {
      matches = (strncmp(dp,src,strlen(src)) == 0);
      if (matches) count++;
      if (matches && ((firstonly == 0) ||
		      ((count == 1) && (firstonly == 1)) ||
		      ((count & 1 == 1) && (firstonly == 2))))
	{
	  strcpy(lp,rep);
	  dp += strlen(src);
	  lp += strlen(rep);
	}
      else
	*lp++ = *dp++;
    }
  *lp = 0;
  strcpy(buffer,lbuff);
  free(lbuff);
}

/* print the parsed structures */
void vtkParseOutput(FILE *fp, FileInfo *data)
{
  int i,j;
  int in_example;
  int anymembers;
  char buffer[20000];
  for (i=0;i<data->NumberOfFunctions;i++)
    {
      int overloaded = 0;
      int maxcount = 0;
      for (j=0;j<i;j++)
	{
	  if (data->Functions[i].Name &&
	      data->Functions[j].Name &&
	      strcmp(data->Functions[i].Name,data->ClassName) &&
	      strcmp(data->Functions[i].Name,data->Functions[j].Name)==0)
	    {
	      overloaded = 1;
	      maxcount = max(maxcount,data->Functions[j].OverloadCount);
	    }
	}
      if (isOverloadedFunction(i,data))
	data->Functions[i].OverloadCount = maxcount+1;
      else
	data->Functions[i].OverloadCount = 0;
    }
  
  fprintf(fp,"#include \"VTKWrap.hpp\"\n\n");
  fprintf(fp,"// FreeMat wrapper for %s object\n//\n",data->ClassName);
    
  if (strcmp("vtkObjectBase",data->ClassName) != 0)
    {
      /* Block inclusion of full streams. */
      fprintf(fp,"#define VTK_STREAMS_FWD_ONLY\n");
    }
  /* Needed to avoid problems when putting all files into a single .cc */
  /* In particular this is only needed for Hybrid kit. The class that complains in vtkLineWidget */
  /* fprintf(fp,"#undef None\n"); */
  fprintf(fp,"#include \"%s.h\"\n\n",data->ClassName);

  fprintf(fp,"//@@Signature\n");
  fprintf(fp,"//sfunction @%s:%s %sConstructorFunction\n",data->ClassName,data->ClassName,data->ClassName);
  fprintf(fp,"//input a\n");
  fprintf(fp,"//output p\n");
  if (data->NumberOfSuperClasses > 0)
    {
      fprintf(fp,"//@@Parents %s ",data->ClassName);
      for (i=0;i<data->NumberOfSuperClasses;i++)
	fprintf(fp,"%s ",data->SuperClasses[i]);
      fprintf(fp,"\n");
    }  
  fprintf(fp,"ArrayVector %sConstructorFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {\n",data->ClassName);  
  fprintf(fp,"  if (arg.size() == 0) {\n");
  fprintf(fp,"    return MakeVTKPointer(%s::New(),\"%s\",eval);\n",data->ClassName,data->ClassName);
  fprintf(fp,"  } else if (arg[0].className() == \"%s\") {\n",data->ClassName);
  fprintf(fp,"    return arg[0];\n");
  fprintf(fp,"  } else {\n");
  fprintf(fp,"    vtkObjectBase *p = GetVTKPointer<vtkObjectBase>(arg[0]);\n");
  fprintf(fp,"    if (!p->IsA(\"%s\"))\n",data->ClassName);
  fprintf(fp,"      throw Exception(\"Unable to type convert supplied object to an instance of type %s\");\n",data->ClassName);
  fprintf(fp,"    %s*q = reinterpret_cast<%s*>(p);\n",data->ClassName,data->ClassName);
  fprintf(fp,"    Array ret(arg[0]);\n");
  fprintf(fp,"    ret.structPtr().setClassName(\"%s\");\n",data->ClassName);
  fprintf(fp,"    return ret;\n");
  fprintf(fp,"  }\n");
  fprintf(fp,"}\n");
	  
  /* insert function handling code here */
  for (i = 0; i < data->NumberOfFunctions; i++)
    {
      currentFunction = data->Functions + i;
      if ( currentFunction->Name == NULL ) continue;
      if ( strcmp("New",currentFunction->Name) != 0)
	{
	  outputFunction(fp, data);
	}
    }	

  /* insert overloaded function handling code here */
  for (i = 0; i < data->NumberOfFunctions; i++)
    {
      currentFunction = data->Functions + i;
      if (currentFunction->OverloadCount == 1)
	outputOverloadedFunction(fp, data);
    }

  outputDisplayFunction(fp, data);
  outputDeleteFunction(fp, data);
  //  outputSubsrefFunction(fp, data);
  //  outputSubsasgnFunction(fp, data);


  if (data->Description)
    {
      fprintf(fp,"//!\n");
      fprintf(fp,"//@Module %s\n",data->ClassName);
      fprintf(fp,"//@@Section %s\n",data->SectionName);
      fprintf(fp,"//@@Usage\n");
      strcpy(buffer,data->Description);
      StringReplace(buffer,"\n","\n//",0);
      StringReplace(buffer,"\\code","\\begin{verbatim}",0);
      StringReplace(buffer,"\\endcode","\\end{verbatim}",0);
      fprintf(fp,"//%s",buffer);
      fprintf(fp,"\n");
      fprintf(fp,"//To create an instance of class %s, simply\n//invoke its constructor as follows\n",data->ClassName);
      fprintf(fp,"//@[\n//  obj = %s\n//@]\n",data->ClassName);
#if 0
      fprintf(fp,"//@@Properties\n");
      fprintf(fp,"//Here is the list of properties for the %s class.  These can be accessed\n",data->ClassName);
      fprintf(fp,"//using the normal Get/Set methods (i.e., @| obj.SetFoo(value) | is equivalent to \n");
      fprintf(fp,"// @|obj.Foo = value|, and @| obj.GetFoo() | is equivalent to @| obj.Foo |).\n");
      fprintf(fp,"//\\begin{itemize}\n");
      for (i=0;i<data->NumberOfFunctions;i++)
	{
	  char buffer[18192];
	  char *dp;
	  FunctionInfo *func = data->Functions+i;
	  if (!func->Name) continue;
	  if (!func->IsValid) continue;
	  if (strncmp("Get", func->Name, 3) == 0)
	    {
	      if (!func->Comment) continue;
	      if (strncmp(func->Comment," Set/Get",7) != 0) continue;
	      strcpy(buffer,func->Name + 3);
	      fprintf(fp,"//\\item @|%s|",buffer);
	      strcpy(buffer,func->Signature);
	      dp = buffer;
	      while (strncmp(dp,"Get",3) != 0) dp++;
	      *dp = 0;
	      fprintf(fp," of type @|%s| ",buffer);
	      if (func->Comment)
		{
		  strcpy(buffer,func->Comment + 8);
		  StringReplace(buffer,"\n","\n//",0);
		  StringReplace(buffer,"\\code","\\begin{verbatim}",0);
		  StringReplace(buffer,"\\endcode","\\end{verbatim}",0);
		  fprintf(fp," - %s",buffer);
		}
	      else
		{
		  fprintf(fp,"\n");
		}
	    }
	}
      fprintf(fp,"//\\end{itemize}\n");
#endif
      anymembers = 0;
      for (i=0;i<data->NumberOfFunctions;i++)
	{
	  FunctionInfo *func = data->Functions+i;
	  if (!func->Name) continue;
	  if (!func->IsValid) continue;
	  if (strcmp("New",func->Name) == 0) continue;
	  if (strcmp("Print",func->Name) == 0) continue;
	  if (strncmp(func->Signature,"static ",strlen("static ")) == 0) continue;
	  anymembers = 1;
	  break;	  
	}
      if (anymembers)
	{
	  fprintf(fp,"//@@Methods\n");
	  fprintf(fp,"//The class %s has several methods that can be used.\n//  They are listed below.\n",data->ClassName);
	  fprintf(fp,"//Note that the documentation is translated automatically from the VTK sources,\n");
	  fprintf(fp,"//and may not be completely intelligible.  When in doubt, consult the VTK website.\n");
	  fprintf(fp,"//In the methods listed below, @|obj| is an instance of the %s class.\n",data->ClassName);
	  fprintf(fp,"//\\begin{itemize}\n");
	  for (i=0;i<data->NumberOfFunctions;i++)
	    {
	      char buffer[4096];
	      char *dp;
	      FunctionInfo *func = data->Functions+i;
	      if (!func->Name) continue;
	      if (!func->IsValid) continue;
	      if (strcmp("New",func->Name) == 0) continue;
	      if (strcmp("Print",func->Name) == 0) continue;
	      /* Copy the signature into a buffer */
	      strcpy(buffer,func->Signature);
	      StringReplace(buffer,"virtual ","",1);
	      StringReplace(buffer," = 0","",1);
	      StringReplace(buffer,"const char *","string ",0);
	      StringReplace(buffer,"char *","string ",0);
	      StringReplace(buffer,"*","",0);
	      StringReplace(buffer,"unsigned ","",0);
	      if (strncmp(buffer,"static ",strlen("static ")) == 0) continue;
	      if (strncmp(buffer,"void ",strlen("void ")) == 0)
		StringReplace(buffer,"void ","obj.",1);
	      else
		StringReplace(buffer," "," = obj.",1);
	      StringReplace(buffer,"const ","",0);
	      StringReplace(buffer,"\n","\n//",0);
	      for (dp = buffer; *dp != 0; dp++) if (*dp == ';') *dp = 0;
	      fprintf(fp,"// \\item @|%s|",buffer);
	      if (!func->Comment) 
		{
		  fprintf(fp,"\n");
	      continue;
		}
	      strcpy(buffer,func->Comment);
	      StringReplace(buffer,"\n","\n//",0);
	      StringReplace(buffer,"\\note","Note:",0);
	      StringReplace(buffer,"\\code","\\begin{verbatim}",0);
	      StringReplace(buffer,"\\endcode","\\end{verbatim}",0);
	      fprintf(fp," - %s",buffer);
	    }
	  fprintf(fp,"//\\end{itemize}\n");
	}
      fprintf(fp,"//!\n");
    }
}

/*
  Change log: 
  13. 01. 2004. Added help for individual methods. 
  Help can be displayed as vtkClass("vtkClassMethod","Help");
  Added narrowing (convert list to scalar) of the return 
  parameter when examining superclasses. 
*/	

