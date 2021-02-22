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

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "vtkParse.h"

int numberOfWrappedFunctions = 0;
FunctionInfo *wrappedFunctions[1000];
extern FunctionInfo *currentFunction;

char* typeNames[17] = { "unknown", "float", "void", "char", "int", "short", "long",
												"double", "unknown", "unknown", "unknown", "unknown", "unknown",
												"unsigned char", "unsigned int", "unsigned short", "unsigned long" };

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
  if (aType == 5000)
    {
    fprintf(fp,"    vtkTclVoidFuncArg *temp%i = new vtkTclVoidFuncArg;\n",i);
    return;
    }
  
  /* ignore void */
  if (((aType % 10) == 2)&&(!((aType%1000)/100)))
    {
    return;
    }

  /* for const * return types prototype with const */
  if ((i == MAX_ARGS) && (aType%2000 >= 1000))
    {
    fprintf(fp,"    const ");
    }
  else
    {
    fprintf(fp,"    ");
    }

  if ((aType%100)/10 == 1)
    {
    fprintf(fp,"unsigned ");
    }

  switch (aType%10)
    {
		/* FIXME 7 should be double but then vtkActor2D wont compile! */
		/* 7 should be double! */
    case 1:   fprintf(fp,"float  "); break;
    case 7:   fprintf(fp,"double "); break;
    case 4:   fprintf(fp,"int    "); break;
    case 5:   fprintf(fp,"short  "); break;
    case 6:   fprintf(fp,"long   "); break;
    case 2:     fprintf(fp,"void   "); break;
    case 3:     fprintf(fp,"char   "); break;
    case 9:     fprintf(fp,"%s ",Id); break;
    case 8: return;
    }

  /* handle array arguements */
  if (count > 1)
    {
    fprintf(fp,"temp%i[%i];\n",i,count);
    return;
    }
  
  switch ((aType%1000)/100)
    {
    case 1: fprintf(fp, " *"); break; /* act " &" */
    case 2: fprintf(fp, "&&"); break;
    case 3: fprintf(fp, " *"); break;
    case 4: fprintf(fp, "&*"); break;
    case 5: fprintf(fp, "*&"); break;
    case 7: fprintf(fp, "**"); break;
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
  switch (currentFunction->ReturnType%1000)
    {
    case 301: case 307:  
		   /* float array */
			fprintf(fp,"      ColumnVector tempResult(%i);\n",currentFunction->HintSize);
      for (i = 0; i < currentFunction->HintSize; i++)
        {
        fprintf(fp,"      tempResult(%i) = (double)temp%i[%i];\n",i,MAX_ARGS,i);
        }
      fprintf(fp,"      retval = tempResult;\n");
      break;
    case 304: case 305: case 306: 
		   /* int array */
			fprintf(fp,"      ColumnVector tempResult(%i);\n",currentFunction->HintSize);
      for (i = 0; i < currentFunction->HintSize; i++)
        {
        fprintf(fp,"      tempResult(%i) = (double)temp%i[%i];\n",i,MAX_ARGS,i);
        }
      fprintf(fp,"      retval = tempResult;\n");
      break;
    case 313: case 314: case 315: case 316:
			fprintf(fp,"      ColumnVector tempResult(%i);\n",currentFunction->HintSize);
      for (i = 0; i < currentFunction->HintSize; i++)
        {
        fprintf(fp,"      tempResult(%i) = (double)temp%i[%i];\n",i,MAX_ARGS,i);
        }
      fprintf(fp,"      retval = tempResult;\n");
      break;
    }
}

