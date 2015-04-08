// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include "imguifilesystem.h"

#ifdef _WIN32
#include <shlobj.h> // Known Directory locations
#   ifndef CSIDL_MYPICTURES
#       define CSIDL_MYPICTURES 0x0027
#   endif //CSIDL_MYPICTURES
#   ifndef CSIDL_MYMUSIC
#       define CSIDL_MYMUSIC 0x000d
#   endif //CSIDL_MYMUSIC
#   ifndef CSIDL_MYVIDEO
#       define CSIDL_MYVIDEO 0x000e
#   endif //CSIDL_MYVIDEO
#else // _WIN32
#   include <unistd.h>     // getpwuid
#   include <pwd.h>        // getenv ?
#endif //#ifdef _WIN32

#include "dirent_portable.h"
#include <sys/stat.h>
#include <ctype.h>  // tolower,...
#include <string.h> // strcmp
#include <stdio.h>  // FILENAME_MAX
#include <new>      // operator new


namespace ImGuiFs {

const int MAX_FILENAME_BYTES = FILENAME_MAX;
const int MAX_PATH_BYTES = PATH_MAX;
enum Sorting {
    SORT_ORDER_ALPHABETIC=0,
    SORT_ORDER_ALPHABETIC_INVERSE=1,
    SORT_ORDER_LAST_MODIFICATION=2,
    SORT_ORDER_LAST_MODIFICATION_INVERSE=3,
    SORT_ORDER_SIZE=4,
    SORT_ORDER_SIZE_INVERSE=5,
    SORT_ORDER_TYPE=6,
    SORT_ORDER_TYPE_INVERSE=7,
    SORT_ORDER_COUNT
};

// Definitions of some helper classes (String,Path,SortingHelper,Directory). Better not expose them in the header file----------
/*
// MAIN ISSUE: All these string handling methods work with ASCII strings,
// but the ones returned by dirent are multibyte OS dependant strings.
// That means that there are some limitations:

// LIMITATIONS:
// -> paths with '/','\\','.' bytes (and possibly a few others) inside multibyte codepoints are not supported       (*)
// -> file extensions composed by characters with more than one byte are not supported                              (**)

//(*) That's because when I call something like: mystring.find_last_of('/') or Path::Split('/')
// the result might not be correct if there's some multibyte codepoint that includes that byte(s) (bytes include '/','\\','.').

// (**) String::ToLower() deeply breaks any multibyte char.
// They're currently used only in:
// Path::GetExtension(...)
// Directory::GetFiles(const string& path,const string& wantedExtensions,const string& unwantedExtensions)
// That's because file extensions must be returned lowercase, so that e.g. ".PNG" and ".png" can be string matched (even on case sensitive file systems).
*/
class String    {
protected:
    String() {}
public:
    inline static void PushBack(ImVector<char[MAX_FILENAME_BYTES]>& rv,const char* s)    {
        const size_t sz = rv.size();
        rv.resize(sz+1);
        strcpy(&rv[sz][0], s ? s : "\0");
    }
#   if (FILENAME_MAX!=PATH_MAX)    // Will this work ? (I don't want to use templates)
    inline static void PushBack(ImVector<char[MAX_PATH_BYTES]>& rv,const char* s)    {
        const size_t sz = rv.size();
        rv.resize(sz+1);
        strcpy(&rv[sz][0], s ? s : "\0");
    }
#   endif //#if (FILENAME_MAX!=PATH_MAX)
    inline static void Substr(const char* text,char* rv,int start,int count=-1)    {
        if (!text) count=0;
        if (count<0) count = (int) strlen(text) - start;
        if (count>0) strncpy(rv,&text[start],count);
        rv[count]='\0';
    }
    inline static int Find(const char* text,const char toFind,int beg=0)   {
        if (!text) return -1;
        for (size_t i=beg,len=strlen(text);i<len;i++)    {
            if (text[i]==toFind) return i;
        }
        return -1;
    }
    inline static int FindLastOf(const char* text,const char toFind)   {
        if (!text) return -1;
        for (int i=(int)strlen(text)-1;i>=0;i--)    {
            if (text[i]==toFind) return i;
        }
        return -1;
    }
    inline static void ToLower(const char* text,char* rv)   {
        if (!text) {
            rv[0]='\0';
            return;
        }
        const size_t len = strlen(text);
        for (size_t i=0;i<len;i++)    {
            rv[i]=tolower(text[i]);
        }
        rv[len]='\0';
    }
    inline static void ToLowerInPlace(char* text)  {
        if (!text) return;
        for (size_t i=0,len = strlen(text);i<len;i++)    {
            char& c = text[i];
            c=tolower(c);
        }
    }
    inline static void Split(const char* text,ImVector<char[MAX_FILENAME_BYTES]>& rv,const char c=' ')  {
        rv.clear();
        if (!text) return;
        const int len = (int)strlen(text);
        if (len==0) return;
        int beg = 0;char tmp[MAX_FILENAME_BYTES];
        for (int i=0;i<len;i++)    {
            const char ch = text[i];
            if (ch==c) {
                Substr(text,tmp,beg,i-beg);
                PushBack(rv,tmp);
                beg = i+1;
            }
        }
        if (beg<len) {
            Substr(text,tmp,beg,len-beg);
            PushBack(rv,tmp);
        }
    }
    inline static void Replace(const char*  baseText,const char textToReplace,const char replacement,char* rv)    {
        strcpy(rv,baseText);
        ReplaceInPlace(rv,textToReplace,replacement);
    }
    inline static void ReplaceInPlace(char* text,const char toReplace,const char replacement)  {
        if (!text) return;
        for (size_t i=0,len = strlen(text);i<len;i++)    {
            char& c = text[i];
            if (c==toReplace) c=replacement;
        }
    }

#   ifdef _WIN32
    // Convert a wide Unicode string to an UTF8 string
    inline static void wide_to_utf8(const wchar_t* wstr,char* rv)    {
        rv[0]='\0';
        if (!wstr) return;
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
        WideCharToMultiByte                  (CP_UTF8, 0, wstr, -1, &rv[0], size_needed, NULL, NULL);
        //rv[size_needed]='\0';              // If the parameter after wstr is -1, the function processes the entire input string, including the terminating null character. Therefore, the resulting character string has a terminating null character, and the length returned by the function includes this character.
        return ;
    }
    // Convert an UTF8 string to a wide Unicode String
    inline static void utf8_to_wide(const char* str,wchar_t* rv)    {
        rv[0]=L'\0';
        if (!str) return;
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
        MultiByteToWideChar                  (CP_UTF8, 0, str, -1, &rv[0], size_needed);
        //rv[size_needed]=L'\0';            // // If the parameter after str is -1, the function processes the entire input string, including the terminating null character. Therefore, the resulting character string has a terminating null character, and the length returned by the function includes this character.
        return;
    }
#   endif // _WIN32
};
class Path	{
protected:
    Path() {}
public:

