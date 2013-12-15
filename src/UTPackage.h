/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 * 
 * Project : OpenRaider, PSKModel viewer, libhel_modelloader
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : UTPackage
 * License : No use w/o permission (C) 2003 Mongoose
 * Comments: This class handles UT packages
 *
 *
 *           This file was generated using Mongoose's C++ 
 *           template generator script.  <stu7440@westga.edu>
 * 
 *-- Test Defines -----------------------------------------------
 *           
 * UNIT_TEST_UTPACKAGE - Builds UTPackage class as a console unit test 
 *
 *-- History ------------------------------------------------ 
 *
 * 2003.07.11:
 * Mongoose - Created, form old PSKModel prototype
 ================================================================*/


#ifndef GUARD__OPENRAIDER_MONGOOSE_UTPACKAGE_H_
#define GUARD__OPENRAIDER_MONGOOSE_UTPACKAGE_H_

typedef struct {

	unsigned int exportCount;
	unsigned int nameCount;

} utx_generation_t;

typedef struct {

	char *objName;;
	unsigned int objFlags;

} utx_name_table_t;

typedef struct {

	int objClass;
	int objSuper;
	unsigned int package;
	int objName;
	unsigned int objFlags;
	int serialSize;
	int serialOffset;

} utx_export_table_t;

typedef struct {

	int objClass;
	int objSuper;
	unsigned int package;
	int objName;

} utx_import_table_t;

typedef struct {

	unsigned int signature;
	unsigned short version;
	unsigned short version2; // aka mod
	unsigned int flags;
	unsigned int nameCount;
	unsigned int nameOffset;
	unsigned int exportCount;
	unsigned int exportOffset;
	unsigned int importCount;
	unsigned int importOffset;

	unsigned int heritageCount; // if version < 68
	unsigned int heritageOffset; // if version < 68

	unsigned int guid[4]; // else if version > 67
	unsigned int generationCount; // else if version > 67
	utx_generation_t *generations; // else if version > 67

	utx_name_table_t *nameTable;
	utx_export_table_t *exportTable;
	utx_import_table_t *importTable;
	unsigned int *heritageTable; // 16byte IDs stored as 4 u32s

} utx_header_t;


/* Index parser algorithm and defines from 'Unreal file reader' source */
#define INDEX_SIGN_BIT  0x80        // Sign bit on first byte
#define INDEX_CONT1_BIT 0x40        // Continuation bit on first byte
#define INDEX_BYT1_MASK 0x3f        // Data mast on first byte
#define INDEX_CONT_BIT  0x80        // Continuation bit on other bytes
#define INDEX_BYTE_MASK 0x7f        // Data mask on other bytes

#define UNP_ByteProperty        1
#define UNP_IntProperty         2
#define UNP_BoolProperty        3
#define UNP_FloatProperty       4
#define UNP_ObjectProperty      5
#define UNP_NameProperty        6
#define UNP_StringProperty      7
#define UNP_ClassProperty       8
#define UNP_ArrayProperty       9
#define UNP_StructProperty      10
#define UNP_VectorProperty      11
#define UNP_RotatorProperty     12
#define UNP_StrProperty         13
#define UNP_MapProperty         14
#define UNP_FixedArrayProperty  15

#define PROPERTY_TYPE_MASK  0x0f
#define PROPERTY_SIZE_MASK  0x70
#define PROPERTY_ARRAY_MASK 0x80

//////////////////////////////////////////////////////////////////////
// UTPackageProperty Class
//////////////////////////////////////////////////////////////////////

class UTPackageProperty
{
public:

	UTPackageProperty();

	~UTPackageProperty();

	void clear();

	int load(utx_header_t *header, FILE *f);
	

	int index;

	unsigned int nameIndex;
	
	unsigned int size;

	unsigned int type;

	unsigned int flags;

	unsigned char *data;
};


//////////////////////////////////////////////////////////////////////
// UTPackage Class
//////////////////////////////////////////////////////////////////////

class UTPackage
{
 public:

	enum
	{
		UT_NULL,
		UT_IMPORT,
		UT_EXPORT
		
	} UT_INDEX;


	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

	UTPackage();
	/*------------------------------------------------------
	 * Pre  : 
	 * Post : Constructs an object of UTPackage
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.07.11: 
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~UTPackage();
	/*------------------------------------------------------
	 * Pre  : UTPackage object is allocated
	 * Post : Deconstructs an object of UTPackage
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.07.11: 
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////



	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////


 private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////


	/* */
};

int load_utx(char *filename, bool hexDump, bool diskDump);
#endif