void return_result(FILE *fp)
{
  switch (currentFunction->ReturnType%1000)
    {
    case 2:
		  /* void, do nothing */
      break;
    case 1: case 7: 
      fprintf(fp,"      retval = temp%i;\n",MAX_ARGS); 
      break;
    case 4:  
		  /* int */
      fprintf(fp,"      retval = temp%i;\n", MAX_ARGS); 
      break;
    case 5:
		  /* short */
      fprintf(fp,"      retval = temp%i;\n", MAX_ARGS); 
      break;
    case 6:
		  /* long */
      fprintf(fp,"      retval = temp%i;\n", MAX_ARGS); 
      break;
    case 14:
		  /* unsigned int */
      fprintf(fp,"      retval = temp%i;\n",MAX_ARGS);
      break;
    case 15: 
		  /* unsigned short */
      fprintf(fp,"      retval = temp%i;\n",MAX_ARGS);
	      break;
    case 16:  
		  /* unsigned long */
      fprintf(fp,"      retval = temp%i;\n",MAX_ARGS);
      break;
    case 13:
		  /* unsigned char */
      fprintf(fp,"      retval = temp%i;\n",MAX_ARGS);
      break;
    case 303:
		  /* string. i.e. char* */ 
      fprintf(fp,"      if (temp%i)\n        {\n        retval = (char*)temp%i;\n",MAX_ARGS,MAX_ARGS); 
      fprintf(fp,"        }\n");
      break;
    case 3:
		  /* char */ 
      fprintf(fp,"      retval = temp%i;\n",MAX_ARGS);
      break;
    case 109:
    case 309:  
/* FIXME
			fprintf(fp,"      309vtkTclGetObjectFromPointer(interp,(void *)temp%i,%sCommand);\n",MAX_ARGS,currentFunction->ReturnClass);
*/
      fprintf(fp,"      vtkObjectBase *tmp_ptr = (vtkObjectBase*)(temp%i);\n",MAX_ARGS);
      fprintf(fp,"      retval = octave_value( new vtk_object(tmp_ptr) );\n");
      break;

    /* handle functions returning vectors */
    /* this is done by looking them up in a hint file */
    case 301: case 307:
    case 304: case 305: case 306:
    case 313: case 314: case 315: case 316:      
      use_hints(fp);
      break;
    default:
      fprintf(fp,"    Tcl_SetResult(interp, (char *) \"unable to return result.\", TCL_VOLATILE);\n");
      break;
    }
}

void handle_return_prototype(FILE *fp)
{
  switch (currentFunction->ReturnType%1000)
    {
    case 109:
    case 309:  
      /* FIXME
			fprintf(fp,"    int %sCommand(ClientData, Tcl_Interp *, int, char *[]);\n",currentFunction->ReturnClass);
			*/
      break;
    }
}