    static void GetAbsolutePath(const char *path, char *rv)  {
        rv[0]='\0';
#   ifndef _WIN32
        if (!path || strlen(path)==0) realpath("./", rv);
        else realpath(path, rv);
#   else //_WIN32
        static const int bufferSize = PATH_MAX+1;   // 4097 is good (PATH_MAX should be in <limits.h>, or something like that)
        static wchar_t buffer[bufferSize];
        static wchar_t wpath[bufferSize];
        String::utf8_to_wide(path ? path : "",wpath);
        ::GetFullPathNameW(&wpath[0],bufferSize,&buffer[0],NULL);
        String::wide_to_utf8(&buffer[0],rv);

        String::ReplaceInPlace(rv,'\\','/');
        size_t len;
        while ( (len=strlen(rv))>0 && rv[len-1]=='/')  rv[len-1]='\0';
        //fprintf(stderr,"AbsolutePath = \"%s\" (len:%d)\n",rv,(int) strlen(rv)); // TO remove!
#   endif // _WIN32
    }
    static void GetDirectoryName(const char *filePath, char *rv) {
        rv[0]='\0';if (!filePath) return;
        const int sz = strlen(filePath);
        if (sz==0 || strcmp(filePath,"/")==0 || strcmp(filePath,"\\")==0) {
            strcpy(rv,filePath);
            return;
        }
        const char c = filePath[sz-1];
        if (c == '/' || c=='\\') {
            char tmp[MAX_PATH_BYTES];
            String::Substr(filePath,tmp,0,sz-1);
            GetDirectoryName(tmp,rv);
            return;
        }

        if (c==':') {
            strcpy(rv,filePath);
            return;
        }
        int beg=String::FindLastOf(filePath,'\\');
        int beg2=String::FindLastOf(filePath,'/');
        beg=(beg>beg2?beg:beg2);
        if (beg==0) {
            String::Substr(filePath,rv,0,1);
            return;
        }
        if (beg!=-1) {
            String::Substr(filePath,rv,0,beg);
            return;
        }
        rv[0]='\0';
        return;
    }
    static void GetFileName(const char *filePath, char *rv)  {
        int beg=String::FindLastOf(filePath,'\\');
        int beg2=String::FindLastOf(filePath,'/');
        beg=(beg>beg2?beg:beg2);
        if (beg!=-1) {
            String::Substr(filePath,rv,beg+1);
            return;
        }
        strcpy(rv,filePath);
        return;
    }
    static void GetExtension(const char* filePath,char *rv) {
        int beg=String::FindLastOf(filePath,'.');
        int beg2=String::FindLastOf(filePath,'/');
        int beg3=String::FindLastOf(filePath,'\\');
        if (beg2!=-1)	{
            if (beg3!=-1) beg2 = beg3;
            else beg2 = beg2 > beg3 ? beg2 : beg3;
        }
        else if (beg3!=-1) beg2 = beg3;
        else {
            if (beg!=-1)  {
                String::Substr(filePath,rv,beg);
                String::ToLowerInPlace(rv);
                return;
            }
        }
        if (beg>beg2)  {
            if (beg!=-1)  {
                String::Substr(filePath,rv,beg);
                String::ToLowerInPlace(rv);
                return;
            }
        }
        rv[0]='\0';
        return;
    }
    static void Combine(const char* directory,const char* fileName,char* rv,bool appendMode=true)  {
        if (!appendMode) rv[0]='\0';
        const size_t size= directory ? strlen(directory) : 0;
        if (size==0) {
            strcat(rv,fileName);
            return;
        }
        strcat(rv,directory);
        if (directory[size-1]!='\\' && directory[size-1]!='/') {
            strcat(rv,"/");
            strcat(rv,fileName);
        }
        else strcat(rv,fileName);
        return;
    }
    static void Append(const char* directory,char* rv)  {
        if (!directory || strlen(directory)==0) return;
        size_t size = strlen(rv);
        if (size>0 && (rv[size-1]!='\\' && rv[size-1]!='/')) {strcat(rv,"/");++size;}
        strcat(rv,directory);
        size = strlen(rv);
        while (size>0 && (rv[size-1]=='\\' || rv[size-1]=='/')) {rv[size-1]='\0';--size;}
        if (size==0 || rv[size-1]==':') strcat(rv,"/");
    }
    static void Split(const char* path,ImVector<char[MAX_FILENAME_BYTES]>& rv,bool leaveIntermediateTrailingSlashes=true) {
        rv.clear();
        static char tex[MAX_PATH_BYTES];
        String::Replace(path,'\\','/',tex);
        size_t len = strlen(tex);
        static char tex2[MAX_PATH_BYTES];
#   ifdef _WIN32
        while ((len = strlen(tex))>0 && tex[len-1]=='/') {
            strncpy(tex2,tex,len+1);
            String::Substr(tex2,tex,0,len-1);
        }
#   endif //_WIN32
        if (len==0) return;
        int beg=-1;
        while ( (beg = String::Find(tex,'/'))!=-1) {
            static char tmp[MAX_FILENAME_BYTES];
            String::Substr(tex,tmp,0,leaveIntermediateTrailingSlashes ? beg+1 : beg);
            String::PushBack(rv,tmp);
            strcpy(tex2,tex);
            String::Substr(tex2,tex,beg+1);
        }
        String::PushBack(rv,tex);
        if (rv.size()>0 && strlen(rv[0])==0) strcpy((char*)&rv[0][0],"/\0");
#   ifdef _WIN32
        if (rv.size()==1 && strlen(rv[0])>0 && rv[0][strlen(rv[0])-1]==':') strcat((char*)&rv[0][0],"/");
#   endif //_WIN32
        return;
    }
    /*
    inline static bool Exists(const char* path) 	{
        struct stat statbuf;
        return (stat(path, &statbuf) != -1);
    }
    */
};
/*
class File {
public:
    inline static bool Exists(const char* filePath) {
        struct stat statbuf;
        return (stat(path, &statbuf) != -1 && (S_ISREG(statbuf.st_mode)));// || (acceptLinks ? S_ISLNK(statbuf.st_mode) : 1));
    }
protected:
    File() {}
};
*/
class SortingHelper {
public:
    typedef int (*SorterSignature)(const struct dirent **e1,const struct dirent **e2);
    inline static const SorterSignature& SetSorter(Sorting sorting) {
        const int isort =(int) sorting;
        if (isort>=0 && isort<(int)SORT_ORDER_COUNT) return (sorter = Sorters[isort]);
        return (sorter = Sorters[0]);
    }
protected:
    SortingHelper() {}
    const static SorterSignature Sorters[];
    static struct stat stat1;
    static struct stat stat2;
    static SorterSignature sorter;
#   ifdef _MSC_VER
    // Never tested (I've just been told that cl.exe does not have strcasecmp: please search the web for other possible alternative implementations)
    inline static int strcasecmp( const char *s1, const char *s2 )  {
        return _stricmp(s1,s2);
        //return lstrcmpiA(s1,s2);  // Not sure this is better
    }
#   endif //_MSC_VER
    // Possible problem: sorting is in ASCII with these methods
    static int Alphasort(const struct dirent **e1,const struct dirent **e2)    {
        return strcasecmp((*e1)->d_name,(*e2)->d_name);
    }
    static int Alphasortinverse (const struct dirent **e1,const struct dirent **e2)    {
        return -strcasecmp((*e1)->d_name,(*e2)->d_name);
    }
    // Please note that calling stat(...) every time inside sorters is a suicide! And that I'm doing right that! (but I guess and hope that on many systems the calls get cached somewhere: otherwise it would take ages to sort)
    static int Lastmodsort (const struct dirent **e1,const struct dirent **e2)    {
        if (stat((*e1)->d_name,&stat1)==-1) return -1;
        if (stat((*e2)->d_name,&stat2)==-1) return  1;
        return (stat1.st_mtime < stat2.st_mtime ? -1 : stat1.st_mtime > stat2.st_mtime ? 1 : 0);
    }
    static int Lastmodsortinverse(const struct dirent **e1,const struct dirent **e2)    {
        if (stat((*e1)->d_name,&stat1)==-1) return  1;
        if (stat((*e2)->d_name,&stat2)==-1) return -1;
        return (stat2.st_mtime < stat1.st_mtime ? -1 : stat2.st_mtime > stat1.st_mtime ? 1 : 0);
    }
    static int Sizesort (const struct dirent **e1,const struct dirent **e2)    {
        if (stat((*e1)->d_name,&stat1)==-1) return -1;
        if (stat((*e2)->d_name,&stat2)==-1) return  1;
        return (stat1.st_size < stat2.st_size ? -1 : stat1.st_size > stat2.st_size ? 1 : 0);
    }
    static int Sizesortinverse(const struct dirent **e1,const struct dirent **e2)    {
        if (stat((*e1)->d_name,&stat1)==-1) return  1;
        if (stat((*e2)->d_name,&stat2)==-1) return -1;
        return (stat2.st_size < stat1.st_size ? -1 : stat2.st_size > stat1.st_size ? 1 : 0);
    }
    // Please note that calculating the file extension every time inside sorters is a suicide (well, much less than before...)!
    static int Typesort(const struct dirent **e1,const struct dirent **e2)    {
        static const int dot = (int) '.';
        const char * p1 = strrchr((const char*) (*e1)->d_name, dot );
        const char * p2 = strrchr((const char*) (*e2)->d_name, dot );
        if (!p1) return (!p2?0:-1);
        else if (!p2)   return 1;
        return strcasecmp(p1,p2);
    }
    static int Typesortinverse (const struct dirent **e1,const struct dirent **e2)    {
        static const int dot = (int) '.';
        const char * p1 = strrchr((const char*) (*e1)->d_name, dot );
        const char * p2 = strrchr((const char*) (*e2)->d_name, dot );
        if (!p1) return (!p2?0:1);
        else if (!p2)   return -1;
        return -strcasecmp(p1,p2);
    }

};
const SortingHelper::SorterSignature SortingHelper::Sorters[] = {&SortingHelper::Alphasort,&SortingHelper::Alphasortinverse,&SortingHelper::Lastmodsort,&SortingHelper::Lastmodsortinverse,&SortingHelper::Sizesort,&SortingHelper::Sizesortinverse,&Typesort,&SortingHelper::Typesortinverse};
SortingHelper::SorterSignature SortingHelper::sorter;
struct stat SortingHelper::stat1;
struct stat SortingHelper::stat2;
class Directory {
public:
    static void GetDirectories(const char* directoryName,ImVector<char[MAX_PATH_BYTES]>& result,ImVector<char[MAX_FILENAME_BYTES]>* pOptionalNamesOut=NULL,Sorting sorting= SORT_ORDER_ALPHABETIC)   {
        result.clear();if (pOptionalNamesOut) pOptionalNamesOut->clear();
        static char tempString[MAX_PATH_BYTES];size_t sz;
        struct dirent **eps = NULL;

        sz = strlen(directoryName);
        static char directoryName2[MAX_PATH_BYTES];
        strcpy(directoryName2,directoryName);
#       ifdef _WIN32
        if (sz>0 && directoryName[sz-1]==':') {directoryName2[sz]='\\';directoryName2[sz+1]='\0';}
#       endif //_WIN32
        const int n = scandir (directoryName2, &eps, DirentGetDirectories, SortingHelper::SetSorter(sorting));

        static char directoryNameWithoutSlash[MAX_PATH_BYTES];
        if (sz>0 && directoryName[sz-1] == '/') String::Substr(directoryName,directoryNameWithoutSlash,0,sz-1);
        else strcpy(directoryNameWithoutSlash,directoryName);

        if (n >= 0) {
            result.reserve((size_t)n);if (pOptionalNamesOut) pOptionalNamesOut->reserve((size_t)n);
            for (int cnt = 0; cnt < n; ++cnt)    {
                const char* pName = &eps[cnt]->d_name[0];
                sz = strlen(pName);
                if (sz==0) continue;
                if (strcmp(pName,".")!=0 && strcmp(pName,"..")!=0 && pName[0]!='.' && pName[sz-1]!='~')    {
                    strcpy(tempString,directoryNameWithoutSlash);
                    strcat(tempString,"/");
                    strcat(tempString,pName);
                    String::PushBack(result,tempString);
                    if (pOptionalNamesOut) String::PushBack(*pOptionalNamesOut,pName);
                }
            }
        }
        if (eps) {free(eps);eps=NULL;}
    }
    static void GetFiles(const char* directoryName,ImVector<char[MAX_PATH_BYTES]>& result,ImVector<char[MAX_FILENAME_BYTES]>* pOptionalNamesOut=NULL, Sorting sorting= SORT_ORDER_ALPHABETIC)    {
        result.clear();if (pOptionalNamesOut) pOptionalNamesOut->clear();
        static char tempString[MAX_PATH_BYTES];size_t sz;
        struct dirent **eps = NULL;

        sz = strlen(directoryName);
        static char directoryName2[MAX_PATH_BYTES];
        strcpy(directoryName2,directoryName);
#       ifdef _WIN32
        if (sz>0 && directoryName[sz-1]==':') {directoryName2[sz]='\\';directoryName2[sz+1]='\0';}
#       endif //_WIN32
        const int n = scandir (directoryName2, &eps, DirentGetFiles, SortingHelper::SetSorter(sorting));

        static char directoryNameWithoutSlash[MAX_PATH_BYTES];
        if (sz>0 && directoryName[sz-1] == '/') String::Substr(directoryName,directoryNameWithoutSlash,0,sz-1);
        else strcpy(directoryNameWithoutSlash,directoryName);

        if (n >= 0) {
            result.reserve((size_t)n);if (pOptionalNamesOut) pOptionalNamesOut->reserve((size_t)n);
            for (int cnt = 0; cnt < n; ++cnt)    {
                const char* pName = &eps[cnt]->d_name[0];
                sz = strlen(pName);
                if (sz==0) continue;
                if (pName[0]!='.' && pName[sz-1]!='~')    {
                    strcpy(tempString,directoryNameWithoutSlash);
                    strcat(tempString,"/");
                    strcat(tempString,pName);
                    String::PushBack(result,tempString);
                    if (pOptionalNamesOut) String::PushBack(*pOptionalNamesOut,pName);
                }
            }
        }
        if (eps) {free(eps);eps=NULL;}
    }

