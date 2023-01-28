/*
   $Header: /home/swd.cvs/include/pdtypes.h,v 1.1 1992/12/22 18:46:03 johnr Exp $
   $Log: pdtypes.h,v $
 * Revision 1.1  1992/12/22  18:46:03  johnr
 * JR:  Standard pdtypes.h, which Ted used to write the mbc code.
 *
*/


#ifndef  pdtypes_h
#define  pdtypes_h   1

/*****

   Assumptions - NONE!

*****/

/* STANDARD TYPEDEFS */
typedef     unsigned    char     BYTE;
typedef     unsigned    char    *BYTEPTR;
typedef     unsigned    char    *CHARPTR;

typedef     unsigned    short    UINT16;
typedef                 short    SINT16;

typedef     unsigned    long     UINT32;
typedef                 long     SINT32;

typedef     unsigned    short    BOOL;

typedef     unsigned    short  (*PROCPTR  ) ();

/* STANDARD DEFINES */
   #ifndef NULL
   #define     NULL        0x0
   #endif
   
   #ifndef NULLPTR
   #define     NULLPTR     (void *) 0x0
   #endif
   
   #ifndef NULLPROC
   #define     NULLPROC    (PROCPTR) 0x0
   #endif

   #ifndef FALSE   
   #define     FALSE       0x00
   #endif
   
   #ifndef TRUE
   #define     TRUE        0x01
   #endif

   #ifndef OK
   #define     OK          0x01
   #endif
   
   #ifndef NOT_OK
   #define     NOT_OK      0x00
   #endif
   
/*****

   Assumptions - Compiler is configured to treat :

         char     as  8 bit quantity
         short    as 16 bit quantity
         long     as 32 bit quantity

    a pointer     as 32 bit quantity

*****/

typedef     unsigned    char     ubyte;

typedef     unsigned    short    uint16;
typedef     unsigned    long     uint32;

typedef                 char     sbyte;
typedef                 short    sint16;
typedef                 long     sint32;

typedef     unsigned    char     *pointer;

typedef     unsigned    long     bool32;
typedef     unsigned    short    bool16;
typedef     unsigned    char     bool8;


/*----- proc pointers, parameter spec unchecked, but helps readability -*/

typedef     unsigned    short    (*procptr  ) ();     /* generic        */
typedef     unsigned    short    (*procptr_n) ();     /* no param passed*/
typedef     unsigned    short    (*procptr_v) ();     /* single 32 bit  */
typedef     unsigned    short    (*procptr_p) ();     /* ptr to record  */

typedef     void                 (*vprocptr  ) ();    /* generic        */
typedef     void                 (*vprocptr_n) ();    /* no param passed*/
typedef     void                 (*vprocptr_v) ();    /* single 32 bit  */
typedef     void                 (*vprocptr_p) ();    /* ptr to record  */


#endif