void get_args(FILE *fp, int i)
{
  int j;
  int start_arg = 2;
  
  /* what arg do we start with */
  for (j = 0; j < i; j++)
    {
    start_arg = start_arg + 
      (currentFunction->ArgCounts[j] ? currentFunction->ArgCounts[j] : 1);
    }
  
  /* handle VAR FUNCTIONS */
  if (currentFunction->ArgTypes[i] == 5000)
    {
    fprintf(fp,"    temp%i->interp = interp;\n",i);
    fprintf(fp,"    temp%i->command = strcpy(new char [strlen(argv[2])+1],argv[2]);\n",i);
    return;
    }

  /* ignore void */
  if (((currentFunction->ArgTypes[i] % 10) == 2)&&
      (!((currentFunction->ArgTypes[i]%1000)/100)))
    {
    return;
    }
  
  switch (currentFunction->ArgTypes[i]%1000)
    {
    case 1: case 7:  
			/* floating point, i.e. double */
      fprintf(fp,"    if ( !args(%i).is_real_type() ) error_flag = 1;\n", start_arg); 
      fprintf(fp,"    else temp%i = args(%i).double_value();\n",i, start_arg);
      break;
    case 4: case 5: case 6:
		  /* int */ 
      fprintf(fp,"    if ( !args(%i).is_real_type() ) error_flag = 1;\n", start_arg); 
      fprintf(fp,"    else temp%i = args(%i).int_value();\n",i, start_arg);
      break;
    case 3:
		  /* char */
      fprintf(fp,"    if ( !args(%i).is_real_type() ) error_flag = 1;\n", start_arg); 
      fprintf(fp,"    else temp%i = (char)args(%i).int_value();\n",i, start_arg);
      break;
    case 13:
		  /* unsigned char */
      fprintf(fp,"    if ( !args(%i).is_real_type() ) error_flag = 1;\n", start_arg); 
      fprintf(fp,"    else temp%i = (unsigned char)args(%i).int_value();\n",i, start_arg);
      break;
    case 14:
		  /* unsigned int */
      fprintf(fp,"    if ( !args(%i).is_real_type() ) error_flag = 1;\n", start_arg); 
      fprintf(fp,"    else temp%i = args(%i).uint_value();\n",i, start_arg);
      break;
    case 15:
		  /* unsigned short */
      fprintf(fp,"    if ( !args(%i).is_real_type() ) error_flag = 1;\n", start_arg); 
      fprintf(fp,"    else temp%i = args(%i).ushort_value();\n",i, start_arg);
      break;
    case 16:
		  /* unsigned long */
      fprintf(fp,"    if ( !args(%i).is_real_type() ) error_flag = 1;\n", start_arg); 
      fprintf(fp,"    else temp%i = args(%i).ulong_value();\n",i, start_arg);
      break;
    case 303:
		  /* char* */
      fprintf(fp,"    char tmp_string%i[1024];\n",i);
			fprintf(fp,"    strcpy(tmp_string%i,args(%i).string_value().c_str()); \n",i,start_arg);
      fprintf(fp,"    temp%i = tmp_string%i;\n",i,i);
      /* FIXME
			fprintf(fp,"    temp%i = args(%i).string_value().c_str();\n",i,start_arg);
			*/
      break;
			 /* FLOAT, int, short, long, double FIX */
    case 301: case 304: case 305: case 306: case 307: case 314: case 313: case 315: case 316:
	    /* Check if the args(start_arg) is a vector */
			if ( currentFunction->ArgCounts[i] <= 1 )
				{
				fprintf(fp,"    if ( (args(%i).rows() !=1) && (args(%i).columns() !=1) ) error_flag = 1;\n",start_arg,start_arg);
				/* now allocate the array and copy octave vector */
				fprintf(fp,"    else \n      {\n");
				fprintf(fp,"      Array<double> vect = args(%i).vector_value();\n",start_arg);
				fprintf(fp,"      int length = args(%i).length();\n",start_arg);
				fprintf(fp,"      temp%i = new %s[length];\n", i, typeNames[currentFunction->ArgTypes[i]%100]);
				fprintf(fp,"      for ( int k = 0; k < length; k++ ) temp%i[k] = (%s)vect(k);\n      }\n",i,typeNames[currentFunction->ArgTypes[i]%100]);
				} else
				{
				fprintf(fp,"    if ( ( args(%i).rows()*args(%i).columns() != %i ) ) error_flag = 1;\n",start_arg,start_arg,currentFunction->ArgCounts[i]);
				/* now allocate the array and copy octave vector */
				fprintf(fp,"    else \n      {\n");
				fprintf(fp,"      Array<double> vect = args(%i).vector_value();\n",start_arg);
				fprintf(fp,"      int length = args(%i).length();\n",start_arg);
				fprintf(fp,"      if ( length != %d ) error_flag = 1;\n",currentFunction->ArgCounts[i]);
				fprintf(fp,"      else for ( int k = 0; k < length; k++ ) temp%i[k] = (%s)vect(k);\n      }\n",i,typeNames[currentFunction->ArgTypes[i]%100]);
				}
			break;
    case 109:
    case 309:
	    fprintf(fp,"    if ( args(%i).type_id() != vtk_object::static_type_id() ) error_flag = 1;\n",start_arg);
# if __WORDSIZE == 64
	  fprintf(fp,"    else temp%i = reinterpret_cast<%s*>( args(%i).uint64_scalar_value().value() );\n",i,currentFunction->ArgClasses[i],start_arg);
# else
	  fprintf(fp,"    else temp%i = reinterpret_cast<%s*>( args(%i).uint32_scalar_value().value() );\n",i,currentFunction->ArgClasses[i],start_arg);
# endif
			/* FIXME
			fprintf(fp,"      retval = octave_value( new vtk_object(temp%i) );\n",MAX_ARGS);
      fprintf(fp,"    309temp%i = (%s *)(vtkTclGetPointerFromObject(argv[%i],(char *) \"%s\",interp,error_flag));\n",i,currentFunction->ArgClasses[i],start_arg,
              currentFunction->ArgClasses[i]);
			*/
      break;
    case 2:    
    case 9:
      break;
    default:
      if (currentFunction->ArgCounts[i] > 1)
        {
        for (j = 0; j < currentFunction->ArgCounts[i]; j++)
          {
          switch (currentFunction->ArgTypes[i]%100)
            {
            case 1: case 7:  
							/* floating point, i.e. double */
				      fprintf(fp,"    if ( !args(%i).is_real_type() ) error_flag = 1;\n", start_arg); 
              fprintf(fp,"    else temp%i[%i] = args(%i).double_value();\n",i,j,start_arg);
              break;
            case 4: case 5: case 6: 
							/* int */
				      fprintf(fp,"    if ( !args(%i).is_real_type() ) error_flag = 1;\n", start_arg); 
              fprintf(fp,"    else temp%i[%i] = args(%i).int_value();\n",i,j,start_arg);
              break;
            case 3:
							/* char */
				      fprintf(fp,"    if ( !args(%i).is_real_type() ) error_flag = 1;\n", start_arg); 
              fprintf(fp,"    else temp%i[%i] = (char)args(%i).int_value();\n",i,j,start_arg);
              break;
            case 13:
							/* unsigned char */
				      fprintf(fp,"    if ( !args(%i).is_real_type() ) error_flag = 1;\n", start_arg); 
              fprintf(fp,"    else temp%i[%i] = (unsigned char)args(%i).int_value();\n",i,j,start_arg);
              break;
            case 14:
							/* unsigned int */
				      fprintf(fp,"    if ( !args(%i).is_real_type() ) error_flag = 1;\n", start_arg); 
              fprintf(fp,"    else temp%i[%i] = args(%i).uint_value();\n",i,j,start_arg);
              break;
            case 15:
							/* unsigned short */
				      fprintf(fp,"    if ( !args(%i).is_real_type() ) error_flag = 1;\n", start_arg); 
              fprintf(fp,"    else temp%i[%i] = args(%i).ushort_value();\n",i,j,start_arg);
              break;
            case 16:
							/* unsigned long */
				      fprintf(fp,"    if ( !args(%i).is_real_type() ) error_flag = 1;\n", start_arg); 
              fprintf(fp,"    else temp%i[%i] = args(%i).ulong_value();\n",i,j,start_arg);
              break;
            }
          start_arg++;
          }
        }
      
    }
}