    // e.g. ".txt;.jpg;.png". To use unwantedExtensions, set wantedExtensions="".
    static void GetFiles(const char* path,ImVector<char[MAX_PATH_BYTES]>& files,const char* wantedExtensions,const char* unwantedExtensions=NULL,ImVector<char[MAX_FILENAME_BYTES]>* pOptionalNamesOut=NULL,Sorting sorting= SORT_ORDER_ALPHABETIC)    {
        ImVector<char[MAX_PATH_BYTES]> filesIn;
        ImVector<char[MAX_FILENAME_BYTES]> namesIn;
        GetFiles(path,filesIn,&namesIn,sorting);
        if ((wantedExtensions==0 || strlen(wantedExtensions)==0) && (unwantedExtensions==0 || strlen(unwantedExtensions)==0)) {files = filesIn;return;}
        files.clear();if (pOptionalNamesOut) pOptionalNamesOut->clear();

        char wext[MAX_PATH_BYTES];String::ToLower(wantedExtensions,wext);
        char woext[MAX_PATH_BYTES];String::ToLower(unwantedExtensions,woext);

        char ext[MAX_PATH_BYTES];
        if (wantedExtensions && strlen(wantedExtensions)>0)	{
            files.reserve(filesIn.size());if (pOptionalNamesOut) pOptionalNamesOut->reserve(namesIn.size());
            ImVector<char[MAX_FILENAME_BYTES]> wExts;String::Split(wext,wExts,';');
            const size_t wExtsSize = wExts.size();
            if (wExtsSize>0)	{
                for (size_t i = 0,sz = filesIn.size();i<sz;i++)	{
                    Path::GetExtension(filesIn[i],ext);
                    for (size_t e=0;e<wExtsSize;e++)	{
                        if (strcmp(ext,wExts[e])==0) {
                            String::PushBack(files,filesIn[i]);
                            if (pOptionalNamesOut) String::PushBack(*pOptionalNamesOut,namesIn[i]);
                        }
                    }
                }
            }
            else return;
        }
        else if (unwantedExtensions && strlen(unwantedExtensions)>0) {
            files.reserve(filesIn.size());if (pOptionalNamesOut) pOptionalNamesOut->reserve(namesIn.size());
            ImVector<char[MAX_FILENAME_BYTES]> woExts;String::Split(woext,woExts,';');
            const size_t woExtsSize = woExts.size();
            if (woExts.size()==0) {files = filesIn;return;}
            bool match;
            for (size_t i = 0,sz = filesIn.size();i<sz;i++)	{
                Path::GetExtension(filesIn[i],ext);
                match = false;
                for (size_t e=0;e<woExtsSize;e++)	{
                    if (strcmp(ext,woExts[e])==0) {
                        match = true;
                        break;
                    }
                }
                if (!match) {
                    String::PushBack(files,filesIn[i]);
                    if (pOptionalNamesOut) String::PushBack(*pOptionalNamesOut,namesIn[i]);
                }
            }
        }
        else {
            files = filesIn;
            if (pOptionalNamesOut) *pOptionalNamesOut = namesIn;
        }
    }

    inline static void Create(const char* directoryName)   {
#       ifndef _WIN32
        const mode_t mode = S_IFDIR | S_IREAD | S_IWRITE | S_IRWXU | S_IRWXG | S_IRWXO;
        mkdir(directoryName,mode);
#       else //_WIN32
        static wchar_t name[PATH_MAX+1];
        String::utf8_to_wide(directoryName,name);
        ::CreateDirectoryW(name,NULL);
#       endif //_WIN32
    }

    inline static bool Exists(const char* path) {
        struct stat statbuf;
        return (stat(path, &statbuf) != -1 && S_ISDIR(statbuf.st_mode));
    }
    inline static const ImVector<char[MAX_PATH_BYTES]> &GetUserKnownDirectories(const ImVector<char[MAX_FILENAME_BYTES]> **pOptionalUserKnownDirectoryDisplayNamesOut,const int** pOptionalNumberKnownUserDirectoriesExceptDrives=NULL,bool forceUpdate=false)  {
        static bool init = false;
        static ImVector<char[MAX_PATH_BYTES]> rv;
        static ImVector<char[MAX_FILENAME_BYTES]> dn;
        static ImVector<char[MAX_PATH_BYTES]> mediaFolders;
        static int numberKnownUserDirectoriesExceptDrives = 0;
        if (pOptionalUserKnownDirectoryDisplayNamesOut) *pOptionalUserKnownDirectoryDisplayNamesOut = &dn;
        if (pOptionalNumberKnownUserDirectoriesExceptDrives) *pOptionalNumberKnownUserDirectoriesExceptDrives = &numberKnownUserDirectoriesExceptDrives;
        if (init && !forceUpdate) return rv;
        init = true;
        rv.clear();dn.clear();
#       ifdef _WIN32
        static const int csid[] = {
            CSIDL_DESKTOP,
            CSIDL_PERSONAL, //(Documents)
            CSIDL_FAVORITES,
            CSIDL_MYMUSIC,
            CSIDL_MYPICTURES,
            CSIDL_RECENT,
            CSIDL_MYVIDEO
        };
        static const char* name[] = {
            "Desktop",
            "Documents",
            "Favorites",
            "Music",
            "Pictures",
            "Recent",
            "Video"
        };
        static const int csidSize = sizeof(csid)/sizeof(csid[0]);
        static const int nameSize = sizeof(name)/sizeof(name[0]);
        IM_ASSERT(csidSize==nameSize);
        if (csidSize!=nameSize) fprintf(stderr,"ERROR in file: imguifilesystem.cpp. Directory::GetUserKnownDirectories(...) csidSize!=nameSize.\n");
        char tmp[MAX_PATH_BYTES] = "C:/";while (tmp[0]<='Z') {if (Directory::Exists(tmp)) String::PushBack(mediaFolders,tmp);tmp[0]=(char)((int)tmp[0]+1);}
        rv.reserve(csidSize+mediaFolders.size());
        dn.reserve(csidSize+mediaFolders.size());
        WCHAR path[MAX_PATH+1];
        for (int i=0;i<csidSize;i++)    {
            if (!GetSpecialFolderPathW(csid[i],&path[0],NULL)) continue;
            static char tmp2[MAX_PATH_BYTES];
            String::wide_to_utf8(&path[0],tmp2);
            String::PushBack(rv,tmp2);
            String::PushBack(dn,name[i]);
        }
        numberKnownUserDirectoriesExceptDrives = (int) rv.size();
        static char mediaFolderName[MAX_PATH_BYTES];
        for (int i=0,msz=mediaFolders.size();i<msz;i++)    {
            const char* mediaFolder = mediaFolders[i];
            String::PushBack(rv,mediaFolder);
            String::Substr(mediaFolder,mediaFolderName,0,strlen(mediaFolder)-1);
            String::PushBack(dn,mediaFolderName);
        }
#       else //_WIN32
        const char* homedir = NULL;
        if ((homedir = getenv("HOME")) == NULL) {
            homedir = getpwuid(getuid())->pw_dir;
        }
        if (homedir==NULL) return rv;
        char homeString[MAX_PATH_BYTES];strcpy(homeString,homedir);
        char userString[MAX_PATH_BYTES];Path::GetFileName(homeString,userString);
        // Known folders ---------------------------------------------
        static const char folder[][MAX_FILENAME_BYTES] = {
            "Desktop",
            "Documents",
            "Downloads",
            "Music",
            "Pictures",
            "Videos"
        };
        static const int folderSize = sizeof(folder)/sizeof(folder[0]);
        rv.reserve(folderSize+1);
        dn.reserve(rv.size());
        String::PushBack(rv,homeString);
        char temp[MAX_PATH_BYTES];
        strcpy(temp,"Home");
        String::PushBack(dn,temp);
        for (int i=0;i<folderSize;i++)    {
            Path::Combine(homeString,folder[i],temp,false);
            if (Directory::Exists(temp)) {
                String::PushBack(rv,temp);
                String::PushBack(dn,folder[i]);
            }
        }
        numberKnownUserDirectoriesExceptDrives = (int) rv.size();
        // Additional Drives --------------------------------------------
        static const char* mountLocations[] = {"/media","/mnt","/Volumes","/vol","/data"};
        static const int mountLocationSize = sizeof(mountLocations)/sizeof(mountLocations[0]);
        static const bool ifHomeSubfolerIsFoundInMountLocationsForgetThatRootMountLocation = true;  // That means: if "/media/myusername" exists, don't add any other "/media/..." entries.
        char userMediaString[MAX_PATH_BYTES];bool lastGood = false;
        for (int mntLocIndex=0,sz = 2*mountLocationSize;mntLocIndex<sz;mntLocIndex++) {
            const int index = mntLocIndex/2;
            const char* mntLocString = mountLocations[index];
            const bool useUserSuffix = (mntLocIndex%2)==0;
            if (useUserSuffix) {
                Path::Combine(mntLocString,userString,userMediaString,false);
                strcpy(temp,userMediaString);
            }
            else if (lastGood && ifHomeSubfolerIsFoundInMountLocationsForgetThatRootMountLocation) {lastGood = false;continue;} // see "ifHomeSubfolerIsFoundInMountLocationsForgetThatRootMountLocation" above
            else strcpy(userMediaString,mntLocString);
            lastGood = Directory::Exists(userMediaString);
            if (!lastGood) continue;
            Directory::GetDirectories(userMediaString,mediaFolders);
            if (mediaFolders.size()==0) continue;
            rv.reserve(rv.size()+mediaFolders.size());
            dn.reserve(rv.size());
            for (int i=0,msz=mediaFolders.size();i<msz;i++)    {
                if (strcmp(mediaFolders[i],temp)==0) continue;  // I we have processed "/media/myusername" once cycle before, exclude it from processing "/media" subfolders
                String::PushBack(rv,mediaFolders[i]);
                static char tmp[MAX_FILENAME_BYTES];
                Path::GetFileName(mediaFolders[i],tmp);
                String::PushBack(dn,tmp);
            }
        }
#       endif //_WIN32

        return rv;
    }


protected:
    Directory() {}

    static int DirentGetDirectories(const struct dirent *de) {
        if (de->d_type==DT_DIR) return 1;
        return 0;
    }
    static int DirentGetFiles(const struct dirent *de) {
        if (de->d_type==DT_REG) return 1;
        return 0;
    }

#   ifdef _WIN32
    static bool GetSpecialFolderPathW(int specialFolderCSIDL,WCHAR* pathOutWithSizeMaxPathPlusOne,HWND parent)  {
        //	CSIDL_DESKTOP,CSIDL_BITBUCKET,CSIDL_CONTROLS,CSIDL_DESKTOP,CSIDL_DESKTOPDIRECTORY,
        //	CSIDL_DRIVES,CSIDL_FONTS,CSIDL_NETHOOD,CSIDL_NETWORK,CSIDL_PERSONAL (Documents)
        //	CSIDL_PRINTERS,CSIDL_PROGRAMS,CSIDL_RECENT,CSIDL_SENDTO,CSIDL_STARTMENU,
        //	CSIDL_STARTUP,CSIDL_TEMPLATES

        //	CSIDL_INTERNET_CACHE,CSIDL_COOKIES,CSIDL_HISTORY,CSIDL_COMMON_APPDATA,
        //	CSIDL_WINDOWS,CSIDL_SYSTEM,CSIDL_PROGRAM_FILES,CSIDL_MYPICTURES,...

        WCHAR* temp_path = pathOutWithSizeMaxPathPlusOne;//[MAX_PATH+1];
        temp_path[0]=L'\0';
        LPITEMIDLIST pidl=NULL;
        if (!SUCCEEDED(::SHGetSpecialFolderLocation(parent,specialFolderCSIDL, &pidl)))
        {
            temp_path[0]=L'\0';return false;
        }
        bool ok=SUCCEEDED(::SHGetPathFromIDListW(pidl,&temp_path[0]));

        LPMALLOC mal = NULL;
        if ( ::SHGetMalloc( & mal ) == E_FAIL || !mal )	::free( pidl );
        else
        {
            mal->Free( pidl );
            mal->Release();
        }
        if (!ok)
        {
            temp_path[0]=L'\0';return false;
        }
        return true;
    }
#   endif //_WIN32

};
// End definitions of some helper classes----------------------------------------------------------------------------------------


// Internal usage----------------------------------------------------------------------------------------
struct FolderInfo    {
    char fullFolder[MAX_PATH_BYTES];
    char currentFolder[MAX_PATH_BYTES];
    int splitPathIndex;
    static ImVector<char[MAX_FILENAME_BYTES]> SplitPath;    // tmp field used internally

    void display() const {
        fprintf(stderr,"fullFolder=\"%s\" currentFolder=\"%s\" splitPathIndex=%d\n",fullFolder,currentFolder,splitPathIndex);
    }
    void getSplitPath(ImVector<char[MAX_FILENAME_BYTES]>& splitPath) const   {
        Path::Split(fullFolder,splitPath);
    }
    const FolderInfo& operator=(const FolderInfo& o) {
        strcpy(currentFolder,o.currentFolder);
        strcpy(fullFolder,o.fullFolder);
        splitPathIndex = o.splitPathIndex;
        return *this;
    }
    inline void reset() {
        currentFolder[0]='\0';fullFolder[0]='\0';splitPathIndex=-1;
    }
    FolderInfo() {reset();}
    FolderInfo(const FolderInfo& o) {*this=o;}

    void fromCurrentFolder(const char* path)   {
        if (!path || strlen(path)==0) reset();
        else {
            strcpy(currentFolder,path);
            strcpy(fullFolder,path);
            Path::Split(fullFolder,SplitPath);
            splitPathIndex = (int) SplitPath.size()-1;
        }
    }
    bool isEqual(const FolderInfo& fi) const {
        return strcmp(fullFolder,fi.fullFolder)==0 && strcmp(currentFolder,fi.currentFolder)==0;
    }
    bool isEqual(const char* path) const {
        return strcmp(fullFolder,path)==0 && strcmp(currentFolder,path)==0;
    }
    int getSplitPathIndexFor(const char* path) const {
        if (!path || strncmp(path,fullFolder,strlen(path))!=0) return -1;

        Path::Split(fullFolder,SplitPath);
        char tmp[MAX_PATH_BYTES];tmp[0]='\0';
        for (int i=0,sz=(int)SplitPath.size();i<sz;i++)    {
            Path::Append(SplitPath[i],tmp);
            //fprintf(stderr,"%d) \"%s\" <-> \"%s\"\n",i,tmp,path);
            if (strcmp(tmp,path)==0) return i;
        }
        return -1;
    }
    bool getFolderInfoForSplitPathIndex(int _splitPathIndex,FolderInfo& rv) const {
        Path::Split(fullFolder,SplitPath);
        const int splitPathSize = (int)SplitPath.size();
        if (_splitPathIndex<0 || _splitPathIndex>=splitPathSize) return false;
        rv = *this;
        rv.splitPathIndex = _splitPathIndex;

        rv.currentFolder[0]='\0';
        if (_splitPathIndex>=0 && _splitPathIndex<splitPathSize) {
            for (int i=0;i<=_splitPathIndex;i++)    {
                Path::Append(SplitPath[i],rv.currentFolder);
                //fprintf(stderr,"%d) \"%s\" (\"%s\")\n",i,rv.currentFolder,SplitPath[i]);
            }
        }
        /*fprintf(stderr,"getFolderInfoForSplitPathIndex(%d):\nSource:    ",_splitPathIndex);
        this->display();
        fprintf(stderr,"Result:   ");
        rv.display();*/
        return true;
    }
};
ImVector<char[MAX_FILENAME_BYTES]> FolderInfo::SplitPath;   // tmp field used internally
struct History {
protected:
    ImVector<FolderInfo> info;
    int currentInfoIndex;  // into info
public:
    inline bool canGoBack()    {
        return currentInfoIndex>0;
    }
    inline bool canGoForward()   {
        return currentInfoIndex>=0 && currentInfoIndex<(int)info.size()-1;
    }
    void reset() {info.clear();currentInfoIndex=-1;}
    History() {reset();}