void outputFunction(FILE *fp, FileInfo *data)
{
  int i;
  int args_ok = 1;
 
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
		if (currentFunction->ArgTypes[i] == 5000) return;
		if ((currentFunction->ArgTypes[i]%10) == 8) args_ok = 0;
    /* if its a pointer arg make sure we have the ArgCount */
    if ((currentFunction->ArgTypes[i]%1000 >= 100) &&
        (currentFunction->ArgTypes[i]%1000 != 303)&&
        (currentFunction->ArgTypes[i]%1000 != 301)&& /* FLOAT* FIX */
        (currentFunction->ArgTypes[i]%1000 != 303)&& /* char* FIX */
        (currentFunction->ArgTypes[i]%1000 != 304)&& /* int* FIX */
        (currentFunction->ArgTypes[i]%1000 != 305)&& /* short* FIX */
        (currentFunction->ArgTypes[i]%1000 != 306)&& /* long* FIX */
        (currentFunction->ArgTypes[i]%1000 != 307)&& /* double* FIX */
        (currentFunction->ArgTypes[i]%1000 != 313)&& /* unsigned char* FIX */
        (currentFunction->ArgTypes[i]%1000 != 314)&& /* unsigned int* FIX */
        (currentFunction->ArgTypes[i]%1000 != 315)&& /* unsigned short* FIX */
        (currentFunction->ArgTypes[i]%1000 != 316)&& /* unsigned long* FIX */
        (currentFunction->ArgTypes[i]%1000 != 309)&&
        (currentFunction->ArgTypes[i]%1000 != 109)) 
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
  if ((currentFunction->ReturnType%10) == 8) args_ok = 0;
  if (((currentFunction->ReturnType%1000)/100 != 3)&&
      ((currentFunction->ReturnType%1000)/100 != 1)&&
      ((currentFunction->ReturnType%1000)/100)) args_ok = 0;
  if (currentFunction->NumberOfArguments && 
      (currentFunction->ArgTypes[0] == 5000)
      &&(currentFunction->NumberOfArguments != 1)) args_ok = 0;

  /* we can't handle void * return types */
  if ((currentFunction->ReturnType%1000) == 302) 
    {
    args_ok = 0;
    }
  
  /* watch out for functions that dont have enough info */
  switch (currentFunction->ReturnType%1000)
    {
    case 301: case 307:
    case 304: case 305: case 306:
    case 313: case 314: case 315: case 316:
      args_ok = currentFunction->HaveHint;
      break;
    }
  /* if the args are OK and it is not a constructor or destructor */
  if (args_ok && 
      strcmp(data->ClassName,currentFunction->Name) &&
      strcmp(data->ClassName,currentFunction->Name + 1))
    {
    int required_args = 0;
    
    /* calc the total required args */
    for (i = 0; i < currentFunction->NumberOfArguments; i++)
      {
      required_args = required_args + 1;
//        (currentFunction->ArgCounts[i] ? currentFunction->ArgCounts[i] : 1);
      }

    fprintf(fp,"  if ( ( method_name == \"%s\" ) && ( nargin == %i ) )\n    {\n",
            currentFunction->Name, required_args + 2);
    /* process the args */
    fprintf(fp,"\n/* ah %s */\n",currentFunction->Signature); 
    for (i = 0; i < currentFunction->NumberOfArguments; i++)
      {
      output_temp(fp, i, currentFunction->ArgTypes[i],
                  currentFunction->ArgClasses[i], 
                  currentFunction->ArgCounts[i]);
      }
    output_temp(fp, MAX_ARGS,currentFunction->ReturnType,
                currentFunction->ReturnClass, 0);
    handle_return_prototype(fp);
    fprintf(fp,"    error_flag = 0;\n\n");
    
    /* now get the required args from the stack */
    for (i = 0; i < currentFunction->NumberOfArguments; i++)
      {
      get_args(fp,i);
      }
    
    fprintf(fp,"    if (!error_flag)\n      {\n");
    switch (currentFunction->ReturnType%1000)
      {
      case 2:
        fprintf(fp,"      vtk_pointer->%s(",currentFunction->Name);
        break;
      case 109:
        fprintf(fp,"      temp%i = &(vtk_pointer)->%s(",MAX_ARGS,currentFunction->Name);
        break;
      default:
        fprintf(fp,"      temp%i = (vtk_pointer)->%s(",MAX_ARGS,currentFunction->Name);
      }
    for (i = 0; i < currentFunction->NumberOfArguments; i++)
      {
      if (i)
        {
        fprintf(fp,",");
        }
      if (currentFunction->ArgTypes[i] == 109)
        {
        fprintf(fp,"*(temp%i)",i);
        }
      else if (currentFunction->ArgTypes[i] == 5000)
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
        (currentFunction->ArgTypes[0] == 5000))
      {
			/* FIXME
      fprintf(fp,"      vtk_pointer->%sArgDelete(vtkTclVoidFuncArgDelete);\n",
              currentFunction->Name);
			*/
      }
    return_result(fp);
    fprintf(fp,"      return retval;\n      }\n");
    fprintf(fp,"    }\n");
    
    wrappedFunctions[numberOfWrappedFunctions] = currentFunction;
    numberOfWrappedFunctions++;
    }
}