    // -------------------------------------------------------------------------------------------------
    void goBack()   {
        if (canGoBack()) --currentInfoIndex;
    }
    void goForward()    {
        if (canGoForward()) ++currentInfoIndex;
    }
    bool switchTo(const char* currentFolder)    {
        if (!currentFolder || strlen(currentFolder)==0) return false;
        if (currentInfoIndex<0) {
            ++currentInfoIndex;
            info.resize(currentInfoIndex+1);
            FolderInfo& fi = info[currentInfoIndex];
            fi.fromCurrentFolder(currentFolder);
            return true;
        }
        else {
            const FolderInfo& lastInfo = info[currentInfoIndex];
            if (lastInfo.isEqual(currentFolder)) return false;
            const int splitPathIndexInsideLastInfo = lastInfo.getSplitPathIndexFor(currentFolder);
            ++currentInfoIndex;
            info.resize(currentInfoIndex+1);
            FolderInfo& fi = info[currentInfoIndex];
            if (splitPathIndexInsideLastInfo==-1)   fi.fromCurrentFolder(currentFolder);
            else {
                fi = lastInfo;
                fi.splitPathIndex = splitPathIndexInsideLastInfo;
                strcpy(fi.currentFolder,currentFolder);
            }
            return true;
        }
    }
    bool switchTo(const FolderInfo& fi) {
        if (!fi.currentFolder || strlen(fi.currentFolder)==0) return false;
        if (currentInfoIndex>=0) {
            const FolderInfo& lastInfo = info[currentInfoIndex];
            if (lastInfo.isEqual(fi)) return false;
        }
        ++currentInfoIndex;
        info.resize(currentInfoIndex+1);
        info[currentInfoIndex] = fi;
        return true;
    }
    //-----------------------------------------------------------------------------------------------------

    inline bool isValid() const {return (currentInfoIndex>=0 && currentInfoIndex<(int)info.size());}
    const FolderInfo* getCurrentFolderInfo() const {return isValid() ? &info[currentInfoIndex] : NULL;}
    const char* getCurrentFolder() const {return isValid() ? &info[currentInfoIndex].currentFolder[0] : NULL;}
    bool getCurrentSplitPath(ImVector<char[MAX_FILENAME_BYTES]>& rv) const {
        if (isValid()) {
            info[currentInfoIndex].getSplitPath(rv);
            return true;
        }
        else return false;
    }
    const int* getCurrentSplitPathIndex() const {return isValid() ? &info[currentInfoIndex].splitPathIndex : NULL;}
    size_t getInfoSize() const {return info.size();}
};

struct Internal {
    ImVector<char[MAX_PATH_BYTES]> dirs,files;
    ImVector<char[MAX_FILENAME_BYTES]> dirNames,fileNames,currentSplitPath;
    char currentFolder[MAX_PATH_BYTES];
    bool forceRescan;
    bool open;
    ImVec2 wndPos;
    ImVec2 wndSize;
    char wndTitle[MAX_PATH_BYTES];
    int sortingMode;

    History history;
    //-----------------------------------------------------
    bool isSelectFolderDialog;
    bool isSaveFileDialog;

    bool allowDirectoryCreation,forbidDirectoryCreation;
    bool allowKnownDirectoriesSection;
    char newDirectoryName[MAX_FILENAME_BYTES];
    char saveFileName[MAX_FILENAME_BYTES];
    //----------------------------------------------------

    char chosenPath[MAX_PATH_BYTES];
    bool rescan;
    int uniqueNumber;

    ImGuiTextFilter filter;
    bool allowFiltering;

    int totalNumBrowsingEntries;
    int numBrowsingColumns;
    int numBrowsingEntriesPerColumn;
    static bool BrowsingPerRow;
    bool allowDisplayByOption;

    bool detectKnownDirectoriesAtEveryOpening;
    bool mustFilterSaveFilePathWithFileFilterExtensionString;

    bool editLocationCheckButtonPressed;
    char editLocationInputText[MAX_PATH_BYTES];


    void resetVariables() {
        strcpy(currentFolder,"./");
        forceRescan = false;
        open = true;

        wndTitle[0] = '\0';
        sortingMode = 0;

        history.reset();

        isSelectFolderDialog = false;
        isSaveFileDialog = false;

        allowDirectoryCreation = true;
        forbidDirectoryCreation = false;
        strcpy(newDirectoryName,"New Folder");
        saveFileName[0] = '\0';

        uniqueNumber = 0;

        rescan = true;
        chosenPath[0] = '\0';

        filter.Clear();
        allowFiltering = false;

        totalNumBrowsingEntries = 0;
        numBrowsingColumns = 1;
        numBrowsingEntriesPerColumn = 1000;

        detectKnownDirectoriesAtEveryOpening = false;
        allowDisplayByOption = false;
        allowKnownDirectoriesSection = true;

        mustFilterSaveFilePathWithFileFilterExtensionString = true;

        editLocationCheckButtonPressed = false;
        strcpy(editLocationInputText,"\0");
    }

    // Just a convenience enum used internally
    enum Color {
        ImGuiCol_Dialog_Directory_Background,
        ImGuiCol_Dialog_Directory_Hover,
        ImGuiCol_Dialog_Directory_Pressed,
        ImGuiCol_Dialog_Directory_Text,

        ImGuiCol_Dialog_File_Background,
        ImGuiCol_Dialog_File_Hover,
        ImGuiCol_Dialog_File_Pressed,
        ImGuiCol_Dialog_File_Text,

        ImGuiCol_Dialog_SelectedFolder_Text,
        ImGuiCol_Dialog_Size

    };
    inline static void ColorCombine(ImVec4& c,const ImVec4& r,const ImVec4& factor)   {
        const float rr = (r.x+r.y+r.z)*0.3334f;
        c.x = rr * factor.x;c.y = rr * factor.y;c.z = rr * factor.z;c.w = r.w;
    }
};
bool Internal::BrowsingPerRow = false;
// End Internal Usage-------------------------------------------------------------------------------------

Dialog::Dialog(bool noKnownDirectoriesSection,bool noCreateDirectorySection,bool noFilteringSection,bool detectKnownDirectoriesAtEachOpening,bool addDisplayByOption,bool dontFilterSaveFilePathsEnteredByTheUser)    {
    internal = (Internal*) ImGui::MemAlloc(sizeof(Internal));
    new(internal) Internal();

    internal->resetVariables();
    static int un = 0;
    internal->uniqueNumber = un++;

    internal->detectKnownDirectoriesAtEveryOpening = detectKnownDirectoriesAtEachOpening;
    internal->allowDisplayByOption = addDisplayByOption;
    internal->forbidDirectoryCreation = noCreateDirectorySection;
    internal->allowKnownDirectoriesSection = !noKnownDirectoriesSection;
    internal->allowFiltering = !noFilteringSection;
    internal->mustFilterSaveFilePathWithFileFilterExtensionString = !dontFilterSaveFilePathsEnteredByTheUser;
}
Dialog::~Dialog()   {
    if (internal) {
        ImGui::MemFree(internal);
        //delete internal;
        internal = NULL;
    }
}
const char* Dialog::getChosenPath() const {return internal->chosenPath;}
const char* Dialog::getLastDirectory() const {return internal->currentFolder;}

// -- from imgui.cpp --
static size_t ImFormatString(char* buf, size_t buf_size, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int w = vsnprintf(buf, buf_size, fmt, args);
    va_end(args);
    buf[buf_size-1] = 0;
    return (w == -1) ? buf_size : (size_t)w;
}
// ---------------------

// 90% of the functionality of the whole imguifilesystem.cpp is inside this single method
const char* ChooseFileMainMethod(Dialog& ist,const char* directory,const bool _isFolderChooserDialog,const bool _isSaveFileDialog,const char* _saveFileName,const char* fileFilterExtensionString,const char* windowTitle,const ImVec2& windowSize,const ImVec2& windowPos,const float windowAlpha) {
    //-----------------------------------------------------------------------------
    Internal& I = *ist.internal;
    char* rv = I.chosenPath;rv[0] = '\0';
    //-----------------------------------------------------
    bool& isSelectFolderDialog = I.isSelectFolderDialog = _isFolderChooserDialog;
    bool& isSaveFileDialog = I.isSaveFileDialog = _isSaveFileDialog;

    bool& allowDirectoryCreation = I.allowDirectoryCreation = I.forbidDirectoryCreation ? false : (isSelectFolderDialog || isSaveFileDialog);
    //----------------------------------------------------------
    static const int* pNumberKnownUserDirectoriesExceptDrives=NULL;
    static const ImVector<char[MAX_FILENAME_BYTES]>* pUserKnownDirectoryDisplayNames=NULL;
    static const ImVector<char[MAX_PATH_BYTES]>* pUserKnownDirectories = &Directory::GetUserKnownDirectories(&pUserKnownDirectoryDisplayNames,&pNumberKnownUserDirectoriesExceptDrives);
    //----------------------------------------------------------
    const ImGuiStyle& style = ImGui::GetStyle();
    ImVec4 dummyButtonColor(0.0f,0.0f,0.0f,0.5f);     // Only the alpha is twickable from here
    static ImVec4 ColorSet[Internal::ImGuiCol_Dialog_Size];
    // Fill ColorSet above and fix dummyButtonColor here
    {
        static const ImVec4 df(0.9,0.9,0.3,1);          // directory color factor
        static const ImVec4 ff(0.7,0.7,0.7,1);          // file color factor

        for (int i=0,sz=(int)Internal::ImGuiCol_Dialog_Directory_Text;i<=sz;i++)    {
            ImVec4& c = ColorSet[i];
            const ImVec4& r = style.Colors[i<sz ? ((int)ImGuiCol_Button + i) : ImGuiCol_Text];
            Internal::ColorCombine(c,r,df);
        }
        for (int i=(int)Internal::ImGuiCol_Dialog_File_Background,sz=(int)Internal::ImGuiCol_Dialog_File_Text;i<=sz;i++)    {
            ImVec4& c = ColorSet[i];
            const ImVec4& r = style.Colors[i<sz ? ((int)ImGuiCol_Button-(int)Internal::ImGuiCol_Dialog_File_Background + i) : ImGuiCol_Text];
            Internal::ColorCombine(c,r,ff);
        }
        if (dummyButtonColor.w>0)   {
            const ImVec4& bbc = style.Colors[ImGuiCol_Button];
            dummyButtonColor.x = bbc.x;dummyButtonColor.y = bbc.y;dummyButtonColor.z = bbc.z;dummyButtonColor.w *= bbc.w;
        }
    }

    if (I.rescan)   {
        char validDirectory[MAX_PATH_BYTES];validDirectory[0]='\0';   // for robustness
        if (directory && strlen(directory)>0) {
            if (Directory::Exists(directory)) strcpy(validDirectory,directory);
            else {
                Path::GetDirectoryName(directory,validDirectory);
                if (!Directory::Exists(validDirectory)) validDirectory[0]='\0';
            }
        }
        Path::GetAbsolutePath(validDirectory,I.currentFolder);

        I.editLocationCheckButtonPressed = false;
        I.history.reset(); // reset history
        I.history.switchTo(I.currentFolder);    // init history
        I.dirs.clear();I.files.clear();I.dirNames.clear();I.fileNames.clear();I.currentSplitPath.clear();
        strcpy(&I.newDirectoryName[0],"New Folder");
        if (_saveFileName) {
            //strcpy(&I.saveFileName[0],_saveFileName);
            Path::GetFileName(_saveFileName,I.saveFileName);    // Better!
        }
        else I.saveFileName[0]='\0';
        isSelectFolderDialog = _isFolderChooserDialog;
        isSaveFileDialog = _isSaveFileDialog;
        allowDirectoryCreation =  I.forbidDirectoryCreation ? false : (isSelectFolderDialog || isSaveFileDialog);
        if (isSelectFolderDialog && I.sortingMode>SORT_ORDER_LAST_MODIFICATION_INVERSE) I.sortingMode = 0;
        I.forceRescan = true;
        I.open = true;
        I.filter.Clear();
        if (!windowTitle || strlen(windowTitle)==0)  {
            if (isSelectFolderDialog)   strcpy(I.wndTitle,"Please select a folder");
            else if (isSaveFileDialog)  strcpy(I.wndTitle,"Please choose/create a file for saving");
            else                        strcpy(I.wndTitle,"Please choose a file");
        }
        else                            strcpy(I.wndTitle,windowTitle);
        strcat(I.wndTitle,"##");
        char tmpWndTitleNumber[12];
        ImFormatString(tmpWndTitleNumber,11,"%d",I.uniqueNumber);
        strcat(I.wndTitle,tmpWndTitleNumber);
        I.wndPos = windowPos;
        I.wndSize = windowSize;
        if (I.wndSize.x<=0) I.wndSize.x = 400;
        if (I.wndSize.y<=0) I.wndSize.y = 400;
        const ImVec2 mousePos = ImGui::GetMousePos();//
        ImGui::GetCursorPos();
        if (I.wndPos.x<=0)  I.wndPos.x = mousePos.x - I.wndSize.x*0.5f;
        if (I.wndPos.y<=0)  I.wndPos.y = mousePos.y - I.wndSize.y*0.5f;
        const ImVec2 screenSize = ImGui::GetIO().DisplaySize;
        if (I.wndPos.x>screenSize.x-I.wndSize.x) I.wndPos.x = screenSize.x-I.wndSize.x;
        if (I.wndPos.y>screenSize.y-I.wndSize.y) I.wndPos.y = screenSize.y-I.wndSize.y;
        if (I.wndPos.x < 0) I.wndPos.x = 0;
        if (I.wndPos.y < 0) I.wndPos.y = 0;
        //fprintf(stderr,"screenSize = %f,%f mousePos = %f,%f wndPos = %f,%f wndSize = %f,%f\n",screenSize.x,screenSize.y,mousePos.x,mousePos.y,wndPos.x,wndPos.y,wndSize.x,wndSize.y);
        if (I.detectKnownDirectoriesAtEveryOpening) pUserKnownDirectories = &Directory::GetUserKnownDirectories(&pUserKnownDirectoryDisplayNames,&pNumberKnownUserDirectoriesExceptDrives,true);
    }
    if (!I.open) return rv;

    if (I.forceRescan)    {
        I.forceRescan = false;
        const int sortingModeForDirectories = (I.sortingMode <= (int)SORT_ORDER_LAST_MODIFICATION_INVERSE) ? I.sortingMode : (I.sortingMode%2);
        Directory::GetDirectories(I.currentFolder,I.dirs,&I.dirNames,(Sorting)sortingModeForDirectories);  // this is because directories don't return their size or their file extensions (so if needed we sort them alphabetically)
        //I.dirNames.resize(I.dirs.size());for (int i=0,sz=I.dirs.size();i<sz;i++)  Path::GetFileName(I.dirs[i],(char*)I.dirNames[i]);

        if (!isSelectFolderDialog)  {
            if (!fileFilterExtensionString || strlen(fileFilterExtensionString)==0) Directory::GetFiles(I.currentFolder,I.files,&I.fileNames,(Sorting)I.sortingMode);
            else                                        Directory::GetFiles(I.currentFolder,I.files,fileFilterExtensionString,NULL,&I.fileNames,(Sorting)I.sortingMode);
            //I.fileNames.resize(I.files.size());for (int i=0,sz=I.files.size();i<sz;i++) Path::GetFileName(I.files[i],(char*)I.fileNames[i]);
        }
        else {
            I.files.clear();I.fileNames.clear();
            I.saveFileName[0]='\0';
            char currentFolderName[MAX_FILENAME_BYTES];
            Path::GetFileName(I.currentFolder,currentFolderName);
            const size_t currentFolderNameSize = strlen(currentFolderName);
            if (currentFolderNameSize==0 || currentFolderName[currentFolderNameSize-1]==':') strcat(currentFolderName,"/");
            strcat(I.saveFileName,currentFolderName);
        }

        I.history.getCurrentSplitPath(I.currentSplitPath);

        const static int approxNumEntriesPerColumn = 20;//(int) (20.f / browseSectionFontScale);// tweakable
        I.totalNumBrowsingEntries = (int)(I.dirs.size()+I.files.size());
        I.numBrowsingColumns = I.totalNumBrowsingEntries/approxNumEntriesPerColumn;
        if (I.numBrowsingColumns<=0) I.numBrowsingColumns = 1;
        if (I.totalNumBrowsingEntries%approxNumEntriesPerColumn>(approxNumEntriesPerColumn/2)) ++I.numBrowsingColumns;
        if (I.numBrowsingColumns>6) I.numBrowsingColumns = 6;
        I.numBrowsingEntriesPerColumn = I.totalNumBrowsingEntries/I.numBrowsingColumns;
        if (I.totalNumBrowsingEntries%I.numBrowsingColumns!=0) ++I.numBrowsingEntriesPerColumn;

        //#       define DEBUG_HISTORY
#       ifdef DEBUG_HISTORY
        if (I.history.getInfoSize()>0) fprintf(stderr,"\nHISTORY: currentFolder:\"%s\" history.canGoBack=%s history.canGoForward=%s currentHistory:\n",I.currentFolder,I.history.canGoBack()?"true":"false",I.history.canGoForward()?"true":"false");
        if (I.history.getCurrentFolderInfo()) I.history.getCurrentFolderInfo()->display();
#       endif //DEBUG_HISTORY
    }

    if (I.rescan) {
        I.rescan = false; // Mandatory

        ImGui::Begin(I.wndTitle, &I.open, I.wndSize,windowAlpha);
        ImGui::SetWindowPos(I.wndPos);
        ImGui::SetWindowSize(I.wndSize);
        //fprintf(stderr,"\"%s\" wndPos={%1.2f,%1.2f}\n",wndTitle.c_str(),wndPos.x,wndPos.y);
    }
    else ImGui::Begin(I.wndTitle, &I.open,ImVec2(0,0),windowAlpha);
    ImGui::Separator();

    //------------------------------------------------------------------------------------
    // History (=buttons: < and >)
    {
        bool historyBackClicked = false;
        bool historyForwardClicked = false;

        // history -----------------------------------------------
        ImGui::PushID("historyDirectoriesID");

        const bool historyCanGoBack = I.history.canGoBack();
        const bool historyCanGoForward = I.history.canGoForward();

        if (!historyCanGoBack) {
            ImGui::PushStyleColor(ImGuiCol_Button,dummyButtonColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,dummyButtonColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,dummyButtonColor);
        }
        historyBackClicked = ImGui::Button("<")&historyCanGoBack;
        ImGui::SameLine();
        if (!historyCanGoBack) {
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
        }

        if (!historyCanGoForward) {
            ImGui::PushStyleColor(ImGuiCol_Button,dummyButtonColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,dummyButtonColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,dummyButtonColor);
        }
        historyForwardClicked = ImGui::Button(">")&historyCanGoForward;
        ImGui::SameLine();
        if (!historyCanGoForward) {
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
        }

        ImGui::PopID();
        // -------------------------------------------------------

        if (historyBackClicked || historyForwardClicked)    {
            ImGui::End();

            if (historyBackClicked)         I.history.goBack();
            else if (historyForwardClicked) I.history.goForward();

            I.forceRescan = true;

            strcpy(I.currentFolder,I.history.getCurrentFolder());
            strcpy(I.editLocationInputText,I.currentFolder);

#       ifdef DEBUG_HISTORY
            if (historyBackClicked) fprintf(stderr,"\nPressed BACK to\t");
            else fprintf(stderr,"\nPressed FORWARD to\t");
            fprintf(stderr,"\"%s\" (%d)\n",I.currentFolder,(int)*I.history.getCurrentSplitPathIndex());
#       undef DEBUG_HISTOTY
#       endif //DEBUG_HISTORY
            return rv;
        }
    }
    //------------------------------------------------------------------------------------
    // Edit Location CheckButton
    bool editLocationInputTextReturnPressed = false;
    {

        bool mustValidateInputPath = false;
        ImGui::PushStyleColor(ImGuiCol_Button,I.editLocationCheckButtonPressed ? dummyButtonColor : style.Colors[ImGuiCol_Button]);

        if (ImGui::Button("L##EditLocationCheckButton"))    {
            I.editLocationCheckButtonPressed = !I.editLocationCheckButtonPressed;
            if (I.editLocationCheckButtonPressed) {
                strcpy(I.editLocationInputText,I.currentFolder);
                ImGui::SetKeyboardFocusHere();
            }
            //if (!I.editLocationCheckButtonPressed) mustValidateInputPath = true;   // or not ? I mean: the user wants to quit or to validate in this case ?
        }

        ImGui::PopStyleColor();

        if (I.editLocationCheckButtonPressed) {
            ImGui::SameLine();
            editLocationInputTextReturnPressed = ImGui::InputText("##EditLocationInputText",I.editLocationInputText,MAX_PATH_BYTES,ImGuiInputTextFlags_AutoSelectAll|ImGuiInputTextFlags_EnterReturnsTrue);
            if (editLocationInputTextReturnPressed)  mustValidateInputPath = true;
            else ImGui::Separator();
        }

        if (mustValidateInputPath)  {
            // it's better to clean the input path here from trailing slashes:
            char cleanEnteredPath[MAX_PATH_BYTES];
            strcpy(cleanEnteredPath,I.editLocationInputText);
            size_t len = strlen(cleanEnteredPath);
            while (len>0 && (cleanEnteredPath[len-1]=='/' || cleanEnteredPath[len-1]=='\\')) {cleanEnteredPath[len-1]='\0';len = strlen(cleanEnteredPath);}

            if (len==0 || strcmp(I.currentFolder,cleanEnteredPath)==0)    I.editLocationCheckButtonPressed = false;
            else if (Directory::Exists(cleanEnteredPath))   {
                I.editLocationCheckButtonPressed = false; // Optional (return to split-path buttons)
                //----------------------------------------------------------------------------------
                I.history.switchTo(cleanEnteredPath);
                strcpy(I.currentFolder,cleanEnteredPath);
                I.forceRescan = true;

            }
            //else fprintf(stderr,"mustValidateInputPath NOOP: \"%s\" \"%s\"\n",I.currentFolder,cleanEnteredPath);
        }
        else ImGui::SameLine();

    }
    //------------------------------------------------------------------------------------
    // Split Path control
    if (!I.editLocationCheckButtonPressed && !editLocationInputTextReturnPressed) {
        bool mustSwitchSplitPath = false;
        const FolderInfo& fi = *I.history.getCurrentFolderInfo();

        ImVec2& framePadding = ImGui::GetStyle().FramePadding;
        const float originalFramePaddingX = framePadding.x;
        framePadding.x = 0;

        // Split Path
        // Tab:
        {
            //-----------------------------------------------------
            // TAB LABELS
            //-----------------------------------------------------
            {
                const int numTabs=(int) I.currentSplitPath.size();
                int newSelectedTab = fi.splitPathIndex;
                for (int t=0;t<numTabs;t++)	{
                    if (t>0) ImGui::SameLine(0,0);
                    if (t==fi.splitPathIndex) {
                        ImGui::PushStyleColor(ImGuiCol_Button,dummyButtonColor);
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,dummyButtonColor);
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive,dummyButtonColor);
                    }
                    ImGui::PushID(&I.currentSplitPath[t]);
                    const bool pressed = ImGui::Button(I.currentSplitPath[t]);
                    ImGui::PopID();
                    if (pressed) {
                        if (fi.splitPathIndex!=t && !mustSwitchSplitPath) mustSwitchSplitPath = true;
                        newSelectedTab = t;
                    }
                    if (t==fi.splitPathIndex) {
                        ImGui::PopStyleColor();
                        ImGui::PopStyleColor();
                        ImGui::PopStyleColor();
                    }
                }
                if (mustSwitchSplitPath) {
                    FolderInfo mfi;
                    fi.getFolderInfoForSplitPathIndex(newSelectedTab,mfi);
                    I.history.switchTo(mfi);
                    I.forceRescan = true;
                    strcpy(I.currentFolder,I.history.getCurrentFolder());
                    strcpy(I.editLocationInputText,I.currentFolder);
                    //fprintf(stderr,"%s\n",I.currentFolder);
                }
            }
        }