/* print the parsed structures */
void vtkParseOutput(FILE *fp, FileInfo *data)
{
  int i,j;
  int in_example;
  
  fprintf(fp,"// Octave wrapper for %s object\n//\n",data->ClassName);
    
#ifdef __CYGWIN__
  if (strcmp("vtkObjectBase",data->ClassName) != 0)
    {
      /* Block inclusion of full streams. */
    fprintf(fp,"#define VTK_STREAMS_FWD_ONLY\n");
    }
  /* Needed to avoid problems when putting all files into a single .cc */
  /* In particular this is only needed for Hybrid kit. The class that complains in vtkLineWidget */
  fprintf(fp,"#undef None\n"); 
  fprintf(fp,"#include \"%s.h\"\n\n",data->ClassName);
  /* Needed to avoid problems when compiling for cygwin */
  fprintf(fp,"#undef Complex\n");
  fprintf(fp,"#include <octave/oct.h>\n");
  fprintf(fp,"#include <octave/parse.h>\n");
  fprintf(fp,"#include <string>\n");
  fprintf(fp,"#include <map>\n");
  fprintf(fp,"#include \"octaviz.h\"\n\n");
#else    
  fprintf(fp,"#include \"OctavizCommon.h\"\n");

  if (strcmp("vtkObjectBase",data->ClassName) != 0)
    {
      /* Block inclusion of full streams. */
    fprintf(fp,"#define VTK_STREAMS_FWD_ONLY\n");
    }
  /* Needed to avoid problems when putting all files into a single .cc */
  /* In particular this is only needed for Hybrid kit. The class that complains in vtkLineWidget */
  /* fprintf(fp,"#undef None\n"); */
  fprintf(fp,"#include \"%s.h\"\n\n",data->ClassName);
  /* Needed to avoid problems when compiling for cygwin */
	/*
	fprintf(fp,"#undef Complex\n");
  fprintf(fp,"#include <octave/oct.h>\n");
  fprintf(fp,"#include <octave/parse.h>\n");
  fprintf(fp,"#include <string>\n");
  fprintf(fp,"#include <map>\n");
  fprintf(fp,"#include \"octaviz.h\"\n\n");
	*/
#endif    
	fprintf(fp,"void populate_help_map%s( std::map<std::string,std::string> &help_map );\n\n",data->ClassName);
	
	/* Make the function that is callable from octave */
  /* Add class description into the function. */
  /* This will displayed as help in octave */
  fprintf(fp,"\nDEFUN_DLD ( %s, args, nargout, \n", data->ClassName);
  fprintf(fp,"  \"-*- texinfo -*-\\n\\\n");
  fprintf(fp,"@deftypefn {Built-in Function} {} %s\\n\\\n\\n\\\n", data->ClassName);
  i = 0;
  in_example = 0;
	if ( data->Description != NULL )
  while ( data->Description[i] != 0 )
    {
    if ( data->Description[i] == '"' ) fprintf(fp,"\\");
       
    /* Curly braces are special characters in Texinfo.  Quote them. */
    if ( data->Description[i] == '{' || data->Description[i] == '}' )
	 fprintf(fp,"@");       

    /* Replace `\code [...] \endcode' constructs by the texinfo equivalent
       @example [..] @end example' */   
    if (strncmp (data->Description + i, "\\code", 5) == 0) {
      fprintf(fp,"@example");
      in_example = 1;
      i += 5;
    }
    if (strncmp (data->Description + i, "\\endcode", 8) == 0) {
      fprintf(fp,"@end example");
      in_example = 0;
      i += 8;
    }
    /* Skip emphasis markup.  This could be replaced by @emph, 
       but it is trick because @emph needs a bracketed argument */
    if (strncmp (data->Description + i, "\\em", 3) == 0) {
      i += 3;
    }
    /* Unscape \sum macro */
    if (strncmp (data->Description + i, "\\sum", 4) == 0) {
      i += 1;
    }
    
    /* Unscape \pre macro -- this will probably look like shit in the final .oct files */
    if (strncmp (data->Description + i, "\\pre", 4) == 0) {
      i += 1;
    }
    
    /* Unscape \post macro -- this will probably look like shit in the final .oct files */
    if (strncmp (data->Description + i, "\\post", 5) == 0) {
      i += 1;
    }

    if ( data->Description[i] == '\n' )
		  {
			/* Don't want the last newline */
			if ( i + 2 < strlen(data->Description) )
			  {
				fprintf(fp,"\\n\\\n");
				/* Eat spaces at beginning of lines */
				if (in_example != 1) 
					while (data->Description[i + 1] == ' ')
						i++;
			  }
			i++;
		  } else
		  {
			fprintf(fp,"%c",data->Description[i++]);
		  }
    }
	fprintf(fp,"\\n\\\n@end deftypefn\")\n{\n" );
	fprintf(fp,"  octave_value retval;\n" );

  /* This associative map holds help for each method (if avaliable) */
	fprintf(fp,"  static std::map<std::string,std::string> help_map;\n");
	fprintf(fp,"  static bool help_populated = false;\n");
	fprintf(fp,"  if ( !help_populated ) \n    {\n");
	fprintf(fp,"    populate_help_map%s( help_map );\n    help_populated = true;\n    }\n\n",data->ClassName);
	
  fprintf(fp,"  int nargin  = args.length ();\n\n" );
  fprintf(fp,"  if ( nargin < 1 )\n    {\n");
  fprintf(fp,"    vtkObjectBase *new_vtk_object = %s::New();\n",data->ClassName,data->ClassName);
  fprintf(fp,"    retval = octave_value( new vtk_object( new_vtk_object, true ) );\n");
  fprintf(fp,"    return retval;\n    }\n\n" );

  fprintf(fp,"  if ( nargin == 1 )\n    {\n    // This can only be the \"New\" command\n");
  fprintf(fp,"    if ( !args(0).is_string() )\n      {\n");
  fprintf(fp,"      error(\"If a single parameter is passed, then it has to be \\\"New\\\" or \\\"List\\\".\");\n");
  fprintf(fp,"      return retval;\n");
  fprintf(fp,"      }\n    if ( args(0).string_value() == \"New\" )\n      {\n");
  fprintf(fp,"      vtkObjectBase *new_vtk_object = %s::New();\n",data->ClassName,data->ClassName);
  fprintf(fp,"      retval = octave_value( new vtk_object( new_vtk_object, true ) );\n");
  fprintf(fp,"      return retval;\n");
  fprintf(fp,"      }\n    if ( args(0).string_value() == \"List\" )\n      {\n");
  fprintf(fp,"      octave_stdout << help_map[\"List\"] << std::endl;\n");
  fprintf(fp,"      return retval;\n      }\n");
  fprintf(fp,"    error(\"If a single parameter is passed, then it has to be \\\"New\\\" or \\\"List\\\".\");\n");
  fprintf(fp,"    return retval;\n");
  fprintf(fp,"    }\n");

  fprintf(fp,"  // The second parameter has to be a string command\n");
  fprintf(fp,"  if ( !args(1).is_string() )\n    {\n");
  fprintf(fp,"    error(\"Second parameter has to be a string command.\");\n");
  fprintf(fp,"    return retval;\n	  }\n\n");

  fprintf(fp,"\n  // If it is not the command New, the first parameter has to be a vtk_object" );
  fprintf(fp,"\n  // or a method name followed by a string Help\n" );
  fprintf(fp,"  if ( ( args(0).type_id() != vtk_object::static_type_id() ) &&\n");
  fprintf(fp,"       ( args(1).string_value() != \"Help\" ) )\n    {\n");
  fprintf(fp,"    error(\"First parameter has to be a vtk_object or the command \\\"New\\\".");
  fprintf(fp," If the first parameter is a method name, then the second has to be Help\");\n");
  fprintf(fp,"    return retval;\n    }\n\n");

  fprintf(fp,"    if ( args(1).string_value() == \"Help\" )\n    {\n");
  fprintf(fp,"    octave_stdout << help_map[args(0).string_value()] << std::endl;\n");
  fprintf(fp,"    return retval;\n    }\n\n");
					
# if __WORDSIZE == 64
  fprintf(fp,"  %s *vtk_pointer = reinterpret_cast<%s*>( args(0).uint64_scalar_value().value() );\n",data->ClassName,data->ClassName);
# else
  fprintf(fp,"  %s *vtk_pointer = reinterpret_cast<%s*>( args(0).uint32_scalar_value().value() );\n",data->ClassName,data->ClassName);
# endif
  fprintf(fp,"  std::string method_name = args(1).string_value();\n");
  fprintf(fp,"  int    error_flag;\n");
  fprintf(fp,"  error_flag = 0; error_flag = error_flag;\n\n");
	
	/* insert function handling code here */
	for (i = 0; i < data->NumberOfFunctions; i++)
	{
		currentFunction = data->Functions + i;
		if ( currentFunction->Name == NULL ) continue;
		if ( strcmp("New",currentFunction->Name) )
			{
			outputFunction(fp, data);
			}
	}	
	
  if (!strcmp("vtkObject",data->ClassName))
    {
    /* Add the AddObserver method to vtkObject. */

    fprintf(fp,"  if ( ( method_name == \"AddObserver\" ) && ( nargin == 4 ) )\n");
    fprintf(fp,"    {\n");
    fprintf(fp,"    error_flag = 0;\n");
    fprintf(fp,"		vtkOctaveCommand* comm = vtkOctaveCommand::New();\n");
    fprintf(fp,"		comm->SetFunctionName(args(3).string_value().c_str());\n");
# if __WORDSIZE == 64
    fprintf(fp,"		vtk_object *vtk_obj = reinterpret_cast<vtk_object*>( args(0).uint64_scalar_value().value() );\n");
# else
    fprintf(fp,"		vtk_object *vtk_obj = reinterpret_cast<vtk_object*>( args(0).uint32_scalar_value().value() );\n");
# endif
    fprintf(fp,"		comm->SetObject( vtk_obj );\n");
    fprintf(fp,"		vtk_pointer->AddObserver(args(2).string_value().c_str(),comm);\n");
    fprintf(fp,"		comm->Delete();\n");
		fprintf(fp,"    return retval;\n");
    fprintf(fp,"    }\n");
	}
				
	
  if (!strcmp("vtkCellLocator",data->ClassName))
    {
    /* Add the IntersectWithLine method to vtkCellLocator. */
	/* IntersectWithLine( double a0[3], double a1[3], double tol, double& t, double x[3], double pcoords[3], int & subId ); */
    fprintf(fp,"	if ( ( method_name == \"IntersectWithLine\" ) && ( nargin == 5 ) )\n");
    fprintf(fp,"    {\n");
    fprintf(fp,"    double  a0[3];\n");
    fprintf(fp,"    double  a1[3];\n");
    fprintf(fp,"		double  tol;\n");
    fprintf(fp,"		double  t;\n");
    fprintf(fp,"		double	x[3];\n");
    fprintf(fp,"		double  pcoords[3];\n");
    fprintf(fp,"		int 		subId;\n");
    fprintf(fp,"    error_flag = 0;\n\n");

    fprintf(fp,"    if ( ( args(2).rows()*args(2).columns() != 3 ) ) error_flag = 1;\n");
    fprintf(fp,"    else \n");
    fprintf(fp,"      {\n");
    fprintf(fp,"      Array<double> vect = args(2).vector_value();\n");
    fprintf(fp,"      int length = args(2).length();\n");
    fprintf(fp,"      if ( length != 3 ) error_flag = 1;\n");
    fprintf(fp,"      else for ( int k = 0; k < length; k++ ) a0[k] = (double)vect(k);\n");
    fprintf(fp,"      }\n");
    fprintf(fp,"    if ( ( args(3).rows()*args(3).columns() != 3 ) ) error_flag = 1;\n");
    fprintf(fp,"    else \n");
    fprintf(fp,"      {\n");
    fprintf(fp,"      Array<double> vect = args(3).vector_value();\n");
    fprintf(fp,"      int length = args(3).length();\n");
    fprintf(fp,"      if ( length != 3 ) error_flag = 1;\n");
    fprintf(fp,"      else for ( int k = 0; k < length; k++ ) a1[k] = (double)vect(k);\n");
    fprintf(fp,"      }\n");
    fprintf(fp,"    if ( ( args(4).rows()*args(4).columns() != 1 ) ) error_flag = 1;\n");
    fprintf(fp,"    else \n");
    fprintf(fp,"      {\n");
    fprintf(fp,"      Array<double> vect = args(4).vector_value();\n");
    fprintf(fp,"			t = (float)vect(0);\n");
    fprintf(fp,"      }\n");
    fprintf(fp,"    if (!error_flag)\n");
    fprintf(fp,"      {\n");
    fprintf(fp,"      int res = vtk_pointer->IntersectWithLine( a0, a1, tol, t, x, pcoords, subId );\n");
    fprintf(fp,"		  octave_value_list retval_list;\n");
    fprintf(fp,"			retval_list(0) = (double)res;\n");
    fprintf(fp,"			retval_list(1) = t;\n");
    fprintf(fp,"      ColumnVector tempv1(3);\n");
    fprintf(fp,"      tempv1(0) = x[0];\n");
    fprintf(fp,"      tempv1(1) = x[1];\n");
    fprintf(fp,"      tempv1(2) = x[2];\n");
    fprintf(fp,"			retval_list(2) = tempv1;\n");
    fprintf(fp,"      ColumnVector tempv2(3);\n");
    fprintf(fp,"      tempv2(0) = pcoords[0];\n");
    fprintf(fp,"      tempv2(1) = pcoords[1];\n");
    fprintf(fp,"      tempv2(2) = pcoords[2];\n");
    fprintf(fp,"			retval_list(3) = tempv2;\n");
    fprintf(fp,"			retval_list(4) = (double)subId;\n");
    fprintf(fp,"      return retval_list;\n");
    fprintf(fp,"      }\n");
    fprintf(fp,"    }\n");
	}
		
  fprintf(fp,"\n  if ( error_flag )\n    {\n");
  fprintf(fp,"    error(\"Method was found but arguments were wrong.\");\n");
  fprintf(fp,"    return retval;\n    }\n");

	/* If get here in vtkObjectBase class that means that the method was not found */
  if (strcmp("vtkObjectBase",data->ClassName) == 0)
    {
		  fprintf(fp,"  error( \"Requested method could not be found.\" );\n");
			fprintf(fp,"  return retval;\n");
			fprintf(fp,"}\n\n" );
			outputFunctionHelp(fp,data);
			fprintf(fp,"\n\n" );
			return;
    } 
	
	fprintf(fp,"\n  // Check superclass methods.\n");
	
	for (i = 0; i < data->NumberOfSuperClasses; i++)
	{
		fprintf(fp,"  retval = feval(\"%s\",args, nargout); \n", data->SuperClasses[i]);
	} 

	fprintf(fp,"  // Reduce the result list to a scalar (if it is a single result)\n");
	fprintf(fp,"  while ( retval.is_list() && retval.length() == 1 )\n");
	fprintf(fp,"    {\n");
	fprintf(fp,"    octave_value_list list = retval.list_value();\n");
	fprintf(fp,"    retval = list(0);\n");
	fprintf(fp,"    }\n");
		
	fprintf(fp,"\n\n" );
  fprintf(fp,"\n\n  return retval;\n}\n\n");

	outputFunctionHelp(fp,data);
	fprintf(fp,"\n\n" );
}

/*
	Change log: 
	13. 01. 2004. Added help for individual methods. 
							    Help can be displayed as vtkClass("vtkClassMethod","Help");
								Added narrowing (convert list to scalar) of the return 
								  parameter when examining superclasses. 
*/	