        framePadding.x = originalFramePaddingX;
    }
    //------------------------------------------------------------------------------------

    // Start collapsable regions----------------------------------------------------------
    // User Known directories-------------------------------------------------------------
    if (I.allowKnownDirectoriesSection && pUserKnownDirectories->size()>0)  {
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Known Directories##UserKnownDirectories"))  {
            static int id;
            ImGui::PushID(&id);

            ImGui::PushStyleColor(ImGuiCol_Text,ColorSet[Internal::ImGuiCol_Dialog_Directory_Text]);
            ImGui::PushStyleColor(ImGuiCol_Button,ColorSet[Internal::ImGuiCol_Dialog_Directory_Background]);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,ColorSet[Internal::ImGuiCol_Dialog_Directory_Hover]);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,ColorSet[Internal::ImGuiCol_Dialog_Directory_Pressed]);

            for (int i=0,sz=(int)pUserKnownDirectories->size();i<sz;i++)  {
                const char* userKnownFolder = (*pUserKnownDirectories)[i];
                const char* userKnownFolderDisplayName = (*pUserKnownDirectoryDisplayNames)[i];
                if (ImGui::SmallButton(userKnownFolderDisplayName) && strcmp(userKnownFolder,I.currentFolder)!=0) {
                    strcpy(I.currentFolder,userKnownFolder);
                    strcpy(I.editLocationInputText,I.currentFolder);
                    I.history.switchTo(I.currentFolder);
                    I.forceRescan = true;
                    //------------------------------------------------------------------------------------------------------------------------------
                }
                if (i!=sz-1 && (i>=*pNumberKnownUserDirectoriesExceptDrives || i%7!=6)) ImGui::SameLine();
            }

            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();

            ImGui::PopID();
        }

    }
    // End User Known directories---------------------------------------------------------
    // Allow directory creation ----------------------------------------------------------
    if (allowDirectoryCreation) {
        ImGui::Separator();
        bool mustCreate = false;

        if (ImGui::CollapsingHeader("New Directory##allowDirectoryCreation"))  {
            static int id;
            ImGui::PushID(&id);

            ImGui::InputText("##createNewFolderName",&I.newDirectoryName[0],MAX_FILENAME_BYTES);
            ImGui::SameLine();
            mustCreate = ImGui::Button("CREATE");

            ImGui::PopID();
        }

        if (mustCreate && strlen(I.newDirectoryName)>0)    {
            char newDirPath[MAX_PATH_BYTES];
            Path::Combine(I.currentFolder,I.newDirectoryName,newDirPath,false);
            if (!Directory::Exists(newDirPath)) {
                //#           define SIMULATING_ONLY
#           ifdef SIMULATING_ONLY
                fprintf(stderr,"creating: \"%s\"\n",newDirPath);
#           undef SIMULATING_ONLY
#           else //SIMULATING_ONLY
                Directory::Create(newDirPath);
                if (!Directory::Exists(newDirPath)) fprintf(stderr,"Error creating new folder: \"%s\"\n",newDirPath);
                else I.forceRescan = true;   // Just update
#           endif //SIMULATING_ONLY
            }
        }
    }
    // End allow directory creation ------------------------------------------------------
    // Filtering entries -----------------------------------------------------------------
    if (I.allowFiltering)  {
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Filtering##fileNameFiltering"))  {
            static int id;
            ImGui::PushID(&id);
            I.filter.Draw();
            ImGui::PopID();
        }

    }
    // End filtering entries -------------------------------------------------------------
    // End collapsable regions------------------------------------------------------------

    // Selection field -------------------------------------------------------------------
    if (isSaveFileDialog || isSelectFolderDialog)   {
        ImGui::Separator();
        bool selectionButtonPressed = false;

        static int id;
        ImGui::PushID(&id);
        if (isSaveFileDialog)   {
            ImGui::AlignFirstTextHeightToWidgets();
            ImGui::Text("File:");ImGui::SameLine();
            ImGui::InputText("##saveFileName",&I.saveFileName[0],MAX_FILENAME_BYTES);
            ImGui::SameLine();
        }
        else {
            ImGui::AlignFirstTextHeightToWidgets();
            ImGui::Text("Folder:");ImGui::SameLine();

            static const ImVec4 sf(1.0,0.8,0.5,1);      // delected folder color factor
            ImVec4& c = ColorSet[Internal::ImGuiCol_Dialog_SelectedFolder_Text];
            const ImVec4& r = style.Colors[ImGuiCol_Text];
            Internal::ColorCombine(c,r,sf);

            ImGui::TextColored(ColorSet[Internal::ImGuiCol_Dialog_SelectedFolder_Text],"%s",&I.saveFileName[0],MAX_FILENAME_BYTES);
            ImGui::SameLine();
        }

        if (isSelectFolderDialog)  selectionButtonPressed = ImGui::Button("Select");
        else selectionButtonPressed = ImGui::Button("Save");

        ImGui::PopID();

        if (selectionButtonPressed) {
            if (isSelectFolderDialog) {
                strcpy(rv,I.currentFolder);
                I.open = true;
            }
            else if (isSaveFileDialog)  {
                if (strlen(I.saveFileName)>0)  {
                    bool pathOk = true;
                    if (I.mustFilterSaveFilePathWithFileFilterExtensionString && fileFilterExtensionString && strlen(fileFilterExtensionString)>0)    {
                        pathOk = false;
                        char saveFileNameExtension[MAX_FILENAME_BYTES];Path::GetExtension(I.saveFileName,saveFileNameExtension);
                        const bool saveFileNameHasExtension = strlen(saveFileNameExtension)>0;
                        //-------------------------------------------------------------------
                        ImVector<char[MAX_FILENAME_BYTES]> wExts;String::Split(fileFilterExtensionString,wExts,';');
                        const size_t wExtsSize = wExts.size();
                        if (!saveFileNameHasExtension)   {
                            if (wExtsSize==0) pathOk = true;    // Bad situation, better allow this case
                            else strcat(I.saveFileName,wExts[0]);
                        }
                        else    {
                            // saveFileNameHasExtension
                            for (size_t i = 0;i<wExtsSize;i++)	{
                                const char* ext = wExts[i];
                                if (strcmp(ext,saveFileNameExtension)==0)   {
                                    pathOk = true;
                                    break;
                                }
                            }
                            if (!pathOk && wExtsSize>0) strcat(I.saveFileName,wExts[0]);
                        }
                    }
                    if (pathOk) {
                        char savePath[MAX_PATH_BYTES];
                        Path::Combine(I.currentFolder,I.saveFileName,savePath,false);
                        strcpy(rv,savePath);
                        I.open = true;
                    }
                }
            }
        }

        //ImGui::Spacing();
    }
    // End selection field----------------------------------------------------------------

    ImGui::Separator();
    // sorting --------------------------------------------------------------------
    ImGui::Text("Sorting by: ");ImGui::SameLine();
    {
        const int oldSortingMode = I.sortingMode;
        const int oldSelectedTab = I.sortingMode/2;
        //-----------------------------------------------------
        // TAB LABELS
        //-----------------------------------------------------
        {
            static const int numTabs=(int)SORT_ORDER_COUNT/2;
            int newSortingMode = oldSortingMode;
            static const char* names[numTabs] = {"Name","Modified","Size","Type"};
            const int numUsedTabs = isSelectFolderDialog ? 2 : numTabs;
            for (int t=0;t<numUsedTabs;t++)	{
                if (t>0) ImGui::SameLine();
                if (t==oldSelectedTab) {
                    ImGui::PushStyleColor(ImGuiCol_Button,dummyButtonColor);
                }
                ImGui::PushID(&names[t]);
                const bool pressed = ImGui::SmallButton(names[t]);
                ImGui::PopID();
                if (pressed) {
                    if (oldSelectedTab==t) {
                        newSortingMode = oldSortingMode;
                        if (newSortingMode%2==0) ++newSortingMode;// 0,2,4
                        else --newSortingMode;
                    }
                    else newSortingMode = t*2;
                }
                if (t==oldSelectedTab) {
                    ImGui::PopStyleColor();
                }
            }

            if (newSortingMode!=oldSortingMode) {
                I.sortingMode = newSortingMode;
                //printf("sortingMode = %d\n",sortingMode);
                I.forceRescan = true;
            }

            //-- Browsing per row -----------------------------------
            if (I.allowDisplayByOption && I.numBrowsingColumns>1)   {
                ImGui::SameLine();
                ImGui::Text("   Display by:");
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Button,dummyButtonColor);
                if (ImGui::SmallButton(!Internal::BrowsingPerRow ? "Column##browsingPerRow" : "Row##browsingPerRow"))   {
                    Internal::BrowsingPerRow = !Internal::BrowsingPerRow;
                }
                ImGui::PopStyleColor();
            }
            //-- End browsing per row -------------------------------
        }
    }
    //-----------------------------------------------------------------------------
    ImGui::Separator();

    //-----------------------------------------------------------------------------
    // MAIN BROWSING FRAME:
    //-----------------------------------------------------------------------------
    {
        ImGui::BeginChild("BrowsingFrame");
        // ImGui::SetScrollPosHere();   // possible future ref: while drawing to place the scroll bar
        ImGui::Columns(I.numBrowsingColumns);

        static int id;
        ImGui::PushID(&id);
        int cntEntries = 0;
        // Directories --------------------------------------------------------------
        if (I.dirs.size()>0)   {
            ImGui::PushStyleColor(ImGuiCol_Text,ColorSet[Internal::ImGuiCol_Dialog_Directory_Text]);
            ImGui::PushStyleColor(ImGuiCol_Button,ColorSet[Internal::ImGuiCol_Dialog_Directory_Background]);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,ColorSet[Internal::ImGuiCol_Dialog_Directory_Hover]);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,ColorSet[Internal::ImGuiCol_Dialog_Directory_Pressed]);

            for (int i=0,sz=(int)I.dirs.size();i<sz;i++) {
                const char* dirName = &I.dirNames[i][0];
                if (I.filter.PassFilter(dirName)) {
                    if (ImGui::SmallButton(dirName)) {
                        strcpy(I.currentFolder,I.dirs[i]);
                        strcpy(I.editLocationInputText,I.currentFolder);
                        I.history.switchTo(I.currentFolder);
                        I.forceRescan = true;
                        //------------------------------------------------------------------------------------------------------------------------------
                    }
                    ++cntEntries;
                    if (Internal::BrowsingPerRow) ImGui::NextColumn();
                    else if (cntEntries==I.numBrowsingEntriesPerColumn) {
                        cntEntries = 0;
                        ImGui::NextColumn();
                    }
                }
            }

            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
        }
        // Files ----------------------------------------------------------------------
        if (!isSelectFolderDialog && I.files.size()>0)   {
            ImGui::PushStyleColor(ImGuiCol_Text,ColorSet[Internal::ImGuiCol_Dialog_File_Text]);
            ImGui::PushStyleColor(ImGuiCol_Button,ColorSet[Internal::ImGuiCol_Dialog_File_Background]);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,ColorSet[Internal::ImGuiCol_Dialog_File_Hover]);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,ColorSet[Internal::ImGuiCol_Dialog_File_Pressed]);


            for (int i=0,sz=(int)I.files.size();i<sz;i++) {
                const char* fileName = &I.fileNames[i][0];
                if (I.filter.PassFilter(fileName)) {
                    if (ImGui::SmallButton(fileName)) {
                        if (!isSaveFileDialog)  {
                            strcpy(rv,I.files[i]);
                            I.open = true;
                        }
                        else {
                            Path::GetFileName(I.files[i],I.saveFileName);
                        }
                    }
                    ++cntEntries;
                    if (Internal::BrowsingPerRow) ImGui::NextColumn();
                    else if (cntEntries==I.numBrowsingEntriesPerColumn) {
                        cntEntries = 0;
                        ImGui::NextColumn();
                    }
                }
            }

            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
        }
        //-----------------------------------------------------------------------------
        ImGui::PopID();
        ImGui::EndChild();

    }
    //-----------------------------------------------------------------------------

    ImGui::End();
    return rv;
}

const char* Dialog::chooseFileDialog(bool dialogTriggerButton,const char* directory,const char* fileFilterExtensionString,const char* windowTitle,const ImVec2& windowSize,const ImVec2& windowPos,const float windowAlpha) {
    if (dialogTriggerButton) {internal->rescan = true;internal->chosenPath[0]='\0';}
    if (dialogTriggerButton || (!internal->rescan && strlen(getChosenPath())==0)) {
        ChooseFileMainMethod(*this,directory,false,false,"",fileFilterExtensionString,windowTitle,windowSize,windowPos,windowAlpha);
    }
    return getChosenPath();
}
const char* Dialog::chooseFolderDialog(bool dialogTriggerButton,const char* directory,const char* windowTitle,const ImVec2& windowSize,const ImVec2& windowPos,const float windowAlpha)  {
    if (dialogTriggerButton) {internal->rescan = true;internal->chosenPath[0]='\0';}
    if (dialogTriggerButton || (!internal->rescan && strlen(getChosenPath())==0)) {
        ChooseFileMainMethod(*this,directory,true,false,"","",windowTitle,windowSize,windowPos,windowAlpha);
    }
    return getChosenPath();
}
const char* Dialog::saveFileDialog(bool dialogTriggerButton,const char* directory,const char* startingFileNameEntry,const char* fileFilterExtensionString,const char* windowTitle,const ImVec2& windowSize,const ImVec2& windowPos,const float windowAlpha)    {
    if (dialogTriggerButton) {internal->rescan = true;internal->chosenPath[0]='\0';}
    if (dialogTriggerButton || (!internal->rescan && strlen(getChosenPath())==0)) {
        ChooseFileMainMethod(*this,directory,false,true,startingFileNameEntry,fileFilterExtensionString,windowTitle,windowSize,windowPos,windowAlpha);
    }
    return getChosenPath();
}

} // namespace ImGuiFs
